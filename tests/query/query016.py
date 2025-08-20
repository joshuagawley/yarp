# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Qt", "libfoo"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "libfoo 1.0.0-1")

test.exit_with_result()