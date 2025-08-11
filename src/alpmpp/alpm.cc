// SPDX-License-Identifier: MIT

#include "alpm.h"

#include <alpm.h>
#include <alpm_list.h>

#include <algorithm>
#include <format>
#include <stdexcept>
#include <utility>

#include "package.h"
#include "util.h"

namespace alpmpp {

Alpm::Alpm(const std::string_view root, const std::string_view dbpath) {
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
                                          const std::string_view name) {
  alpm_pkg_t *pkg = alpm_db_get_pkg(db, name.data());
  return pkg != nullptr ? std::make_optional<AlpmPackage>(pkg) : std::nullopt;
}

std::string_view Alpm::OptionGetRoot() const {
  return alpm_option_get_root(handle_);
}

std::vector<alpm_db_t *> Alpm::GetSyncDbs() const {
  const alpm_list_t *list = alpm_get_syncdbs(handle_);
  std::vector<alpm_db_t *> result;
  result.reserve(alpm_list_count(list));

  for (const alpm_list_t *elem = list; elem != nullptr;
       elem = alpm_list_next(elem)) {
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

std::optional<AlpmPackage> Alpm::SyncGetNewVersion(
    const AlpmPackage &pkg) const {
  alpm_pkg_t *new_pkg =
      alpm_sync_get_new_version(pkg.GetHandle(), alpm_get_syncdbs(handle_));
  return new_pkg == nullptr ? std::nullopt
                            : std::make_optional<AlpmPackage>(new_pkg, true);
}

bool Alpm::PkgShouldIgnore(const AlpmPackage &pkg) const {
  return alpm_pkg_should_ignore(handle_, pkg.GetHandle());
}

bool Alpm::FileListContains(const std::vector<AlpmFile> &files,
                            const std::string_view path) {
  return std::ranges::any_of(
      files, [&path](const AlpmFile &file) { return file.name() == path; });
}
}  // namespace alpmpp