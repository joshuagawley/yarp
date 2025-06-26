// SPDX-License-Identifier: MIT

#ifndef PACMANPP_VERSION_HANDLER_H_
#define PACMANPP_VERSION_HANDLER_H_

#include "alpm.h"
#include "config.h"
#include "operation_handler.h"

namespace pacmanpp {

class VersionHandler : public OperationHandler {
 public:
  constexpr VersionHandler(Config &config, Alpm &alpm)
      : OperationHandler(config, alpm) {}

  virtual ~VersionHandler() = default;

  int Execute() override;
};

}  // namespace pacmanpp

#endif  // PACMANPP_VERSION_HANDLER_H_