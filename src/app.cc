// SPDX-License-Identifier: MIT

#include "app.h"

#include <alpm.h>
#include <alpm_list.h>
#include <sys/types.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <locale>
#include <print>
#include <span>
#include <sstream>
#include <string_view>
#include <vector>

#include "argument_parser.h"
#include "operation.h"
#include "src/alpm.h"
#include "src/alpm_package.h"

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
  std::vector<AlpmPackage> results;

  if (targets.empty()) {
    // Get the entire package cache if no specific targets are given

    // NB: This list is owned by the alpm library and should not be freed
    // manually
    alpm_list_t *tmp_results = alpm_->DbGetPkgCache(local_db);
    if (tmp_results == nullptr) {
      throw std::runtime_error("Error: no packages found in local database");
    }
    for (alpm_list_t *item = tmp_results; item != nullptr; item = item->next) {
      AlpmPackage pkg = AlpmPackage{static_cast<alpm_pkg_t *>(item->data)};
      results.push_back(pkg);
    }
  } else {
    for (std::string_view target : targets) {
      std::optional<AlpmPackage> pkg = alpm_->DbGetPkg(local_db, target);
      if (!pkg) {
        std::println("Error: package {} not found", target);
        continue;
      }
    }
  }

  for (const AlpmPackage &pkg : results) {
    if ((query_options_ & QueryOptions::kChangelog) ==
        QueryOptions::kChangelog) {
      PrintPkgChangelog(pkg);
    } else if ((query_options_ & QueryOptions::kInfo) == QueryOptions::kInfo) {
      PrintPkgInfo(pkg);
    } else {
      std::println("{} {}", pkg.GetName(), pkg.GetVersion());
    }
  }
}

void App::PrintPkgChangelog(const AlpmPackage &pkg) {
  void *fp = pkg.ChangelogOpen();
  std::string_view pkg_name = pkg.GetName();

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

  pkg.ChangelogClose(fp);
  std::println("");  // Add newline at the end
}

void App::PrintPkgInfo(const AlpmPackage &pkg) {
  std::stringstream ss;

  std::println(ss, "Name            : {}", pkg.GetName());
  std::println(ss, "Version         : {}", pkg.GetVersion());
  std::println(ss, "Description     : {}", pkg.GetDesc());
  std::println(ss, "Architecture    : {}", pkg.GetArch());
  std::println(ss, "URL             : {}", pkg.GetURL());

  PrintAlpmList(
      ss, pkg, "Licenses        : ", std::bind(&AlpmPackage::GetLicenses, pkg));
  PrintPkgList(ss, pkg,
               "Groups          : ", std::bind(&AlpmPackage::GetGroups, pkg));
  PrintDependsList(
      ss, pkg, "Provides        : ", std::bind(&AlpmPackage::GetProvides, pkg));
  PrintDependsList(
      ss, pkg, "Depends On      : ", std::bind(&AlpmPackage::GetDepends, pkg));
  PrintOptDependsList(ss, pkg, "Optional Deps   : ");
  // alpm_pkg_compute_* don't free the list, so we set free_list to true
  PrintAlpmList(ss, pkg, "Required By     : ",
                std::bind(&AlpmPackage::ComputeRequiredBy, pkg), true);
  PrintAlpmList(ss, pkg, "Optional For    : ",
                std::bind(&AlpmPackage::ComputeOptionalFor, pkg), true);

  PrintDependsList(ss, pkg, "Conflicts With  : ",
                   std::bind(&AlpmPackage::GetConflicts, pkg));
  PrintDependsList(
      ss, pkg, "Replaces        : ", std::bind(&AlpmPackage::GetReplaces, pkg));
  PrintHumanizedSize(
      ss, pkg, "Installed Size  :", std::bind(&AlpmPackage::GetISize, pkg));
  std::println(ss, "Packager        : {}", pkg.GetPackager());
  PrintHumanizedDate(ss, pkg, "Build Date      : ",
                     std::bind(&AlpmPackage::GetBuildDate, pkg));
  PrintHumanizedDate(ss, pkg, "Install Date    : ",
                     std::bind(&AlpmPackage::GetInstallDate, pkg));
  PrintInstallReason(ss, pkg.GetReason());
  PrintInstallScript(ss, pkg.HasScriptlet());
  PrintPkgValidation(ss, pkg);

  std::println("{}", ss.str());
}

void App::PrintAlpmList(
    std::stringstream &ss, const AlpmPackage &pkg, std::string_view prefix,
    std::function<alpm_list_t *(const AlpmPackage &)> attribute_getter,
    bool free_list) {
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

  if (free_list) {
    alpm_list_free_inner(list, [](void *data) {
      // Free the string data, but not the list itself
      free(data);
    });
    alpm_list_free(list);
  }
}

void App::PrintPkgList(
    std::stringstream &ss, const AlpmPackage &pkg, std::string_view prefix,
    std::function<alpm_list_t *(const AlpmPackage &)> attribute_getter) {
  alpm_list_t *list = attribute_getter(pkg);
  if (list == nullptr) {
    std::println(ss, "{}None", prefix);
    return;
  }

  std::print(ss, "{}", prefix);
  for (alpm_list_t *item = list; item != nullptr; item = item->next) {
    AlpmPackage pkg = AlpmPackage{static_cast<alpm_pkg_t *>(item->data)};
    if (item->next != nullptr) {
      std::print(ss, "{}  ",
                 pkg.GetName());  // Not the last item, add space
    } else {
      std::println(ss, "{}",
                   pkg.GetName());  // Last item, no trailing space
    }
  }
}

void App::PrintDependsList(
    std::stringstream &ss, const AlpmPackage &pkg, std::string_view prefix,
    std::function<alpm_list_t *(const AlpmPackage &)> attribute_getter) {
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

void App::PrintOptDependsList(std::stringstream &ss, const AlpmPackage &pkg,
                              std::string_view prefix) {
  alpm_list_t *list = pkg.GetOptDepends();
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

void App::PrintHumanizedSize(
    std::stringstream &ss, const AlpmPackage &pkg, std::string_view prefix,
    std::function<off_t(const AlpmPackage &)> size_getter) {
  constexpr std::array<const char *, 6> units{"B",   "KiB", "MiB",
                                              "GiB", "TiB", "PiB"};
  double size_d = static_cast<double>(size_getter(pkg));
  int i = 0;

  while (size_d >= 1024.0 && i < 5) {
    size_d /= 1024.0;
    ++i;
  }

  std::print(ss, "{} ", prefix);
  if (i == 0) {
    std::println(ss, "{:.0f} {}", size_d, units[i]);
  } else {
    std::println(ss, "{:.2f} {}", size_d, units[i]);
  }
}

void App::PrintHumanizedDate(
    std::stringstream &ss, const AlpmPackage &pkg, std::string_view prefix,
    std::function<alpm_time_t(const AlpmPackage &)> date_getter) {
  // We use C-style time API instead of std::chrono
  const std::time_t time = static_cast<std::time_t>(date_getter(pkg));
  const tm *local_time = std::localtime(&time);

  // std::put_time doesn't work with std::format
  ss << prefix << std::put_time(local_time, "%a %d %b %Y %H:%M:%S %Z") << '\n';
}

void App::PrintInstallReason(std::stringstream &ss, alpm_pkgreason_t reason) {
  switch (reason) {
    case ALPM_PKG_REASON_EXPLICIT:
      std::println(ss, "Install Reason  : Explicitly installed");
      break;
    case ALPM_PKG_REASON_DEPEND:
      std::println(ss,
                   "Install Reason  : Installed as a dependency for another "
                   "package");
      break;
    default:
      std::println(ss, "Install Reason  : Unknown");
      break;
  }
}

void App::PrintInstallScript(std::stringstream &ss, bool has_scriptlet) {
  const char *scriptlet_str = has_scriptlet ? "Yes" : "No";
  std::println(ss, "Install Script  : {}", scriptlet_str);
}

void App::PrintPkgValidation(std::stringstream &ss, const AlpmPackage &pkg) {
  const int validation = pkg.GetValidation();
  std::print(ss, "Validated By    : ");
  if (validation) {
    std::vector<std::string> validation_methods;

    if (validation & ALPM_PKG_VALIDATION_NONE) {
      std::println(ss, "None");
    } else {
      std::vector<std::string> validation_methods;

      if (validation & ALPM_PKG_VALIDATION_MD5SUM) {
        validation_methods.push_back("MD5 Sum");
      }
      if (validation & ALPM_PKG_VALIDATION_SHA256SUM) {
        validation_methods.push_back("SHA-256 Sum");
      }
      if (validation & ALPM_PKG_VALIDATION_SIGNATURE) {
        validation_methods.push_back("Signature");
      }

      for (const std::string_view method : validation_methods) {
        if (method != validation_methods.back()) {
          std::print(ss, "{}  ", method);
        }
        std::print(ss, "{}", method);
      }
    }
  } else {
    std::println(ss, "Unknown");
  }
}

}  // namespace pacmanpp
