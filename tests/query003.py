# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:
    env.add_package("foo", "1.0.0", "foo")

    result = test.run(
        ["--root", str(env.root), "--dbpath", str(env.db_path), "-Q", "bar"]
    )

    test.assert_returncode(result, 0)
    test.assert_equals(result.stdout, "Error: package bar not found\n")

test.exit_with_result()
