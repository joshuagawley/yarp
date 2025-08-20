# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Qd", "pacman"])

test.assert_returncode(result, 0)
test.assert_equals(result.stdout, "pacman 5.2.2-3\n")

test.exit_with_result()