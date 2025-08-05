# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Qo", "pacman"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "/usr/bin/pacman is owned by pacman 5.2.2-3")

test.exit_with_result()