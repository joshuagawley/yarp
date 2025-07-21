// SPDX-License-Identifier: MIT

#include "noop_handler.h"

#include <cstdlib>
#include <print>

namespace pacmanpp {

int NoOpHandler::Execute() {
  std::println("Error: no operation specified (use -h for help)");
  return EXIT_FAILURE;
}

}  // namespace pacmanpp