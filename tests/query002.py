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
test.test_exact_output(["--root", "/var/empty",
                        # "--config", str(config_path)
                        "--dbpath", str(db_path), "-Q", "base"], "base 2-2\n")
test.exit_with_result()
