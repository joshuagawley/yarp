// SPDX-License-Identifier: MIT

#ifndef PACMANPP_ARG_PARSER_H_
#define PACMANPP_ARG_PARSER_H_

#include <getopt.h>

#include <array>

#include "bitwise_enum.h"

namespace {

constexpr const char *kOptString = "hQ::";

static constexpr std::array<struct option, 3> kOpts = {{
    {"help", no_argument, nullptr, 'h'},
    {nullptr, 0, nullptr, 0},
}};

}  // namespace

namespace pacmanpp {

enum class Operation { kNone = 1 << 0, kHelp = 1 << 1 };

template <>
struct util::EnableEnumBitwiseOperators<Operation> {
  static constexpr bool enabled = true;
};

class ArgumentParser {
 public:
  constexpr ArgumentParser(const int argc, char **argv)
      : argc_(argc), argv_(argv) {}

  constexpr void ParseArgs(Operation &operation) {
    int option_index = 0;
    int ch;

    while ((ch = getopt_long(argc_, argv_, kOptString,
                             const_cast<option *>(kOpts.data()),
                             &option_index)) != -1) {
      switch (ch) {
        case 'h':
          operation = Operation::kHelp;
          break;
        default:
          operation = Operation::kNone;
          break;
      }
    }
  };

 private:
  const int argc_;
  char **argv_;
};

}  // namespace pacmanpp

#endif  // PACMANPP_ARG_PARSER_H_