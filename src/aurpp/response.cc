// SPDX-License-Identifier: MIT

#include "response.h"

namespace aurpp {

std::expected<RpcResponse, std::string> RpcResponse::Parse(
    std::ifstream &file_contents) {
  Json::CharReaderBuilder reader_builder;
  reader_builder["collectComments"] = false;
  Json::Value json;
  std::string errors;

  const bool success =
      Json::parseFromStream(reader_builder, file_contents, &json, &errors);
  if (!success) {
    return std::unexpected{errors};
  }

  std::vector<AurPackage> packages;
  for (const Json::Value &info : json["results"]) {
    packages.push_back(AurPackage::FromJson(info));
  }
  return RpcResponse{packages};
}

}  // namespace aurpp