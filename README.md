# Input parser (for C++)

- Author: Gian Luis Bolivar Diana
- Email: _gianluisbolivar1@gmail.com_

## Highlights
- Requires C++23
- MIT License

## Parser
In order to create a parser we must include the folder _<parser.hpp>_. To start parsing arguments create  __Parser__ object:

```cpp
#include <parser.hpp>

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
$ ./a.out

$ ./a.out -v
Hello World!

$ ./a.out --verbose
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
  $ ./a.out
  Really important information!

  $ ./a.out -v

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
  $ ./a.out
  Bye!

  $ ./a.out -g
  Hi!

  $ ./a.out --greeting
  Hi!
  ```

### Singles
A single option is an option that must be placed with an extra argument. Originally is stored as a _std::string_.
For example:

```cpp
auto parser = input::Parser()
  .addOption([] -> auto {
    return input::SingleOption()
      .addNames("-n")
      .addDescription("The name of the person using this program");
  });

parser.parse(argc, argv);
std::cout << parser.getValue<std::string>("-n") << " is using this program!\n";
```

And we can execute the code:
```bash
$ ./a.out
terminate called after throwing an instance of 'input::ParsingError'
  what():  Missing option -n
Aborted

$ ./a.out -n
terminate called after throwing an instance of 'input::ParsingError'
  what():  After the -n option should be an extra argument!
Aborted

$ ./a.out -n Luke
Luke is using this program!
```

### Compounds
A compound option is an option that must be placed with at least one extra argument. It stores its values on a _std::vector<std::string>_.
Here's an example:

```cpp
auto parser = input::Parser()
  .addOption([] -> auto {
    return input::CompoundOption()
      .addNames("-n", "--numbers")
      .addDescription("The numbers to be added together")
      .toDouble();
  });

parser.parse(argc, argv);

double result = 0;
for (auto& number : parser.getValue<std::vector<double>>("-n")) {
  result += number;
}
std::cout << "The sum of the number is: " << result << '\n';
```

And when executed, we get the following output:
```bash
$ ./a.out
terminate called after throwing an instance of 'input::ParsingError'
  what():  Missing option -n
Aborted

$ ./a.out -n
terminate called after throwing an instance of 'input::ParsingError'
  what():  After the -n option should be at least an extra argument!
Aborted

$ ./a.out -n 1 2
The sum of the number is: 3

$ ./a.out -n 1 2 3.4
The sum of the number is: 6.4

$ ./a.out -n 1 2.201 3.4 -4
The sum of the number is: 2.601

$ ./a.out -n 1 2.201 3.4 -0.123
The sum of the number is: 6.478
```

- __Transformation (each element)__

  This options allows the user to provide a callback function that will be applied to each of the elements stored using the _elementsTo_ method. The callback provided must have one _std::string_ parameter and return variable with the type desired.

  ```cpp
  auto parser = input::Parser()
    .addOption([] -> auto {
      return input::CompoundOption()
        .addNames("--approves")
        .addDescription("A group of y's and n's that will be counted")
        .elementsTo<bool>([](const std::string& element) {
          return element == "y" || element == "Y";
        });
    });

  parser.parse(argc, argv);

  int amount = 0;
  for (const auto approve : parser.getValue<std::vector<bool>>("--approves")) {
    amount += (approve ? 1 : 0);
  }
  std::cout << "There are " << amount << " people that aproves\n";
  ```

- __Transformation (all vector)__

  It also brings the possibility of transforming all the set calling the _to_ method.

  ```cpp
  struct Coordinate {
    int x;
    int y;
  };

  auto parser = input::Parser()
    .addOption([] -> auto {
      return input::CompoundOption()
        .addNames("-c", "--coordinate")
        .addDescription("The coordinate of a point (x, y)")
        .to<Coordinate>([](const std::vector<std::string>& value) {
          return Coordinate{
            std::stoi(value[0]), std::stoi(value[1])
          };
        });
    });

  parser.parse(argc, argv);
  auto [x, y] = parser.getValue<Coordinate>("-c");
  std::cout << "The coordinate is (" << x << ", " << y << ")\n";
  ```

## CMake Integration

Just clone the repository and add these lines to your _CMakeLists.txt_ file.
```cmake
cmake_minimum_required(VERSION 3.22)

PROJECT(my_project)
add_executable(my_project src/main.cpp)

target_link_libraries(${PROJECT_NAME} input_parser)
add_subdirectory(InputParser)
```