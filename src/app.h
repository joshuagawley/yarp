// SPDX-License-Identifier: MIT

#ifndef PACMANPP_SRC_APP_H_
#define PACMANPP_SRC_APP_H_

#include <alpm.h>
#include <alpm_list.h>

#include <climits>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "alpm.h"
#include "config.h"
#include "src/operation.h"

namespace pacmanpp {

class App {
 public:
  App(std::span<char *> args);

  int Run();

 private:
  void PrintVerbose() const;
  void HandleQuery(const std::vector<std::string> &targets);
  void PrintHelp() const;
  void PrintPkgChangelog(alpm_pkg_t *pkg);
  void PrintPkgInfo(alpm_pkg_t *pkg);

  void PrintAlpmList(
      std::stringstream &ss, alpm_pkg_t *pkg, std::string_view prefix,
      std::function<alpm_list_t *(alpm_pkg_t *)> attribute_getter,
      bool free_list = false);

  void PrintPkgList(
      std::stringstream &ss, alpm_pkg_t *pkg, std::string_view prefix,
      std::function<alpm_list_t *(alpm_pkg_t *)> attribute_getter);

  void PrintDependsList(
      std::stringstream &ss, alpm_pkg_t *pkg, std::string_view prefix,
      std::function<alpm_list_t *(alpm_pkg_t *)> attribute_getter);

  void PrintOptDependsList(std::stringstream &ss, alpm_pkg_t *pkg,
                           std::string_view prefix);

  void PrintPkgLicenses(std::stringstream &ss, alpm_pkg_t *pkg);
  void PrintPkgGroups(std::stringstream &ss, alpm_pkg_t *pkg);
  void PrintDepends(std::stringstream &ss, alpm_pkg_t *pkg);

  void PrintHumanizedSize(std::stringstream &ss, alpm_pkg_t *pkg,
                          std::string_view prefix,
                          std::function<off_t(alpm_pkg_t *)> size_getter);

  void PrintHumanizedDate(std::stringstream &ss, alpm_pkg_t *pkg,
                          std::string_view prefix,
                          std::function<alpm_time_t(alpm_pkg_t *)> date_getter);

  void PrintInstallReason(std::stringstream &ss, alpm_pkgreason_t reason);

  void PrintInstallScript(std::stringstream &ss, bool has_scriptlet);

  // use unique_ptr for lazy initialization
  std::unique_ptr<Alpm> alpm_;
  Config config_;
  Operation operation_ = Operation::kNone;
  QueryOptions query_options_ = QueryOptions::kNone;
  std::vector<std::string> targets_;
  const char *program_name_;
};

}  // namespace pacmanpp

#endif  // PACMANPP_SRC_APP_H_