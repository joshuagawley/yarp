# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:

    result = test.run(
        ["-Qe", "powertop"]
    )

    test.assert_returncode(result, 0)
    test.assert_contains(result.stdout, "jsoncpp 1.9.6-3")

test.exit_with_result()