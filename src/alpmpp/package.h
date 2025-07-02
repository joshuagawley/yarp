// SPDX-License-Identifier: MIT

#ifndef ALPMPP_PACKAGE_H_
#define ALPMPP_PACKAGE_H_

#include <alpm.h>
#include <alpm_list.h>

#include <string_view>
#include <vector>

#include "depend.h"
#include "file.h"
#include "types.h"

namespace alpmpp {

class AlpmPackage {
 public:
  constexpr AlpmPackage(alpm_pkg_t *pkg, bool owned = false)
      : pkg_(pkg), owned_{owned} {}

  ~AlpmPackage() {
    if (owned_) alpm_pkg_free(pkg_);
  }

  // Copy constructor
  AlpmPackage(const AlpmPackage&) = delete;
  
  // Copy assignment
  AlpmPackage& operator=(const AlpmPackage&) = delete;
  
  // Move constructor
  AlpmPackage(AlpmPackage&& other) noexcept 
      : pkg_(other.pkg_), owned_(other.owned_) {
    other.pkg_ = nullptr;
    other.owned_ = false;
  }
  
  // Move assignment
  AlpmPackage& operator=(AlpmPackage&& other) noexcept {
    if (this != &other) {
      if (owned_) alpm_pkg_free(pkg_);
      pkg_ = other.pkg_;
      owned_ = other.owned_;
      other.pkg_ = nullptr;
      other.owned_ = false;
    }
    return *this;
  }

  std::string_view GetName() const noexcept;
  std::string_view GetVersion() const noexcept;
  std::string_view GetDesc() const noexcept;
  std::string_view GetArch() const noexcept;
  std::string_view GetURL() const noexcept;
  std::string_view GetPackager() const noexcept;

  std::vector<AlpmDepend> GetOptDepends() const noexcept;
  std::vector<AlpmDepend> GetDepends() const noexcept;
  std::vector<AlpmDepend> GetProvides() const noexcept;
  std::vector<std::string_view> GetGroups() const noexcept;
  std::vector<std::string_view> GetLicenses() const noexcept;
  std::vector<AlpmDepend> GetConflicts() const noexcept;
  std::vector<AlpmDepend> GetReplaces() const noexcept;
  std::vector<AlpmFile> GetFiles() const noexcept;

  std::vector<std::string> ComputeOptionalFor() const noexcept;
  std::vector<std::string> ComputeRequiredBy() const noexcept;

  alpm_time_t GetBuildDate() const noexcept;
  alpm_time_t GetInstallDate() const noexcept;

  off_t GetISize() const noexcept;
  PkgReason GetReason() const noexcept;
  PkgValidation GetValidation() const noexcept;

  bool HasScriptlet() const noexcept;

  void *ChangelogOpen() const;
  std::size_t ChangelogRead(void *fp, char *buf, std::size_t size) const;
  int ChangelogClose(void *fp) const noexcept;

  std::string GetFileList(std::string_view root_path) const;
  std::string GetInfo() const;

 private:
  alpm_pkg_t *pkg_;
  bool owned_ = false;
};

}  // namespace alpmpp

#endif  // ALPMPP_PACKAGE_H_