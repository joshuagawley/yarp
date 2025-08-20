# SPDX-License-Identifier: MIT

import pathlib
import sys
sys.path.insert(0, '..')
import pptest

test = pptest.Test(sys.argv[1], pathlib.Path(sys.argv[2]))

result = test.run(["-Qn", "pacman"])

# pacman is in official repo, so it's native (returns 0)
test.assert_returncode(result, 0)
test.assert_equals(result.stdout, "pacman 5.2.2-3\n")

test.exit_with_result()