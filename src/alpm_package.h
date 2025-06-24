// SPDX-License-Identifier: MIT

#ifndef PACMANPP_SRC_ALPM_PACKAGE_H_
#define PACMANPP_SRC_ALPM_PACKAGE_H_

#include <alpm.h>
#include <alpm_list.h>

#include <string_view>

namespace pacmanpp {

class AlpmPackage {
 public:
  explicit AlpmPackage(alpm_pkg_t *pkg) : pkg_(pkg) {}

  std::string_view GetName() const;
  std::string_view GetVersion() const;
  std::string_view GetDesc() const;
  std::string_view GetArch() const;
  std::string_view GetURL() const;
  std::string_view GetPackager() const;

  alpm_list_t *GetOptDepends() const;
  alpm_list_t *GetDepends() const;
  alpm_list_t *GetProvides() const;
  alpm_list_t *GetGroups() const;
  alpm_list_t *GetLicenses() const;
  alpm_list_t *GetConflicts() const;
  alpm_list_t *GetReplaces() const;

  alpm_list_t *ComputeOptionalFor() const;
  alpm_list_t *ComputeRequiredBy() const;

  alpm_time_t GetBuildDate() const;
  alpm_time_t GetInstallDate() const;

  off_t GetISize() const;
  alpm_pkgreason_t GetReason() const;
  int GetValidation() const;

  bool HasScriptlet() const;

  void *ChangelogOpen() const;
  size_t ChangelogRead(void *fp, char *buf, size_t size) const;
  void ChangelogClose(void *fp) const;

  std::string GetInfo() const;

 private:
  alpm_pkg_t *pkg_;
};

}  // namespace pacmanpp

#endif  // PACMANPP_SRC_ALPM_PACKAGE_H_