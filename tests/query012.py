# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:

    result = test.run(
        ["-Qk", "pacman"]
    )

    test.assert_returncode(result, 0)
    test.assert_contains(result.stdout, "pacman: 422 total files, 0 missing files")

test.exit_with_result()