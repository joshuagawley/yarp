// SPDX-License-Identifier: MIT

#ifndef AURPP_REQUEST_H_
#define AURPP_REQUEST_H_

#include <aurpp/package.h>
#include <curl/curl.h>

#include <algorithm>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>
#include <vector>

namespace aurpp {

namespace detail {

inline std::string UrlEscape(const std::string_view sv) {
  char *ptr = curl_easy_escape(nullptr, sv.data(), sv.size());
  std::string escaped{ptr};
  curl_free(ptr);
  return escaped;
}

}  // namespace detail

class HttpRequest {
 public:
  enum class Command { kGet, kPost };

  struct QueryParameter {
    std::string_view key;
    std::string_view value;

    constexpr bool operator==(const QueryParameter &) const = default;
  };

  virtual ~HttpRequest() = default;

  constexpr explicit HttpRequest(const Command command) : command_(command) {}

  [[nodiscard]] constexpr Command command() const { return command_; }

  [[nodiscard]] virtual std::string Url(std::string_view base_url) const = 0;
  [[nodiscard]] virtual std::string Payload() const = 0;

 protected:
  Command command_;
};

class RpcRequest : public HttpRequest {
 public:
  constexpr RpcRequest(const Command command, std::string endpoint)
      : HttpRequest(command), endpoint_(std::move(endpoint)) {}

  [[nodiscard]] std::string Url(std::string_view base_url) const override {
    return std::format("{}{}", base_url, endpoint_);
  }
  [[nodiscard]] std::string Payload() const override;

  constexpr void AddArg(const std::string_view key,
                        const std::string_view value) {
    params_.emplace_back(key, value);
  }

 private:
  std::string endpoint_;
  std::vector<QueryParameter> params_;
};

class RawRequest final : public HttpRequest {
 public:
  static RawRequest ForSourceFile(const AurPackage &package,
                                  std::string_view file_name) {
    return RawRequest{std::format("/cgit/aur.git/plain/{}?h={}", file_name,
                                  detail::UrlEscape(package.package_base()))};
  }

  constexpr explicit RawRequest(std::string url_path)
      : HttpRequest(Command::kGet), url_path_(std::move(url_path)) {}

  RawRequest(const RawRequest &) = delete;
  RawRequest &operator=(const RawRequest &) = delete;

  RawRequest(RawRequest &&) = default;
  RawRequest &operator=(RawRequest &&) = default;

  [[nodiscard]] std::string Url(std::string_view base_url) const override {
    return std::format("{}{}", base_url, url_path_);
  }

  [[nodiscard]] constexpr std::string Payload() const override { return ""; }

 private:
  std::string url_path_;
};

class InfoRequest final : public RpcRequest {
 public:
  constexpr explicit InfoRequest(const std::vector<std::string> &args)
      : InfoRequest() {
    for (const std::string &arg : args) {
      AddArg(arg);
    }
  }

  constexpr explicit InfoRequest(const std::vector<AurPackage> &packages)
      : InfoRequest() {
    for (const AurPackage &package : packages) {
      AddArg(std::string{package.name()});
    }
  }

  constexpr InfoRequest() : RpcRequest{Command::kPost, "/rpc/v5/info"} {}

  InfoRequest(const InfoRequest &) = delete;
  InfoRequest &operator=(const InfoRequest &) = delete;

  InfoRequest(InfoRequest &&) = default;
  InfoRequest &operator=(InfoRequest &&) = default;

  constexpr void AddArg(const std::string_view arg) {
    RpcRequest::AddArg("arg[]", arg);
  }
};

class SearchRequest final : public RpcRequest {
 public:
  enum class SearchBy {
    kInvalid,
    kName,
    kNameDesc,
    kMaintainer,
    kDepends,
    kMakeDepends,
    kCheckDepends,
    kSubmitter,
    kProvides,
    kConflicts,
    kReplaces,
    kKeywords,
    kGroups,
    kCoMaintainers
  };

  static constexpr SearchBy ParseSearchBy(const std::string_view search_by) {
    if (search_by == "name") {
      return SearchBy::kName;
    } else if (search_by == "name-desc") {
      return SearchBy::kNameDesc;
    } else if (search_by == "maintainer") {
      return SearchBy::kMaintainer;
    } else if (search_by == "depends") {
      return SearchBy::kDepends;
    } else if (search_by == "makedepends") {
      return SearchBy::kMakeDepends;
    } else if (search_by == "checkdepends") {
      return SearchBy::kCheckDepends;
    } else if (search_by == "submitter") {
      return SearchBy::kSubmitter;
    } else if (search_by == "provides") {
      return SearchBy::kProvides;
    } else if (search_by == "conflicts") {
      return SearchBy::kConflicts;
    } else if (search_by == "replaces") {
      return SearchBy::kReplaces;
    } else if (search_by == "keywords") {
      return SearchBy::kKeywords;
    } else if (search_by == "groups") {
      return SearchBy::kGroups;
    } else if (search_by == "comaintainers") {
      return SearchBy::kCoMaintainers;
    } else {
      return SearchBy::kInvalid;
    }
  }

  SearchRequest(const SearchBy search_by, const std::string_view arg)
      : RpcRequest(Command::kGet, std::format("/rpc/v5/search/{}?by={}", arg,
                                              SearchByToString(search_by))) {}

  SearchRequest(const SearchRequest &) = delete;
  SearchRequest &operator=(const SearchRequest &) = delete;

  SearchRequest(SearchRequest &&) = default;
  SearchRequest &operator=(SearchRequest &&) = default;

 private:
  static constexpr std::string SearchByToString(const SearchBy search_by) {
    switch (search_by) {
      case SearchBy::kName:
        return "name";
      case SearchBy::kNameDesc:
        return "name-desc";
      case SearchBy::kMaintainer:
        return "maintainer";
      case SearchBy::kDepends:
        return "depends";
      case SearchBy::kMakeDepends:
        return "makedepends";
      case SearchBy::kCheckDepends:
        return "checkdepends";
      case SearchBy::kSubmitter:
        return "submitter";
      case SearchBy::kProvides:
        return "provides";
      case SearchBy::kConflicts:
        return "conflicts";
      case SearchBy::kReplaces:
        return "replaces";
      case SearchBy::kKeywords:
        return "keywords";
      case SearchBy::kGroups:
        return "groups";
      case SearchBy::kCoMaintainers:
        return "comaintainers";
      default:
        return "";
    }
  }
};

}  // namespace aurpp

template <>
struct std::formatter<aurpp::HttpRequest::QueryParameter> {
  std::formatter<std::string_view> sv_formatter;

  constexpr auto parse(std::format_parse_context &ctx) {
    return sv_formatter.parse(ctx);
  }

  constexpr auto format(const aurpp::HttpRequest::QueryParameter &param,
                        std::format_context &ctx) const {
    std::string temp;
    std::format_to(std::back_inserter(temp), "{}={}", param.key,
                   aurpp::detail::UrlEscape(param.value));
    return sv_formatter.format(temp, ctx);
  }
};

#endif  // AURPP_REQUEST_H_
