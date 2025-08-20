# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])
test.test_exact_output(["-Q", "base"], "base 2-2\n")
test.exit_with_result()
