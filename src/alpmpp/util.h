// SPDX-License-Identifier: MIT

#ifndef ALPMPP_UTIL_H_
#define ALPMPP_UTIL_H_

#include <alpm_list.h>

#include <sstream>
#include <vector>

namespace alpmpp {

namespace util {

template <typename T>
  requires std::formattable<T, char>
void PrintStringVector(std::stringstream &ss, const std::string_view prefix,
                       const std::vector<T> &vector) {
  if (vector.empty()) {
    std::println(ss, "{}None", prefix);
  } else {
    std::print(ss, "{}", prefix);
    for (const T &elem : vector) {
      if (elem != *(std::end(vector) - 1)) {
        std::print(ss, "{}  ", elem);  // Not the last item, add space
      } else {
        std::println(ss, "{}", elem);  // Last item, no trailing space
      }
    }
  }
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

inline alpm_list_t *StringVectorToAlpmList(const std::vector<std::string> &vec) {
  alpm_list_t *list = nullptr;

  for (const std::string &str : vec) {
    list = alpm_list_add(list, const_cast<void *>(reinterpret_cast<const void*>(str.c_str())));
  }

  return list;
}

}  // namespace util
}  // namespace alpmpp

#endif  // ALPMPP_UTIL_H_
