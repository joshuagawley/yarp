// SPDX-License-Identifier: MIT

#include "alpm.h"

#include <format>
#include <stdexcept>

namespace pacmanpp {

Alpm::Alpm(std::string_view root, std::string_view dbpath) {
  handle_ = alpm_initialize(root.data(), dbpath.data(), &err);
  if (err != ALPM_ERR_OK) {
    throw std::runtime_error(std::format("Error: failed to initialize alpm: {}",
                                         alpm_strerror(err)));
  }
}

Alpm::~Alpm() { alpm_release(handle_); }

alpm_db_t *Alpm::GetLocalDb() const {
  alpm_db_t *db = alpm_get_localdb(handle_);
  if (db == nullptr) {
    throw std::runtime_error("Error: failed to get local database");
  }
  return db;
}

alpm_list_t *Alpm::DbGetPkgCache(alpm_db_t *db) {
  return alpm_db_get_pkgcache(db);
}

alpm_pkg_t *Alpm::DbGetPkg(alpm_db_t *db, std::string_view name) {
  return alpm_db_get_pkg(db, name.data());
}

const char *Alpm::PkgGetName(alpm_pkg_t *pkg) { return alpm_pkg_get_name(pkg); }

const char *Alpm::PkgGetVersion(alpm_pkg_t *pkg) {
  return alpm_pkg_get_version(pkg);
}

void *Alpm::PkgChangelogOpen(alpm_pkg_t *pkg) {
  return alpm_pkg_changelog_open(pkg);
}

std::size_t Alpm::PkgChangelogRead(void *ptr, std::size_t size,
                                   const alpm_pkg_t *pkg, void *fp) {
  return alpm_pkg_changelog_read(ptr, size, pkg, fp);
}

int Alpm::PkgChangelogClose(const alpm_pkg_t *pkg, void *fp) {
  return alpm_pkg_changelog_close(pkg, fp);
}

}  // namespace pacmanpp