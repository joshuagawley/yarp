# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:
    result = test.run(["--root", str(env.root), "-Q"])
    test.assert_returncode(result, 0)
    test.assert_equals(result.stdout, "")

test.exit_with_result()
