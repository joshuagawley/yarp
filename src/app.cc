// SPDX-License-Identifier: MIT

#include "app.h"

#include <alpm.h>
#include <alpm_list.h>
#include <sys/types.h>

#include <cstdlib>
#include <ctime>
#include <print>
#include <span>
#include <string_view>
#include <vector>

#include "alpm.h"
#include "alpm_package.h"
#include "argument_parser.h"
#include "operation.h"
#include "query_handler.h"
#include "settings.h"
#include "src/operation_handler.h"
#include "src/settings.h.in"
#include "src/version_handler.h"

namespace pacmanpp {

App::App(std::span<char *> args) {
  auto arg_parser = ArgumentParser{static_cast<int>(args.size()), args.data()};
  arg_parser.ParseArgs(operation_, query_options_, targets_, config_);

  if (operation_ == Operation::kQuery) {
    alpm_ = std::make_unique<Alpm>(config_.get_root(), config_.get_db_path());
  }
}

int App::Run() {
  if (config_.IsVerbose()) {
    PrintVerbose();
  }

  switch (operation_) {
    case Operation::kNone:
      std::println("Error: no operation specified (use -h for help)");
      return EXIT_FAILURE;
    case Operation::kHelp:
      PrintHelp();
      return EXIT_SUCCESS;
    case Operation::kQuery: {
      return ExecuteOperation<QueryHandler>(query_options_, targets_);
    }
    case Operation::kVersion:
      return ExecuteOperation<VersionHandler>();
  }
}

void App::PrintVerbose() const {
  std::println("Root      : {}", config_.get_root());
  std::println("DB Path   : {}", config_.get_db_path());
}

void App::PrintHelp() const {
  std::println("Usage: {} <operation>", kPacmanppName);
  std::println("operations:");
  std::println("  {} {{-h, --help}}                  Show this help message",
               kPacmanppName);
  std::println("  {} {{-Q, --query}} [package(s)]    Query installed packages",
               kPacmanppName);
}

void App::PrintVersion() const {
  std::println("{} version {}", kPacmanppName, kPacmanppVersion);
}

}  // namespace pacmanpp
