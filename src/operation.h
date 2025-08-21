// SPDX-License-Identifier: MIT

#ifndef PACMANPP_OPERATION_H_
#define PACMANPP_OPERATION_H_

#include <cstdint>

#include "bitwise_enum.h"

namespace yarp {

enum class Operation : std::uint8_t {
  kNone = 1 << 0,
  kHelp = 1 << 1,
  kQuery = 1 << 2,
  kVersion = 1 << 3,
};

template <>
struct EnableEnumBitwiseOperators<Operation> {
  static constexpr bool enabled = true;
};

enum class QueryOptions : std::uint16_t {
  kNone = 1 << 0,
  kIsFile = 1 << 1,
  kInfo = 1 << 2,
  kList = 1 << 3,
  kUnrequired = 1 << 4,
  kDeps = 1 << 5,
  kExplicit = 1 << 6,
  kOwns = 1 << 7,
  kSearch = 1 << 8,
  kChangelog = 1 << 9,
  kUpgrade = 1 << 10,
  kCheck = 1 << 11,
  kForeign = 1 << 12,
  kNative = 1 << 13,
  kGroups = 1 << 14,
};

template <>
struct EnableEnumBitwiseOperators<QueryOptions> {
  static constexpr bool enabled = true;
};

}  // namespace yarp

#endif  // PACMANPP_OPERATION_H_
