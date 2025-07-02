// SPDX-License-Identifier: MIT

#ifndef ALPMPP_ALPM_H_
#define ALPMPP_ALPM_H_

#include <alpm.h>
#include <alpm_list.h>

#include <cstddef>
#include <cstring>
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

  static void *PkgChangelogOpen(alpm_pkg_t *pkg);

  static std::size_t PkgChangelogRead(void *ptr, std::size_t size,
                                      const alpm_pkg_t *pkg, void *fp);

  static int PkgChangelogClose(const alpm_pkg_t *pkg, void *fp);

  const char *OptionGetRoot();

  std::vector<alpm_db_t *> GetSyncDbs() const;

 private:
  alpm_handle_t *handle_;
  alpm_errno_t err = ALPM_ERR_OK;
};

}  // namespace alpmpp

#endif  // ALPMPP_ALPM_H_