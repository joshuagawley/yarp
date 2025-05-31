# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:
    result = test.run(["-r /", "-b /var/lib/pacman"])
    test.assert_returncode(result, 1)
    test.assert_equals(
        result.stdout, "Error: no operation specified (use -h for help)\n"
    )

test.exit_with_result()
