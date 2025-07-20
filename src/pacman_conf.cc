// SPDX-License-Identifier: MIT

#include "pacman_conf.h"

#include <glob.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>

namespace {

class Glob {
 public:
  explicit Glob(const std::string_view pattern) {
    glob_ok_ = glob(pattern.data(), GLOB_NOCHECK, nullptr, &glob_) == 0;
    if (glob_ok_) {
      results_ = std::span(glob_.gl_pathv, glob_.gl_pathc);
    }
  }

  [[nodiscard]] constexpr bool ok() const { return glob_ok_; }

  ~Glob() {
    if (glob_ok_) globfree(&glob_);
  }

  using iterator = std::span<char*>::iterator;
  iterator begin() { return results_.begin(); }
  iterator end() { return results_.end(); }

 private:
  glob_t glob_;
  bool glob_ok_;
  std::span<char*> results_;
};

class FileReader {
 public:
  explicit FileReader(const std::filesystem::path& path) : file_(path) {}

  ~FileReader() {
    if (ok()) file_.close();
  }

  bool GetLine(std::string& line) {
    return static_cast<bool>(std::getline(file_, line));
  }

  bool ok() const noexcept { return file_.is_open(); }

 private:
  std::ifstream file_;
};

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

std::pair<std::string, std::string> SplitKeyValue(const std::string_view line) {
  const std::size_t kEqPos = line.find('=');
  std::string key;
  std::string value;

  if (kEqPos != std::string::npos) {
    key = line.substr(0, kEqPos);
    value = line.substr(kEqPos + 1);

    Trim(key);
    Trim(value);
  } else {
    // Some options don't have values (like UseSyslog, Color, etc.)
    key = line;
  }
  return {key, value};
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

namespace pacmanpp {

namespace detail {

struct ParseState {
  std::string section;
  Repository *current_repo = nullptr;
};

} // namespace pacmanpp::detail

std::expected<void, std::string> PacmanConf::ParseFromFile(
    const std::filesystem::path& config_file) {
  detail::ParseState state{};
  return ParseOneFile(config_file, state);
}


std::expected<void, std::string> PacmanConf::ParseOneFile(
    const std::filesystem::path& path, detail::ParseState &state) {
  FileReader reader{path};

  for (std::string line; reader.GetLine(line);) {
    Trim(line);

    // Skip empty lines and commments
    if (line.empty() || line.front() == '#') continue;

    // Check for section header
    if (line.front() == '[' && line.back() == ']') {
      state.section = line.substr(1, line.size() - 2);
      if (state.section != "options") {
        // Vector takes ownership so we get the pointer of the last item pushed
        // (i.e. the Repository object we just created)
        Repository repo{};
        repos_.push_back(repo);
        state.current_repo = &repos_.back();
        state.current_repo->name = state.section;
      }
      continue;
    }

    // Parse key-value pairs

    auto [key, value] = SplitKeyValue(line);

    // Process based on current section
    if (state.section == "options") {
      if (key == "RootDir") {
        root_dir_ = value;
      } else if (key == "DBPath") {
        db_path_ = value;
      } else if (key == "CacheDir") {
        cache_dirs_ = SplitByWhitespace(value);
      } else if (key == "LogFile") {
        log_file_ = value;
      } else if (key == "GPGDir") {
        gpg_dir_ = value;
      } else if (key == "HookDir") {
        // We need to always keep the first hook,
        // so we extend rather than assign
        std::vector<std::string> additional_hook_dirs =
            SplitByWhitespace(value);
        hook_dirs_.insert(std::end(hook_dirs_),
                          std::begin(additional_hook_dirs),
                          std::end(additional_hook_dirs));
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
    } else if (state.current_repo != nullptr) {
      // Repository section
      if (key == "Server") {
        state.current_repo->servers.push_back(value);
      } else if (key == "Include") {
        // In a real implementation, we'd parse the included file
        // For now, just store the include path
        Glob includes{value};
        if (!includes.ok()) {
          return std::unexpected("Could not parse glob");
        }

        for (const std::string_view include_path : includes) {
          if (!ParseOneFile(include_path, state).has_value()) {
            return std::unexpected(std::format("Could not parse Includes file : {}", include_path));
          }
        }
      } else if (key == "SigLevel") {
        state.current_repo->sig_level = ParseSigLevel(value);
      } else if (key == "Usage") {
        state.current_repo->usage = SplitByWhitespace(value);
      }
    }
  }
  if (reader.ok()) {
    return {};
  } else {
    return std::unexpected(std::format("Could not parse file at {}", path.c_str()));
  }
}

}  // namespace pacmanpp