// SPDX-License-Identifier: MIT

#ifndef AURPP_PACKAGE_H
#define AURPP_PACKAGE_H

#include <json/json.h>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace aurpp {

class AurPackage {
 public:
  constexpr AurPackage() = default;

  static AurPackage FromJson(const Json::Value &json);

      [[nodiscard]] constexpr std::string_view name() const noexcept {
    return name_;
  }

  [[nodiscard]] constexpr std::string_view version() const noexcept {
    return version_;
  }

  [[nodiscard]] constexpr std::optional<std::string_view> description()
      const noexcept {
    return description_;
  }

  [[nodiscard]] constexpr std::optional<std::string_view> maintainer()
      const noexcept {
    return maintainer_;
  }

  [[nodiscard]] constexpr std::optional<std::string_view> url() const noexcept {
    return url_;
  }

  [[nodiscard]] constexpr std::uint64_t num_votes() const noexcept {
    return num_votes_;
  }

  [[nodiscard]] constexpr double popularity() const noexcept {
    return popularity_;
  }

  [[nodiscard]] constexpr std::optional<std::uint64_t> out_of_date()
      const noexcept {
    return out_of_date_;
  }

  [[nodiscard]] constexpr std::string_view package_base() const noexcept {
    return package_base_;
  }

  [[nodiscard]] constexpr std::int64_t package_base_id() const noexcept {
    return package_base_id_;
  }

  [[nodiscard]] constexpr std::int64_t first_submitted() const noexcept {
    return first_submitted_;
  }

  [[nodiscard]] constexpr std::int64_t last_modified() const noexcept {
    return last_modified_;
  }

  [[nodiscard]] constexpr std::optional<std::string_view> url_path()
      const noexcept {
    return url_path_;
  }

  [[nodiscard]] constexpr std::int64_t id() const noexcept { return id_; }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>> depends()
      const noexcept {
    return depends_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>> make_depends()
      const noexcept {
    return make_depends_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>> opt_depends()
      const noexcept {
    return opt_depends_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>>
  check_depends() const noexcept {
    return check_depends_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>> conflicts()
      const noexcept {
    return conflicts_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>> provides()
      const noexcept {
    return provides_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>> replaces()
      const noexcept {
    return replaces_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>> groups()
      const noexcept {
    return groups_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>> license()
      const noexcept {
    return license_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>> keywords()
      const noexcept {
    return keywords_;
  }

  constexpr void set_name(const std::string_view name) { name_ = name; }

  constexpr void set_version(const std::string_view version) {
    version_ = version;
  }

  constexpr void set_description(const std::string_view description) {
    description_ = description;
  }

  constexpr void set_maintainer(const std::string_view maintainer) {
    maintainer_ = maintainer;
  }

  constexpr void set_url(const std::string_view url) { url_ = url; }

  constexpr void set_num_votes(const std::uint64_t num_votes) {
    num_votes_ = num_votes;
  }

  constexpr void set_popularity(const double popularity) {
    popularity_ = popularity;
  }

  constexpr void set_out_of_date(const std::uint64_t out_of_date) {
    out_of_date_ = out_of_date;
  }

  constexpr void set_package_base(const std::string_view package_base) {
    package_base_ = package_base;
  }

  constexpr void set_package_base_id(const std::int64_t package_base_id) {
    package_base_id_ = package_base_id;
  }

  constexpr void set_first_submitted(const std::int64_t first_submitted) {
    first_submitted_ = first_submitted;
  }

  constexpr void set_last_modified(const std::int64_t last_modified) {
    last_modified_ = last_modified;
  }

  constexpr void set_url_path(const std::string_view url_path) {
    url_path_ = url_path;
  }

  constexpr void set_id(const std::int64_t id) { id_ = id; }

  constexpr void set_depends(std::vector<std::string> depends) {
    depends_ = std::move(depends);
  }
  constexpr void set_make_depends(std::vector<std::string> make_depends) {
    make_depends_ = std::move(make_depends);
  }

  constexpr void set_opt_depends(std::vector<std::string> opt_depends) {
    opt_depends_ = std::move(opt_depends);
  }

  constexpr void set_check_depends(std::vector<std::string> check_depends) {
    check_depends_ = std::move(check_depends);
  }

  constexpr void set_conflicts(std::vector<std::string> conflicts) {
    conflicts_ = std::move(conflicts);
  }

  constexpr void set_provides(std::vector<std::string> provides) {
    provides_ = std::move(provides);
  }

  constexpr void set_replaces(std::vector<std::string> replaces) {
    replaces_ = std::move(replaces);
  }

  constexpr void set_groups(std::vector<std::string> groups) {
    groups_ = std::move(groups);
  }

  constexpr void set_license(std::vector<std::string> license) {
    license_ = std::move(license);
  }

  constexpr void set_keywords(std::vector<std::string> keywords) {
    keywords_ = std::move(keywords);
  }

  [[nodiscard]] std::string GetGitUrl(std::string_view aur_base_url) const;

  [[nodiscard]] std::string GetWebUrl(std::string_view aur_base_url) const;

 private:
  std::string name_;
  std::string version_;
  std::optional<std::string> description_;
  std::optional<std::string> maintainer_;
  std::optional<std::string> url_;
  std::uint64_t num_votes_{};
  double popularity_{};
  std::optional<std::uint64_t> out_of_date_;
  std::string package_base_;
  std::int64_t package_base_id_{};
  std::int64_t first_submitted_{};
  std::int64_t last_modified_{};
  std::optional<std::string> url_path_;
  std::int64_t id_{};

  // The following fields are optional and only returned when using
  // the `multiinfo` method
  std::optional<std::vector<std::string>> depends_;
  std::optional<std::vector<std::string>> make_depends_;
  std::optional<std::vector<std::string>> opt_depends_;
  std::optional<std::vector<std::string>> check_depends_;
  std::optional<std::vector<std::string>> conflicts_;
  std::optional<std::vector<std::string>> provides_;
  std::optional<std::vector<std::string>> replaces_;
  std::optional<std::vector<std::string>> groups_;
  std::optional<std::vector<std::string>> license_;
  std::optional<std::vector<std::string>> keywords_;
};

}  // namespace aurpp

#endif  // AURPP_PACKAGE_H
