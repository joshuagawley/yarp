// SPDX-License-Identifier: MIT

#ifndef PACMANPP_SRC_APP_H_
#define PACMANPP_SRC_APP_H_

#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "alpmpp/alpm.h"
#include "config.h"
#include "operation.h"

namespace pacmanpp {

class App {
 public:
  explicit App(std::span<char *> args);

  int Run();

 private:
  void PrintVerbose() const;

  // use unique_ptr for lazy initialization
  std::unique_ptr<alpmpp::Alpm> alpm_;
  Config config_;
  Operation operation_ = Operation::kNone;
  QueryOptions query_options_ = QueryOptions::kNone;
  std::vector<std::string> targets_;
};

}  // namespace pacmanpp

#endif  // PACMANPP_SRC_APP_H_