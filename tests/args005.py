# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])
test.test_verbose_output("-v")
test.exit_with_result()
