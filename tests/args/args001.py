# SPDX-License-Identifier: MIT

import pathlib
import sys
sys.path.insert(0, '..')
import pptest

test = pptest.Test(sys.argv[1], pathlib.Path(sys.argv[2]))
result = test.run([])

test.assert_returncode(result, 1)
test.assert_equals(result.stderr, "Error: no operation specified (use -h for help)\n")

test.exit_with_result()
