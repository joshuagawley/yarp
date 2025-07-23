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

# Query for a non-existent package
result = test.run(
    ["--root", "/var/empty", "--dbpath", str(db_path), "-Q", "bar"]
)

test.assert_returncode(result, 1)
test.assert_equals(result.stdout, "Error: package bar not found\n")

test.exit_with_result()
