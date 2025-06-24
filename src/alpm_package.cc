// SPDX-License-Identifier: MIT

#include "alpm_package.h"

namespace pacmanpp {

std::string_view AlpmPackage::GetName() const {
  return alpm_pkg_get_name(pkg_);
}

std::string_view AlpmPackage::GetVersion() const {
  return alpm_pkg_get_version(pkg_);
}

std::string_view AlpmPackage::GetDesc() const {
  return alpm_pkg_get_desc(pkg_);
}

std::string_view AlpmPackage::GetArch() const {
  return alpm_pkg_get_arch(pkg_);
}

std::string_view AlpmPackage::GetURL() const { return alpm_pkg_get_url(pkg_); }

std::string_view AlpmPackage::GetPackager() const {
  return alpm_pkg_get_packager(pkg_);
}

alpm_list_t *AlpmPackage::GetOptDepends() const {
  return alpm_pkg_get_optdepends(pkg_);
}

alpm_list_t *AlpmPackage::GetDepends() const {
  return alpm_pkg_get_depends(pkg_);
}

alpm_list_t *AlpmPackage::GetProvides() const {
  return alpm_pkg_get_provides(pkg_);
}

alpm_list_t *AlpmPackage::GetGroups() const {
  return alpm_pkg_get_groups(pkg_);
}

alpm_list_t *AlpmPackage::GetLicenses() const {
  return alpm_pkg_get_licenses(pkg_);
}

alpm_list_t *AlpmPackage::GetConflicts() const {
  return alpm_pkg_get_conflicts(pkg_);
}

alpm_list_t *AlpmPackage::GetReplaces() const {
  return alpm_pkg_get_replaces(pkg_);
}

alpm_list_t *AlpmPackage::ComputeOptionalFor() const {
  return alpm_pkg_compute_optionalfor(pkg_);
}

alpm_list_t *AlpmPackage::ComputeRequiredBy() const {
  return alpm_pkg_compute_requiredby(pkg_);
}

alpm_time_t AlpmPackage::GetBuildDate() const {
  return alpm_pkg_get_builddate(pkg_);
}

alpm_time_t AlpmPackage::GetInstallDate() const {
  return alpm_pkg_get_installdate(pkg_);
}

off_t AlpmPackage::GetISize() const { return alpm_pkg_get_isize(pkg_); }

alpm_pkgreason_t AlpmPackage::GetReason() const {
  return alpm_pkg_get_reason(pkg_);
}

bool AlpmPackage::HasScriptlet() const { return alpm_pkg_has_scriptlet(pkg_); }

int AlpmPackage::GetValidation() const { return alpm_pkg_get_validation(pkg_); }

void *AlpmPackage::ChangelogOpen() const {
  return alpm_pkg_changelog_open(pkg_);
}

std::size_t AlpmPackage::ChangelogRead(void *fp, char *buf,
                                       std::size_t size) const {
  return alpm_pkg_changelog_read(buf, size, pkg_, fp);
}

void AlpmPackage::ChangelogClose(void *fp) const {
  alpm_pkg_changelog_close(pkg_, fp);
}

}  // namespace pacmanpp