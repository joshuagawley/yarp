# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])
result = test.run([])

test.assert_returncode(result, 1)
test.assert_equals(result.stderr, "Error: no operation specified (use -h for help)\n")

test.exit_with_result()
