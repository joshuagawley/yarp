# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run_raw(test.pacmanpp, ["--dbpath", str(test.db_path), "-Qs", "pacman"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "local/pacman 5.2.2-3 (base-devel)\n    A library-based package manager with dependency support")

test.exit_with_result()