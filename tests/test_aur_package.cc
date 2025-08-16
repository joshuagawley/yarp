// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <expected>
#include <fstream>
#include <optional>

#include "aurpp/package.h"

std::expected<Json::Value, std::string> ReadFromFile(const std::string &path) {
  std::ifstream json_file(path);

  Json::CharReaderBuilder reader_builder;
  reader_builder["collectComments"] = false;
  Json::Value result;
  std::string errors;

  const bool success =
      Json::parseFromStream(reader_builder, json_file, &result, &errors);
  if (success) {
    return result;
  } else {
    return errors;
  }
}

SCENARIO("AurPackage behavior", "[AurPackage]") {
  std::expected<Json::Value, std::string> maybe_json =
      ReadFromFile("paru.json");

  GIVEN("A default initialized AurPackage") {
    WHEN("We try to parse an empty file")
    THEN("The compiled-in defaults are used.") {
      aurpp::AurPackage pkg;

      REQUIRE(pkg.name().empty());
      REQUIRE(pkg.version().empty());
      REQUIRE(pkg.description() == std::nullopt);
      REQUIRE(pkg.maintainer() == std::nullopt);
      REQUIRE(pkg.url() == std::nullopt);
      REQUIRE(pkg.num_votes() == 0);
      REQUIRE(pkg.popularity() == 0.0);
      REQUIRE(pkg.out_of_date() == std::nullopt);
      REQUIRE(pkg.package_base().empty());
      REQUIRE(pkg.package_base_id() == 0);
      REQUIRE(pkg.first_submitted() == 0);
      REQUIRE(pkg.last_modified() == 0);
      REQUIRE(pkg.url_path() == std::nullopt);
      REQUIRE(pkg.id() == 0);
      REQUIRE(pkg.depends() == std::nullopt);
      REQUIRE(pkg.make_depends() == std::nullopt);
      REQUIRE(pkg.opt_depends() == std::nullopt);
      REQUIRE(pkg.check_depends() == std::nullopt);
      REQUIRE(pkg.conflicts() == std::nullopt);
      REQUIRE(pkg.provides() == std::nullopt);
      REQUIRE(pkg.replaces() == std::nullopt);
      REQUIRE(pkg.groups() == std::nullopt);
      REQUIRE(pkg.license() == std::nullopt);
      REQUIRE(pkg.keywords() == std::nullopt);
    }
  }

  GIVEN("A JSON file") {
    THEN("The package contents are updated successfully") {
      REQUIRE(maybe_json.has_value());
      aurpp::AurPackage pkg = aurpp::AurPackage::FromJson(maybe_json.value()["results"][0]);

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
      REQUIRE(pkg.depends() == std::vector<std::string>{"git", "pacman", "libalpm.so>=14"});
      REQUIRE(pkg.make_depends() == std::vector<std::string>{"cargo"});
      REQUIRE(pkg.opt_depends() == std::vector<std::string>{"bat", "devtools"});
      // TODO: check_depends is made into a vector when parsing a null value instead of remaining a std::nullopt
      REQUIRE(pkg.check_depends() == std::nullopt);
      REQUIRE(pkg.conflicts() == std::nullopt);
      REQUIRE(pkg.provides() == std::nullopt);
      REQUIRE(pkg.replaces() == std::nullopt);
      REQUIRE(pkg.groups() == std::nullopt);
      REQUIRE(pkg.license() == std::vector<std::string>{"GPL-3.0-or-later"});
      REQUIRE(pkg.keywords() == std::vector<std::string>{"AUR", "helper", "pacman", "rust", "wrapper", "yay"});
    }
  }
}