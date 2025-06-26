// SPDX-License-Identifier: MIT

#include "alpm.h"

#include <alpm.h>
#include <alpm_list.h>

#include <format>
#include <stdexcept>

#include "alpm_package.h"

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

std::optional<AlpmPackage> Alpm::DbGetPkg(alpm_db_t *db,
                                          std::string_view name) {
  alpm_pkg_t *pkg = alpm_db_get_pkg(db, name.data());
  return pkg != nullptr ? std::make_optional<AlpmPackage>(pkg) : std::nullopt;
}

const char *Alpm::DepComputeString(const alpm_depend_t *dep) {
  return alpm_dep_compute_string(dep);
}

const char *Alpm::OptionGetRoot() { return alpm_option_get_root(handle_); }

}  // namespace pacmanpp