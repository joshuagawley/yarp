# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Sa", "paru"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "aur/paru 2.1.0-1 [+1068 ~22]\n    Feature packed AUR helper\n")
test.exit_with_result()
