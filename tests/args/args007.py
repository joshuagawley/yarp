# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])
result = test.run(["--verbose", "--config", "test_pacman_conf.conf"])

test.assert_returncode(result, 1)
test.assert_contains(result.stdout, "Conf File  : test_pacman_conf.conf\n")
test.exit_with_result()
