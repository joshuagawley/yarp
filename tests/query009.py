# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:

    result = test.run(
        ["-Qd", "gcc-libs"]
    )

    test.assert_returncode(result, 0)
    test.assert_contains(result.stdout, "gcc-libs 15.1.1+r7+gf36ec88aa85a-1")

test.exit_with_result()