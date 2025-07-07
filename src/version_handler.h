// SPDX-License-Identifier: MIT

#ifndef PACMANPP_VERSION_HANDLER_H_
#define PACMANPP_VERSION_HANDLER_H_

#include "alpmpp/alpm.h"
#include "config.h"
#include "operation_handler.h"

namespace pacmanpp {

class VersionHandler final : public OperationHandler {
 public:
  constexpr VersionHandler(Config &config, alpmpp::Alpm &alpm)
      : OperationHandler(config, alpm) {}

  ~VersionHandler() override = default;

  int Execute() override;
};

}  // namespace pacmanpp

#endif  // PACMANPP_VERSION_HANDLER_H_