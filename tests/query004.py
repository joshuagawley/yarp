# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:
    pkg = env.create_package("foo", "1.0.0", "bar")
    pkg.files = ["bin/foobar"]
    pkg.groups = ["bar"]
    pkg.url = "https://foobar.com/foo"
    pkg.license = "MIT"
    pkg.architecture = "x86_64"
    pkg.packager = "Joshua Gawley"

    result = test.run(
        ["--root", str(env.root), "--dbpath", str(env.db_path), "-Qi", "foo"]
    )

    test.assert_returncode(result, 0)
    test.assert_contains(result.stdout, "Name            : foo\n")
    test.assert_contains(result.stdout, "Version         : 1.0.0\n")
    test.assert_contains(result.stdout, "Description     : bar\n")
    test.assert_contains(result.stdout, "Architecture    : x86_64\n")
    # test.assert_contains(result.stdout, "URL             : https://foobar.com/foo\n")
    # test.assert_contains(result.stdout, "License         : MIT\n")
    # test.assert_contains(result.stdout, "Packager        : Joshua Gawley\n")
