// SPDX-License-Identifier: MIT

#ifndef PACMANPP_SRC_CONFIG_H_
#define PACMANPP_SRC_CONFIG_H_

#include <filesystem>

namespace pacmanpp {

class Config {
 public:
  Config(const Config &) = delete;
  Config &operator=(const Config &) = delete;

  static Config &get() noexcept {
    static Config instance;
    return instance;
  }

  constexpr std::string get_root() const noexcept { return root_; }

  constexpr std::string get_db_path() const noexcept { return db_path_; }

  void set_root(std::string_view new_root) noexcept { root_ = new_root; }

  void set_db_path(std::string_view new_db_path) noexcept {
    db_path_ = new_db_path;
  }

 private:
  Config() {}

  std::filesystem::path root_ = "/";
  std::filesystem::path db_path_ = "/var/lib/pacman";
};

}  // namespace pacmanpp

#endif  // PACMANPP_SRC_CONFIG_H_