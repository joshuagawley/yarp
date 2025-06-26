// SPDX-License-Identifier: MIT

#ifndef PACMANPP_SRC_APP_H_
#define PACMANPP_SRC_APP_H_

#include <alpm.h>
#include <alpm_list.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "alpm.h"
#include "config.h"
#include "operation.h"

namespace pacmanpp {

class App {
 public:
  App(std::span<char *> args);

  int Run();

 private:
  template <typename Handler, typename... Args>
  int ExecuteOperation(Args &&...args) {
    Handler handler{config_, *alpm_, std::forward<Args>(args)...};
    return handler.Execute();
  }

  void PrintVerbose() const;

  // use unique_ptr for lazy initialization
  std::unique_ptr<Alpm> alpm_;
  Config config_;
  Operation operation_ = Operation::kNone;
  QueryOptions query_options_ = QueryOptions::kNone;
  std::vector<std::string> targets_;
};

}  // namespace pacmanpp

#endif  // PACMANPP_SRC_APP_H_