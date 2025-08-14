// SPDX-License-Identifier: MIT

#ifndef AURPP_PACKAGE_H
#define AURPP_PACKAGE_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace aurpp {

class AurPackage {
 public:
  constexpr AurPackage() = default;

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

  [[nodiscard]] constexpr std::optional<std::vector<std::string>>
  depends() const noexcept {
    return depends_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>>
  make_depends() const noexcept {
    return make_depends_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>>
  opt_depends() const noexcept {
    return opt_depends_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>>
  check_depends() const noexcept {
    return check_depends_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>>
  conflicts() const noexcept {
    return conflicts_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>>
  provides() const noexcept {
    return provides_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>>
  replaces() const noexcept {
    return replaces_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>>
  groups() const noexcept {
    return groups_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>>
  license() const noexcept {
    return license_;
  }

  [[nodiscard]] constexpr std::optional<std::vector<std::string>>
  keywords() const noexcept {
    return keywords_;
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
