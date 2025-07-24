# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Qk", "alsa-lib"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "alsa-lib:")
test.assert_contains(result.stdout, "total files")

test.exit_with_result()