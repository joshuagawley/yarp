# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:
    pkg = pptest.TestPackage("foo", "1.0.0", "foo")

    pkg.add_changelog_entry("1.0.0", ["stable release"])
    pkg.add_changelog_entry("0.9.0", ["remove foo"])

    env.add_package(pkg)

    result = test.run(
        ["-v", "--root", str(env.root), "--dbpath", str(env.db_path), "-Qc", "foo"]
    )

    test.assert_returncode(result, 0)
    test.assert_contains(result.stdout, "Changelog for foo:")
    test.assert_contains(result.stdout, "1.0.0:\nstable release")
    test.assert_contains(result.stdout, "0.9.0:\nremove foo")

test.exit_with_result()
