// SPDX-License-Identifier: MIT

#include <unistd.h>

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <optional>
#include <stdexcept>

#include "../src/pacman_conf.h"

SCENARIO("PacmanConf parsing behavior", "[PacmanConf]") {
  GIVEN("A default initialized PacmanConf") {
    pacmanpp::PacmanConf conf;

    WHEN("We try to parse a non-existant file")
    THEN("an exception is thrown.") {
      REQUIRE_THROWS_AS(conf.ParseFromFile("black_hole.conf"),
                        std::runtime_error);
    }

    WHEN("We try to parse an empty file")
    THEN("The compiled-in defaults are used.") {
      REQUIRE(conf.root_dir() == "/");
      REQUIRE(conf.db_path() == "/var/lib/pacman/");
      REQUIRE(conf.cache_dir() == "/var/cache/pacman/pkg/");
      REQUIRE(conf.hook_dir() == "/etc/pacman.d/hooks/");
      REQUIRE(conf.gpg_dir() == "/etc/pacman.d/gnupg/");
      REQUIRE(conf.log_file() == "/var/log/pacman.log/");
      REQUIRE(conf.hold_pkg().empty());
      REQUIRE(conf.ignore_pkg().empty());
      REQUIRE(conf.ignore_group().empty());
      REQUIRE(conf.architecture().empty());
      REQUIRE(conf.xfer_command().empty());
      REQUIRE(conf.no_upgrade().empty());
      REQUIRE(conf.no_extract().empty());
      REQUIRE(conf.clean_method().empty());
      REQUIRE(conf.sig_level().empty());
      REQUIRE(conf.local_file_sig_level().empty());
      REQUIRE(conf.remote_file_sig_level().empty());
      REQUIRE(conf.download_user() == std::nullopt);
      REQUIRE(!conf.use_syslog());
      REQUIRE(!conf.color());
      REQUIRE(conf.use_delta() == 0.0);
      REQUIRE(!conf.total_download());
    }

    WHEN("A pacman.conf file is parsed") {
      conf.ParseFromFile("test_pacman_conf.conf");

      THEN("Any non-default values override the compiled-in defaults.") {
        REQUIRE(conf.root_dir() == "/");
        REQUIRE(conf.db_path() == "/var/lib/pacman/");
        REQUIRE(conf.cache_dir() == "/var/cache/pacman/pkg/");
        REQUIRE(conf.log_file() == "/var/log/pacman.log");
        REQUIRE(conf.gpg_dir() == "/etc/pacman.d/gnupg/");
        REQUIRE(conf.hook_dir() == "/etc/pacman.d/hooks/");
        REQUIRE(conf.hold_pkg() == std::vector<std::string>{"pacman", "glibc"});
        REQUIRE(conf.architecture() == std::vector<std::string>{"x86_64"});
        REQUIRE(conf.ignore_pkg() ==
                std::vector<std::string>{"linux-ck-headers", "linux-ck",
                                         "vim-youcompleteme*", "brackets-bin"});
        REQUIRE(conf.color());
        REQUIRE(conf.chomp());
        REQUIRE(conf.check_space());
        REQUIRE(conf.verbose_pkg_lists());
        REQUIRE(conf.disable_sandbox());
        REQUIRE((conf.download_user().has_value() &&
                 conf.download_user().value() == "foo"));
        REQUIRE(conf.sig_level() ==
                std::vector<std::string>{"Required", "DatabaseOptional"});
        REQUIRE(conf.local_file_sig_level() ==
                std::vector<std::string>{"Optional"});
        REQUIRE(conf.remote_file_sig_level() ==
                std::vector<std::string>{"Required"});
      }
    }
  }
}