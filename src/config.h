// SPDX-License-Identifier: MIT

#ifndef PACMANPP_CONFIG_H_
#define PACMANPP_CONFIG_H_

#include <filesystem>

namespace pacmanpp {

class Config {
 public:
  constexpr Config() {}

  Config(const Config &) = delete;
  Config &operator=(const Config &) = delete;

  Config(Config &&) = delete;
  Config &operator=(Config &&) = delete;

  [[nodiscard]] constexpr bool IsVerbose() const noexcept { return verbose_; }

  [[nodiscard]] constexpr std::string get_root() const noexcept {
    return root_;
  }

  [[nodiscard]] constexpr std::string get_db_path() const noexcept {
    return db_path_;
  }

  constexpr void set_verbose(bool new_verbose) { verbose_ = new_verbose; }

  void set_root(std::string_view new_root) noexcept { root_ = new_root; }

  void set_db_path(std::string_view new_db_path) noexcept {
    db_path_ = new_db_path;
  }

 private:
  bool verbose_ = false;
  std::filesystem::path root_ = "/";
  std::filesystem::path db_path_ = "/var/lib/pacman";
};

}  // namespace pacmanpp

#endif  // PACMANPP_CONFIG_H_