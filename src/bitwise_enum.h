// SPDX-License-Identifier: MIT

#ifndef PACMANPP_SRC_BITWISE_ENUM_H_
#define PACMANPP_SRC_BITWISE_ENUM_H_

#include <type_traits>
#include <utility>

namespace pacmanpp {

template <typename Enum>
struct EnableEnumBitwiseOperators {
  static constexpr bool enabled = false;
};

template <typename Enum>
concept BitwiseEnum =
    std::is_enum_v<Enum> && EnableEnumBitwiseOperators<Enum>::enabled;

template <typename Enum>
  requires BitwiseEnum<Enum>
constexpr Enum operator|(const Enum lhs, const Enum rhs) {
  return static_cast<Enum>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

template <typename Enum>
  requires BitwiseEnum<Enum>
constexpr Enum operator&(const Enum lhs, const Enum rhs) {
  return static_cast<Enum>(std::to_underlying(lhs) & std::to_underlying(rhs));
}

template <typename Enum>
  requires BitwiseEnum<Enum>
constexpr Enum operator^(const Enum lhs, const Enum rhs) {
  return static_cast<Enum>(std::to_underlying(lhs) ^ std::to_underlying(rhs));
}

template <typename Enum>
  requires BitwiseEnum<Enum>
constexpr Enum operator~(const Enum lhs) {
  return static_cast<Enum>(~std::to_underlying(lhs));
}

template <typename Enum>
  requires BitwiseEnum<Enum>
Enum &operator|=(Enum &lhs, const Enum rhs) {
  lhs = static_cast<Enum>(std::to_underlying(lhs) | std::to_underlying(rhs));
  return lhs;
}

template <typename Enum>
  requires BitwiseEnum<Enum>
Enum &operator&=(Enum &lhs, const Enum rhs) {
  lhs = static_cast<Enum>(std::to_underlying(lhs) & std::to_underlying(rhs));
  return lhs;
}

template <typename Enum>
  requires BitwiseEnum<Enum>
Enum &operator^=(Enum &lhs, const Enum rhs) {
  lhs = static_cast<Enum>(std::to_underlying(lhs) ^ std::to_underlying(rhs));
  return lhs;
}

}  // namespace pacmanpp

#endif  // PACMANPP_SRC_BITWISE_ENUM_H_
