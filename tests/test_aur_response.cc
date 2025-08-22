// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <expected>
#include <fstream>

#include "aurpp/response.h"

SCENARIO("AurResponse parsing behavior", "[AurResponse]") {
  GIVEN("A JSON file representing a response") {
    THEN("The package contents are updated successfully") {
      std::ifstream file{"paru.json"};
      std::ostringstream ss;
      ss << file.rdbuf();
      std::expected<aurpp::RpcResponse, std::string> maybe_response = aurpp::RpcResponse::Parse(std::string{ss.str()});
      REQUIRE(maybe_response.has_value() == true);

      const aurpp::AurPackage &pkg = maybe_response.value().packages[0];
      REQUIRE(pkg.name() == "paru");
      REQUIRE(pkg.version() == "2.1.0-1");
      REQUIRE(pkg.description() == "Feature packed AUR helper");
      REQUIRE(pkg.maintainer() == "Morganamilo");
      REQUIRE(pkg.url() == "https://github.com/morganamilo/paru");
      REQUIRE(pkg.num_votes() == 1061);
      REQUIRE(pkg.popularity() == 24.920245);
      REQUIRE(pkg.out_of_date() == std::nullopt);
      REQUIRE(pkg.package_base() == "paru");
      REQUIRE(pkg.package_base_id() == 158891);
      REQUIRE(pkg.first_submitted() == 1603068230);
      REQUIRE(pkg.last_modified() == 1751964728);
      REQUIRE(pkg.url_path() == "/cgit/aur.git/snapshot/paru.tar.gz");
      REQUIRE(pkg.id() == 1772991);
      REQUIRE(pkg.depends() ==
              std::vector<std::string>{"git", "pacman", "libalpm.so>=14"});
      REQUIRE(pkg.make_depends() == std::vector<std::string>{"cargo"});
      REQUIRE(pkg.opt_depends() == std::vector<std::string>{"bat", "devtools"});
      // TODO: check_depends is made into a vector when parsing a null value
      // instead of remaining a std::nullopt
      REQUIRE(pkg.check_depends() == std::nullopt);
      REQUIRE(pkg.conflicts() == std::nullopt);
      REQUIRE(pkg.provides() == std::nullopt);
      REQUIRE(pkg.replaces() == std::nullopt);
      REQUIRE(pkg.groups() == std::nullopt);
      REQUIRE(pkg.license() == std::vector<std::string>{"GPL-3.0-or-later"});
      REQUIRE(pkg.keywords() == std::vector<std::string>{"AUR", "helper",
                                                         "pacman", "rust",
                                                         "wrapper", "yay"});
    }
  }
}
