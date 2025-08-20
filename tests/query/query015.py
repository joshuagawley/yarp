# SPDX-License-Identifier: MIT

import pathlib
import sys
sys.path.insert(0, '..')
import pptest

test = pptest.Test(sys.argv[1], pathlib.Path(sys.argv[2]))

# This test queries a package file directly, not the database
result = test.run(["-Qp", "../yay-12.5.0-1-x86_64.pkg.tar.zst"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "yay 12.5.0-1")

test.exit_with_result()