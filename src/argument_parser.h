// SPDX-License-Identifier: MIT

#ifndef PACMANPP_ARG_PARSER_H_
#define PACMANPP_ARG_PARSER_H_

#include <getopt.h>

#include <array>
#include <string>
#include <vector>

#include "config.h"
#include "operation.h"

namespace {

constexpr const char *kOptString = "hQciv";

static constexpr std::array<struct option, 8> kOpts = {{
    {"help", no_argument, nullptr, 'h'},
    {"query", optional_argument, nullptr, 'Q'},
    {"changelog", no_argument, nullptr, 'c'},
    {"info", no_argument, nullptr, 'i'},
    {"root", required_argument, nullptr, 'r'},
    {"dbpath", required_argument, nullptr, 'b'},
    {"verbose", no_argument, nullptr, 'v'},
    {nullptr, 0, nullptr, 0},
}};

}  // namespace

namespace pacmanpp {

class ArgumentParser {
 public:
  constexpr ArgumentParser(const int argc, char **argv)
      : argc_(argc), argv_(argv) {}

  constexpr void ParseArgs(Operation &operation, QueryOptions &query_options,
                           std::vector<std::string> &targets, Config &config) {
    int option_index = 0;
    int ch;

    while ((ch = getopt_long(argc_, argv_, kOptString,
                             const_cast<option *>(kOpts.data()),
                             &option_index)) != -1) {
      switch (ch) {
        case 'h':
          operation = Operation::kHelp;
          break;
        case 'Q':
          operation = Operation::kQuery;
          break;
        case 'c':
          query_options |= QueryOptions::kChangelog;
          break;
        case 'i':
          query_options |= QueryOptions::kInfo;
          break;
        case 'v':
          config.set_verbose(true);
          break;
        // Handle long-only options
        case 'r':
          config.set_root(optarg);
          break;
        case 'b':
          config.set_db_path(optarg);
          break;
        default:
          operation = Operation::kNone;
          break;
      }
    }

    // we need to process any remaining optional arguments after processing
    // the command line arguments with getopt
    if (operation == Operation::kQuery && optind < argc_) {
      // TODO: option_index doesn't update in getopt loop
      for (int i = optind; i < argc_; ++i) {
        targets.emplace_back(argv_[i]);
      }
    }
  };

 private:
  const int argc_;
  char **argv_;
};

}  // namespace pacmanpp

#endif  // PACMANPP_ARG_PARSER_H_