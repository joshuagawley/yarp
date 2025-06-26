// SPDX-License-Identifier: MIT

#ifndef PACMANPP_QUERY_HANDLER_H_
#define PACMANPP_QUERY_HANDLER_H_

#include "alpm.h"
#include "config.h"
#include "operation.h"
#include "operation_handler.h"
#include "src/alpm_package.h"

namespace pacmanpp {

class QueryHandler : public OperationHandler {
 public:
  constexpr QueryHandler(Config &config, Alpm &alpm, QueryOptions query_options,
                         std::vector<std::string> targets)
      : OperationHandler(config, alpm),
        options_(query_options),
        targets_(std::move(targets)),
        local_db_(alpm_.GetLocalDb()) {}

  virtual ~QueryHandler() = default;

  int Execute() override;

 private:
  int HandleGroups() const;
  std::vector<AlpmPackage> GetPkgList() const;
  void PrintPkgFileList(const AlpmPackage &pkg) const;

  QueryOptions options_;
  std::vector<std::string> targets_;
  alpm_db_t *local_db_ = nullptr;
};

}  // namespace pacmanpp

#endif  // PACMANPP_QUERY_HANDLER_H_