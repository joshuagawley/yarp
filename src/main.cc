// SPDX-License-Identifier: MIT

#include <cstddef>
#include <print>

#include "app.h"

int main(int argc, char **argv) {
  try {
    auto app = pacmanpp::App{{argv, static_cast<std::size_t>(argc)}};
    return app.Run();
  } catch (const std::exception &e) {
    std::println(stderr, "Error: {}", e.what());
    return EXIT_FAILURE;
  }
}
