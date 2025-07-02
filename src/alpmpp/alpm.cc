// SPDX-License-Identifier: MIT

#include "alpm.h"

#include <alpm.h>
#include <alpm_list.h>

#include <format>
#include <stdexcept>
#include <utility>

#include "package.h"
#include "util.h"

namespace alpmpp {

Alpm::Alpm(std::string_view root, std::string_view dbpath) {
  handle_ = alpm_initialize(root.data(), dbpath.data(), &err);
  if (err != ALPM_ERR_OK) {
    throw std::runtime_error(std::format("Error: failed to initialize alpm: {}",
                                         alpm_strerror(err)));
  }
}

Alpm::~Alpm() {
  alpm_unregister_all_syncdbs(handle_);
  alpm_release(handle_);
}

alpm_db_t *Alpm::GetLocalDb() const {
  alpm_db_t *db = alpm_get_localdb(handle_);
  if (db == nullptr) {
    throw std::runtime_error("Error: failed to get local database");
  }
  return db;
}

std::vector<AlpmPackage> Alpm::DbGetPkgCache(alpm_db_t *db) {
  return util::AlpmListToVector<alpm_pkg_t *, AlpmPackage>(
      alpm_db_get_pkgcache(db));
}

std::optional<AlpmPackage> Alpm::DbGetPkg(alpm_db_t *db,
                                          std::string_view name) {
  alpm_pkg_t *pkg = alpm_db_get_pkg(db, name.data());
  return pkg != nullptr ? std::make_optional<AlpmPackage>(pkg) : std::nullopt;
}

std::string_view Alpm::OptionGetRoot() const {
  return alpm_option_get_root(handle_);
}

std::vector<alpm_db_t *> Alpm::GetSyncDbs() const {
  alpm_list_t *list = alpm_get_syncdbs(handle_);
  std::vector<alpm_db_t *> result;
  result.reserve(alpm_list_count(list));

  for (alpm_list_t *elem = list; elem != nullptr; elem = alpm_list_next(elem)) {
    result.emplace_back(static_cast<alpm_db_t *>(elem->data));
  }
  return result;
}

alpm_db_t *Alpm::RegisterSyncDb(std::string_view name, int siglevel) const {
  return alpm_register_syncdb(handle_, name.data(), siglevel);
}

std::optional<AlpmPackage> Alpm::LoadPkg(const std::filesystem::path &file_name,
                                         bool full, PkgValidation level) const {
  alpm_pkg_t *pkg = nullptr;

  alpm_pkg_load(handle_, file_name.c_str(), full, std::to_underlying(level),
                &pkg);

  return pkg == nullptr ? std::nullopt
                        : std::make_optional<AlpmPackage>(pkg, true);
}

std::string_view Alpm::StrError() const { return alpm_strerror(err); }

}  // namespace alpmpp