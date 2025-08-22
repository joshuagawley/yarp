// SPDX-License-Identifier: MIT

#include <aurpp/request.h>
#include <unistd.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using Catch::Matchers::EndsWith;

constexpr std::string_view kBaseUrl = "https://aur.archlinux.org";

SCENARIO("AUR Request behavior", "[AURRequest]") {
  GIVEN("An InfoRequest") {
    WHEN("An argument is passed")
    THEN("The URL is built correctly.") {
      aurpp::InfoRequest request;

      request.AddArg("foo");

      const std::string url = request.Url(kBaseUrl);
      REQUIRE_THAT(url, EndsWith("/rpc/v5/info"));

      const std::string payload = request.Payload();
      REQUIRE(payload == "arg[]=foo");
    }

    THEN("Parameter values are encoded correctly") {
      aurpp::InfoRequest request;

      request.AddArg("c++");

      const std::string payload = request.Payload();
      REQUIRE(payload == "arg[]=c%2B%2B");
    }
  }

  GIVEN("A SearchRequest") {
    THEN("The URL is built correctly.") {
      aurpp::SearchRequest request{aurpp::SearchRequest::SearchBy::kName,
                                   "foo"};

      const std::string url = request.Url(kBaseUrl);
      REQUIRE_THAT(url, EndsWith("/rpc/v5/search/foo?by=name"));
    }
  }

  GIVEN("A RawRequest") {
    THEN("The URL is built correctly") {
      aurpp::RawRequest request{"/foo/bar/baz"};

      const std::string url = request.Url(kBaseUrl);
      REQUIRE(url == std::format("{}/foo/bar/baz", kBaseUrl));
    }

    THEN("Repository names are escaped correctly") {
      aurpp::AurPackage pkg;
      pkg.set_package_base("libc++");
      const aurpp::RawRequest request =
          aurpp::RawRequest::ForSourceFile(pkg, "PKGBUILD");

      const std::string url = request.Url(kBaseUrl);
      REQUIRE_THAT(url, EndsWith("/PKGBUILD?h=libc%2B%2B"));
    }
  }
}
