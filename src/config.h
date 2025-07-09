// SPDX-License-Identifier: MIT

#ifndef PACMANPP_CONFIG_H_
#define PACMANPP_CONFIG_H_

#include <filesystem>

#include "pacman_conf.h"

namespace pacmanpp {

class Config {
 public:
  constexpr Config() = default;

  explicit constexpr Config(const PacmanConf &pacman_conf)
      : pacman_conf_(pacman_conf) {}

  Config(const Config &) = delete;
  Config &operator=(const Config &) = delete;

  Config(Config &&) = delete;
  Config &operator=(Config &&) = delete;

  [[nodiscard]] constexpr bool verbose() const noexcept { return verbose_; }

  [[nodiscard]] constexpr std::string root_dir() const noexcept {
    return pacman_conf_.root_dir();
  }

  [[nodiscard]] constexpr std::string db_path() const noexcept {
    return pacman_conf_.db_path();
  }

  [[nodiscard]] constexpr std::string cache_dir() const noexcept {
    return pacman_conf_.cache_dir();
  }

  [[nodiscard]] constexpr std::string hook_dir() const noexcept {
    return pacman_conf_.hook_dir();
  }

  // [[nodiscard]] constexpr std::string lock_file() const noexcept {
  //   return pacman_conf_.lock_file();
  // }

  [[nodiscard]] constexpr std::string log_file() const noexcept {
    return pacman_conf_.log_file();
  }

  [[nodiscard]] constexpr std::string gpg_dir() const noexcept {
    return pacman_conf_.gpg_dir();
  }

  [[nodiscard]] constexpr const std::vector<Repository> &repos()
      const noexcept {
    return pacman_conf_.repos();
  }

  void ParseFromConfig(const std::string_view path = "/etc/pacman.conf") {
    pacman_conf_.ParseFromFile(path);
  }

  constexpr void set_verbose(const bool new_verbose) { verbose_ = new_verbose; }

  void set_root(const std::string_view new_root_dir) noexcept {
    pacman_conf_.set_root_dir(new_root_dir);
  }

  void set_db_path(const std::string_view new_db_path) noexcept {
    pacman_conf_.set_db_path(new_db_path);
  }

 private:
  bool verbose_ = false;
  PacmanConf pacman_conf_;
};

}  // namespace pacmanpp

#endif  // PACMANPP_CONFIG_H_