// SPDX-License-Identifier: MIT

#include "depend.h"

#include <alpm.h>

namespace alpmpp {

std::string AlpmDepend::ComputeString() const {
  return alpm_dep_compute_string(depend_);
}

}  // namespace alpmpp
