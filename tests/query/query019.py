# SPDX-License-Identifier: MIT

import pathlib
import sys
sys.path.insert(0, '..')
import pptest

test = pptest.Test(sys.argv[1], pathlib.Path(sys.argv[2]))

result = test.run_raw(test.pacmanpp, ["--dbpath", str(test.db_path), "-Qo", "bar"])

test.assert_returncode(result, 1)
test.assert_contains(result.stderr, "Error: Could not find bar in PATH")

test.exit_with_result()