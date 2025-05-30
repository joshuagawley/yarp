# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

with pptest.TestEnvironment() as env:
    result = test.run(["--root", str(env.root), "--dbpath", str(env.db_path), "-Q"])
    test.assert_returncode(result, 0)
    test.assert_equals(result.stdout, "")

test.exit_with_result()
