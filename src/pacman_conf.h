// SPDX-License-Identifier: MIT

#ifndef PACMANPP_PACMAN_CONF_H_
#define PACMANPP_PACMAN_CONF_H_

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace pacmanpp {

struct Repository {
  std::string name;
  std::vector<std::string> servers;
  std::vector<std::string> sig_level;
  std::vector<std::string> usage;
};

class PacmanConf {
 public:
  void ParseFromFile(const std::string_view config_file);

  // Accessors
  [[nodiscard]] constexpr const std::filesystem::path& root_dir()
      const noexcept {
    return root_dir_;
  }

  [[nodiscard]] constexpr const std::filesystem::path& db_path()
      const noexcept {
    return db_path_;
  }

  [[nodiscard]] constexpr const std::filesystem::path& cache_dir()
      const noexcept {
    return cache_dir_;
  }

  [[nodiscard]] constexpr const std::filesystem::path& hook_dir()
      const noexcept {
    return hook_dir_;
  }

  [[nodiscard]] constexpr const std::filesystem::path& gpg_dir()
      const noexcept {
    return gpg_dir_;
  }

  [[nodiscard]] constexpr const std::filesystem::path& log_file()
      const noexcept {
    return log_file_;
  }

  [[nodiscard]] constexpr const std::vector<std::string>& hold_pkg()
      const noexcept {
    return hold_pkg_;
  }

  [[nodiscard]] constexpr const std::vector<std::string>& ignore_pkg()
      const noexcept {
    return ignore_pkg_;
  }

  [[nodiscard]] constexpr const std::vector<std::string>& ignore_group()
      const noexcept {
    return ignore_group_;
  }

  [[nodiscard]] constexpr const std::vector<std::string>& architecture()
      const noexcept {
    return architecture_;
  }

  [[nodiscard]] constexpr const std::string& xfer_command() const noexcept {
    return xfer_command_;
  }

  [[nodiscard]] constexpr const std::vector<std::string>& no_upgrade()
      const noexcept {
    return no_upgrade_;
  }

  [[nodiscard]] constexpr const std::vector<std::string>& no_extract()
      const noexcept {
    return no_extract_;
  }

  [[nodiscard]] constexpr const std::vector<std::string>& clean_method()
      const noexcept {
    return clean_method_;
  }

  [[nodiscard]] constexpr const std::vector<std::string>& sig_level()
      const noexcept {
    return sig_level_;
  }

  [[nodiscard]] constexpr const std::vector<std::string>& local_file_sig_level()
      const noexcept {
    return local_file_sig_level_;
  }

  [[nodiscard]] constexpr const std::vector<std::string>&
  remote_file_sig_level() const noexcept {
    return remote_file_sig_level_;
  }

  [[nodiscard]] constexpr const std::optional<std::string>& download_user()
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

  [[nodiscard]] constexpr const std::vector<Repository>& repos()
      const noexcept {
    return repos_;
  }

 private:
  std::filesystem::path root_dir_ = "/";
  std::filesystem::path db_path_ = "/var/lib/pacman/";
  std::filesystem::path cache_dir_ = "/var/cache/pacman/pkg/";
  std::filesystem::path hook_dir_ = "/etc/pacman.d/hooks/";
  std::filesystem::path gpg_dir_ = "/etc/pacman.d/gnupg/";
  std::filesystem::path log_file_ = "/var/log/pacman.log";
  std::vector<std::string> hold_pkg_;
  std::vector<std::string> ignore_pkg_;
  std::vector<std::string> ignore_group_;
  std::vector<std::string> architecture_;
  std::string xfer_command_;
  std::vector<std::string> no_upgrade_;
  std::vector<std::string> no_extract_;
  std::vector<std::string> clean_method_;
  std::vector<std::string> sig_level_;
  std::vector<std::string> local_file_sig_level_;
  std::vector<std::string> remote_file_sig_level_;
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

// Mutators
//   constexpr void set_root_dir(std::filesystem::path root_dir) noexcept {
//     root_dir_ = std::move(root_dir);
//   }

//   constexpr void set_db_path(std::filesystem::path db_path) noexcept {
//     db_path_ = std::move(db_path);
//   }

//   constexpr void set_cache_dir(std::filesystem::path cache_dir) noexcept {
//     cache_dir_ = std::move(cache_dir);
//   }

//   constexpr void set_hook_dir(std::filesystem::path hook_dir) noexcept {
//     hook_dir_ = std::move(hook_dir);
//   }

//   constexpr void set_gpg_dir(std::filesystem::path gpg_dir) noexcept {
//     gpg_dir_ = std::move(gpg_dir);
//   }

//   constexpr void set_log_file(std::filesystem::path log_file) noexcept {
//     log_file_ = std::move(log_file);
//   }

//   constexpr void set_hold_pkg(std::vector<std::string> hold_pkg) noexcept {
//     hold_pkg_ = std::move(hold_pkg);
//   }

//   constexpr void set_ignore_pkg(std::vector<std::string> ignore_pkg) noexcept
//   {
//     ignore_pkg_ = std::move(ignore_pkg);
//   }

//   constexpr void set_ignore_group(
//       std::vector<std::string> ignore_group) noexcept {
//     ignore_group_ = std::move(ignore_group);
//   }

//   constexpr void set_architecture(
//       std::vector<std::string> architecture) noexcept {
//     architecture_ = std::move(architecture);
//   }

//   constexpr void set_xfer_command(std::string xfer_command) noexcept {
//     xfer_command_ = std::move(xfer_command);
//   }

//   constexpr void set_no_upgrade(std::vector<std::string> no_upgrade) noexcept
//   {
//     no_upgrade_ = std::move(no_upgrade);
//   }

//   constexpr void set_no_extract(std::vector<std::string> no_extract) noexcept
//   {
//     no_extract_ = std::move(no_extract);
//   }

//   constexpr void set_clean_method(
//       std::vector<std::string> clean_method) noexcept {
//     clean_method_ = std::move(clean_method);
//   }

//   constexpr void set_sig_level(std::vector<std::string> sig_level) noexcept {
//     sig_level_ = std::move(sig_level);
//   }

//   constexpr void set_local_file_sig_level(
//       std::vector<std::string> local_file_sig_level) noexcept {
//     local_file_sig_level_ = std::move(local_file_sig_level);
//   }

//   constexpr void set_remote_file_sig_level(
//       std::vector<std::string> remote_file_sig_level) noexcept {
//     remote_file_sig_level_ = std::move(remote_file_sig_level);
//   }

//   constexpr void set_download_user(
//       std::optional<std::string> download_user) noexcept {
//     download_user_ = std::move(download_user);
//   }

//   constexpr void set_use_syslog(bool use_syslog) noexcept {
//     use_syslog_ = use_syslog;
//   }

//   constexpr void set_color(bool color) noexcept { color_ = color; }

//   constexpr void set_use_delta(double use_delta) noexcept {
//     use_delta_ = use_delta;
//   }
//   constexpr void set_total_download(bool total_download) noexcept {
//     total_download_ = total_download;
//   }
//   constexpr void set_check_space(bool check_space) noexcept {
//     check_space_ = check_space;
//   }
//   constexpr void set_verbose_pkg_lists(bool verbose_pkg_lists) noexcept {
//     verbose_pkg_lists_ = verbose_pkg_lists;
//   }
//   constexpr void set_disable_download_timeout(
//       bool disable_download_timeout) noexcept {
//     disable_download_timeout_ = disable_download_timeout;
//   }
//   constexpr void set_parallel_downloads(int parallel_downloads) noexcept {
//     parallel_downloads_ = parallel_downloads;
//   }
//   constexpr void set_disable_sandbox(bool disable_sandbox) noexcept {
//     disable_sandbox_ = disable_sandbox;
//   }
//   constexpr void set_chomp(bool chomp) noexcept { chomp_ = chomp; }
//   constexpr void set_repos(std::vector<Repository> repos) noexcept {
//     repos_ = std::move(repos);
//   }

}  // namespace pacmanpp

#endif  // PACMANPP_PACMAN_CONF_H_