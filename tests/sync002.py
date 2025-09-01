# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Ss", "pacman"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "core/pacman 5.2.2-3 (base-devel)\n    A library-based package manager with dependency support\n")
test.exit_with_result()
