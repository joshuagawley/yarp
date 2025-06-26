# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])
test.test_version_output("--version")
test.exit_with_result()