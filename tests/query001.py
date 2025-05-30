# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])
test.test_exact_output(["-Q", "pacman"], "pacman 7.0.0.r6.gc685ae6-2\n")
test.exit_with_result()
