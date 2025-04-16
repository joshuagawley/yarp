// SPDX-License-Identifier: MIT

#include <cstdlib>
#include <print>

int main(int argc, char **argv) {
  if (argc == 1) {
    std::println("Error: no operation specified (use -h for help)");
    return EXIT_FAILURE;
  }
  if (argc > 2) {
    std::println("Error: too many arguments (use -h for help)");
    return EXIT_FAILURE;
  }
  if (argv[1][0] == '-') {
    if (argv[1][1] == 'h') {
      std::println("Usage: {} <operation>", argv[0]);
      std::println("operations:");
      std::println("  pacmanpp {{-h, --help}}     Show this help message");
      return EXIT_SUCCESS;
    } else {
      std::println("Error: unknown option '{}'", argv[1]);
      return EXIT_FAILURE;
    }
  }

  std::println("Hello, World!");
  return 0;
}
