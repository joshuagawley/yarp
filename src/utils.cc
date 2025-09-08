// SPDX-License-Identifier: MIT

#include "utils.h"

#include <alpmpp/util.h>

#include <print>
#include <ranges>
#include <sstream>

namespace yarp::utils {

std::expected<std::string, std::string> PrintPkgSearch(
    alpm_db_t *db, const std::vector<std::string> &targets) {
  const std::vector<alpmpp::AlpmPackage> search_list =
      alpmpp::Alpm::DbSearch(db, targets);

  if (search_list.empty()) {
    return std::unexpected("Error: could not determine search list");
  }

  std::string result;

  for (const alpmpp::AlpmPackage &pkg : search_list) {
    const std::vector<std::string_view> groups = pkg.groups();

    std::format_to(std::back_inserter(result), "{}/{} {}", alpm_db_get_name(db),
                   pkg.name(), pkg.version());
    if (!groups.empty()) {
      std::format_to(std::back_inserter(result), " (");
      alpmpp::util::PrintJoined(std::back_inserter(result), groups, " ", "");
      std::format_to(std::back_inserter(result), ")");
    }
    std::format_to(std::back_inserter(result), "\n    {}\n", pkg.desc());
  }
  return result;
}

}  // namespace yarp::utils