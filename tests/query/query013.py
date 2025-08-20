# SPDX-License-Identifier: MIT

import pathlib
import sys
sys.path.insert(0, '..')
import pptest

test = pptest.Test(sys.argv[1], pathlib.Path(sys.argv[2]))

result = test.run(["-Qm", "pacman"])

# pacman is in official repo, so it's not foreign (returns 1)
test.assert_returncode(result, 1)
test.assert_contains(result.stdout, "")

test.exit_with_result()