// SPDX-License-Identifier: MIT

#ifndef PACMANPP_SRC_OPERATION_H_
#define PACMANPP_SRC_OPERATION_H_

#include <cstdint>

#include "src/bitwise_enum.h"

namespace pacmanpp {

enum class Operation {
  kNone = 1 << 0,
  kHelp = 1 << 1,
  kQuery = 1 << 2,
};

template <>
struct util::EnableEnumBitwiseOperators<Operation> {
  static constexpr bool enabled = true;
};

enum class QueryOptions : std::uint16_t {
  kIsFile = 1 << 0,
  kInfo = 1 << 1,
  kList = 1 << 2,
  kUnrequired = 1 << 3,
  kDeps = 1 << 4,
  kExplicit = 1 << 5,
  kOwns = 1 << 6,
  kSearch = 1 << 7,
  kChangelog = 1 << 8,
  kUpgrade = 1 << 9,
  kCheck = 1 << 10,
  kLocality = 1 << 11,
};

template <>
struct util::EnableEnumBitwiseOperators<QueryOptions> {
  static constexpr bool enabled = true;
};

}  // namespace pacmanpp

#endif  // PACMANPP_SRC_OPERATION_H_
