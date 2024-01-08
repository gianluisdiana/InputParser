# Input parser (for C++)

- Author: Gian Luis Bolivar Diana
- Email: _gianluisbolivar1@gmail.com_

## Parser
In order to create a parser we must include the folder _"include/parser.hpp"_. To start parsing arguments create  __Parser__ object:

```cpp
auto parser = input::Parser();
```

To add new options, simply call `addOption`. This method receives a function that doesn't receives parameter and returns an `BaseOption` child (see [Option Types](#options)).

```cpp
parser.addOption([] -> auto {
  return input::FlagOption();
});
```

To parse the arguments, call the method `parse` with the amount of arguments and the array of string. This method will throw a `ParsingError` if the arguments provided at the command line don't correspond to the ones described previously.
A basic how to use information can be displayed calling `displayUsage`.

```cpp
try {
  parser.parse(argc, argv);
} catch (input::ParsingError& error) {
  std::cerr << error.what() << "\n";
  parser.displayUsage();
  return 1;
}
```


## Options
This parser supports three option types: flag, single and compound.

### Flags
A flag option is an option that must be placed alone. Represents a boolean value.
For example:

```cpp
auto flag_option = input::FlagOption()
  .addNames("-v", "--verbose")
  .addDescription("Whether if the program will display the output or not")
  .addDefaultValue(false);
auto parser = input::Parser()
  .addOption([flag_option] -> auto { return flag_option; });

try {
  parser.parse(argc, argv);
} catch (input::ParsingError& error) {
  std::cerr << error.what() << "\n";
  parser.displayUsage();
  return 1;
}

const bool verbose = parser.getValue<bool>("-v");
if (verbose) std::cout << "Hello World!\n";
```

And we can execute the code:
```bash
./a.exe

./a.exe -v
Hello World!

./a.exe --verbose
Hello World!
```

- __Default value__

  The option manage default values different from the others two: if an option has a default value but the program is called with the flag activated, the value to be assigned will the opposite of the default. Look at this example:

  ```cpp
  auto parser = input::Parser()
    .addOption([] -> auto { return input::FlagOption()
      .addNames("-v", "--verbose")
      .addDescription("Whether if the program will display information or not")
      .addDefaultValue(true);
    });
  parser.parse(argc, argv);
  if (parser.getValue<bool>("-v")) std::cout << "Really important information!\n";
  ```

  We can call the program like this:
  ```bash
  ./a.out
  Really important information!

  ./a.out -v

  ```

- __Transformation__

  Using a flag option allows you to create a different type value from a boolean. For example:

  ```cpp
  auto parser = input::Parser()
    .addOption([] -> auto { return input::FlagOption()
      .addNames("-g", "--greeting")
      .addDescription("Whether if the program will say hi or bye")
      .addDefaultValue(false)
      .to<std::string>([](const bool& value) -> std::string {
        return value ? "Hi!" : "Bye!";
      });
    });
  parser.parse(argc, argv);
  std::cout << parser.getValue<std::string>("-g") << '\n';
  ```

  When executing the code, the string displayed will depend:

  ```bash
  ./a.out
  Bye!

  ./a.out -g
  Hi!

  ./a.out --greeting
  Hi!
  ```