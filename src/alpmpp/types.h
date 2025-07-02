// SPDX-License-Identifier: MIT

#ifndef ALPMPP_TYPES_H_
#define ALPMPP_TYPES_H_

#include <alpm.h>

#include "bitwise_enum.h"

namespace alpmpp {

enum class PkgReason {
  kExplicit = ALPM_PKG_REASON_EXPLICIT,
  kDepend = ALPM_PKG_REASON_DEPEND,
  kUnknown = ALPM_PKG_REASON_UNKNOWN,
};

enum class PkgValidation {
  kUnknown = ALPM_PKG_VALIDATION_UNKNOWN,
  kNone = ALPM_PKG_VALIDATION_NONE,
  kMd5 = ALPM_PKG_VALIDATION_MD5SUM,
  kSha256 = ALPM_PKG_VALIDATION_SHA256SUM,
  kSignature = ALPM_PKG_VALIDATION_SIGNATURE
};

template <>
struct EnableEnumBitwiseOperators<PkgValidation> {
  static constexpr bool enabled = true;
};

}  // namespace alpmpp

#endif  // ALPMPP_TYPES_H_