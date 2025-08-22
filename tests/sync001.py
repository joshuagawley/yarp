# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Sa", "paru"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "paru-bin 2.1.0-1\n")
test.exit_with_result()
