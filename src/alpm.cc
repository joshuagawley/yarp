// SPDX-License-Identifier: MIT

#include "alpm.h"

#include <alpm.h>
#include <alpm_list.h>

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

const char *Alpm::PkgGetDesc(alpm_pkg_t *pkg) { return alpm_pkg_get_desc(pkg); }

const char *Alpm::PkgGetArch(alpm_pkg_t *pkg) { return alpm_pkg_get_arch(pkg); }

const char *Alpm::PkgGetURL(alpm_pkg_t *pkg) { return alpm_pkg_get_url(pkg); }

alpm_list_t *Alpm::PkgGetLicenses(alpm_pkg_t *pkg) {
  return alpm_pkg_get_licenses(pkg);
}

alpm_list_t *Alpm::PkgGetGroups(alpm_pkg_t *pkg) {
  return alpm_pkg_get_groups(pkg);
}

alpm_list_t *Alpm::PkgGetDepends(alpm_pkg_t *pkg) {
  return alpm_pkg_get_depends(pkg);
}

alpm_list_t *Alpm::PkgGetMakeDepends(alpm_pkg_t *pkg) {
  return alpm_pkg_get_makedepends(pkg);
}

alpm_list_t *Alpm::PkgGetCheckDepends(alpm_pkg_t *pkg) {
  return alpm_pkg_get_checkdepends(pkg);
}

alpm_list_t *Alpm::PkgGetOptDepends(alpm_pkg_t *pkg) {
  return alpm_pkg_get_optdepends(pkg);
}

alpm_list_t *Alpm::PkgGetReplaces(alpm_pkg_t *pkg) {
  return alpm_pkg_get_replaces(pkg);
}

alpm_list_t *Alpm::PkgGetConflicts(alpm_pkg_t *pkg) {
  return alpm_pkg_get_conflicts(pkg);
}

alpm_list_t *Alpm::PkgGetProvides(alpm_pkg_t *pkg) {
  return alpm_pkg_get_provides(pkg);
}

const char *Alpm::PkgGetPackager(alpm_pkg_t *pkg) {
  return alpm_pkg_get_packager(pkg);
}

alpm_time_t Alpm::PkgGetBuildDate(alpm_pkg_t *pkg) {
  return alpm_pkg_get_builddate(pkg);
}

alpm_pkgreason_t Alpm::PkgGetReason(alpm_pkg_t *pkg) {
  return alpm_pkg_get_reason(pkg);
}

alpm_time_t Alpm::PkgGetInstallDate(alpm_pkg_t *pkg) {
  return alpm_pkg_get_installdate(pkg);
}

bool Alpm::PkgHasScriptlet(alpm_pkg_t *pkg) {
  return alpm_pkg_has_scriptlet(pkg);
}

alpm_list_t *Alpm::PkgComputeRequiredBy(alpm_pkg_t *pkg) {
  return alpm_pkg_compute_requiredby(pkg);
}

alpm_list_t *Alpm::PkgComputeOptionalFor(alpm_pkg_t *pkg) {
  return alpm_pkg_compute_optionalfor(pkg);
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

const char *Alpm::DepComputeString(const alpm_depend_t *dep) {
  return alpm_dep_compute_string(dep);
}

}  // namespace pacmanpp