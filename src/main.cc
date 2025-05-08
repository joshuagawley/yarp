// SPDX-License-Identifier: MIT

#include <alpm.h>
#include <alpm_list.h>

#include <cstdlib>
#include <print>

#include "argument_parser.h"

int main(int argc, char **argv) {
  auto operation = pacmanpp::Operation::kNone;
  pacmanpp::ArgumentParser arg_parser(argc, argv);
  arg_parser.ParseArgs(operation);

  switch (operation) {
    case pacmanpp::Operation::kNone:
      std::println("Error: no operation specified (use -h for help)");
      return EXIT_FAILURE;
    case pacmanpp::Operation::kHelp:
      std::println("Usage: {} <operation>", argv[0]);
      std::println("operations:");
      std::println("  pacmanpp {{-h, --help}}     Show this help message");
      return EXIT_SUCCESS;
  }
}
