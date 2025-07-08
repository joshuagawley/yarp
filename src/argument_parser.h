// SPDX-License-Identifier: MIT

#ifndef PACMANPP_ARG_PARSER_H_
#define PACMANPP_ARG_PARSER_H_

#include <getopt.h>

#include <array>
#include <string>
#include <vector>

#include "config.h"
#include "operation.h"

namespace pacmanpp {

namespace detail {

constexpr std::string_view kOptString = "cdehkmnpQVgilv";

constexpr std::array<option, 17> kOpts = {{
    {"help", no_argument, nullptr, 'h'},
    {"query", optional_argument, nullptr, 'Q'},
    {"version", no_argument, nullptr, 'V'},
    {"changelog", no_argument, nullptr, 'c'},
    {"groups", no_argument, nullptr, 'g'},
    {"deps", no_argument, nullptr, 'd'},
    {"explicit", no_argument, nullptr, 'e'},
    {"info", no_argument, nullptr, 'i'},
    {"check", no_argument, nullptr, 'k'},
    {"list", no_argument, nullptr, 'l'},
    {"foreign", no_argument, nullptr, 'm'},
    {"native", no_argument, nullptr, 'n'},
    {"file", no_argument, nullptr, 'p'},
    {"root", required_argument, nullptr, 'r'},
    {"dbpath", required_argument, nullptr, 'b'},
    {"verbose", no_argument, nullptr, 'v'},
    {nullptr, 0, nullptr, 0},
}};

}  // namespace detail

class ArgumentParser {
 public:
  constexpr ArgumentParser(const int argc, char **argv)
      : argc_(argc), argv_(argv) {}

  constexpr void ParseArgs(Operation &operation, QueryOptions &query_options,
                           std::vector<std::string> &targets,
                           Config &config) const {
    int option_index = 0;
    int ch;

    while ((ch = getopt_long(argc_, argv_, detail::kOptString.data(),
                             detail::kOpts.data(),
                             &option_index)) != -1) {
      switch (ch) {
        case 'h':
          operation = Operation::kHelp;
          break;
        case 'Q':
          operation = Operation::kQuery;
          break;
        case 'V':
          operation = Operation::kVersion;
          break;
        case 'c':
          query_options |= QueryOptions::kChangelog;
          break;
        case 'g':
          query_options |= QueryOptions::kGroups;
          break;
        case 'd':
          query_options |= QueryOptions::kDeps;
          break;
        case 'e':
          query_options |= QueryOptions::kExplicit;
          break;
        case 'i':
          query_options |= QueryOptions::kInfo;
          break;
        case 'k':
          query_options |= QueryOptions::kCheck;
          break;
        case 'l':
          query_options |= QueryOptions::kList;
          break;
        case 'm':
          query_options |= QueryOptions::kForeign;
          break;
        case 'n':
          query_options |= QueryOptions::kNative;
          break;
        case 'p':
          query_options |= QueryOptions::kIsFile;
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
      for (int i = optind; i < argc_; ++i) {
        targets.emplace_back(argv_[i]);
      }
    }

    config.ParseFromConfig();
  }

 private:
  const int argc_;
  char **argv_;
};

}  // namespace pacmanpp

#endif  // PACMANPP_ARG_PARSER_H_