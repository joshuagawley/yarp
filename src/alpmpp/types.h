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

enum class SigLevel {
  kPackage = ALPM_SIG_PACKAGE,
  kPackageOptional = ALPM_SIG_PACKAGE_OPTIONAL,
  kPackageMarginalOk = ALPM_SIG_PACKAGE_MARGINAL_OK,
  kPackageUnknownOk = ALPM_SIG_PACKAGE_UNKNOWN_OK,

  kDatabase = ALPM_SIG_DATABASE,
  kDatabaseOptional = ALPM_SIG_DATABASE_OPTIONAL,
  kDatabaseMarginalOk = ALPM_SIG_DATABASE_MARGINAL_OK,
  kDatabaseUnknownOk = ALPM_SIG_DATABASE_UNKNOWN_OK,

  kUseDefault = ALPM_SIG_USE_DEFAULT,
};

template <>
struct EnableEnumBitwiseOperators<SigLevel> {
  static constexpr bool enabled = true;
};

enum class DbUsage {
  kSync = ALPM_DB_USAGE_SYNC,
  kSearch = ALPM_DB_USAGE_SEARCH,
  kInstall = ALPM_DB_USAGE_INSTALL,
  kUpgrade = ALPM_DB_USAGE_UPGRADE,
  kAll = ALPM_DB_USAGE_ALL,
};

template <>
struct EnableEnumBitwiseOperators<DbUsage> {
  static constexpr bool enabled = true;
};

}  // namespace alpmpp

#endif  // ALPMPP_TYPES_H_