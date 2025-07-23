# SPDX-License-Identifier: MIT

import pptest
import sys
from pathlib import Path

test = pptest.Test(sys.argv[1])

# Get the path to the test-data directory in the build directory
build_dir = Path(sys.argv[1]).parent
test_data_dir = build_dir / "tests" / "test-data"
config_path = test_data_dir / "pacman.conf"
db_path = test_data_dir / "db"

# Run the query using the mock database
result = test.run([
    "--root", "/var/empty",
    # "--config", str(config_path)
    "--dbpath", str(db_path),
    "-Q", "pacman"
])

# Check the result
test.assert_returncode(result, 0)
test.assert_equals(result.stdout, "pacman 5.2.2-3\n")
test.exit_with_result()
