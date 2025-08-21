// SPDX-License-Identifier: MIT

#ifndef PACMANPP_ARG_PARSER_H_
#define PACMANPP_ARG_PARSER_H_

#include <string>
#include <vector>

#include "config.h"
#include "operation.h"

namespace yarp {

class ArgumentParser {
 public:
  constexpr ArgumentParser(const int argc, char **argv)
      : argc_(argc), argv_(argv) {}

  std::expected<void, std::string> ParseArgs(Operation &operation, QueryOptions &query_options,
                 std::vector<std::string> &targets, Config &config) const;

 private:
  const int argc_;
  char **argv_;
};

}  // namespace yarp

#endif  // PACMANPP_ARG_PARSER_H_