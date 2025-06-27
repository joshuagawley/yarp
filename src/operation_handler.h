// SPDX-License-Identifier: MIT

#ifndef PACMANPP_OPERATION_HANDLER_H_
#define PACMANPP_OPERATION_HANDLER_H_

#include "alpmpp/alpm.h"
#include "config.h"

namespace pacmanpp {

class OperationHandler {
 public:
  constexpr OperationHandler(Config &config, Alpm &alpm)
      : config_(config), alpm_(alpm) {}

  virtual ~OperationHandler() = default;

 protected:
  virtual int Execute() = 0;

  Config &config_;
  Alpm &alpm_;
};

}  // namespace pacmanpp

#endif  // PACMANPP_OPERATION_HANDLER_H_