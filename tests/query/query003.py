# SPDX-License-Identifier: MIT

import pathlib
import sys
sys.path.insert(0, '..')
import pptest

test = pptest.Test(sys.argv[1], pathlib.Path(sys.argv[2]))

result = test.run(["-Q", "bar"])

test.assert_returncode(result, 1)
test.assert_equals(result.stderr, "Error: package bar not found\n")

test.exit_with_result()
