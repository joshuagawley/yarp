// SPDX-License-Identifier: MIT

#include "help_handler.h"

#include <cstdlib>
#include <print>

#include "settings.h"

namespace pacmanpp {

int HelpHandler::Execute() {
  std::println("Usage: {} <operation>", kPacmanppName);
  std::println("operations:");
  std::println("  {} {{-h, --help}}                  Show this help message",
               kPacmanppName);
  std::println("  {} {{-Q, --query}} [package(s)]    Query installed packages",
               kPacmanppName);
  return EXIT_SUCCESS;
}

}  // namespace pacmanpp