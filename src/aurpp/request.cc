// SPDX-License-Identifier: MIT

#include "request.h"

#include <format>
#include <sstream>

namespace aurpp {

std::string RpcRequest::Payload() const {
  std::stringstream ss{};

  for (const QueryParameter &param : params_) {
    if (param != params_.back()) {
      std::print(ss, "{}&", param);
    } else {
      std::print(ss, "{}", param);
    }
  }

  return ss.str();
}

}  // namespace aurpp