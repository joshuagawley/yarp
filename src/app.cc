// SPDX-License-Identifier: MIT

#include "app.h"

#include <sys/types.h>

#include <cstdlib>
#include <print>
#include <span>

#include "alpmpp/alpm.h"
#include "alpmpp/util.h"
#include "argument_parser.h"
#include "help_handler.h"
#include "noop_handler.h"
#include "operation.h"
#include "query_handler.h"
#include "version_handler.h"

namespace pacmanpp {

using OperationHandler =
    std::variant<NoOpHandler, HelpHandler, QueryHandler, VersionHandler>;

App::App(std::span<char *> args) {
  const auto arg_parser =
      ArgumentParser{static_cast<int>(args.size()), args.data()};
  arg_parser.ParseArgs(operation_, query_options_, targets_, config_);

  alpm_ = std::make_unique<alpmpp::Alpm>(config_.root_dir(), config_.db_path());

  // Register sync databases
  for (const Repository &repo : config_.repos()) {
    alpm_db_t *db =
        alpm_->RegisterSyncDb(repo.name, std::to_underlying(repo.sig_level));
    if (db == nullptr) {
      throw std::runtime_error(
          std::format("Could not register db, name : {}", repo.name));
    }
  }
}

int App::Run() {
  if (config_.verbose()) {
    PrintVerbose();
  }

  OperationHandler handler;

  switch (operation_) {
    case Operation::kNone:
      handler = NoOpHandler{};
      break;
    case Operation::kHelp:
      handler = HelpHandler{};
      break;
    case Operation::kQuery:
      handler = QueryHandler{alpm_.get(), &config_, query_options_,
                             std::move(targets_)};
      break;
    case Operation::kVersion:
      handler = VersionHandler{};
      break;
    default:
      return EXIT_SUCCESS;
  }

  return std::visit([](auto &h) { return h.Execute(); }, handler);
}

void App::PrintVerbose() const {
  std::stringstream ss;
  std::println(ss, "Root       : {}", config_.root_dir());
  std::println(ss, "DB Path    : {}", config_.db_path());

  alpmpp::util::PrintStringVector(ss, "Cache Dirs : ", config_.cache_dirs());
  alpmpp::util::PrintStringVector(ss, "Hook Dirs  : ", config_.hook_dirs());
  // std::println("Lock File  : {}", config_.lock_file());
  std::println(ss, "Log File   : {}", config_.log_file());
  std::println(ss, "GPG Dir    : {}", config_.gpg_dir());
  alpmpp::util::PrintStringVector(ss, "Targets    : ", targets_);

  std::print("{}", ss.str());
}

}  // namespace pacmanpp
