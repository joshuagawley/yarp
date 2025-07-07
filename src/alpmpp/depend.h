// SPDX-License-Identifier: MIT

#ifndef ALPMPP_DEPEND_H_
#define ALPMPP_DEPEND_H_

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

  [[nodiscard]] constexpr std::string_view GetName() const noexcept {
    return depend_->name;
  }

  [[nodiscard]] std::string ComputeString() const;

 private:
  alpm_depend_t *depend_;
};

}  // namespace alpmpp

#endif  // ALPMPP_DEPEND_H_