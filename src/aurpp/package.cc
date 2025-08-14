// SPDX-License-Identifier: MIT

#include "package.h"

#include <format>

namespace aurpp {

std::string AurPackage::GetGitUrl(std::string_view aur_base_url) const {
  return std::format("{}/{}.git", aur_base_url, package_base_);
}

std::string AurPackage::GetWebUrl(std::string_view aur_base_url) const {
  return std::format("{}/packages/{}", aur_base_url, name_);
}

}  // namespace aurpp