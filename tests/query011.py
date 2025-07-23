# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Qe", "polybar"])

test.assert_returncode(result, 0)
test.assert_equals(result.stdout, "polybar 1.0.0-1\n")

test.exit_with_result()