# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Q", "bar"])

test.assert_returncode(result, 1)
test.assert_equals(result.stderr, "Error: package bar not found\n")

test.exit_with_result()
