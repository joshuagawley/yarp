// SPDX-License-Identifier: MIT

#ifndef ALPMPP_FILE_H_
#define ALPMPP_FILE_H_

#include <alpm.h>
#include <alpm_list.h>
#include <sys/types.h>

#include <string_view>

namespace alpmpp {

class AlpmFile {
 public:
  constexpr explicit AlpmFile(alpm_file_t *file) : file_(file) {}

  [[nodiscard]] constexpr std::string_view name() const noexcept { return file_->name; }
  [[nodiscard]] constexpr off_t size() const noexcept { return file_->size; }
  [[nodiscard]] constexpr mode_t mode() const noexcept {
    return file_->mode;
  }

 private:
  alpm_file_t *file_;
};

}  // namespace alpmpp

#endif  // ALPMPP_FILE_H_