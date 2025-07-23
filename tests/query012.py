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

result = test.run(
    ["--root", "/var/empty", "--dbpath", str(db_path), "-Qk", "alsa-lib"]
)

test.assert_returncode(result, 0)
# TODO: CI run prints missing files instead of expected `0 missing files`
test.assert_contains(result.stdout, "alsa-lib:")
test.assert_contains(result.stdout, "total files")

test.exit_with_result()