// SPDX-License-Identifier: MIT

#include "pacman_conf.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <ranges>
#include <stdexcept>
#include <string>

namespace pacmanpp {

namespace {
constexpr auto IsNotSpace = [](const unsigned char ch) {
  return !std::isspace(ch);
};

void LeftTrim(std::string& str) {
  str.erase(std::begin(str), std::ranges::find_if(str, IsNotSpace));
}

void RightTrim(std::string& str) {
  str.erase(std::find_if(std::rbegin(str), std::rend(str), IsNotSpace).base(),
            std::end(str));
}

void Trim(std::string& str) {
  LeftTrim(str);
  RightTrim(str);
}

std::string RightTrimCopy(std::string str) {
  RightTrim(str);
  return str;
}

std::string LeftTrimCopy(std::string str) {
  LeftTrim(str);
  return str;
}

std::string TrimCopy(std::string str) {
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

alpmpp::SigLevel ParseSigLevel(const std::string& sig_level_str) {
  alpmpp::SigLevel result{};

  for (const std::string_view sig_level : SplitByWhitespace(sig_level_str)) {
    std::string tail{};

    const std::size_t kPackagePos = sig_level.find("Package");
    const std::size_t kDatabasePos = sig_level.find("Database");

    bool kIsPackage = kPackagePos != std::string_view::npos;
    bool kIsDatabase = kDatabasePos != std::string_view::npos;

    if (kIsDatabase) {
      tail = sig_level.substr(8);
    } else if (kIsPackage) {
      tail = sig_level.substr(7);
    } else {
      // Flag affects both databases and packages
      kIsDatabase = true;
      kIsPackage = true;
      tail = sig_level;
    }

    if (tail == "Never") {
      if (kIsDatabase) {
        result &= ~alpmpp::SigLevel::kDatabase;
      }

      if (kIsPackage) {
        result &= ~alpmpp::SigLevel::kPackage;
      }
    } else if (tail == "Optional") {
      if (kIsDatabase) {
        result |=
            (alpmpp::SigLevel::kDatabase | alpmpp::SigLevel::kDatabaseOptional);
      }
      if (kIsPackage) {
        result |=
            (alpmpp::SigLevel::kPackage | alpmpp::SigLevel::kPackageOptional);
      }
    } else if (tail == "Required") {
      if (kIsDatabase) {
        result |= alpmpp::SigLevel::kDatabase;
        result &= ~alpmpp::SigLevel::kDatabaseOptional;
      }
      if (kIsPackage) {
        result |= alpmpp::SigLevel::kPackage;
        result &= ~alpmpp::SigLevel::kPackageOptional;
      }
    } else if (tail == "TrustOnly") {
      if (kIsDatabase) {
        result &= ~(alpmpp::SigLevel::kDatabaseMarginalOk |
                   alpmpp::SigLevel::kDatabaseUnknownOk);
      }
      if (kIsPackage) {
        result &= ~(alpmpp::SigLevel::kPackageMarginalOk |
                   alpmpp::SigLevel::kPackageUnknownOk);
      }
    } else if (tail == "TrustAll") {
      if (kIsDatabase) {
        result |= (alpmpp::SigLevel::kDatabaseMarginalOk |
                   alpmpp::SigLevel::kDatabaseUnknownOk);
      }
      if (kIsPackage) {
        result |= (alpmpp::SigLevel::kPackageMarginalOk |
                   alpmpp::SigLevel::kPackageUnknownOk);
      }
    }
  }

  result &= ~alpmpp::SigLevel::kUseDefault;

  return result;
}

}  // namespace

void PacmanConf::ParseFromFile(const std::string_view config_file) {
  std::ifstream file{std::string(config_file)};
  if (!file.is_open()) {
    throw std::runtime_error(std::format("Failed to open config file: {}",
                                         std::string(config_file)));
  }
  std::string line;
  std::string current_section;
  Repository* current_repo = nullptr;

  while (std::getline(file, line)) {
    // Remove comments
    if (std::size_t comment_pos = line.find('#');
        comment_pos != std::string::npos) {
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
        current_repo->sig_level = sig_level_;
      } else {
        current_repo = nullptr;
      }
      continue;
    }

    // Parse key-value pairs
    std::size_t eq_pos = line.find('=');
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
        sig_level_ = ParseSigLevel(value);
      } else if (key == "LocalFileSigLevel") {
        local_file_sig_level_ = ParseSigLevel(value);
      } else if (key == "RemoteFileSigLevel") {
        remote_file_sig_level_ = ParseSigLevel(value);
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
        current_repo->sig_level = ParseSigLevel(value);
      } else if (key == "Usage") {
        current_repo->usage = SplitByWhitespace(value);
      }
    }
  }
}

}  // namespace pacmanpp