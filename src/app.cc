// SPDX-License-Identifier: MIT

#include "app.h"

#include <alpm.h>

#include <print>
#include <span>

#include "argument_parser.h"
#include "operation.h"

namespace pacmanpp {

App::App(std::span<char *> args) : program_name_(*args.cbegin()) {
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
    case Operation::kQuery:
      HandleQuery(targets_);
      return EXIT_SUCCESS;
  }
  return EXIT_SUCCESS;
}

void App::PrintVerbose() const {
  std::println("Root      : {}", config_.get_root());
  std::println("DB Path   : {}", config_.get_db_path());
}

void App::PrintHelp() const {
  std::println("Usage: {} <operation>", program_name_);
  std::println("operations:");
  std::println(
      "  pacmanpp {{-h, --help}}                  Show this help message");
  std::println(
      "  pacmanpp {{-Q, --query}} [package(s)]    Query installed packages");
}

void App::HandleQuery(const std::vector<std::string> &targets) {
  alpm_db_t *local_db = alpm_->GetLocalDb();

  alpm_list_t *results = nullptr;
  if (targets.empty()) {
    results = alpm_->DbGetPkgCache(local_db);
    if (results == nullptr) {
      alpm_list_free(results);
      throw std::runtime_error("Error: no packages found in local database");
    }
  } else {
    for (std::string_view target : targets) {
      alpm_pkg_t *pkg = alpm_->DbGetPkg(local_db, target);
      if (pkg == nullptr) {
        std::println("Error: package {} not found", target);
        continue;
      }
      results = alpm_list_add(results, pkg);
    }
  }

  for (alpm_list_t *item = results; item != nullptr; item = item->next) {
    alpm_pkg_t *pkg = static_cast<alpm_pkg_t *>(item->data);

    if ((query_options_ & QueryOptions::kChangelog) ==
        QueryOptions::kChangelog) {
      PrintPkgChangelog(pkg);
    } else {
      std::println("{} {}", alpm_pkg_get_name(pkg), alpm_pkg_get_version(pkg));
    }
  }

  alpm_list_free(results);
}

void App::PrintPkgChangelog(alpm_pkg_t *pkg) {
  void *fp = nullptr;
  const char *pkg_name = alpm_->PkgGetName(pkg);

  if ((fp = alpm_->PkgChangelogOpen(pkg)) == nullptr) {
    std::println(stderr, "No changelog available for {}", pkg_name);
    return;
  }

  std::println("Changelog for {}", pkg_name);
  char buf[1024];
  std::size_t ret = 0;
  while ((ret = alpm_->PkgChangelogRead(buf, 1024, pkg, fp))) {
    fwrite(buf, 1, ret, stdout);
  }
  alpm_->PkgChangelogClose(pkg, fp);
  putchar('\n');
}

}  // namespace pacmanpp
