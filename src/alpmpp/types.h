// SPDX-License-Identifier: MIT

#ifndef PACMANPP_ALPMPP_TYPES_H_
#define PACMANPP_ALPMPP_TYPES_H_

#include <alpm.h>

namespace alpmpp {

enum class PkgReason {
  kExplicit = ALPM_PKG_REASON_EXPLICIT,
  kDepend = ALPM_PKG_REASON_DEPEND,
  kUnknown = ALPM_PKG_REASON_UNKNOWN,
};

}  // namespace alpmpp

#endif  // PACMANPP_ALPMPP_TYPES_H_