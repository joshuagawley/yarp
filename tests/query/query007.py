# SPDX-License-Identifier: MIT

import pathlib
import sys
sys.path.insert(0, '..')
import pptest

test = pptest.Test(sys.argv[1], pathlib.Path(sys.argv[2]))

result = test.run(["-Qg", "base-devel"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "base-devel pacman\n")

test.exit_with_result()