// SPDX-License-Identifier: MIT

#include "package.h"

#include <alpm.h>

#include <format>
#include <iomanip>
#include <print>
#include <sstream>
#include <string>
#include <vector>

#include "depend.h"
#include "file.h"
#include "types.h"
#include "util.h"

namespace {

void PrintDependsList(std::stringstream &ss, const std::string_view prefix,
                      std::vector<alpmpp::AlpmDepend> depends) {
  if (depends.empty()) {
    std::println(ss, "{}None", prefix);
  } else {
    std::print(ss, "{}", prefix);
    for (alpmpp::AlpmDepend &depend : depends) {
      if (&depend != &depends.back()) {
        std::print(ss, "{}  ",
                   depend.name());  // Not the last item, add space
      } else {
        std::println(ss, "{}",
                     depend.name());  // Last item, no trailing space
      }
    }
  }
}

void PrintOptDependsList(std::stringstream &ss,
                         std::vector<alpmpp::AlpmDepend> opt_depends) {
  constexpr std::string_view kPrefix{"Optional Deps   : "};

  if (opt_depends.empty()) {
    std::println(ss, "{}None", kPrefix);
  } else {
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
  auto size_d = static_cast<double>(size);
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
  const auto time = static_cast<std::time_t>(alpm_time);
  const tm *local_time = std::localtime(&time);

  // std::put_time doesn't work with std::format
  ss << prefix << std::put_time(local_time, "%a %d %b %Y %H:%M:%S %Z") << '\n';
}

void PrintInstallScript(std::stringstream &ss, const bool has_scriptlet) {
  const std::string_view scriptlet_str = has_scriptlet ? "Yes" : "No";
  std::println(ss, "Install Script  : {}", scriptlet_str);
}

void PrintValidation(std::stringstream &ss,
                     const alpmpp::PkgValidation validation) {
  std::print(ss, "Validated By    : ");
  if ((validation & alpmpp::PkgValidation::kNone) ==
      alpmpp::PkgValidation::kNone) {
    std::println(ss, "None");
  } else {
    std::vector<std::string> validation_methods;

    if ((validation & alpmpp::PkgValidation::kMd5) ==
        alpmpp::PkgValidation::kMd5) {
      validation_methods.emplace_back("MD5 Sum");
    }
    if ((validation & alpmpp::PkgValidation::kSha256) ==
        alpmpp::PkgValidation::kSha256) {
      validation_methods.emplace_back("SHA-256 Sum");
    }
    if ((validation & alpmpp::PkgValidation::kSignature) ==
        alpmpp::PkgValidation::kSignature) {
      validation_methods.emplace_back("Signature");
    }

    for (const std::string_view method : validation_methods) {
      if (method != validation_methods.back()) {
        std::print(ss, "{}  ", method);
      }
      std::print(ss, "{}", method);
    }
  }
}

}  // namespace

namespace alpmpp {

std::string AlpmPackage::GetFileList(std::string_view root_path) const {
  std::stringstream ss;

  for (const AlpmFile &file : files()) {
    std::println(ss, "{} {}{}", name(), root_path, file.name());
  }

  return ss.str();
}

std::string AlpmPackage::GetInfo() const {
  std::stringstream ss;

  std::println(ss, "Name            : {}", name());
  std::println(ss, "Version         : {}", version());
  std::println(ss, "Description     : {}", desc());
  std::println(ss, "Architecture    : {}", arch());
  std::println(ss, "URL             : {}", url());

  util::PrintStringVector(ss, "Licenses        : ", licenses());
  util::PrintStringVector(ss, "Groups          : ", groups());
  PrintDependsList(ss, "Provides        : ", provides());
  PrintDependsList(ss, "Depends On      : ", depends());
  PrintOptDependsList(ss, opt_depends());
  // alpm_pkg_compute_* don't free the list, so we set free_list to true
  util::PrintStringVector(ss, "Required By     : ", ComputeRequiredBy());
  util::PrintStringVector(ss, "Optional For    : ", ComputeOptionalFor());

  PrintDependsList(ss, "Conflicts With  : ", conflicts());
  PrintDependsList(ss, "Replaces        : ", replaces());
  PrintHumanizedSize(ss, "Installed Size  :", i_size());
  std::println(ss, "Packager        : {}", packager());
  PrintHumanizedDate(ss, "Build Date      : ", build_date());
  PrintHumanizedDate(ss, "Install Date    : ", install_date());
  PrintInstallReason(ss, reason());
  PrintInstallScript(ss, HasScriptlet());
  PrintValidation(ss, validation());

  return ss.str();
}

std::string_view AlpmPackage::name() const noexcept {
  return alpm_pkg_get_name(pkg_);
}

std::string_view AlpmPackage::version() const noexcept {
  return alpm_pkg_get_version(pkg_);
}

std::string_view AlpmPackage::desc() const noexcept {
  return alpm_pkg_get_desc(pkg_);
}

std::string_view AlpmPackage::arch() const noexcept {
  return alpm_pkg_get_arch(pkg_);
}

std::string_view AlpmPackage::url() const noexcept {
  return alpm_pkg_get_url(pkg_);
}

std::string_view AlpmPackage::packager() const noexcept {
  return alpm_pkg_get_packager(pkg_);
}

std::vector<AlpmDepend> AlpmPackage::opt_depends() const noexcept {
  return util::AlpmListToVector<alpm_depend_t *, AlpmDepend>(
      alpm_pkg_get_optdepends(pkg_));
}

std::vector<AlpmDepend> AlpmPackage::depends() const noexcept {
  return util::AlpmListToVector<alpm_depend_t *, AlpmDepend>(
      alpm_pkg_get_depends(pkg_));
}

std::vector<AlpmDepend> AlpmPackage::provides() const noexcept {
  return util::AlpmListToVector<alpm_depend_t *, AlpmDepend>(
      alpm_pkg_get_provides(pkg_));
}

std::vector<std::string_view> AlpmPackage::groups() const noexcept {
  return util::AlpmListToVector<const char *, std::string_view>(
      (alpm_pkg_get_groups(pkg_)));
}

std::vector<std::string_view> AlpmPackage::licenses() const noexcept {
  return util::AlpmListToVector<const char *, std::string_view>(
      alpm_pkg_get_licenses(pkg_));
}

std::vector<AlpmDepend> AlpmPackage::conflicts() const noexcept {
  return util::AlpmListToVector<alpm_depend_t *, AlpmDepend>(
      alpm_pkg_get_conflicts(pkg_));
}

std::vector<AlpmDepend> AlpmPackage::replaces() const noexcept {
  return util::AlpmListToVector<alpm_depend_t *, AlpmDepend>(
      alpm_pkg_get_replaces(pkg_));
}

std::vector<AlpmFile> AlpmPackage::files() const noexcept {
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

alpm_time_t AlpmPackage::build_date() const noexcept {
  return alpm_pkg_get_builddate(pkg_);
}

alpm_time_t AlpmPackage::install_date() const noexcept {
  return alpm_pkg_get_installdate(pkg_);
}

off_t AlpmPackage::i_size() const noexcept { return alpm_pkg_get_isize(pkg_); }

PkgReason AlpmPackage::reason() const noexcept {
  return static_cast<PkgReason>(alpm_pkg_get_reason(pkg_));
}

bool AlpmPackage::HasScriptlet() const noexcept {
  return alpm_pkg_has_scriptlet(pkg_);
}

PkgValidation AlpmPackage::validation() const noexcept {
  return static_cast<PkgValidation>(alpm_pkg_get_validation(pkg_));
}

void *AlpmPackage::ChangelogOpen() const {
  return alpm_pkg_changelog_open(pkg_);
}

std::size_t AlpmPackage::ChangelogRead(void *fp, char *buf,
                                       const std::size_t size) const {
  return alpm_pkg_changelog_read(buf, size, pkg_, fp);
}

int AlpmPackage::ChangelogClose(void *fp) const noexcept {
  return alpm_pkg_changelog_close(pkg_, fp);
}

}  // namespace alpmpp