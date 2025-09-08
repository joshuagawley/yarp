// SPDX-License-Identifier: MIT

#include "request.h"

#include <format>
#include <sstream>

namespace aurpp {

std::string RpcRequest::Payload() const {
  std::string result;

  for (const QueryParameter &param : params_) {
    if (param != params_.back()) {
      std::format_to(std::back_inserter(result), "{}&", param);
    } else {
      std::format_to(std::back_inserter(result), "{}", param);
    }
  }

  return result;
}

}  // namespace aurpp