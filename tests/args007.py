# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])
result = test.run(["--config", "test_pacman_conf.py"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "Conf File : test_pacman_conf.conf\n")
test.exit_with_result()
