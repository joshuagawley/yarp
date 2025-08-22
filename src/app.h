// SPDX-License-Identifier: MIT

#ifndef PACMANPP_SRC_APP_H_
#define PACMANPP_SRC_APP_H_

#include <alpmpp/alpm.h>
#include <aurpp/client.h>

#include <memory>
#include <string>
#include <vector>

#include "config.h"
#include "operation.h"

namespace yarp {

class App {
 public:
  explicit App(std::span<char *> args);

  int Run();

 private:
  void PrintVerbose() const;

  aurpp::Client aur_client_;
  // use unique_ptr for lazy initialization
  std::unique_ptr<alpmpp::Alpm> alpm_;
  Config config_;
  Operation operation_ = Operation::kNone;
  QueryOptions query_options_ = QueryOptions::kNone;
  SyncOptions sync_options_ = SyncOptions::kNone;
  std::vector<std::string> targets_;
};

}  // namespace yarp

#endif  // PACMANPP_SRC_APP_H_