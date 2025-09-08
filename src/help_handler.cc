// SPDX-License-Identifier: MIT

#include "help_handler.h"

#include <cstdlib>
#include <print>
#include <sstream>

#include "settings.h"

namespace yarp {

int HelpHandler::Execute() {
  std::string result;

  std::format_to(std::back_inserter(result), "Usage: {} <operation>\n",
                 kYarpName);
  std::format_to(std::back_inserter(result), "operations:\n");
  std::format_to(std::back_inserter(result), "  {} {{-h, --help}}\n",
                 kYarpName);
  std::format_to(std::back_inserter(result), "  {} {{-V, --version}}\n",
                 kYarpName);
  std::format_to(std::back_inserter(result),
                 "  {} {{-Q, --query}} [options] [package(s)]\n", kYarpName);
  std::format_to(std::back_inserter(result),
                 "  {} {{-S, --sync}}  [options] [package(s)]\n", kYarpName);
  std::format_to(
      std::back_inserter(result),
      "Use '{}' {{-h --help}} with an operation for available options",
      kYarpName);

  std::print("{}", result);

  return EXIT_SUCCESS;
}

}  // namespace yarp