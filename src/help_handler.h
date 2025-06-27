// SPDX-License-Identifier: MIT

#ifndef PACMANPP_HELP_HANDLER_H_
#define PACMANPP_HELP_HANDLER_H_

#include "alpmpp/alpm.h"
#include "config.h"
#include "operation_handler.h"

namespace pacmanpp {

class HelpHandler : public OperationHandler {
 public:
  constexpr HelpHandler(Config &config, Alpm &alpm)
      : OperationHandler(config, alpm) {}

  virtual ~HelpHandler() = default;

  int Execute() override;
};

}  // namespace pacmanpp

#endif  // PACMANPP_VERSION_HANDLER_H_