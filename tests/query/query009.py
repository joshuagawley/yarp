# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Qd", "jsoncpp"])

# jsoncpp is found as dependency in mock database
test.assert_returncode(result, 0)
test.assert_equals(result.stdout, "jsoncpp 1.9.4-1\n")

test.exit_with_result()