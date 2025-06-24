// SPDX-License-Identifier: MIT

#ifndef PACMANPP_SRC_APP_H_
#define PACMANPP_SRC_APP_H_

#include <alpm.h>
#include <alpm_list.h>

#include <memory>
#include <string>
#include <vector>

#include "alpm.h"
#include "config.h"
#include "src/alpm_package.h"
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
  void PrintPkgChangelog(const AlpmPackage &pkg) const;
  void PrintPkgInfo(const AlpmPackage &pkg) const;

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