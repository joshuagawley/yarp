// SPDX-License-Identifier: MIT

#include "package.h"

#include <format>
#include <functional>

namespace {

constexpr std::vector<std::string> JsonValueToStringVector(
    const Json::Value &value) {
  std::vector<std::string> vec;
  vec.reserve(value.size());
  std::transform(std::begin(value), std::end(value), std::back_inserter(vec),
                 [](const auto &elem) { return elem.asString(); });
  return vec;
}

}  // namespace

namespace aurpp {

AurPackage AurPackage::FromJson(const Json::Value &json) {
  AurPackage pkg{};

  pkg.set_name(json["Name"].asString());
  pkg.set_version(json["Version"].asString());
  if (const Json::Value &desc = json["Description"]; !desc.isNull()) {
    pkg.set_description(desc.asString());
  }
  if (const Json::Value &maintainer = json["Maintainer"];
      !maintainer.isNull()) {
    pkg.set_maintainer(maintainer.asString());
  }
  if (const Json::Value &url = json["URL"]; !url.isNull()) {
    pkg.set_url(url.asString());
  }
  pkg.set_num_votes(json["NumVotes"].asInt());
  pkg.set_popularity(json["Popularity"].asDouble());
  if (const Json::Value &out_of_date = json["OutOfDate"];
      !out_of_date.isNull()) {
    pkg.set_out_of_date(out_of_date.asInt());
  }
  pkg.set_package_base(json["PackageBase"].asString());
  pkg.set_package_base_id(json["PackageBaseID"].asInt());
  pkg.set_first_submitted(json["FirstSubmitted"].asInt());
  pkg.set_last_modified(json["LastModified"].asInt());
  if (const Json::Value &url_path = json["URLPath"]; !url_path.isNull()) {
    pkg.set_url_path(url_path.asString());
  }
  pkg.set_id(json["ID"].asInt());
  if (const Json::Value &depends = json["Depends"]; !depends.isNull()) {
    pkg.set_depends(JsonValueToStringVector(depends));
  }
  if (const Json::Value &make_depends = json["MakeDepends"];
      !make_depends.isNull()) {
    pkg.set_make_depends(JsonValueToStringVector(make_depends));
  }
  if (const Json::Value &opt_depends = json["OptDepends"];
      !opt_depends.isNull()) {
    pkg.set_opt_depends(JsonValueToStringVector(opt_depends));
  }
  if (const Json::Value &check_depends = json["CheckDepends"];
      !check_depends.isNull()) {
    pkg.set_check_depends(JsonValueToStringVector(check_depends));
  }
  if (const Json::Value &conflicts = json["Conflicts"]; !conflicts.isNull()) {
    pkg.set_conflicts(JsonValueToStringVector(conflicts));
  }
  if (const Json::Value &provides = json["Provides"]; !provides.isNull()) {
    pkg.set_provides(JsonValueToStringVector(provides));
  }
  if (const Json::Value &replaces = json["Replaces"]; !replaces.isNull()) {
    pkg.set_replaces(JsonValueToStringVector(replaces));
  }
  if (const Json::Value &groups = json["Groups"]; !groups.isNull()) {
    pkg.set_groups(JsonValueToStringVector(groups));
  }
  if (const Json::Value &license = json["License"]; !license.isNull()) {
    pkg.set_license(JsonValueToStringVector(license));
  }
  if (const Json::Value &keywords = json["Keywords"]; !keywords.isNull()) {
    pkg.set_keywords(JsonValueToStringVector(keywords));
  }

  return pkg;
}

std::string AurPackage::GetGitUrl(std::string_view aur_base_url) const {
  return std::format("{}/{}.git", aur_base_url, package_base_);
}

std::string AurPackage::GetWebUrl(std::string_view aur_base_url) const {
  return std::format("{}/packages/{}", aur_base_url, name_);
}

}  // namespace aurpp