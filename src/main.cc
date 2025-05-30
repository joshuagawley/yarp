// SPDX-License-Identifier: MIT

#include <alpm_list.h>

#include <cstdlib>
#include <print>

#include "alpm.h"
#include "argument_parser.h"
#include "config.h"

void HandleQuery(pacmanpp::Alpm &alpm,
                 const std::vector<std::string> &targets) {
  alpm_db_t *local_db = alpm.GetLocalDb();

  alpm_list_t *results = nullptr;
  if (targets.empty()) {
    results = alpm.DbGetPkgCache(local_db);
    if (results == nullptr) {
      alpm_list_free(results);
      throw std::runtime_error("Error: no packages found in local database");
    }
  } else {
    for (std::string_view target : targets) {
      alpm_pkg_t *pkg = alpm.DbGetPkg(local_db, target);
      if (pkg == nullptr) {
        std::println("Error: package {} not found", target);
        continue;
      }
      results = alpm_list_add(results, pkg);
    }
  }

  for (alpm_list_t *item = results; item != nullptr; item = item->next) {
    alpm_pkg_t *pkg = static_cast<alpm_pkg_t *>(item->data);
    std::println("{} {}", alpm_pkg_get_name(pkg), alpm_pkg_get_version(pkg));
  }

  alpm_list_free(results);
}

int main(int argc, char **argv) {
  auto config = pacmanpp::Config::get();
  pacmanpp::Alpm alpm(config.get_root(), config.get_db_path());
  auto operation = pacmanpp::Operation::kNone;
  std::vector<std::string> targets;
  pacmanpp::ArgumentParser arg_parser(argc, argv);
  arg_parser.ParseArgs(operation, targets);

  switch (operation) {
    case pacmanpp::Operation::kNone:
      std::println("Error: no operation specified (use -h for help)");
      return EXIT_FAILURE;
    case pacmanpp::Operation::kHelp:
      std::println("Usage: {} <operation>", argv[0]);
      std::println("operations:");
      std::println(
          "  pacmanpp {{-h, --help}}                  Show this help message");
      std::println(
          "  pacmanpp {{-Q, --query}} [package(s)]    Query installed "
          "packages");
      return EXIT_SUCCESS;
    case pacmanpp::Operation::kQuery:
      HandleQuery(alpm, targets);
      break;
  }

  return EXIT_SUCCESS;
}
