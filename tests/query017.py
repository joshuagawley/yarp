# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])
# Refresh sync database to latest version
test.run_pacman(["-Sy"])

result = test.run(["-Qu", "cmake"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "cmake 3.20.0-1 -> 3.20.2-1")

test.exit_with_result()