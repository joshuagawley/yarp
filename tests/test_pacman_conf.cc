// SPDX-License-Identifier: MIT

#include <unistd.h>

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>

#include "../src/pacman_conf.h"

namespace fs = std::filesystem;

class TempFile {
 public:
  TempFile() {
    // Create a temporary file
    char temp_template[] = "/tmp/pacman_conf_test_XXXXXX";
    int fd = mkstemp(temp_template);
    if (fd == -1) {
      throw std::runtime_error("Failed to create temporary file");
    }
    close(fd);
    path_ = temp_template;
  }

  ~TempFile() {
    if (fs::exists(path_)) {
      fs::remove(path_);
    }
  }

  void write(const std::string& content) {
    std::ofstream file(path_);
    if (!file) {
      throw std::runtime_error("Failed to open temporary file for writing");
    }
    file << content;
  }

  const std::string& path() const { return path_; }

 private:
  std::string path_;
};

TEST_CASE("PacmanConf::ParseFromFile basic functionality", "[pacman_conf]") {
  using namespace pacmanpp;

  SECTION("Parse non-existent file throws exception") {
    PacmanConf conf;
    REQUIRE_THROWS_AS(conf.ParseFromFile("/non/existent/file.conf"),
                      std::runtime_error);
  }

  SECTION("Parse empty file returns valid config with defaults") {
    TempFile temp;
    temp.write("");

    PacmanConf conf;
    REQUIRE_NOTHROW(conf.ParseFromFile(temp.path()));

    CHECK(conf.root_dir() == "/");
    CHECK(conf.db_path() == "/var/lib/pacman/");
    CHECK(conf.cache_dir() == "/var/cache/pacman/pkg/");
    CHECK(conf.log_file() == "/var/log/pacman.log");
    CHECK(conf.gpg_dir() == "/etc/pacman.d/gnupg/");
    CHECK(conf.hook_dir() == "/etc/pacman.d/hooks/");
  }

  SECTION("Parse basic options") {
    TempFile temp;
    temp.write(R"(
[options]
RootDir = /custom/root
DBPath = /custom/db
CacheDir = /custom/cache
LogFile = /custom/log
GPGDir = /custom/gpg
HookDir = /custom/hooks
)");

    PacmanConf conf;
    REQUIRE_NOTHROW(conf.ParseFromFile(temp.path()));

    CHECK(conf.root_dir() == "/custom/root");
    CHECK(conf.db_path() == "/custom/db");
    CHECK(conf.cache_dir() == "/custom/cache");
    CHECK(conf.log_file() == "/custom/log");
    CHECK(conf.gpg_dir() == "/custom/gpg");
    CHECK(conf.hook_dir() == "/custom/hooks");
  }

  SECTION("Parse multi-value options") {
    TempFile temp;
    temp.write(R"(
[options]
HoldPkg = pacman glibc systemd
IgnorePkg = firefox chromium
IgnoreGroup = kde-applications gnome
Architecture = x86_64 i686
)");

    PacmanConf conf;
    REQUIRE_NOTHROW(conf.ParseFromFile(temp.path()));

    CHECK(conf.hold_pkg() ==
          std::vector<std::string>{"pacman", "glibc", "systemd"});
    CHECK(conf.ignore_pkg() ==
          std::vector<std::string>{"firefox", "chromium"});
    CHECK(conf.ignore_group() ==
          std::vector<std::string>{"kde-applications", "gnome"});
    CHECK(conf.architecture() == std::vector<std::string>{"x86_64", "i686"});
  }

  SECTION("Parse boolean options") {
    TempFile temp;
    temp.write(R"(
[options]
UseSyslog
Color
CheckSpace
VerbosePkgLists
DisableDownloadTimeout
DisableSandbox
)");

    PacmanConf conf;
    REQUIRE_NOTHROW(conf.ParseFromFile(temp.path()));

    CHECK(conf.use_syslog() == true);
    CHECK(conf.color() == true);
    CHECK(conf.check_space() == true);
    CHECK(conf.verbose_pkg_lists() == true);
    CHECK(conf.disable_download_timeout() == true);
    CHECK(conf.disable_sandbox() == true);
  }

  SECTION("Parse numeric options") {
    TempFile temp;
    temp.write(R"(
[options]
ParallelDownloads = 5
)");

    PacmanConf conf;
    REQUIRE_NOTHROW(conf.ParseFromFile(temp.path()));

    CHECK(conf.parallel_downloads() == 5);
  }

  SECTION("Parse optional string options") {
    TempFile temp;
    temp.write(R"(
[options]
DownloadUser = alpm
)");

    PacmanConf conf;
    REQUIRE_NOTHROW(conf.ParseFromFile(temp.path()));

    CHECK(conf.download_user().has_value());
    CHECK(conf.download_user().value() == "alpm");
  }

  SECTION("Parse comments and empty lines") {
    TempFile temp;
    temp.write(R"(
# This is a comment
[options]
# Another comment
RootDir = /  # Inline comment

# Empty line above
HoldPkg = pacman glibc
)");

    PacmanConf conf;
    REQUIRE_NOTHROW(conf.ParseFromFile(temp.path()));

    CHECK(conf.root_dir() == "/");
    CHECK(conf.hold_pkg() == std::vector<std::string>{"pacman", "glibc"});
  }

  SECTION("Parse repository sections") {
    TempFile temp;
    temp.write(R"(
[options]
Architecture = x86_64

[core]
Include = /etc/pacman.d/mirrorlist

[extra]
Server = https://mirror1.example.com/$repo/os/$arch
Server = https://mirror2.example.com/$repo/os/$arch

[custom]
SigLevel = Optional TrustAll
Server = file:///home/custompkgs
)");

    PacmanConf conf;
    REQUIRE_NOTHROW(conf.ParseFromFile(temp.path()));

    auto& repos = conf.repos();
    REQUIRE(repos.size() == 3);

    CHECK(repos[0].name == "core");
    CHECK(repos[0].servers.size() == 1);
    CHECK(repos[0].servers[0] == "Include:/etc/pacman.d/mirrorlist");

    CHECK(repos[1].name == "extra");
    CHECK(repos[1].servers.size() == 2);
    CHECK(repos[1].servers[0] == "https://mirror1.example.com/$repo/os/$arch");
    CHECK(repos[1].servers[1] == "https://mirror2.example.com/$repo/os/$arch");

    CHECK(repos[2].name == "custom");
    CHECK(repos[2].sig_level ==
          std::vector<std::string>{"Optional", "TrustAll"});
    CHECK(repos[2].servers.size() == 1);
    CHECK(repos[2].servers[0] == "file:///home/custompkgs");
  }

  SECTION("Parse Architecture auto value") {
    TempFile temp;
    temp.write(R"(
[options]
Architecture = auto
)");

    PacmanConf conf;
    REQUIRE_NOTHROW(conf.ParseFromFile(temp.path()));

    // When set to "auto", the implementation sets it to x86_64 as a placeholder
    CHECK(conf.architecture() == std::vector<std::string>{"x86_64"});
  }

  SECTION("Parse signature levels") {
    TempFile temp;
    temp.write(R"(
[options]
SigLevel = Required DatabaseOptional
LocalFileSigLevel = Optional
RemoteFileSigLevel = Required TrustedOnly
)");

    PacmanConf conf;
    REQUIRE_NOTHROW(conf.ParseFromFile(temp.path()));

    CHECK(conf.sig_level() ==
          std::vector<std::string>{"Required", "DatabaseOptional"});
    CHECK(conf.local_file_sig_level() ==
          std::vector<std::string>{"Optional"});
    CHECK(conf.remote_file_sig_level() ==
          std::vector<std::string>{"Required", "TrustedOnly"});
  }

  //     SECTION("Parse mixed sections") {
  //         TempFile temp;
  //         temp.write(R"(
  // [options]
  // HoldPkg = pacman glibc
  // CheckSpace

  // [core]
  // Include = /etc/pacman.d/mirrorlist

  // [options]
  // # Should not affect options parsing - sections don't repeat
  // Color

  // [extra]
  // Server = https://mirror.example.com/$repo/os/$arch
  // )");

  //         PacmanConf conf;
  //         auto result = conf.ParseFromFile(temp.path());
  //         REQUIRE(result.has_value());

  //         auto& parsed = result.value();
  //         CHECK(parsed.hold_pkg() == std::vector<std::string>{"pacman",
  //         "glibc"}); CHECK(parsed.check_space() == true);
  //         CHECK(parsed.color() == false);  // Second [options] section should
  //         be ignored

  //         auto& repos = parsed.repos();
  //         CHECK(repos.size() == 2);
  //         CHECK(repos[0].name == "core");
  //         CHECK(repos[1].name == "extra");
  //     }
}