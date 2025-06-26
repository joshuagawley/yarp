# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:

    result = test.run(
        ["-Ql", "powertop"]
    )

    test.assert_returncode(result, 0)
    test.assert_contains(result.stdout, "powertop /usr/bin/powertop\n")
    test.assert_contains(result.stdout, "powertop /usr/share/bash-completion/completions/powertop\n")
    test.assert_contains(result.stdout, "powertop /usr/share/man/man8/powertop.8.gz\n")
    test.assert_contains(result.stdout, "powertop /usr/share/locale/en_GB/LC_MESSAGES/powertop.mo\n")

test.exit_with_result()
