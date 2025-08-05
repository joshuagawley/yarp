# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Qo", "bar"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "Error: no package owns bar")

test.exit_with_result()