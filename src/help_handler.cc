// SPDX-License-Identifier: MIT

#include "help_handler.h"

#include <cstdlib>
#include <print>
#include <sstream>

#include "settings.h"

namespace yarp {

int HelpHandler::Execute() {
  std::stringstream ss{};
  std::println(ss, "Usage: {} <operation>", kYarpName);
  std::println(ss, "operations:");
  std::println(ss, "  {} {{-h, --help}}", kYarpName);
  std::println(ss, "  {} {{-V, --version}}", kYarpName);
  std::println(ss, "  {} {{-Q, --query}} [options] [package(s)]", kYarpName);
  std::println(ss, "  {} {{-S, --sync}}  [options] [package(s)]", kYarpName);
  std::print(ss,
             "Use '{}' {{-h --help}} with an operation for available options",
             kYarpName);

  std::print("{}", ss.str());
  return EXIT_SUCCESS;
}

}  // namespace yarp