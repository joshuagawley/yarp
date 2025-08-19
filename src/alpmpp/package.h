// SPDX-License-Identifier: MIT

#ifndef ALPMPP_PACKAGE_H_
#define ALPMPP_PACKAGE_H_

#include <alpm.h>

#include <alpmpp/depend.h>
#include <alpmpp/file.h>
#include <alpmpp/types.h>

#include <string_view>
#include <vector>

namespace alpmpp {

class AlpmPackage {
 public:
  constexpr explicit AlpmPackage(alpm_pkg_t *pkg, bool owned = false)
      : pkg_(pkg), owned_{owned} {}

  ~AlpmPackage() {
    if (owned_) alpm_pkg_free(pkg_);
  }

  AlpmPackage(const AlpmPackage &) = delete;
  AlpmPackage &operator=(const AlpmPackage &) = delete;

  AlpmPackage(AlpmPackage &&other) noexcept
      : pkg_(other.pkg_), owned_(other.owned_) {
    other.pkg_ = nullptr;
    other.owned_ = false;
  }

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

  [[nodiscard]] std::string_view name() const noexcept;
  [[nodiscard]] std::string_view version() const noexcept;
  [[nodiscard]] std::string_view desc() const noexcept;
  [[nodiscard]] std::string_view arch() const noexcept;
  [[nodiscard]] std::string_view url() const noexcept;
  [[nodiscard]] std::string_view packager() const noexcept;

  [[nodiscard]] std::vector<AlpmDepend> opt_depends() const noexcept;
  [[nodiscard]] std::vector<AlpmDepend> depends() const noexcept;
  [[nodiscard]] std::vector<AlpmDepend> provides() const noexcept;
  [[nodiscard]] std::vector<std::string_view> groups() const noexcept;
  [[nodiscard]] std::vector<std::string_view> licenses() const noexcept;
  [[nodiscard]] std::vector<AlpmDepend> conflicts() const noexcept;
  [[nodiscard]] std::vector<AlpmDepend> replaces() const noexcept;
  [[nodiscard]] std::vector<AlpmFile> files() const noexcept;
  [[nodiscard]] alpm_time_t build_date() const noexcept;
  [[nodiscard]] alpm_time_t install_date() const noexcept;
  [[nodiscard]] off_t i_size() const noexcept;
  [[nodiscard]] PkgReason reason() const noexcept;
  [[nodiscard]] PkgValidation validation() const noexcept;

  [[nodiscard]] std::vector<std::string> ComputeOptionalFor() const noexcept;
  [[nodiscard]] std::vector<std::string> ComputeRequiredBy() const noexcept;

  [[nodiscard]] bool HasScriptlet() const noexcept;

  [[nodiscard]] void *ChangelogOpen() const;
  std::size_t ChangelogRead(void *fp, char *buf, std::size_t size) const;
  int ChangelogClose(void *fp) const noexcept;

  [[nodiscard]] alpm_db_t *GetDb() const noexcept;

  [[nodiscard]] std::string GetFileList(std::string_view root_path) const;
  [[nodiscard]] std::string GetInfo() const;

  [[nodiscard]] constexpr alpm_pkg_t *GetHandle() const noexcept {
    return pkg_;
  }

 private:
  alpm_pkg_t *pkg_;
  bool owned_ = false;
};

}  // namespace alpmpp

#endif  // ALPMPP_PACKAGE_H_