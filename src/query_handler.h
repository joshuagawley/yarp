// SPDX-License-Identifier: MIT

#ifndef PACMANPP_QUERY_HANDLER_H_
#define PACMANPP_QUERY_HANDLER_H_

#include "alpmpp/alpm.h"
#include "bitwise_enum.h"
#include "config.h"
#include "operation.h"
#include "operation_handler.h"

namespace pacmanpp {

enum class PkgLocality { kUnset = 1 << 0, kNative = 1 << 1, kForeign = 1 << 2 };

template <>
struct EnableEnumBitwiseOperators<PkgLocality> {
  static constexpr bool enabled = true;
};

class QueryHandler final : public OperationHandler {
 public:
  constexpr QueryHandler(Config &config, alpmpp::Alpm &alpm,
                         QueryOptions query_options,
                         std::vector<std::string> targets)
      : OperationHandler(config, alpm),
        options_(query_options),
        targets_(std::move(targets)),
        local_db_(alpm_.GetLocalDb()) {}

  ~QueryHandler() override = default;

  int Execute() override;

 private:
  [[nodiscard]] int HandleGroups() const;
  [[nodiscard]] std::vector<alpmpp::AlpmPackage> GetPkgList() const;
  void PrintPkgFileList(const alpmpp::AlpmPackage &pkg) const;
  void CheckPkgFiles(const alpmpp::AlpmPackage &pkg) const;
  [[nodiscard]] PkgLocality GetPkgLocality(
      const alpmpp::AlpmPackage &pkg) const;
  [[nodiscard]] bool FilterPkg(const alpmpp::AlpmPackage &pkg) const;

  QueryOptions options_;
  std::vector<std::string> targets_;
  alpm_db_t *local_db_ = nullptr;
};

}  // namespace pacmanpp

#endif  // PACMANPP_QUERY_HANDLER_H_