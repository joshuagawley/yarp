// SPDX-License-Identifier: MIT

#include "app.h"

#include <alpm.h>
#include <alpm_list.h>
#include <sys/types.h>

#include <print>
#include <span>
#include <sstream>
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
    } else if ((query_options_ & QueryOptions::kInfo) == QueryOptions::kInfo) {
      PrintPkgInfo(pkg);
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

void App::PrintPkgInfo(alpm_pkg_t *pkg) {
  std::stringstream ss;

  std::println(ss, "Name            : {}", alpm_->PkgGetName(pkg));
  std::println(ss, "Version         : {}", alpm_->PkgGetVersion(pkg));
  std::println(ss, "Description     : {}", alpm_->PkgGetDesc(pkg));
  std::println(ss, "Architecture    : {}", alpm_->PkgGetArch(pkg));
  std::println(ss, "URL             : {}", alpm_->PkgGetURL(pkg));

  PrintAlpmList(ss, pkg, "Licenses        : ", alpm_->PkgGetLicenses);
  PrintPkgList(ss, pkg, "Groups          : ", alpm_->PkgGetGroups);
  PrintDependsList(ss, pkg, "Provides        : ", alpm_->PkgGetProvides);
  PrintDependsList(ss, pkg, "Depends On      : ", alpm_->PkgGetDepends);
  PrintOptDependsList(ss, pkg, "Optional Deps   : ");

  PrintDependsList(ss, pkg, "Conflicts With  : ", alpm_->PkgGetConflicts);
  PrintDependsList(ss, pkg, "Replaces        : ", alpm_->PkgGetReplaces);
  PrintHumanizedSize(ss, pkg, "Installed Size  :", alpm_->PkgGetISize);
  std::println(ss, "Packager        : {}", alpm_->PkgGetPackager(pkg));
  // std::println(ss, "Build Date      : {}", alpm_->PkgGetBuildDate(pkg));
  // std::println(ss, "Install Date    : {}", alpm_->PkgGetInstallDate(pkg));

  // switch (alpm_->PkgGetReason(pkg)) {
  //   case ALPM_PKG_REASON_EXPLICIT:
  //     std::println(ss, "Install Reason  : Explicitly installed");

  //   case ALPM_PKG_REASON_DEPEND:
  //     std::println(
  //         ss,
  //         "Install Reason  : Installed as a dependency for another package");

  //   default:
  //     std::println(ss, "Install Reason  : Unknown");
  // }

  // std::println(ss, "Install Script  : {}", alpm_->PkgGetInstallScript(pkg));
  // std::println(ss, "Validated By  : {}", alpm_->PkgGetValidatedBy(pkg));

  std::println("{}", ss.str());
}

void App::PrintAlpmList(
    std::stringstream &ss, alpm_pkg_t *pkg, std::string_view prefix,
    std::function<alpm_list_t *(alpm_pkg_t *)> attribute_getter) {
  alpm_list_t *list = attribute_getter(pkg);
  if (list == nullptr) {
    std::println(ss, "{}None", prefix);
    return;
  }

  std::print(ss, "{}", prefix);
  for (alpm_list_t *item = list; item != nullptr; item = item->next) {
    const char *item_str = static_cast<const char *>(item->data);
    if (item->next != nullptr) {
      std::print(ss, "{}  ", item_str);  // Not the last item, add space
    } else {
      std::println(ss, "{}", item_str);  // Last item, no trailing space
    }
  }
}

void App::PrintPkgList(
    std::stringstream &ss, alpm_pkg_t *pkg, std::string_view prefix,
    std::function<alpm_list_t *(alpm_pkg_t *)> attribute_getter) {
  alpm_list_t *list = attribute_getter(pkg);
  if (list == nullptr) {
    std::println(ss, "{}None", prefix);
    return;
  }

  std::print(ss, "{}", prefix);
  for (alpm_list_t *item = list; item != nullptr; item = item->next) {
    alpm_pkg_t *pkg = static_cast<alpm_pkg_t *>(item->data);
    if (item->next != nullptr) {
      std::print(ss, "{}  ",
                 alpm_->PkgGetName(pkg));  // Not the last item, add space
    } else {
      std::println(ss, "{}",
                   alpm_->PkgGetName(pkg));  // Last item, no trailing space
    }
  }
}

void App::PrintDependsList(
    std::stringstream &ss, alpm_pkg_t *pkg, std::string_view prefix,
    std::function<alpm_list_t *(alpm_pkg_t *)> attribute_getter) {
  alpm_list_t *list = attribute_getter(pkg);
  if (list == nullptr) {
    std::println(ss, "{}None", prefix);
    return;
  }

  std::print(ss, "{}", prefix);
  for (alpm_list_t *item = list; item != nullptr; item = item->next) {
    alpm_depend_t *pkg = static_cast<alpm_depend_t *>(item->data);
    if (item->next != nullptr) {
      std::print(ss, "{}  ",
                 pkg->name);  // Not the last item, add space
    } else {
      std::println(ss, "{}",
                   pkg->name);  // Last item, no trailing space
    }
  }
}

void App::PrintOptDependsList(std::stringstream &ss, alpm_pkg_t *pkg,
                              std::string_view prefix) {
  alpm_list_t *list = alpm_->PkgGetOptDepends(pkg);
  if (list == nullptr) {
    std::println(ss, "{}None", prefix);
    return;
  }

  std::print(ss, "{}", prefix);
  for (alpm_list_t *item = list; item != nullptr; item = item->next) {
    alpm_depend_t *opt_dep = static_cast<alpm_depend_t *>(item->data);
    const char *dep_string = alpm_->DepComputeString(opt_dep);
    if (item->next != nullptr) {
      std::print(ss, "{}  ",
                 dep_string);  // Not the last item, add space
    } else {
      std::println(ss, "{}",
                   dep_string);  // Last item, no trailing space
    }
  }
}

void App::PrintHumanizedSize(std::stringstream &ss, alpm_pkg_t *pkg,
                             std::string_view prefix,
                             std::function<off_t(alpm_pkg_t *)> size_getter) {
  constexpr std::array<const char *, 6> units{"B",   "KiB", "MiB",
                                              "GiB", "TiB", "PiB"};
  double size_d = static_cast<double>(size_getter(pkg));
  int i = 0;

  while (size_d >= 1024.0 && i < 5) {
    size_d /= 1024.0;
    ++i;
  }

  std::print(ss, "{}", prefix);
  if (i == 0) {
    std::println(ss, "{:.0f} {}", size_d, units[i]);
  } else {
    std::println(ss, "{:.2f} {}", size_d, units[i]);
  }
}

}  // namespace pacmanpp
