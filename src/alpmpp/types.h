// SPDX-License-Identifier: MIT

#ifndef PACMANPP_ALPMPP_TYPES_H_
#define PACMANPP_ALPMPP_TYPES_H_

#include <alpm.h>

namespace alpmpp {

enum class AlpmPkgReason {
  kExplicit = ALPM_PKG_REASON_EXPLICIT,
  kUnknown = ALPM_PKG_REASON_UNKNOWN,
  kDepend = ALPM_PKG_REASON_DEPEND
};

}  // namespace alpmpp

#endif  // PACMANPP_ALPMPP_TYPES_H_