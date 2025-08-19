// SPDX-License-Identifier: MIT

#include <alpm.h>

#include <alpmpp/depend.h>

namespace alpmpp {

std::string AlpmDepend::ComputeString() const {
  return alpm_dep_compute_string(depend_);
}

}  // namespace alpmpp
