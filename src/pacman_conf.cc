// SPDX-License-Identifier: MIT

#include "pacman_conf.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

namespace pacmanpp {

namespace {

constexpr auto IsNotSpace = [](const unsigned char ch) {
  return !std::isspace(ch);
};

inline void LeftTrim(std::string& str) {
  str.erase(std::begin(str),
            std::find_if(std::begin(str), std::end(str), IsNotSpace));
}

inline void RightTrim(std::string& str) {
  str.erase(std::find_if(std::rbegin(str), std::rend(str), IsNotSpace).base(),
            std::end(str));
}

inline void Trim(std::string& str) {
  LeftTrim(str);
  RightTrim(str);
}

inline std::string RightTrimCopy(std::string str) {
  RightTrim(str);
  return str;
}

inline std::string LeftxTrimCopy(std::string str) {
  LeftTrim(str);
  return str;
}

inline std::string TrimCopy(std::string str) {
  Trim(str);
  return str;
}

std::vector<std::string> SplitByWhitespace(std::string str) {
  std::vector<std::string> result;
  std::size_t pos = 0;

  while ((pos = str.find(' ')) != std::string::npos) {
    result.emplace_back(str.substr(0, pos));
    str.erase(0, pos + 1);
  }
  result.push_back(str);

  return result;
}

}  // namespace

void PacmanConf::ParseFromFile(const std::string_view config_file) {
  std::ifstream file{std::string(config_file)};
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open config file: " +
                             std::string(config_file));
  }
  std::string line;
  std::string current_section;
  Repository* current_repo = nullptr;

  while (std::getline(file, line)) {
    // Remove comments
    std::size_t comment_pos = line.find('#');
    if (comment_pos != std::string::npos) {
      line = line.substr(0, comment_pos);
    }

    Trim(line);
    if (line.empty()) {
      continue;
    }

    // Check for section header
    if (line.front() == '[' && line.back() == ']') {
      current_section = line.substr(1, line.length() - 2);

      // If it's not the options section, it's a repository
      if (current_section != "options") {
        repos_.emplace_back();
        current_repo = &repos_.back();
        current_repo->name = current_section;
      } else {
        current_repo = nullptr;
      }
      continue;
    }

    // Parse key-value pairs
    size_t eq_pos = line.find('=');
    std::string key;
    std::string value;

    if (eq_pos != std::string::npos) {
      key = line.substr(0, eq_pos);
      value = line.substr(eq_pos + 1);

      Trim(key);
      Trim(value);
    } else {
      // Some options don't have values (like UseSyslog, Color, etc.)
      key = line;
    }

    // Process based on current section
    if (current_section == "options") {
      if (key == "RootDir") {
        root_dir_ = value;
      } else if (key == "DBPath") {
        db_path_ = value;
      } else if (key == "CacheDir") {
        cache_dir_ = value;
      } else if (key == "LogFile") {
        log_file_ = value;
      } else if (key == "GPGDir") {
        gpg_dir_ = value;
      } else if (key == "HookDir") {
        hook_dir_ = value;
      } else if (key == "HoldPkg") {
        hold_pkg_ = SplitByWhitespace(value);
      } else if (key == "IgnorePkg") {
        ignore_pkg_ = SplitByWhitespace(value);
      } else if (key == "IgnoreGroup") {
        ignore_group_ = SplitByWhitespace(value);
      } else if (key == "Architecture") {
        if (value == "auto") {
          // In a real implementation, we'd detect the architecture
          architecture_ = {"x86_64"};
        } else {
          architecture_ = SplitByWhitespace(value);
        }
      } else if (key == "XferCommand") {
        xfer_command_ = value;
      } else if (key == "NoUpgrade") {
        no_upgrade_ = SplitByWhitespace(value);
      } else if (key == "NoExtract") {
        no_extract_ = SplitByWhitespace(value);
      } else if (key == "CleanMethod") {
        clean_method_ = SplitByWhitespace(value);
      } else if (key == "SigLevel") {
        sig_level_ = SplitByWhitespace(value);
      } else if (key == "LocalFileSigLevel") {
        local_file_sig_level_ = SplitByWhitespace(value);
      } else if (key == "RemoteFileSigLevel") {
        remote_file_sig_level_ = SplitByWhitespace(value);
      } else if (key == "DownloadUser") {
        download_user_ = value;
      } else if (key == "UseSyslog") {
        use_syslog_ = true;
      } else if (key == "Color") {
        color_ = true;
      } else if (key == "UseDelta" && !value.empty()) {
        use_delta_ = std::stod(value);
      } else if (key == "TotalDownload") {
        total_download_ = true;
      } else if (key == "CheckSpace") {
        check_space_ = true;
      } else if (key == "VerbosePkgLists") {
        verbose_pkg_lists_ = true;
      } else if (key == "DisableDownloadTimeout") {
        disable_download_timeout_ = true;
      } else if (key == "ParallelDownloads" && !value.empty()) {
        parallel_downloads_ = std::stoi(value);
      } else if (key == "DisableSandbox") {
        disable_sandbox_ = true;
      } else if (key == "ILoveCandy") {
        chomp_ = true;
      }
    } else if (current_repo != nullptr) {
      // Repository section
      if (key == "Server") {
        current_repo->servers.push_back(value);
      } else if (key == "Include") {
        // In a real implementation, we'd parse the included file
        // For now, just store the include path
        current_repo->servers.push_back("Include:" + value);
      } else if (key == "SigLevel") {
        current_repo->sig_level = SplitByWhitespace(value);
      } else if (key == "Usage") {
        current_repo->usage = SplitByWhitespace(value);
      }
    }
  }
}

}  // namespace pacmanpp