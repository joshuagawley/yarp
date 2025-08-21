// SPDX-License-Identifier: MIT

#include "argument_parser.h"

#include <getopt.h>

#include <array>

namespace {

constexpr std::string_view kOptString = "cdehkmnopstuQVgilv";

constexpr std::array<option, 22> kOpts = {{
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
    {"owns", no_argument, nullptr, 'o'},
    {"file", no_argument, nullptr, 'p'},
    {"root", required_argument, nullptr, 'r'},
    {"search", no_argument, nullptr, 's'},
    {"unrequired", no_argument, nullptr, 't'},
    {"upgrade", no_argument, nullptr, 'u'},
    {"dbpath", required_argument, nullptr, 'b'},
    {"verbose", no_argument, nullptr, 'v'},
    {"config", required_argument, nullptr, 0},
    {nullptr, 0, nullptr, 0},
}};

}  // namespace

namespace yarp {

std::expected<void, std::string> ArgumentParser::ParseArgs(Operation &operation,
                               QueryOptions &query_options,
                               std::vector<std::string> &targets,
                               Config &config) const {
  int option_index = 0;
  int ch;

  while ((ch = getopt_long(argc_, argv_, kOptString.data(), kOpts.data(),
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
      case 'o':
        query_options |= QueryOptions::kOwns;
        break;
      case 'p':
        query_options |= QueryOptions::kIsFile;
        break;
      case 's':
        query_options |= QueryOptions::kSearch;
        break;
      case 't':
        query_options |= QueryOptions::kUnrequired;
        break;
      case 'u':
        query_options |= QueryOptions::kUpgrade;
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
      case 0:
        if (kOpts[option_index].flag != 0) {
          operation = Operation::kNone;
          break;
        } else if (std::string_view{kOpts[option_index].name} ==
                   std::string_view{"config"}) {
          config.set_conf_file(optarg);
          break;
        }
    }
  }

  // we need to process any remaining optional arguments after processing
  // the command line arguments with getopt
  if (operation == Operation::kQuery && optind < argc_) {
    for (int i = optind; i < argc_; ++i) {
      targets.emplace_back(argv_[i]);
    }
  }

  return config.ParseFromConfig();
}

}  // namespace  pacmanpp