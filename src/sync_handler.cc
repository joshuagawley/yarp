// SPDX-License-Identifier: MIT

#include "sync_handler.h"

#include <print>

namespace yarp {

int SyncHandler::Execute() const {
  if ((options_ & SyncOptions::kAur) == SyncOptions::kAur) {
    return SearchAur();
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
      for (const aurpp::AurPackage &pkg : packages) {
        std::println("{} {}", pkg.name(), pkg.version());
      }
      return 0;
    } else {
      std::println("{}", maybe_response.error());
      return 1;
    }
  }
  return 0;
}

}  // namespace yarp
