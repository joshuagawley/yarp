// SPDX-License-Identifier: MIT

#ifndef PACMANPP_ALPM_H_
#define PACMANPP_ALPM_H_

#include <alpm.h>
#include <alpm_list.h>

#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

namespace pacmanpp {

namespace util {

template <typename T>
constexpr std::vector<T> AlpmListToVector(alpm_list_t *list) {
  std::vector<T> result;

  alpm_list_t *item;
  int count = 0;
  for (item = list; item != nullptr; item = item->next) {
    ++count;
  }
  result.reserve(count);

  for (item = list; item != nullptr; item = item->next) {
    result.push_back(*static_cast<T *>(item->data));
  }

  return result;
}

constexpr std::vector<std::string> AlpmListToStringVector(alpm_list_t *list) {
  std::vector<std::string> result;

  alpm_list_t *item;
  int count = 0;
  for (item = list; item != nullptr; item = item->next) {
    ++count;
  }
  result.reserve(count);

  for (item = list; item != nullptr; item = item->next) {
    result.emplace_back(static_cast<const char *>(item->data));
  }

  return result;
}

template <typename T>
constexpr alpm_list_t *VectorToAlpmList(const std::vector<T> &vec) {
  alpm_list_t *list = nullptr;

  for (const auto &item : vec) {
    T *new_item = new T(item);
    list = alpm_list_add(list, new_item);
  }

  return list;
}

constexpr alpm_list_t *StringVectorToAlpmList(
    const std::vector<std::string> &vec) {
  alpm_list_t *list = nullptr;

  for (const auto &str : vec) {
    char *new_str = strndup(str.c_str(), str.size());
    list = alpm_list_add(list, new_str);
  }

  return list;
}

}  // namespace util

class Alpm {
 public:
  Alpm(std::string_view root, std::string_view dbpath);
  ~Alpm();

  alpm_db_t *GetLocalDb() const;

  static alpm_list_t *DbGetPkgCache(alpm_db_t *db);

  static alpm_pkg_t *DbGetPkg(alpm_db_t *db, std::string_view name);

  static const char *PkgGetName(alpm_pkg_t *pkg);

  static const char *PkgGetVersion(alpm_pkg_t *pkg);

  static const char *PkgGetDesc(alpm_pkg_t *pkg);

  static const char *PkgGetArch(alpm_pkg_t *pkg);

  static const char *PkgGetURL(alpm_pkg_t *pkg);

  static alpm_list_t *PkgGetLicenses(alpm_pkg_t *pkg);

  static alpm_list_t *PkgGetGroups(alpm_pkg_t *pkg);

  static alpm_list_t *PkgGetDepends(alpm_pkg_t *pkg);

  static alpm_list_t *PkgGetMakeDepends(alpm_pkg_t *pkg);

  static alpm_list_t *PkgGetCheckDepends(alpm_pkg_t *pkg);

  static alpm_list_t *PkgGetOptDepends(alpm_pkg_t *pkg);

  static alpm_list_t *PkgGetReplaces(alpm_pkg_t *pkg);

  static alpm_list_t *PkgGetConflicts(alpm_pkg_t *pkg);

  static alpm_list_t *PkgGetProvides(alpm_pkg_t *pkg);

  static const char *PkgGetPackager(alpm_pkg_t *pkg);

  static alpm_time_t PkgGetBuildDate(alpm_pkg_t *pkg);

  static alpm_pkgreason_t PkgGetReason(alpm_pkg_t *pkg);

  static off_t PkgGetISize(alpm_pkg_t *pkg) { return alpm_pkg_get_isize(pkg); }

  static alpm_time_t PkgGetInstallDate(alpm_pkg_t *pkg);

  static int PkgGetValidation(alpm_pkg_t *pkg);

  static bool PkgHasScriptlet(alpm_pkg_t *pkg);

  static alpm_list_t *PkgComputeRequiredBy(alpm_pkg_t *pkg);

  static alpm_list_t *PkgComputeOptionalFor(alpm_pkg_t *pkg);

  static void *PkgChangelogOpen(alpm_pkg_t *pkg);

  static std::size_t PkgChangelogRead(void *ptr, std::size_t size,
                                      const alpm_pkg_t *pkg, void *fp);

  static int PkgChangelogClose(const alpm_pkg_t *pkg, void *fp);

  static const char *DepComputeString(const alpm_depend_t *dep);

 private:
  alpm_handle_t *handle_;
  alpm_errno_t err = ALPM_ERR_OK;
};

}  // namespace pacmanpp

#endif  // PACMANPP_ALPM_H_