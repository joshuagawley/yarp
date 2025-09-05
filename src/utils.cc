// SPDX-License-Identifier: MIT

#include <print>
#include <sstream>

#include "utils.h"

namespace yarp::utils {

std::expected<void, std::string> PrintPkgSearch(alpm_db_t *db, const std::vector<std::string> &targets) {
  const std::vector<alpmpp::AlpmPackage> search_list =
      alpmpp::Alpm::DbSearch(db, targets);

  if (search_list.empty()) {
    return std::unexpected("Error: could not determine search list");
  }

  std::stringstream ss;

  for (const alpmpp::AlpmPackage &pkg : search_list) {
    const std::vector<std::string_view> groups = pkg.groups();

    std::print(ss, "{}/{} {}", alpm_db_get_name(db), pkg.name(),
               pkg.version());
    if (!groups.empty()) {
      std::print(ss, " (");
      for (const std::string_view &group : groups) {
        if (group != *(std::end(groups) - 1)) {
          std::print(ss, "{}  ", group);  // Not the last item, add space
        } else {
          std::print(ss, "{}", group);  // Last item, no trailing space
        }
      }
      std::print(ss, ")");
    }
    std::println(ss, "\n    {}", pkg.desc());
  }
  std::println("{}", ss.str());

  return {};
}

} //