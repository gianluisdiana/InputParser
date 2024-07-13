/**
 * @file parser.tpp
 * @author Gian Luis Bolivar Diana (gianluisbolivar1@gmail.com)
 * @version 0.5
 * @date July 13, 2024
 * @copyright Copyright (c) 2024
 *
 * @brief File containing the implementation of a c++ command line parser
 * that uses the BaseOption class to represent the options that can be parsed,
 * and the ParsingError class to represent the errors that can be generated
 * when parsing the arguments provided.
 *
 */

#ifndef INPUT_PARSER_PARSER_TPP_
#define INPUT_PARSER_PARSER_TPP_

#ifndef INPUT_PARSER_PARSER_HPP_
#error "parser.tpp should only be included by parser.hpp"
#endif  // INPUT_PARSER_PARSER_HPP_

#include <input_parser/parser.hpp>

namespace input_parser {

// --------------------------------- Adders --------------------------------- //

template <typename... Types>
template <typename OptionType>
requires option_kind<OptionType, Types...>
Parser<Types...> &Parser<Types...>::addOption(OptionType &option) {
  const auto &reference_name = option.getNames().front();
  for (const auto &name : option.getNames()) {
    if (hasOptionName(name)) {
      throw std::invalid_argument("Option already exists!");
    }
    names_.insert({name, reference_name});
  }
  std::get<OptionMap<OptionType>>(options_).insert({reference_name, option});
  return *this;
}

// --------------------------------- Parse --------------------------------- //

template <typename... Types>
void Parser<Types...>::parse(unsigned int argc, char *raw_argv[]) {
  const std::vector<std::string> argv(raw_argv, raw_argv + argc);
  for (unsigned int index = 1; index < argc; ++index) {
    if (!hasOptionName(argv[index])) {
      throw ParsingError("Invalid arguments provided!");
    }
    const std::string &reference_name = names_.at(argv[index]);
    if (hasFlag(reference_name)) {
      parseFlag(reference_name);
    } else if (hasSingle(reference_name)) {
      index += parseSingle(argv, index);
    } else {
      index += parseCompound(argv, index);
    }
  }
  checkMissingOptions();
}

// --------------------------------- Checks -------------------------------- //

template <typename... Types>
void Parser<Types...>::checkMissingOptions() const {
  std::string missing_options;
  std::apply(
    [this, &missing_options](const auto &...option_maps) {
      auto checkOption = [&](const auto option_map) {
        for (const auto &[name, option_ref] : option_map) {
          auto &option = option_ref.get();
          if (!option.isValid()) {
            missing_options += "Missing option: " + name + "\n";
          }
        }
      };
      (checkOption(option_maps), ...);
    },
    options_
  );
  if (!missing_options.empty()) { throw ParsingError(missing_options); }
}

// ---------------------------- Individual parsers -------------------------- //

template <typename... Types>
void Parser<Types...>::parseFlag(const std::string &flag_name) {
  const std::string &reference_name = names_.at(flag_name);
  std::apply(
    [&, this](const auto &...option_maps) {
      const auto setFlagValue = [&](const auto &option_map) {
        if constexpr (same_as_any<
                        std::decay_t<decltype(option_map)>,
                        OptionMap<FlagOption<Types>>...>) {
          if (option_map.contains(reference_name)) {
            option_map.at(reference_name).get().setValue(true);
          }
        }
      };
      (setFlagValue(option_maps), ...);
    },
    options_
  );
}

template <typename... Types>
unsigned int Parser<Types...>::parseSingle(
  const std::vector<std::string> &arguments, unsigned int index
) {
  if (index + 1 >= arguments.size() || hasOptionName(arguments[index + 1])) {
    throw ParsingError(
      "After the " + arguments[index] + " option should be an extra argument!"
    );
  }
  const std::string &reference_name = names_.at(arguments[index]);
  std::apply(
    [&](const auto &...option_maps) {
      const auto setSingleValue = [&](const auto &option_map) {
        if constexpr (same_as_any<
                        std::decay_t<decltype(option_map)>,
                        OptionMap<SingleOption<Types>>...>) {
          if (option_map.contains(reference_name)) {
            option_map.at(reference_name).get().setValue(arguments[index + 1]);
          }
        }
      };
      (setSingleValue(option_maps), ...);
    },
    options_
  );
  return 1;
}

template <typename... Types>
unsigned int Parser<Types...>::parseCompound(
  const std::vector<std::string> &arguments, const unsigned int index
) {
  std::vector<std::string> values;
  unsigned int local_index = index + 1;
  while (local_index < arguments.size() &&
         !hasOptionName(arguments[local_index])) {
    values.push_back(arguments[local_index++]);
  }
  if (values.empty()) {
    throw ParsingError(
      "After the " + arguments[index] +
      " option should be at least an extra argument!"
    );
  }
  const std::string &reference_name = names_.at(arguments[index]);
  std::apply(
    [&](const auto &...option_maps) {
      const auto setCompoundValue = [&](const auto &option_map) {
        if constexpr (same_as_any<
                        std::decay_t<decltype(option_map)>,
                        OptionMap<CompoundOption<Types>>...>) {
          if (option_map.contains(reference_name)) {
            option_map.at(reference_name).get().setValue(values);
          }
        }
      };
      (setCompoundValue(option_maps), ...);
    },
    options_
  );
  return local_index - index - 1;
}

template <typename... Types>
std::string Parser<Types...>::usage() const {
  std::string usage = "Usage:\n";
  return usage;
}

}  // namespace input_parser

#endif  // INPUT_PARSER_PARSER_TPP_