# SPDX-License-Identifier: MIT

import pathlib
import sys
sys.path.insert(0, '..')
import pptest

test = pptest.Test(sys.argv[1], pathlib.Path(sys.argv[2]))

result = test.run(["-Qe", "jsoncpp"])

# jsoncpp is not found as explicitly installed in mock database
test.assert_returncode(result, 1)
test.assert_contains(result.stdout, "")

test.exit_with_result()