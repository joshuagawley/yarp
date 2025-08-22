// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <expected>

#include "aurpp/client.h"
#include "aurpp/request.h"
#include "aurpp/response.h"

SCENARIO("Client HTTP request functionality", "[Client]") {
  GIVEN("A Client instance") {
    aurpp::Client client;

    WHEN("Making an InfoRequest for a known package") {
      aurpp::InfoRequest request;
      request.AddArg("paru");

      auto result =
          client.Execute<aurpp::InfoRequest, aurpp::RpcResponse>(request);

      THEN("The request should succeed") {
        REQUIRE(result.has_value());
        AND_THEN("The response should contain package information") {
          const auto &response = result.value();
          REQUIRE_FALSE(response.packages.empty());

          if (!response.packages.empty()) {
            const auto &pkg = response.packages[0];
            REQUIRE(pkg.name() == "paru");
          }
        }
      }
    }

    WHEN("Making a SearchRequest for packages") {
      aurpp::SearchRequest request(aurpp::SearchRequest::SearchBy::kName,
                                   "paru");

      auto result =
          client.Execute<aurpp::SearchRequest, aurpp::RpcResponse>(request);

      THEN("The request should succeed") {
        REQUIRE(result.has_value());
        AND_THEN("The response should contain search results") {
          const auto &response = result.value();
          REQUIRE_FALSE(response.packages.empty());
        }
      }
    }

    WHEN("Making a RawRequest for a source file") {
      THEN("The request should be constructed properly") {
        aurpp::AurPackage mock_package;

        auto raw_request =
            aurpp::RawRequest::ForSourceFile(mock_package, "PKGBUILD");
        const std::string url = raw_request.Url("https://aur.archlinux.org");
        REQUIRE(url.find("PKGBUILD") != std::string::npos);
      }
    }
  }

  GIVEN("A Client with custom base URL") {
    aurpp::Client client("https://custom.aur.server");

    WHEN("Constructing a request URL") {
      aurpp::InfoRequest request;
      const std::string url = request.Url("https://custom.aur.server");

      THEN("The URL should use the custom base") {
        REQUIRE(url.find("https://custom.aur.server") == 0);
      }
    }
  }
}

SCENARIO("Client error handling", "[Client]") {
  GIVEN("A Client with invalid server") {
    aurpp::Client client("https://invalid.server.that.does.not.exist");

    WHEN("Making a request") {
      aurpp::InfoRequest request;
      request.AddArg("test");

      auto result =
          client.Execute<aurpp::InfoRequest, aurpp::RpcResponse>(request);

      THEN("The request should fail with an error") {
        REQUIRE_FALSE(result.has_value());
        AND_THEN("The error message should be descriptive") {
          REQUIRE_FALSE(result.error().empty());
        }
      }
    }
  }
}