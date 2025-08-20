# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Qm", "pacman"])

# pacman is in official repo, so it's not foreign (returns 1)
test.assert_returncode(result, 1)
test.assert_contains(result.stdout, "")

test.exit_with_result()