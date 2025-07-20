// SPDX-License-Identifier: MIT

#ifndef PACMANPP_PACMAN_CONF_H_
#define PACMANPP_PACMAN_CONF_H_

#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "alpmpp/types.h"

namespace pacmanpp {

namespace detail {

struct ParseState;

}  // namespace detail

struct Repository {
  std::string name;
  std::vector<std::string> servers;
  alpmpp::SigLevel sig_level;
  std::vector<std::string> usage;
};

class PacmanConf {
 public:
  std::expected<void, std::string> ParseFromFile(
      const std::filesystem::path &config_file);

  // Accessors
  [[nodiscard]] constexpr const std::filesystem::path &root_dir()
      const noexcept {
    return root_dir_;
  }

  [[nodiscard]] constexpr const std::filesystem::path &db_path()
      const noexcept {
    return db_path_;
  }

  [[nodiscard]] constexpr const std::vector<std::string> &cache_dirs()
      const noexcept {
    return cache_dirs_;
  }

  [[nodiscard]] constexpr const std::vector<std::string> &hook_dirs()
      const noexcept {
    return hook_dirs_;
  }

  [[nodiscard]] constexpr const std::filesystem::path &gpg_dir()
      const noexcept {
    return gpg_dir_;
  }

  [[nodiscard]] constexpr const std::filesystem::path &log_file()
      const noexcept {
    return log_file_;
  }

  [[nodiscard]] constexpr const std::vector<std::string> &hold_pkg()
      const noexcept {
    return hold_pkg_;
  }

  [[nodiscard]] constexpr const std::vector<std::string> &ignore_pkg()
      const noexcept {
    return ignore_pkg_;
  }

  [[nodiscard]] constexpr const std::vector<std::string> &ignore_group()
      const noexcept {
    return ignore_group_;
  }

  [[nodiscard]] constexpr const std::vector<std::string> &architecture()
      const noexcept {
    return architecture_;
  }

  [[nodiscard]] constexpr const std::string &xfer_command() const noexcept {
    return xfer_command_;
  }

  [[nodiscard]] constexpr const std::vector<std::string> &no_upgrade()
      const noexcept {
    return no_upgrade_;
  }

  [[nodiscard]] constexpr const std::vector<std::string> &no_extract()
      const noexcept {
    return no_extract_;
  }

  [[nodiscard]] constexpr const std::vector<std::string> &clean_method()
      const noexcept {
    return clean_method_;
  }

  [[nodiscard]] constexpr alpmpp::SigLevel sig_level() const noexcept {
    return sig_level_;
  }

  [[nodiscard]] constexpr alpmpp::SigLevel local_file_sig_level()
      const noexcept {
    return local_file_sig_level_;
  }

  [[nodiscard]] constexpr alpmpp::SigLevel remote_file_sig_level()
      const noexcept {
    return remote_file_sig_level_;
  }

  [[nodiscard]] constexpr const std::optional<std::string> &download_user()
      const noexcept {
    return download_user_;
  }

  [[nodiscard]] constexpr bool use_syslog() const noexcept {
    return use_syslog_;
  }

  [[nodiscard]] constexpr bool color() const noexcept { return color_; }

  [[nodiscard]] constexpr double use_delta() const noexcept {
    return use_delta_;
  }

  [[nodiscard]] constexpr bool total_download() const noexcept {
    return total_download_;
  }

  [[nodiscard]] constexpr bool check_space() const noexcept {
    return check_space_;
  }

  [[nodiscard]] constexpr bool verbose_pkg_lists() const noexcept {
    return verbose_pkg_lists_;
  }

  [[nodiscard]] constexpr bool disable_download_timeout() const noexcept {
    return disable_download_timeout_;
  }

  [[nodiscard]] constexpr int parallel_downloads() const noexcept {
    return parallel_downloads_;
  }

  [[nodiscard]] constexpr bool disable_sandbox() const noexcept {
    return disable_sandbox_;
  }

  [[nodiscard]] constexpr bool chomp() const noexcept { return chomp_; }

  [[nodiscard]] constexpr const std::vector<Repository> &repos()
      const noexcept {
    return repos_;
  }

  constexpr void set_root_dir(const std::string_view root_dir) {
    root_dir_ = root_dir;
  }

  constexpr void set_db_path(const std::string_view db_path) {
    db_path_ = db_path;
  }

 private:
  std::expected<void, std::string> ParseOneFile(
      const std::filesystem::path &path, detail::ParseState &state);

  std::filesystem::path root_dir_ = "/";
  std::filesystem::path db_path_ = "/var/lib/pacman/";
  std::vector<std::string> cache_dirs_ = {"/var/cache/pacman/pkg/"};
  std::vector<std::string> hook_dirs_ = {"/usr/share/libalpm/hooks/",
                                         "/etc/pacman.d/hooks/"};
  std::filesystem::path gpg_dir_ = "/etc/pacman.d/gnupg/";
  std::filesystem::path log_file_ = "/var/log/pacman.log/";
  std::vector<std::string> hold_pkg_;
  std::vector<std::string> ignore_pkg_;
  std::vector<std::string> ignore_group_;
  std::vector<std::string> architecture_;
  std::string xfer_command_;
  std::vector<std::string> no_upgrade_;
  std::vector<std::string> no_extract_;
  std::vector<std::string> clean_method_;
  alpmpp::SigLevel sig_level_{};
  alpmpp::SigLevel local_file_sig_level_{};
  alpmpp::SigLevel remote_file_sig_level_{};
  std::optional<std::string> download_user_;
  bool use_syslog_ = false;
  bool color_ = false;
  double use_delta_ = 0.0;
  bool total_download_ = false;
  bool check_space_ = false;
  bool verbose_pkg_lists_ = false;
  bool disable_download_timeout_ = false;
  int parallel_downloads_ = 1;
  bool disable_sandbox_ = false;
  bool chomp_ = false;
  std::vector<Repository> repos_;
};

}  // namespace pacmanpp

#endif  // PACMANPP_PACMAN_CONF_H_