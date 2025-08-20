# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Qg"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "base-devel pacman\n")

test.exit_with_result()
