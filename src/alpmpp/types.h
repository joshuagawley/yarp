// SPDX-License-Identifier: MIT

#ifndef ALPMPP_TYPES_H_
#define ALPMPP_TYPES_H_

#include <alpm.h>

namespace alpmpp {

enum class PkgReason {
  kExplicit = ALPM_PKG_REASON_EXPLICIT,
  kDepend = ALPM_PKG_REASON_DEPEND,
  kUnknown = ALPM_PKG_REASON_UNKNOWN,
};

}  // namespace alpmpp

#endif  // ALPMPP_TYPES_H_