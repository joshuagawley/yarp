// SPDX-License-Identifier: MIT

#ifndef PACMANPP_QUERY_HANDLER_H_
#define PACMANPP_QUERY_HANDLER_H_

#include "alpmpp/alpm.h"
#include "config.h"
#include "operation.h"

namespace pacmanpp {

enum class PkgLocality { kUnset = 1 << 0, kNative = 1 << 1, kForeign = 1 << 2 };

template <>
struct EnableEnumBitwiseOperators<PkgLocality> {
  static constexpr bool enabled = true;
};

class QueryHandler {
 public:
  constexpr QueryHandler(alpmpp::Alpm *alpm, Config *config,
                         const QueryOptions query_options,
                         std::vector<std::string> targets)
      : alpm_(alpm),
        config_(config),
        options_(query_options),
        targets_(std::move(targets)),
        local_db_(alpm_->GetLocalDb()) {}

  QueryHandler(const QueryHandler &) = delete;
  QueryHandler &operator=(const QueryHandler &) = delete;

  QueryHandler(QueryHandler &&) = default;
  QueryHandler &operator=(QueryHandler &&) = default;

  int Execute();

 private:
  [[nodiscard]] int HandleGroups() const;
  [[nodiscard]] int HandleOwns() const;
  [[nodiscard]] int HandleSearch() const;
  [[nodiscard]] std::vector<alpmpp::AlpmPackage> GetPkgList() const;
  void PrintPkgFileList(const alpmpp::AlpmPackage &pkg) const;
  void CheckPkgFiles(const alpmpp::AlpmPackage &pkg) const;
  [[nodiscard]] PkgLocality GetPkgLocality(
      const alpmpp::AlpmPackage &pkg) const;
  [[nodiscard]] bool FilterPkg(const alpmpp::AlpmPackage &pkg) const;
  [[nodiscard]] bool IsUpgradable(const alpmpp::AlpmPackage &pkg) const;
  [[nodiscard]] std::expected<void, std::string> PrintPkgSearch() const;

  alpmpp::Alpm *alpm_;
  Config *config_;
  QueryOptions options_;
  std::vector<std::string> targets_;
  alpm_db_t *local_db_ = nullptr;
};

}  // namespace pacmanpp

#endif  // PACMANPP_QUERY_HANDLER_H_