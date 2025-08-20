# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run_raw(test.pacmanpp, ["--dbpath", str(test.db_path), "-Qs", "pacmanpp"])

test.assert_returncode(result, 1)
test.assert_contains(result.stdout, "")

test.exit_with_result()