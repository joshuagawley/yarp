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

  std::string_view GetName() const noexcept;
  std::string_view GetVersion() const noexcept;
  std::string_view GetDesc() const noexcept;
  std::string_view GetArch() const noexcept;
  std::string_view GetURL() const noexcept;
  std::string_view GetPackager() const noexcept;

  alpm_list_t *GetOptDepends() const noexcept;
  alpm_list_t *GetDepends() const noexcept;
  alpm_list_t *GetProvides() const noexcept;
  alpm_list_t *GetGroups() const noexcept;
  alpm_list_t *GetLicenses() const noexcept;
  alpm_list_t *GetConflicts() const noexcept;
  alpm_list_t *GetReplaces() const noexcept;
  alpm_filelist_t *GetFiles() const noexcept;

  alpm_list_t *ComputeOptionalFor() const noexcept;
  alpm_list_t *ComputeRequiredBy() const noexcept;

  alpm_time_t GetBuildDate() const noexcept;
  alpm_time_t GetInstallDate() const noexcept;

  off_t GetISize() const noexcept;
  alpm_pkgreason_t GetReason() const noexcept;
  int GetValidation() const noexcept;

  bool HasScriptlet() const noexcept;

  void *ChangelogOpen() const;
  std::size_t ChangelogRead(void *fp, char *buf, std::size_t size) const;
  int ChangelogClose(void *fp) const noexcept;

  std::string GetFileList(std::string_view root_path) const;
  std::string GetInfo() const;

 private:
  alpm_pkg_t *pkg_;
};

}  // namespace pacmanpp

#endif  // PACMANPP_SRC_ALPM_PACKAGE_H_