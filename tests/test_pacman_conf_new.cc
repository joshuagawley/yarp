// SPDX-License-Identifier: MIT

#include <unistd.h>

#include <catch2/catch_test_macros.hpp>
#include <filesystem>

#include "../src/pacman_conf.h"

SCENARIO("Default values are overridded by values in a pacman.conf file",
         "[PacmanConf]") {
  GIVEN("A default initialized PacmanConf") {
    pacmanpp::PacmanConf conf;

    WHEN(
        "A pacman.conf file is parsed correctly, with any non-default values "
        "overriding the compiled-in defaults.") {
      conf.ParseFromFile("test_pacman_conf.conf");

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