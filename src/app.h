// SPDX-License-Identifier: MIT

#ifndef PACMANPP_SRC_APP_H_
#define PACMANPP_SRC_APP_H_

#include <alpm_list.h>

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

  void PrintPkgList(
      std::stringstream &ss, alpm_pkg_t *pkg, std::string_view prefix,
      std::function<alpm_list_t *(alpm_pkg_t *)> attribute_getter);
  void PrintPkgLicenses(std::stringstream &ss, alpm_pkg_t *pkg);
  void PrintPkgGroups(std::stringstream &ss, alpm_pkg_t *pkg);
  void PrintDepends(std::stringstream &ss, alpm_pkg_t *pkg);

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