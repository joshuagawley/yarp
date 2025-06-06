// SPDX-License-Identifier: MIT

#ifndef PACMANPP_SRC_OPERATION_H_
#define PACMANPP_SRC_OPERATION_H_

#include <cstdint>

#include "src/bitwise_enum.h"

namespace pacmanpp {

enum class Operation : std::uint8_t {
  kNone = 1 << 0,
  kHelp = 1 << 1,
  kQuery = 1 << 2,
};

template <>
struct util::EnableEnumBitwiseOperators<Operation> {
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
  kLocality = 1 << 12,
};

template <>
struct util::EnableEnumBitwiseOperators<QueryOptions> {
  static constexpr bool enabled = true;
};

}  // namespace pacmanpp

#endif  // PACMANPP_SRC_OPERATION_H_
