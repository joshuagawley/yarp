// SPDX-License-Identifier: MIT

#ifndef PACMANPP_SRC_OPERATION_H_
#define PACMANPP_SRC_OPERATION_H_

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

}  // namespace pacmanpp

#endif  // PACMANPP_SRC_OPERATION_H_
