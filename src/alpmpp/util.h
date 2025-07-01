// SPDX-License-Identifier: MIT

#ifndef PACMANPP_ALPMPP_UTIL_H_
#define PACMANPP_ALPMPP_UTIL_H_

#include <alpm.h>
#include <alpm_list.h>

#include <vector>

namespace alpmpp {

namespace util {

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

}  // namespace util
}  // namespace alpmpp

#endif  // PACMANPP_ALPMPP_UTIL_H_
