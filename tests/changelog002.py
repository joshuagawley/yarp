# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:

    # TODO: Add a mock package intead of using a real one
    result = test.run(
        ["-Q", "--changelog", "powertop"]
    )

    test.assert_returncode(result, 0)
    test.assert_contains(result.stdout, "Changelog for powertop:")
    test.assert_contains(result.stdout, "2012-05-12 Jaroslav Lichtblau (Dragonlord) <svetlemodry@archlinux.org>\n\t* powertop 2.0-1")

test.exit_with_result()
