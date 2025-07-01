// SPDX-License-Identifier: MIT

#ifndef PACMANPP_ALPMPP_DEPEND_H_
#define PACMANPP_ALPMPP_DEPEND_H_

#include <alpm.h>

#include <string>

namespace alpmpp {

enum class DepMod {
  kAny = ALPM_DEP_MOD_ANY,
  kEq = ALPM_DEP_MOD_EQ,
  kGt = ALPM_DEP_MOD_GT,
  kGe = ALPM_DEP_MOD_GE,
  kLt = ALPM_DEP_MOD_LT,
  kLe = ALPM_DEP_MOD_LE,
};

class AlpmDepend {
 public:
  constexpr AlpmDepend(alpm_depend_t *depend) : depend_(depend) {}

  constexpr std::string_view GetName() const noexcept { return depend_->name; }

  std::string ComputeString();

 private:
  alpm_depend_t *depend_;
};

}  // namespace alpmpp

#endif  // PACMANPP_ALPMPP_DEPEND_H_