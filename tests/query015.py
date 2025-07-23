# SPDX-License-Identifier: MIT

from pathlib import Path
import pptest
import sys

test = pptest.Test(sys.argv[1])

# Get the path to the test-data directory in the build directory
build_dir = Path(sys.argv[1]).parent
test_data_dir = build_dir / "tests" / "test-data"
config_path = test_data_dir / "pacman.conf"
db_path = test_data_dir / "db"

# This test queries a package file directly, not the database
result = test.run(
    ["-Qp", "yay-12.5.0-1-x86_64.pkg.tar.zst"]
)

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "yay 12.5.0-1")

test.exit_with_result()