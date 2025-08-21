# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run_raw(test.yarp, ["--dbpath", str(test.db_path), "-Qo", "cmake"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "cmake is owned by cmake 3.20.0-1")

test.exit_with_result()