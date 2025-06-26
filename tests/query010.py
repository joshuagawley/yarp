# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:

    result = test.run(
        ["-Qe", "jsoncpp"]
    )

    test.assert_returncode(result, 1)
    test.assert_contains(result.stdout, "")

test.exit_with_result()