// SPDX-License-Identifier: MIT

#include "query_handler.h"

#include <alpm.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <iostream>
#include <print>
#include <ranges>

#include "alpmpp/file.h"
#include "alpmpp/package.h"
#include "alpmpp/types.h"
#include "alpmpp/util.h"
#include "operation.h"

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

std::optional<std::filesystem::path> GetFromPath(
    const std::filesystem::path &file_name) {
  const std::string_view env_path = std::getenv("PATH");
  if (env_path.empty()) return std::nullopt;

  auto valid_paths = env_path | std::views::split(':') |
                     std::views::transform([&file_name](auto &&range) {
                       std::string_view dir(std::begin(range), std::end(range));
                       // Remove trailing slashes
                       while (!dir.empty() && dir.back() == '/') {
                         dir.remove_suffix(1);
                       }
                       return std::filesystem::path(dir) / file_name;
                     }) |
                     std::views::filter([](const std::filesystem::path &path) {
                       std::error_code ec;
                       return std::filesystem::exists(path, ec) && !ec;
                     });
  const auto it = std::begin(valid_paths);
  return it != std::end(valid_paths) ? std::optional(*it) : std::nullopt;
}

}  // namespace

namespace pacmanpp {

int QueryHandler::Execute() {
  // We check if we're querying for groups before checking for empty targets
  // because if no targets are given when querying for groups, we simply print
  // all installed groups
  if ((options_ & QueryOptions::kGroups) == QueryOptions::kGroups) {
    return HandleGroups();
  } else if (targets_.empty()) {
    std::println(stderr, "Error: no targets specified (use -h for help)");
    return EXIT_FAILURE;
  } else if ((options_ & QueryOptions::kOwns) == QueryOptions::kOwns) {
    return HandleOwns();
  } else {
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

          std::print(" -> {}", new_pkg.version());

          if (alpm_->PkgShouldIgnore(new_pkg) ||
              !(usage & ALPM_DB_USAGE_UPGRADE)) {
            std::print(" [ignored]");
          }
        } else {
          std::println();
        }
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

int QueryHandler::HandleOwns() const {
  std::filesystem::path root_dir = alpm_->OptionGetRoot();
  const std::vector<alpmpp::AlpmPackage> pkg_list =
      alpmpp::Alpm::DbGetPkgCache(local_db_);
  std::error_code ec;
  bool found = false;

  for (const std::string_view target : targets_) {
    std::filesystem::path path{target};
    if (path.empty()) {
      std::println(stderr, "Error: empty string passed into file owner query");
      return EXIT_FAILURE;
    }

    if (!std::filesystem::exists(std::filesystem::symlink_status(path, ec))) {
      const std::optional<std::filesystem::path> resolved_path =
          GetFromPath(path);
      if (!resolved_path.has_value()) {
        std::println(stderr, "Error: Could not find {} in PATH", target);
        return EXIT_FAILURE;
      }
      path = resolved_path.value();
    }

    const std::filesystem::path canonicalized =
        std::filesystem::weakly_canonical(path, ec);
    if (ec) return ec.value();

    // Alpm needs the relative path from the root dir
    const std::filesystem::path relative_path =
        std::filesystem::relative(canonicalized, root_dir);

    for (const alpmpp::AlpmPackage &pkg : pkg_list) {
      if (alpmpp::Alpm::FileListContains(pkg.files(), relative_path.c_str())) {
        std::println("{} is owned by {} {}", target, pkg.name(), pkg.version());
        found = true;
      }
    }
    if (!found) {
      std::println(stderr, "No package owns {}", target);
      return 1;
    } else {
      return 0;
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
