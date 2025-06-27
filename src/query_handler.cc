// SPDX-License-Identifier: MIT

#include "query_handler.h"

#include <cstdlib>
#include <filesystem>
#include <print>

#include "alpmpp/alpm_package.h"
#include "operation.h"

namespace {

void PrintPkgChangelog(const alpmpp::AlpmPackage &pkg) {
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

void PrintPkgInfo(const alpmpp::AlpmPackage &pkg) {
  std::println("{}", pkg.GetInfo());
}

}  // namespace

namespace pacmanpp {

int QueryHandler::Execute() {
  if ((options_ & QueryOptions::kGroups) == QueryOptions::kGroups) {
    return HandleGroups();
  }

  std::vector<alpmpp::AlpmPackage> pkg_list = GetPkgList();
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
      std::println("{} {}", pkg.GetName(), pkg.GetVersion());
    }
  }

  return EXIT_SUCCESS;
}

std::vector<alpmpp::AlpmPackage> QueryHandler::GetPkgList() const {
  std::vector<alpmpp::AlpmPackage> pkg_list;

  const bool kOnlyDeps =
      (options_ & QueryOptions::kDeps) == QueryOptions::kDeps;
  const bool kOnlyExplicit =
      (options_ & QueryOptions::kExplicit) == QueryOptions::kExplicit;

  if (targets_.empty()) {
    // Get the entire package cache if no specific targets are given

    // NB: This list is owned by the alpm library and should not be freed
    // manually
    const alpm_list_t *tmp_results = alpm_.DbGetPkgCache(local_db_);
    if (tmp_results == nullptr) {
      throw std::runtime_error("Error: no packages found in local database");
    }
    for (const alpm_list_t *item = tmp_results; item != nullptr;
         item = item->next) {
      alpmpp::AlpmPackage pkg{static_cast<alpm_pkg_t *>(item->data)};
      if ((kOnlyDeps && pkg.GetReason() != ALPM_PKG_REASON_DEPEND) ||
          (kOnlyExplicit && pkg.GetReason() != ALPM_PKG_REASON_EXPLICIT)) {
        continue;
      }

      pkg_list.push_back(pkg);
    }
  } else {
    for (std::string_view target : targets_) {
      std::optional<alpmpp::AlpmPackage> pkg =
          alpm_.DbGetPkg(local_db_, target);
      if (!pkg.has_value()) {
        std::println("Error: package {} not found", target);
        continue;
      }
      if ((kOnlyDeps && (*pkg).GetReason() != ALPM_PKG_REASON_DEPEND) ||
          (kOnlyExplicit && (*pkg).GetReason() != ALPM_PKG_REASON_EXPLICIT)) {
        continue;
      }
      pkg_list.push_back(std::move(*pkg));
    }
  }

  return pkg_list;
}

int QueryHandler::HandleGroups() const {
  if (targets_.empty()) {
    alpm_list_t *all_groups = alpm_db_get_groupcache(local_db_);
    for (alpm_list_t *elem = all_groups; elem != nullptr; elem = elem->next) {
      alpm_group_t *group = static_cast<alpm_group_t *>(elem->data);

      for (const alpm_list_t *pkgs = group->packages; pkgs != nullptr;
           pkgs = pkgs->next) {
        alpmpp::AlpmPackage pkg =
            alpmpp::AlpmPackage{static_cast<alpm_pkg_t *>(pkgs->data)};
        std::println("{} {}", group->name, pkg.GetName());
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
        alpmpp::AlpmPackage pkg =
            alpmpp::AlpmPackage{static_cast<alpm_pkg_t *>(pkgs->data)};
        std::println("{} {}", group->name, pkg.GetName());
      }
    }
  }
  return EXIT_SUCCESS;
}

void QueryHandler::CheckPkgFiles(const alpmpp::AlpmPackage &pkg) const {
  int errors{};
  const alpm_filelist_t *files = pkg.GetFiles();
  const std::string_view root = alpm_.OptionGetRoot();

  for (std::size_t i = 0; i < files->count; ++i) {
    const alpm_file_t file = files->files[i];
    // TODO: see if we can avoid creating a new string here
    const std::string absolute_file_name = std::format("{}{}", root, file.name);
    if (std::filesystem::exists(absolute_file_name)) {
      const bool expect_dir = *(std::end(absolute_file_name) - 1) == '/';
      const bool is_dir = std::filesystem::is_directory(absolute_file_name);
      if (expect_dir != is_dir) {
        std::println("{}: {} (File type mismatch)", pkg.GetName(),
                     absolute_file_name);
        ++errors;
      }
    } else {
      ++errors;
    }
  }

  std::println("{}: {} total files, {} missing files", pkg.GetName(),
               files->count, errors);
}

void QueryHandler::PrintPkgFileList(const alpmpp::AlpmPackage &pkg) const {
  std::println("{}", pkg.GetFileList(alpm_.OptionGetRoot()));
}

}  // namespace pacmanpp
