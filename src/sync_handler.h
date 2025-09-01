// SPDX-License-Identifier: MIT

#ifndef YARP_SYNC_HANDLER_H_
#define YARP_SYNC_HANDLER_H_

#include <alpmpp/alpm.h>
#include <client.h>

#include "config.h"
#include "operation.h"

namespace yarp {

class SyncHandler {
 public:
  constexpr SyncHandler(alpmpp::Alpm *alpm, aurpp::Client *aur_client,
                        Config *config, const SyncOptions sync_options,
                        std::vector<std::string> targets)
      : alpm_(alpm),
        aur_client_(aur_client),
        config_(config),
        options_(sync_options),
        targets_(std::move(targets)) {}

  SyncHandler(const SyncHandler &) = delete;
  SyncHandler &operator=(const SyncHandler &) = delete;

  SyncHandler(SyncHandler &&) = default;
  SyncHandler &operator=(SyncHandler &&) = default;

  [[nodiscard]] int Execute() const;

 private:
  [[nodiscard]] int HandleSearch() const;
  [[nodiscard]] int SearchAur() const;
  [[nodiscard]] int SearchRepos() const;

  alpmpp::Alpm *alpm_;
  aurpp::Client *aur_client_;
  Config *config_;
  SyncOptions options_;
  std::vector<std::string> targets_;
};

}  // namespace yarp

#endif  // YARP_SYNC_HANDLER_H_
