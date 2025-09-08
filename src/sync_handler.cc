// SPDX-License-Identifier: MIT

#include "sync_handler.h"

#include <utils.h>

#include <print>

namespace {

void PrintPkgInfo(const aurpp::AurPackage &package) {
  std::string result;

  std::format_to(std::back_inserter(result), "aur/{} {} [+{} ~{:.2}]\n", package.name(), package.version(),
               package.num_votes(), package.popularity());
  if (package.description().has_value()) {
    std::format_to(std::back_inserter(result), "    {}", package.description().value());
  }

  std::println("{}", result);
}

}  // namespace

namespace yarp {

int SyncHandler::Execute() const {
  if ((options_ & SyncOptions::kAur) == SyncOptions::kAur) {
    return SearchAur();
  } else if ((options_ & SyncOptions::kSearch) == SyncOptions::kSearch) {
    return HandleSearch();
  } else {
    return 0;
  }
}

int SyncHandler::HandleSearch() const {
  const int repo_search_result = SearchRepos();
  const int aur_search_result = SearchAur();

  if (repo_search_result == 1 && aur_search_result == 1) {
    std::println("Error: targets not found in either official repos or AUR");
    return 1;
  } else {
    return 0;
  }
}

int SyncHandler::SearchAur() const {
  for (const std::string_view target : targets_) {
    const aurpp::SearchRequest request{aurpp::SearchRequest::SearchBy::kName,
                                       target};
    std::expected<aurpp::RpcResponse, std::string> maybe_response =
        aur_client_->Execute<aurpp::SearchRequest, aurpp::RpcResponse>(request);
    if (maybe_response.has_value()) {
      const std::vector<aurpp::AurPackage> packages =
          maybe_response.value().packages;
      std::ranges::for_each(packages, PrintPkgInfo);
      return 0;
    } else {
      std::println("{}", maybe_response.error());
      return 1;
    }
  }
  return 0;
}

int SyncHandler::SearchRepos() const {
  const int total_errors = std::ranges::fold_left(
             alpm_->GetSyncDbs(), 0,
             [this](const int errors, alpm_db_t *db) {
               std::expected<std::string, std::string> search_result =
                   utils::PrintPkgSearch(db, targets_);
               if (search_result.has_value()) {
                 std::println("{}", search_result.value());
                 return errors;
               } else {
                 return errors + 1;
               }
             });
  return total_errors > 0;
}

}  // namespace yarp
