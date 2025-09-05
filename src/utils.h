// SPDX-License-Identifier: MIT

#ifndef YARP_UTIL_H_
#define YARP_UTIL_H_

#include <alpmpp/alpm.h>

#include <expected>
#include <string>
#include <vector>

namespace yarp::utils {

std::expected<void, std::string> PrintPkgSearch(alpm_db_t *db, const std::vector<std::string> &targets);

}  // namespace yarp::utils

#endif  // YARP_UTIL_H_
