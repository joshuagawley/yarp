# SPDX-License-Identifier: MIT

import pathlib
import sys
sys.path.insert(0, '..')
import pptest

test = pptest.Test(sys.argv[1], pathlib.Path(sys.argv[2]))

result = test.run_raw(test.pacmanpp, ["--dbpath", str(test.db_path), "-Qo", "cmake"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "cmake is owned by cmake 3.20.0-1")

test.exit_with_result()