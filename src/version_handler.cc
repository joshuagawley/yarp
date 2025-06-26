// SPDX-License-Identifier: MIT

#include "version_handler.h"

#include <cstdlib>
#include <print>

#include "settings.h"

namespace pacmanpp {

int VersionHandler::Execute() {
  std::println("{} version {}", kPacmanppName, kPacmanppVersion);
  return EXIT_SUCCESS;
}

}  // namespace pacmanpp