# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:
    pkg = env.create_package("foo", "1.0.0", "foo")

    pkg.add_changelog_entry("1.0.0", ["stable release"])
    pkg.add_changelog_entry("0.9.0", ["remove foo"])

    result = test.run(
        ["--root", str(env.root), "--dbpath", str(env.db_path), "-Qc", "bar"]
    )

    test.assert_returncode(result, 0)
    test.assert_contains(result.stdout, "Changelog for foo:")
    test.assert_contains(result.stdout, "1.0.0:\nstable release")
    test.assert_contains(result.stdout, "0.9.0:\nremove foo")

test.exit_with_result()
