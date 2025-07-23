# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Qn", "pacman"])

# pacman is in official repo, so it's native (returns 0)
test.assert_returncode(result, 0)
test.assert_equals(result.stdout, "pacman 5.2.2-3\n")

test.exit_with_result()