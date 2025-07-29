// SPDX-License-Identifier: MIT

#include "query_handler.h"

#include <alpm.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <print>

#include "alpmpp/file.h"
#include "alpmpp/types.h"
#include "operation.h"
#include "alpmpp/package.h"

namespace {

void PrintPkgChangelog(const alpmpp::AlpmPackage &pkg) {
  void *fp = pkg.ChangelogOpen();
  const std::string_view pkg_name = pkg.name();

  if (fp == nullptr) {
    std::println(stderr, "No changelog available for {}", pkg_name);
  } else {
    std::println("Changelog for {}:", pkg_name);

    std::array<char, 1024> buffer{};
    std::size_t bytes_read = 0;

    while ((bytes_read = pkg.ChangelogRead(fp, buffer.data(), buffer.size())) >
           0) {
      std::print("{}", std::string_view(buffer.data(), bytes_read));
    }

    if (const int result = pkg.ChangelogClose(fp); !result) {
      std::println("Error: could not close changelog.");
    }
    std::println("");
  }
}

void PrintPkgInfo(const alpmpp::AlpmPackage &pkg) {
  std::println("{}", pkg.GetInfo());
}

bool IsUnrequired(const alpmpp::AlpmPackage &pkg) {
  std::vector<std::string> required_by = pkg.ComputeRequiredBy();
  return required_by.empty();
}

}  // namespace

namespace pacmanpp {

int QueryHandler::Execute() {
  if ((options_ & QueryOptions::kGroups) == QueryOptions::kGroups) {
    return HandleGroups();
  }

  const std::vector<alpmpp::AlpmPackage> pkg_list = GetPkgList();
  if (pkg_list.empty()) return EXIT_FAILURE;

  for (const alpmpp::AlpmPackage &pkg : pkg_list) {
    if ((options_ & QueryOptions::kChangelog) == QueryOptions::kChangelog) {
      PrintPkgChangelog(pkg);
    } else if ((options_ & QueryOptions::kList) == QueryOptions::kList) {
      PrintPkgFileList(pkg);
    } else if ((options_ & QueryOptions::kInfo) == QueryOptions::kInfo) {
      PrintPkgInfo(pkg);
    } else if ((options_ & QueryOptions::kCheck) == QueryOptions::kCheck) {
      CheckPkgFiles(pkg);
    } else {
      std::print("{} {}", pkg.name(), pkg.version());

      if ((options_ & QueryOptions::kUpgrade) == QueryOptions::kUpgrade) {
        int usage;
        alpmpp::AlpmPackage new_pkg = alpm_->SyncGetNewVersion(pkg).value();
        alpm_db_t *db = new_pkg.GetDb();
        alpm_db_get_usage(db, &usage);

        std::print("-> {}", new_pkg.version());

        if (alpm_->PkgShouldIgnore(new_pkg) || !(usage & ALPM_DB_USAGE_UPGRADE)) {
          std::print(" [ignored]");
        }
      } else {
        std::println();
      }
    }
  }

  return EXIT_SUCCESS;
}

std::vector<alpmpp::AlpmPackage> QueryHandler::GetPkgList() const {
  std::vector<alpmpp::AlpmPackage> pkg_list{};

  if ((options_ & QueryOptions::kIsFile) == QueryOptions::kIsFile) {
    for (const std::string_view path : targets_) {
      std::optional<alpmpp::AlpmPackage> pkg =
          alpm_->LoadPkg(path, true, alpmpp::PkgValidation::kUnknown);
      if (pkg.has_value()) {
        pkg_list.push_back(std::move(*pkg));
      } else {
        std::println("Error: Could not load package {}: {}", path,
                     alpm_->StrError());
      }
    }
  } else {
    if (targets_.empty()) {
      // Get the entire package cache if no specific targets are given

      // NB: This list is owned by the alpm library and should not be freed
      // manually
      pkg_list = alpmpp::Alpm::DbGetPkgCache(local_db_);
    } else {
      for (const std::string_view target : targets_) {
        std::optional<alpmpp::AlpmPackage> pkg =
            alpmpp::Alpm::DbGetPkg(local_db_, target);
        if (pkg.has_value()) {
          pkg_list.push_back(std::move(*pkg));
        } else {
          std::println("Error: package {} not found", target);
        }
      }
    }

    std::erase_if(pkg_list, std::bind_front(&QueryHandler::FilterPkg, this));
  }
  return pkg_list;
}

int QueryHandler::HandleGroups() const {
  if (targets_.empty()) {
    const alpm_list_t *all_groups = alpm_db_get_groupcache(local_db_);
    for (const alpm_list_t *elem = all_groups; elem != nullptr;
         elem = elem->next) {
      auto *group = static_cast<alpm_group_t *>(elem->data);

      for (const alpm_list_t *pkgs = group->packages; pkgs != nullptr;
           pkgs = pkgs->next) {
        alpmpp::AlpmPackage pkg{static_cast<alpm_pkg_t *>(pkgs->data)};
        std::println("{} {}", group->name, pkg.name());
      }
    }
  } else {
    for (std::string_view target : targets_) {
      alpm_group_t *group = alpm_db_get_group(local_db_, target.data());
      if (group == nullptr) {
        std::println(stderr, "Error: group '{}' was not found", target);
        return EXIT_SUCCESS;
      }

      for (const alpm_list_t *pkgs = group->packages; pkgs != nullptr;
           pkgs = pkgs->next) {
        alpmpp::AlpmPackage pkg{static_cast<alpm_pkg_t *>(pkgs->data)};
        std::println("{} {}", group->name, pkg.name());
      }
    }
  }
  return EXIT_SUCCESS;
}

void QueryHandler::CheckPkgFiles(const alpmpp::AlpmPackage &pkg) const {
  int errors{};
  const std::vector<alpmpp::AlpmFile> files = pkg.files();
  const std::string_view root = alpm_->OptionGetRoot();

  for (const alpmpp::AlpmFile &file : files) {
    // TODO: see if we can avoid creating a new string here
    const std::string absolute_file_name =
        std::format("{}{}", root, file.name());
    if (std::filesystem::exists(absolute_file_name)) {
      const bool expect_dir = absolute_file_name.back() == '/';
      const bool is_dir = std::filesystem::is_directory(absolute_file_name);
      if (expect_dir != is_dir) {
        std::println("{}: {} (File type mismatch)", pkg.name(),
                     absolute_file_name);
        ++errors;
      }
    } else {
      ++errors;
    }
  }

  std::println("{}: {} total files, {} missing files", pkg.name(), files.size(),
               errors);
}

PkgLocality QueryHandler::GetPkgLocality(const alpmpp::AlpmPackage &pkg) const {
  const std::vector<alpm_db_t *> sync_dbs = alpm_->GetSyncDbs();
  const std::string_view pkg_name = pkg.name();

  const bool pkg_in_sync_db =
      std::ranges::any_of(sync_dbs, [pkg_name](alpm_db_t *db) {
        return alpmpp::Alpm::DbGetPkg(db, pkg_name).has_value();
      });

  return pkg_in_sync_db ? PkgLocality::kNative : PkgLocality::kForeign;
}

void QueryHandler::PrintPkgFileList(const alpmpp::AlpmPackage &pkg) const {
  std::println("{}", pkg.GetFileList(alpm_->OptionGetRoot()));
}

bool QueryHandler::IsUpgradable(const alpmpp::AlpmPackage &pkg) const {
  return alpm_->SyncGetNewVersion(pkg).has_value();
}

bool QueryHandler::FilterPkg(const alpmpp::AlpmPackage &pkg) const {
  const bool kOnlyDeps =
      (options_ & QueryOptions::kDeps) == QueryOptions::kDeps;
  const bool kOnlyExplicit =
      (options_ & QueryOptions::kExplicit) == QueryOptions::kExplicit;
  const bool kOnlyNative =
      (options_ & QueryOptions::kNative) == QueryOptions::kNative;
  const bool kOnlyForeign =
      (options_ & QueryOptions::kForeign) == QueryOptions::kForeign;
  const bool kOnlyUnrequired =
      (options_ & QueryOptions::kUnrequired) == QueryOptions::kUnrequired;
  const bool kOnlyUpgrade =
      (options_ & QueryOptions::kUpgrade) == QueryOptions::kUpgrade;

  return (kOnlyDeps && pkg.reason() != alpmpp::PkgReason::kDepend) ||
         (kOnlyExplicit && pkg.reason() != alpmpp::PkgReason::kExplicit) ||
         (kOnlyForeign && GetPkgLocality(pkg) != PkgLocality::kForeign) ||
         (kOnlyNative && GetPkgLocality(pkg) != PkgLocality::kNative) ||
         (kOnlyUnrequired && !IsUnrequired(pkg)) ||
         (kOnlyUpgrade && !IsUpgradable(pkg));
}

}  // namespace pacmanpp
