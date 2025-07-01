// SPDX-License-Identifier: MIT

#include "package.h"

#include <alpm.h>
#include <alpm_list.h>

#include <format>
#include <iomanip>
#include <print>
#include <sstream>
#include <string>
#include <vector>

#include "depend.h"
#include "src/alpmpp/file.h"
#include "types.h"
#include "util.h"

namespace {

template <typename T>
  requires std::formattable<T, char>
void PrintStringVector(std::stringstream &ss, const std::string_view prefix,
                       const std::vector<T> &vector) {
  if (vector.empty()) {
    std::println(ss, "{}None", prefix);
    return;
  }

  std::print(ss, "{}", prefix);
  for (const T &elem : vector) {
    if (elem != *(std::end(vector) - 1)) {
      std::print(ss, "{}  ", elem);  // Not the last item, add space
    } else {
      std::println(ss, "{}", elem);  // Last item, no trailing space
    }
  }
}

void PrintDependsList(std::stringstream &ss, const std::string_view prefix,
                      std::vector<alpmpp::AlpmDepend> depends) {
  if (depends.empty()) {
    std::println(ss, "{}None", prefix);
    return;
  }

  std::print(ss, "{}", prefix);
  for (alpmpp::AlpmDepend &depend : depends) {
    if (&depend != &depends.back()) {
      std::print(ss, "{}  ",
                 depend.GetName());  // Not the last item, add space
    } else {
      std::println(ss, "{}",
                   depend.GetName());  // Last item, no trailing space
    }
  }
}

void PrintOptDependsList(std::stringstream &ss,
                         std::vector<alpmpp::AlpmDepend> opt_depends) {
  constexpr std::string_view kPrefix{"Optional Deps   : "};

  if (opt_depends.empty()) {
    std::println(ss, "{}None", kPrefix);
    return;
  }

  std::print(ss, "{}", kPrefix);
  for (alpmpp::AlpmDepend &depend : opt_depends) {
    std::string dep_string = depend.ComputeString();
    if (&depend != &opt_depends.back()) {
      std::print(ss, "{}  ",
                 dep_string);  // Not the last item, add space
    } else {
      std::println(ss, "{}",
                   dep_string);  // Last item, no trailing space
    }
  }
}

void PrintInstallReason(std::stringstream &ss, const alpmpp::PkgReason reason) {
  switch (reason) {
    case alpmpp::PkgReason::kExplicit:
      std::println(ss, "Install Reason  : Explicitly installed");
      break;
    case alpmpp::PkgReason::kDepend:
      std::println(ss,
                   "Install Reason  : Installed as a dependency for another "
                   "package");
      break;
    case alpmpp::PkgReason::kUnknown:
      std::println(ss, "Install Reason  : Unknown");
      break;
  }
}

void PrintHumanizedSize(std::stringstream &ss, const std::string_view prefix,
                        const off_t size) {
  constexpr std::array<const char *, 6> units{"B",   "KiB", "MiB",
                                              "GiB", "TiB", "PiB"};
  double size_d = static_cast<double>(size);
  std::size_t i{};

  while (size_d >= 1024.0 && i < 5) {
    size_d /= 1024.0;
    ++i;
  }

  std::print(ss, "{} ", prefix);
  if (i == 0) {
    std::println(ss, "{:.0f} {}", size_d, units[i]);
  } else {
    std::println(ss, "{:.2f} {}", size_d, units[i]);
  }
}

void PrintHumanizedDate(std::stringstream &ss, const std::string_view prefix,
                        const alpm_time_t alpm_time) {
  // We use C-style time API instead of std::chrono
  const std::time_t time = static_cast<std::time_t>(alpm_time);
  const tm *local_time = std::localtime(&time);

  // std::put_time doesn't work with std::format
  ss << prefix << std::put_time(local_time, "%a %d %b %Y %H:%M:%S %Z") << '\n';
}

void PrintInstallScript(std::stringstream &ss, const bool has_scriptlet) {
  const std::string_view scriptlet_str = has_scriptlet ? "Yes" : "No";
  std::println(ss, "Install Script  : {}", scriptlet_str);
}

void PrintValidation(std::stringstream &ss, const int validation) {
  std::print(ss, "Validated By    : ");
  if (validation) {
    if (validation & ALPM_PKG_VALIDATION_NONE) {
      std::println(ss, "None");
    } else {
      std::vector<std::string> validation_methods;

      if (validation & ALPM_PKG_VALIDATION_MD5SUM) {
        validation_methods.push_back("MD5 Sum");
      }
      if (validation & ALPM_PKG_VALIDATION_SHA256SUM) {
        validation_methods.push_back("SHA-256 Sum");
      }
      if (validation & ALPM_PKG_VALIDATION_SIGNATURE) {
        validation_methods.push_back("Signature");
      }

      for (auto it = std::begin(validation_methods);
           it != std::end(validation_methods); ++it) {
        if (std::next(it) != std::end(validation_methods)) {
          std::print(ss, "{}  ", *it);
        }
        std::print(ss, "{}", *it);
      }
    }
  } else {
    std::println(ss, "Unknown");
  }
}

}  // namespace

namespace alpmpp {

std::string AlpmPackage::GetFileList(std::string_view root_path) const {
  std::stringstream ss;

  const std::vector<AlpmFile> files = GetFiles();

  for (const AlpmFile &file : files) {
    std::println(ss, "{} {}{}", GetName(), root_path, file.GetName());
  }

  return ss.str();
}

std::string AlpmPackage::GetInfo() const {
  std::stringstream ss;

  std::println(ss, "Name            : {}", GetName());
  std::println(ss, "Version         : {}", GetVersion());
  std::println(ss, "Description     : {}", GetDesc());
  std::println(ss, "Architecture    : {}", GetArch());
  std::println(ss, "URL             : {}", GetURL());

  PrintStringVector(ss, "Licenses        : ", GetLicenses());
  PrintStringVector(ss, "Groups          : ", GetGroups());
  PrintDependsList(ss, "Provides        : ", GetProvides());
  PrintDependsList(ss, "Depends On      : ", GetDepends());
  PrintOptDependsList(ss, GetOptDepends());
  // alpm_pkg_compute_* don't free the list, so we set free_list to true
  PrintStringVector(ss, "Required By     : ", ComputeRequiredBy());
  PrintStringVector(ss, "Optional For    : ", ComputeOptionalFor());

  PrintDependsList(ss, "Conflicts With  : ", GetConflicts());
  PrintDependsList(ss, "Replaces        : ", GetReplaces());
  PrintHumanizedSize(ss, "Installed Size  :", GetISize());
  std::println(ss, "Packager        : {}", GetPackager());
  PrintHumanizedDate(ss, "Build Date      : ", GetBuildDate());
  PrintHumanizedDate(ss, "Install Date    : ", GetInstallDate());
  PrintInstallReason(ss, GetReason());
  PrintInstallScript(ss, HasScriptlet());
  PrintValidation(ss, GetValidation());

  return ss.str();
}

std::string_view AlpmPackage::GetName() const noexcept {
  return alpm_pkg_get_name(pkg_);
}

std::string_view AlpmPackage::GetVersion() const noexcept {
  return alpm_pkg_get_version(pkg_);
}

std::string_view AlpmPackage::GetDesc() const noexcept {
  return alpm_pkg_get_desc(pkg_);
}

std::string_view AlpmPackage::GetArch() const noexcept {
  return alpm_pkg_get_arch(pkg_);
}

std::string_view AlpmPackage::GetURL() const noexcept {
  return alpm_pkg_get_url(pkg_);
}

std::string_view AlpmPackage::GetPackager() const noexcept {
  return alpm_pkg_get_packager(pkg_);
}

std::vector<AlpmDepend> AlpmPackage::GetOptDepends() const noexcept {
  return util::AlpmListToVector<alpm_depend_t *, AlpmDepend>(
      alpm_pkg_get_optdepends(pkg_));
}

std::vector<AlpmDepend> AlpmPackage::GetDepends() const noexcept {
  return util::AlpmListToVector<alpm_depend_t *, AlpmDepend>(
      alpm_pkg_get_depends(pkg_));
}

std::vector<AlpmDepend> AlpmPackage::GetProvides() const noexcept {
  return util::AlpmListToVector<alpm_depend_t *, AlpmDepend>(
      alpm_pkg_get_provides(pkg_));
}

std::vector<std::string_view> AlpmPackage::GetGroups() const noexcept {
  return util::AlpmListToVector<const char *, std::string_view>(
      (alpm_pkg_get_groups(pkg_)));
}

std::vector<std::string_view> AlpmPackage::GetLicenses() const noexcept {
  return util::AlpmListToVector<const char *, std::string_view>(
      alpm_pkg_get_licenses(pkg_));
}

std::vector<AlpmDepend> AlpmPackage::GetConflicts() const noexcept {
  return util::AlpmListToVector<alpm_depend_t *, AlpmDepend>(
      alpm_pkg_get_conflicts(pkg_));
}

std::vector<AlpmDepend> AlpmPackage::GetReplaces() const noexcept {
  return util::AlpmListToVector<alpm_depend_t *, AlpmDepend>(
      alpm_pkg_get_replaces(pkg_));
}

std::vector<AlpmFile> AlpmPackage::GetFiles() const noexcept {
  std::vector<AlpmFile> result;
  alpm_filelist_t *file_list = alpm_pkg_get_files(pkg_);
  for (std::size_t i = 0; i < file_list->count; ++i) {
    result.emplace_back(&file_list->files[i]);
  }
  return result;
}

std::vector<std::string> AlpmPackage::ComputeOptionalFor() const noexcept {
  return util::AlpmListToVector<const char *, std::string>(
      alpm_pkg_compute_optionalfor(pkg_));
}

std::vector<std::string> AlpmPackage::ComputeRequiredBy() const noexcept {
  return util::AlpmListToVector<const char *, std::string>(
      alpm_pkg_compute_requiredby(pkg_));
}

alpm_time_t AlpmPackage::GetBuildDate() const noexcept {
  return alpm_pkg_get_builddate(pkg_);
}

alpm_time_t AlpmPackage::GetInstallDate() const noexcept {
  return alpm_pkg_get_installdate(pkg_);
}

off_t AlpmPackage::GetISize() const noexcept {
  return alpm_pkg_get_isize(pkg_);
}

PkgReason AlpmPackage::GetReason() const noexcept {
  return static_cast<PkgReason>(alpm_pkg_get_reason(pkg_));
}

bool AlpmPackage::HasScriptlet() const noexcept {
  return alpm_pkg_has_scriptlet(pkg_);
}

int AlpmPackage::GetValidation() const noexcept {
  return alpm_pkg_get_validation(pkg_);
}

void *AlpmPackage::ChangelogOpen() const {
  return alpm_pkg_changelog_open(pkg_);
}

std::size_t AlpmPackage::ChangelogRead(void *fp, char *buf,
                                       std::size_t size) const {
  return alpm_pkg_changelog_read(buf, size, pkg_, fp);
}

int AlpmPackage::ChangelogClose(void *fp) const noexcept {
  return alpm_pkg_changelog_close(pkg_, fp);
}

}  // namespace alpmpp