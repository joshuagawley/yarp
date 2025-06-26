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
#include "src/settings.h.in"

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
      QueryHandler query_handler{config_, *alpm_, query_options_, targets_};
      return query_handler.Execute();
    }
    case Operation::kVersion:
      PrintVersion();
      return EXIT_SUCCESS;
  }
  return EXIT_SUCCESS;
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

void App::HandleQuery(const std::vector<std::string> &targets) {
  alpm_db_t *local_db = alpm_->GetLocalDb();
  std::vector<AlpmPackage> results;

  // TODO: refactor this mess
  if ((query_options_ & QueryOptions::kGroups) == QueryOptions::kGroups) {
    if (targets.empty()) {
      alpm_list_t *all_groups = alpm_db_get_groupcache(local_db);
      for (alpm_list_t *elem = all_groups; elem != nullptr; elem = elem->next) {
        alpm_group_t *group = static_cast<alpm_group_t *>(elem->data);

        for (const alpm_list_t *pkgs = group->packages; pkgs != nullptr;
             pkgs = pkgs->next) {
          AlpmPackage pkg = AlpmPackage{static_cast<alpm_pkg_t *>(pkgs->data)};
          std::println("{} {}", group->name, pkg.GetName());
        }
      }
    } else {
      for (std::string_view target : targets) {
        alpm_group_t *group = alpm_db_get_group(local_db, target.data());
        if (group == nullptr) {
          std::println(stderr, "Error: group '{}' was not found", target);
          return;
        }

        for (const alpm_list_t *pkgs = group->packages; pkgs != nullptr;
             pkgs = pkgs->next) {
          AlpmPackage pkg = AlpmPackage{static_cast<alpm_pkg_t *>(pkgs->data)};
          std::println("{} {}", group->name, pkg.GetName());
        }
      }
    }
    return;
  }

  if (targets.empty()) {
    // Get the entire package cache if no specific targets are given

    // NB: This list is owned by the alpm library and should not be freed
    // manually
    const alpm_list_t *tmp_results = alpm_->DbGetPkgCache(local_db);
    if (tmp_results == nullptr) {
      throw std::runtime_error("Error: no packages found in local database");
    }
    for (const alpm_list_t *item = tmp_results; item != nullptr;
         item = item->next) {
      AlpmPackage pkg = AlpmPackage{static_cast<alpm_pkg_t *>(item->data)};
      results.push_back(pkg);
    }
  } else {
    for (std::string_view target : targets) {
      std::optional<AlpmPackage> pkg = alpm_->DbGetPkg(local_db, target);
      if (!pkg.has_value()) {
        std::println("Error: package {} not found", target);
        continue;
      }
      results.push_back(pkg.value());
    }
  }

  for (const AlpmPackage &pkg : results) {
    if ((query_options_ & QueryOptions::kChangelog) ==
        QueryOptions::kChangelog) {
      PrintPkgChangelog(pkg);
    } else if ((query_options_ & QueryOptions::kList) == QueryOptions::kList) {
      PrintPkgFileList(pkg);
    } else if ((query_options_ & QueryOptions::kInfo) == QueryOptions::kInfo) {
      PrintPkgInfo(pkg);
    } else {
      std::println("{} {}", pkg.GetName(), pkg.GetVersion());
    }
  }
}

void App::PrintPkgChangelog(const AlpmPackage &pkg) const {
  void *fp = pkg.ChangelogOpen();
  const std::string_view pkg_name = pkg.GetName();

  if (fp == nullptr) {
    std::println(stderr, "No changelog available for {}", pkg_name);
    return;
  }

  std::println("Changelog for {}:", pkg_name);

  std::array<char, 1024> buffer;
  std::size_t bytes_read = 0;

  while ((bytes_read = pkg.ChangelogRead(fp, buffer.data(), buffer.size())) >
         0) {
    // Use std::print to output the data
    std::print("{}", std::string_view(buffer.data(), bytes_read));
  }

  const int result = pkg.ChangelogClose(fp);
  if (!result) {
    std::println("Error: could not close changelog.");
  }
  std::println("");  // Add newline at the end
}

void App::PrintPkgFileList(const AlpmPackage &pkg) const {
  std::println("{}", pkg.GetFileList(alpm_->OptionGetRoot()));
}

void App::PrintPkgInfo(const AlpmPackage &pkg) const {
  std::println("{}", pkg.GetInfo());
}

void App::PrintVersion() const {
  std::println("{} version {}", kPacmanppName, kPacmanppVersion);
}

}  // namespace pacmanpp
