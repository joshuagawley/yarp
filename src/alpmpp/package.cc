// SPDX-License-Identifier: MIT

#include "package.h"

#include <alpm.h>
#include <alpmpp/depend.h>
#include <alpmpp/file.h>
#include <alpmpp/types.h>
#include <alpmpp/util.h>

#include <algorithm>
#include <format>
#include <iomanip>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

namespace {

template <typename OutputIter>
void PrintDependsList(OutputIter output_iter, const std::string_view prefix,
                      std::vector<alpmpp::AlpmDepend> depends) {
  auto names =
      depends | std::views::transform([](auto &dep) { return dep.name(); });
  alpmpp::util::PrintJoinedLine(output_iter, prefix, names);
}

template <typename OutputIter>
void PrintOptDependsList(OutputIter output_iter,
                         std::vector<alpmpp::AlpmDepend> opt_depends) {
  constexpr std::string_view kPrefix{"Optional Deps   : "};

  auto dep_strings = opt_depends | std::views::transform([](auto &dep) {
                       return dep.ComputeString();
                     });

  alpmpp::util::PrintJoinedLine(output_iter, kPrefix, dep_strings);
}

template <typename OutputIter>
void PrintInstallReason(OutputIter output_iter,
                        const alpmpp::PkgReason reason) {
  switch (reason) {
    case alpmpp::PkgReason::kExplicit:
      std::format_to(output_iter, "Install Reason  : Explicitly installed\n");
      break;
    case alpmpp::PkgReason::kDepend:
      std::format_to(output_iter,
                     "Install Reason  : Installed as a dependency for another "
                     "package\n");
      break;
    case alpmpp::PkgReason::kUnknown:
      std::format_to(output_iter, "Install Reason  : Unknown\n");
      break;
  }
}

template <typename OutputIter>
void PrintHumanizedSize(OutputIter output_iter, const std::string_view prefix,
                        const off_t size) {
  constexpr std::array units{std::string_view{"B"},   std::string_view{"KiB"},
                             std::string_view{"MiB"}, std::string_view{"GiB"},
                             std::string_view{"TiB"}, std::string_view{"PiB"}};
  auto size_d = static_cast<double>(size);
  std::size_t i{};

  while (size_d >= 1024.0 && i < 5) {
    size_d /= 1024.0;
    ++i;
  }

  std::format_to(output_iter, "{} ", prefix);
  if (i == 0) {
    std::format_to(output_iter, "{:.0f} {}\n", size_d, units[i]);
  } else {
    std::format_to(output_iter, "{:.2f} {}\n", size_d, units[i]);
  }
}

template <typename OutputIter>
void PrintHumanizedDate(OutputIter output_iter, const std::string_view prefix,
                        const alpm_time_t alpm_time) {
  // We use C-style time API instead of std::chrono
  const auto time = static_cast<std::time_t>(alpm_time);
  const tm *local_time = std::localtime(&time);

  // std::put_time doesn't work with std::format
  std::stringstream ss;
  ss << prefix << std::put_time(local_time, "%a %d %b %Y %H:%M:%S %Z") << '\n';
  std::format_to(output_iter, "{}", ss.str());
}

template <typename OutputIter>
void PrintInstallScript(OutputIter output_iter, const bool has_scriptlet) {
  const std::string_view scriptlet_str = has_scriptlet ? "Yes" : "No";
  std::format_to(output_iter, "Install Script  : {}\n", scriptlet_str);
}

template <typename OutputIter>
void PrintValidation(OutputIter output_iter,
                     const alpmpp::PkgValidation validation) {
  std::format_to(output_iter, "Validated By    : ");

  if ((validation & alpmpp::PkgValidation::kNone) ==
      alpmpp::PkgValidation::kNone) {
    std::format_to(output_iter, "None\n");
  } else {
    constexpr std::array kValidations{
        std::pair{alpmpp::PkgValidation::kMd5, std::string_view{"MD5 Sum"}},
        std::pair{alpmpp::PkgValidation::kSha256,
                  std::string_view{"SHA-256 Sum"}},
        std::pair{alpmpp::PkgValidation::kSignature,
                  std::string_view{"Signature"}},
    };

    auto active_validations =
        kValidations | std::views::filter([validation](const auto &pair) {
          return (validation & pair.first) == pair.first;
        }) |
        std::views::transform([](const auto &pair) { return pair.second; });

    auto joined =
        active_validations | std::views::join_with(std::string_view{" "});

    std::ranges::for_each(joined, [output_iter](const char c) {
      std::format_to(output_iter, "{}", c);
    });

    std::format_to(output_iter, "\n");
  }
}

}  // namespace

namespace alpmpp {

std::string AlpmPackage::GetFileList(std::string_view root_path) const {
  std::string result;

  for (const AlpmFile &file : files()) {
    std::format_to(std::back_inserter(result), "{} {}{}\n", name(), root_path,
                   file.name());
  }

  return result;
}

std::string AlpmPackage::GetInfo() const {
  std::string result;

  std::format_to(std::back_inserter(result), "Name            : {}\n", name());
  std::format_to(std::back_inserter(result), "Version         : {}\n",
                 version());
  std::format_to(std::back_inserter(result), "Description     : {}\n", desc());
  std::format_to(std::back_inserter(result), "Architecture    : {}\n", arch());
  std::format_to(std::back_inserter(result), "URL             : {}\n", url());

  util::PrintJoinedLine(std::back_inserter(result), "Licenses        : ", licenses());
  util::PrintJoinedLine(std::back_inserter(result),
                        "Groups          : ", groups());
  PrintDependsList(std::back_inserter(result),
                   "Provides        : ", provides());
  PrintDependsList(std::back_inserter(result), "Depends On      : ", depends());
  PrintOptDependsList(std::back_inserter(result), opt_depends());
  // alpm_pkg_compute_* don't free the list, so we set free_list to true
  util::PrintJoinedLine(std::back_inserter(result),
                        "Required By     : ", ComputeRequiredBy());
  util::PrintJoinedLine(std::back_inserter(result),
                        "Optional For    : ", ComputeOptionalFor());

  PrintDependsList(std::back_inserter(result),
                   "Conflicts With  : ", conflicts());
  PrintDependsList(std::back_inserter(result),
                   "Replaces        : ", replaces());
  PrintHumanizedSize(std::back_inserter(result), "Installed Size  :", i_size());
  std::format_to(std::back_inserter(result), "Packager        : {}\n",
                 packager());
  PrintHumanizedDate(std::back_inserter(result),
                     "Build Date      : ", build_date());
  PrintHumanizedDate(std::back_inserter(result),
                     "Install Date    : ", install_date());
  PrintInstallReason(std::back_inserter(result), reason());
  PrintInstallScript(std::back_inserter(result), HasScriptlet());
  PrintValidation(std::back_inserter(result), validation());

  return result;
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
      alpm_pkg_get_groups(pkg_));
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
  const alpm_filelist_t *file_list = alpm_pkg_get_files(pkg_);
  std::span files_span{file_list->files, file_list->count};

  return files_span |
         std::views::transform([](auto &file) { return AlpmFile{&file}; }) |
         std::ranges::to<std::vector>();
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

alpm_db_t *AlpmPackage::GetDb() const noexcept { return alpm_pkg_get_db(pkg_); }

}  // namespace alpmpp