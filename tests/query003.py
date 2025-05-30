# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])
test.test_exact_output(["-Q", "firefox"], "Error: package firefox not found\n")
test.exit_with_result()
