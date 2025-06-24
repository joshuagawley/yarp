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
    test.assert_contains(result.stdout, "URL             : https://github.com/fenrus75/powertop/\n")
    test.assert_contains(result.stdout, "Licenses        : GPL2\n")
    test.assert_contains(result.stdout, "Packager        : T.J. Townsend <blakkheim@archlinux.org>\n")
    test.assert_contains(result.stdout, "Groups          : None\n")
    test.assert_contains(result.stdout, "Provides        : None\n")
    test.assert_contains(result.stdout, "Depends On      : gcc-libs  libnl  ncurses  pciutils\n")
    test.assert_contains(result.stdout, "Optional Deps   : xorg-xset: for the --calibrate function\n")
    # test.assert_contains(result.stdout, "Required By     : None\n")
    # test.assert_contains(result.stdout, "Optional For    : None\n")
    test.assert_contains(result.stdout, "Conflicts With  : None\n")
    test.assert_contains(result.stdout, "Replaces        : None\n")
    test.assert_contains(result.stdout, "Installed Size  : 494.15 KiB\n")
    test.assert_contains(result.stdout, "Build Date      : Sat 13 Jul 2024 17:31:39 BST\n")
    test.assert_contains(result.stdout, "Install Date    : Sun 22 Jun 2025 17:46:41 BST\n")

test.exit_with_result()
