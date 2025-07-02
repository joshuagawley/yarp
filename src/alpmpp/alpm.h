// SPDX-License-Identifier: MIT

#ifndef ALPMPP_ALPM_H_
#define ALPMPP_ALPM_H_

#include <alpm.h>
#include <alpm_list.h>

#include <cstddef>
#include <cstring>
#include <filesystem>
#include <optional>

#include "package.h"

namespace alpmpp {

class Alpm {
 public:
  Alpm(std::string_view root, std::string_view dbpath);
  ~Alpm();

  alpm_db_t *GetLocalDb() const;

  static std::vector<AlpmPackage> DbGetPkgCache(alpm_db_t *db);

  static std::optional<AlpmPackage> DbGetPkg(alpm_db_t *db,
                                             std::string_view name);

  std::optional<AlpmPackage> LoadPkg(const std::filesystem::path &file_name,
                                     bool full, PkgValidation level) const;

  std::string_view OptionGetRoot() const;

  std::vector<alpm_db_t *> GetSyncDbs() const;

  alpm_db_t *RegisterSyncDb(std::string_view name, int siglevel) const;

  std::string_view StrError() const;

 private:
  alpm_handle_t *handle_;
  alpm_errno_t err = ALPM_ERR_OK;
};

}  // namespace alpmpp

#endif  // ALPMPP_ALPM_H_