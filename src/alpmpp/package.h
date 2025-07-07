// SPDX-License-Identifier: MIT

#ifndef ALPMPP_PACKAGE_H_
#define ALPMPP_PACKAGE_H_

#include <alpm.h>

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
  AlpmPackage(const AlpmPackage &) = delete;

  // Copy assignment
  AlpmPackage &operator=(const AlpmPackage &) = delete;

  // Move constructor
  AlpmPackage(AlpmPackage &&other) noexcept
      : pkg_(other.pkg_), owned_(other.owned_) {
    other.pkg_ = nullptr;
    other.owned_ = false;
  }

  // Move assignment
  AlpmPackage &operator=(AlpmPackage &&other) noexcept {
    if (this != &other) {
      if (owned_) alpm_pkg_free(pkg_);
      pkg_ = other.pkg_;
      owned_ = other.owned_;
      other.pkg_ = nullptr;
      other.owned_ = false;
    }
    return *this;
  }

  [[nodiscard]] std::string_view GetName() const noexcept;
  [[nodiscard]] std::string_view GetVersion() const noexcept;
  [[nodiscard]] std::string_view GetDesc() const noexcept;
  [[nodiscard]] std::string_view GetArch() const noexcept;
  [[nodiscard]] std::string_view GetURL() const noexcept;
  [[nodiscard]] std::string_view GetPackager() const noexcept;

  [[nodiscard]] std::vector<AlpmDepend> GetOptDepends() const noexcept;
  [[nodiscard]] std::vector<AlpmDepend> GetDepends() const noexcept;
  [[nodiscard]] std::vector<AlpmDepend> GetProvides() const noexcept;
  [[nodiscard]] std::vector<std::string_view> GetGroups() const noexcept;
  [[nodiscard]] std::vector<std::string_view> GetLicenses() const noexcept;
  [[nodiscard]] std::vector<AlpmDepend> GetConflicts() const noexcept;
  [[nodiscard]] std::vector<AlpmDepend> GetReplaces() const noexcept;
  [[nodiscard]] std::vector<AlpmFile> GetFiles() const noexcept;

  [[nodiscard]] std::vector<std::string> ComputeOptionalFor() const noexcept;
  [[nodiscard]] std::vector<std::string> ComputeRequiredBy() const noexcept;

  [[nodiscard]] alpm_time_t GetBuildDate() const noexcept;
  [[nodiscard]] alpm_time_t GetInstallDate() const noexcept;

  [[nodiscard]] off_t GetISize() const noexcept;
  [[nodiscard]] PkgReason GetReason() const noexcept;
  [[nodiscard]] PkgValidation GetValidation() const noexcept;

  [[nodiscard]] bool HasScriptlet() const noexcept;

  [[nodiscard]] void *ChangelogOpen() const;
  std::size_t ChangelogRead(void *fp, char *buf, std::size_t size) const;
  int ChangelogClose(void *fp) const noexcept;

  [[nodiscard]] std::string GetFileList(std::string_view root_path) const;
  [[nodiscard]] std::string GetInfo() const;

 private:
  alpm_pkg_t *pkg_;
  bool owned_ = false;
};

}  // namespace alpmpp

#endif  // ALPMPP_PACKAGE_H_