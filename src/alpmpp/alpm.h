// SPDX-License-Identifier: MIT

#ifndef ALPMPP_ALPM_H_
#define ALPMPP_ALPM_H_

#include <alpm.h>

#include <filesystem>
#include <optional>

#include "package.h"

namespace alpmpp {

class Alpm {
 public:
  Alpm(std::string_view root, std::string_view dbpath);
  ~Alpm();

  [[nodiscard]] alpm_db_t *GetLocalDb() const;

  static std::vector<AlpmPackage> DbGetPkgCache(alpm_db_t *db);

  static std::optional<AlpmPackage> DbGetPkg(alpm_db_t *db,
                                             std::string_view name);

  [[nodiscard]] std::optional<AlpmPackage> LoadPkg(
      const std::filesystem::path &file_name, bool full,
      PkgValidation level) const;

  [[nodiscard]] std::string_view OptionGetRoot() const;

  [[nodiscard]] std::vector<alpm_db_t *> GetSyncDbs() const;

  [[nodiscard]] alpm_db_t *RegisterSyncDb(std::string_view name,
                                          int siglevel) const;

  [[nodiscard]] std::string_view StrError() const;

  [[nodiscard]] std::optional<AlpmPackage> SyncGetNewVersion(
      const AlpmPackage &pkg) const;

  [[nodiscard]] bool PkgShouldIgnore(const AlpmPackage &pkg) const;

  [[nodiscard]] static bool FileListContains(const std::vector<AlpmFile> &files,
                                             std::string_view path);

 private:
  alpm_handle_t *handle_;
  alpm_errno_t err = ALPM_ERR_OK;
};

}  // namespace alpmpp

#endif  // ALPMPP_ALPM_H_