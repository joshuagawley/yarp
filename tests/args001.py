# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])
result = test.run([])

test.test_exact_output(
    [], "Error: no operation specified (use -h for help)\n", expected_returncode=1
)
test.exit_with_result()
