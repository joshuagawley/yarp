# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:

    result = test.run(
        ["-Qi", "powertop"]
    )

    test.assert_returncode(result, 0)
    test.assert_contains(result.stdout, "Name            : powertop\n")
    test.assert_contains(result.stdout, "Version         : 2.15-2\n")
    test.assert_contains(result.stdout, "Description     : A tool to diagnose issues with power consumption and power management\n")
    test.assert_contains(result.stdout, "Architecture    : x86_64\n")
    # test.assert_contains(result.stdout, "URL             : https://foobar.com/foo\n")
    # test.assert_contains(result.stdout, "License         : MIT\n")
    # test.assert_contains(result.stdout, "Packager        : Joshua Gawley\n")

test.exit_with_result()
