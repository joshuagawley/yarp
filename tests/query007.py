# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:

    result = test.run(
        ["-Qg", "ttf-nerd-fonts-symbols"]
    )

    test.assert_returncode(result, 0)
    test.assert_contains(result.stdout, "nerd-fonts ttf-nerd-fonts-symbols\n")

test.exit_with_result()