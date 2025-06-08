# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:
    pkg = env.create_package("foo", "1.0.0", "foo")

    result = test.run(
        ["--root", str(env.root), "--dbpath", str(env.db_path), "-Q", "foo"]
    )

    test.assert_returncode(result, 0)
    test.assert_equals(result.stdout, "foo 1.0.0\n")
