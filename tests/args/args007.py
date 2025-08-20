# SPDX-License-Identifier: MIT

import pathlib
import sys
sys.path.insert(0, '..')
import pptest

test = pptest.Test(sys.argv[1], pathlib.Path(sys.argv[2]))
result = test.run(["--verbose", "--config", "../test_pacman_conf.conf"])

test.assert_returncode(result, 1)
test.assert_contains(result.stdout, "Conf File  : ../test_pacman_conf.conf\n")
test.exit_with_result()
