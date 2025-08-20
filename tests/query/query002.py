# SPDX-License-Identifier: MIT

import pathlib
import sys
sys.path.insert(0, '..')
import pptest

test = pptest.Test(sys.argv[1], pathlib.Path(sys.argv[2]))
test.test_exact_output(["-Q", "base"], "base 2-2\n")
test.exit_with_result()
