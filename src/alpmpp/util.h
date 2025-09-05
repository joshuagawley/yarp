// SPDX-License-Identifier: MIT

#ifndef ALPMPP_UTIL_H_
#define ALPMPP_UTIL_H_

#include <alpm_list.h>

#include <sstream>
#include <vector>

namespace alpmpp {

namespace util {

template <std::ranges::input_range Range>
constexpr void PrintJoined(std::stringstream &ss, Range &&range,
                           std::string_view separator,
                           std::string_view empty_text = "None") {
  auto it = std::ranges::begin(range);
  auto end = std::ranges::end(range);

  if (it == end) {
    std::print(ss, "{}", empty_text);
  } else {
    std::print(ss, "{}", *it);
    ++it;
    for (; it != end; ++it) {
      std::print(ss, "{}{}", separator, *it);
    }
  }
}

template <std::ranges::input_range Range>
constexpr void PrintJoinedLine(std::stringstream &ss, std::string_view prefix,
                               Range &&range, std::string_view separator = " ",
                               std::string_view empty_text = "None") {
  std::print(ss, "{}", prefix);
  PrintJoined(ss, std::forward<Range>(range), separator, empty_text);
  std::println(ss, "");
}

template <typename Input, typename Output>
constexpr std::vector<Output> AlpmListToVector(const alpm_list_t *list) {
  std::vector<Output> result;
  result.reserve(alpm_list_count(list));

  for (const alpm_list_t *elem = list; elem != nullptr;
       elem = alpm_list_next(elem)) {
    result.emplace_back(static_cast<Input>(elem->data));
  }

  return result;
}

inline alpm_list_t *StringVectorToAlpmList(
    const std::vector<std::string> &vec) {
  alpm_list_t *list = nullptr;

  for (const std::string &str : vec) {
    list = alpm_list_add(
        list, const_cast<void *>(reinterpret_cast<const void *>(str.c_str())));
  }

  return list;
}

}  // namespace util
}  // namespace alpmpp

#endif  // ALPMPP_UTIL_H_
