// SPDX-License-Identifier: MIT

#ifndef AURPP_CLIENT_H_
#define AURPP_CLIENT_H_

#include <aurpp/request.h>
#include <aurpp/response.h>
#include <curl/curl.h>

#include <concepts>
#include <expected>
#include <memory>
#include <string>

namespace aurpp {

namespace detail {

constexpr std::size_t WriteCallback(void *contents, const std::size_t size,
                                    const std::size_t nmemb, void *userp) {
  const std::size_t total_size = size * nmemb;
  static_cast<std::string *>(userp)->append(static_cast<char *>(contents),
                                            total_size);
  return total_size;
}

}  // namespace detail

// TODO: rewrite this using curl's multi interface.
class Client {
 public:
  explicit Client(std::string base_url = "https://aur.archlinux.org")
      : curl_handle_{curl_easy_init(), curl_easy_cleanup},
        base_url_(std::move(base_url)) {
    if (curl_handle_) {
      curl_easy_setopt(curl_handle_.get(), CURLOPT_HTTP_VERSION,
                       CURL_HTTP_VERSION_3);
      curl_easy_setopt(curl_handle_.get(), CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl_handle_.get(), CURLOPT_USERAGENT, "yarp/1.0");
    }
  }

  Client(const Client &) = delete;
  Client &operator=(const Client &) = delete;

  Client(Client &&) = delete;
  Client &operator=(Client &&) = delete;

  template <typename RequestType, typename ResponseType>
    requires std::derived_from<RequestType, HttpRequest>
  std::expected<ResponseType, std::string> Execute(const RequestType &request);

 private:
  std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl_handle_;
  std::string base_url_;
};

template <typename RequestType, typename ResponseType>
  requires std::derived_from<RequestType, HttpRequest>
std::expected<ResponseType, std::string> Client::Execute(
    const RequestType &request) {
  if (!curl_handle_) {
    return std::unexpected{"CURL handle not initialized"};
  }

  std::string response_data{};
  const std::string url = request.Url(base_url_);

  curl_easy_setopt(curl_handle_.get(), CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl_handle_.get(), CURLOPT_WRITEFUNCTION,
                   detail::WriteCallback);
  curl_easy_setopt(curl_handle_.get(), CURLOPT_WRITEDATA, &response_data);

  switch (request.command()) {
    case HttpRequest::Command::kGet: {
      curl_easy_setopt(curl_handle_.get(), CURLOPT_HTTPGET, 1L);
      break;
    }
    case HttpRequest::Command::kPost: {
      const std::string payload = request.Payload();
      curl_easy_setopt(curl_handle_.get(), CURLOPT_POST, 1L);
      curl_easy_setopt(curl_handle_.get(), CURLOPT_POSTFIELDS, payload.c_str());
      curl_easy_setopt(curl_handle_.get(), CURLOPT_POSTFIELDSIZE,
                       payload.size());
      break;
    }
    default:
      std::unreachable();
  }

  if (const CURLcode res = curl_easy_perform(curl_handle_.get());
      res != CURLE_OK) {
    return std::unexpected{
        std::format("CURL error: {}", curl_easy_strerror(res))};
  }

  long http_code = 0;
  curl_easy_getinfo(curl_handle_.get(), CURLINFO_RESPONSE_CODE, &http_code);
  if (http_code != 200) {
    return std::unexpected{std::format("HTTP error: {}", http_code)};
  }

  return ResponseType::Parse(response_data);
}

}  // namespace aurpp

#endif  // AURPP_CLIENT_H_
