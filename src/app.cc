// SPDX-License-Identifier: MIT

#include "app.h"

#include <sys/types.h>

#include <cstdlib>
#include <print>
#include <span>

#include "alpmpp/alpm.h"
#include "argument_parser.h"
#include "help_handler.h"
#include "operation.h"
#include "query_handler.h"
#include "version_handler.h"

namespace pacmanpp {

App::App(std::span<char *> args) {
  auto arg_parser = ArgumentParser{static_cast<int>(args.size()), args.data()};
  arg_parser.ParseArgs(operation_, query_options_, targets_, config_);

  alpm_ =
      std::make_unique<alpmpp::Alpm>(config_.get_root(), config_.get_db_path());
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
      return ExecuteOperation<HelpHandler>();
    case Operation::kQuery:
      return ExecuteOperation<QueryHandler>(query_options_, targets_);
    case Operation::kVersion:
      return ExecuteOperation<VersionHandler>();
    default:
      return EXIT_SUCCESS;
  }
}

void App::PrintVerbose() const {
  std::println("Root      : {}", config_.get_root());
  std::println("DB Path   : {}", config_.get_db_path());
}

}  // namespace pacmanpp
