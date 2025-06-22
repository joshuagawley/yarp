// SPDX-License-Identifier: MIT

#include "app.h"

#include <alpm.h>
#include <alpm_list.h>

#include <print>
#include <span>
#include <string_view>
#include <vector>

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
    // Get the entire package cache if no specific targets are given

    // NB: This list is owned by the alpm library and should not be freed
    // manually
    results = alpm_->DbGetPkgCache(local_db);
    if (results == nullptr) {
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
      std::println("{} {}", alpm_->PkgGetName(pkg), alpm_->PkgGetVersion(pkg));
    }
  }

  // Only free the list if we created it ourselves (when targets is not empty)
  if (!targets.empty()) {
    alpm_list_free(results);
  }
}

void App::PrintPkgChangelog(alpm_pkg_t *pkg) {
  void *fp = alpm_->PkgChangelogOpen(pkg);
  const char *pkg_name = alpm_->PkgGetName(pkg);

  if (fp == nullptr) {
    std::println(stderr, "No changelog available for {}", pkg_name);
    return;
  }

  std::println("Changelog for {}:", pkg_name);

  std::array<char, 1024> buffer;
  std::size_t bytes_read = 0;

  while ((bytes_read = alpm_->PkgChangelogRead(buffer.data(), buffer.size(),
                                               pkg, fp)) > 0) {
    // Use std::print to output the data
    std::print("{}", std::string_view(buffer.data(), bytes_read));
  }

  alpm_->PkgChangelogClose(pkg, fp);
  std::println("");  // Add newline at the end
}

}  // namespace pacmanpp
