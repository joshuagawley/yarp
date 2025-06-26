# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:

    result = test.run(
        ["-Qk", "powertop"]
    )

    test.assert_returncode(result, 0)
    test.assert_contains(result.stdout, "powertop: 41 total files, 0 missing files")

test.exit_with_result()