// SPDX-License-Identifier: MIT

#ifndef AURPP_RESPONSE_H_
#define AURPP_RESPONSE_H_

#include <aurpp/package.h>

#include <expected>
#include <fstream>
#include <string>
#include <vector>

namespace aurpp {

struct RpcResponse {
  static std::expected<RpcResponse, std::string> Parse(
      std::ifstream &file_contents);

  constexpr RpcResponse() = default;
  constexpr explicit RpcResponse(std::vector<AurPackage> packages)
      : packages(std::move(packages)) {}

  constexpr RpcResponse(const RpcResponse &) = default;
  constexpr RpcResponse &operator=(const RpcResponse &) = default;

  constexpr RpcResponse(RpcResponse &&) = default;
  constexpr RpcResponse &operator=(RpcResponse &&) = default;

  std::vector<AurPackage> packages;
};

struct RawResponse {
  constexpr RawResponse() = default;
  constexpr explicit RawResponse(std::string bytes) : bytes(std::move(bytes)) {}

  constexpr RawResponse(const RawResponse &) = default;
  constexpr RawResponse &operator=(const RawResponse &) = default;

  constexpr RawResponse(RawResponse &&) = default;
  constexpr RawResponse &operator=(RawResponse &&) = default;

  std::string bytes;
};

}  // namespace aurpp

#endif  // AURPP_RESPONSE_H_
