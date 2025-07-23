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
    ["--root", "/var/empty", "--dbpath", str(db_path), "-Ql", "alsa-lib"]
)

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "alsa-lib /var/empty/usr/bin/aserver\n")
test.assert_contains(result.stdout, "alsa-lib /var/empty/usr/include/alsa/asoundlib.h\n")
test.assert_contains(result.stdout, "alsa-lib /var/empty/usr/include/alsa/conf.h\n")
test.assert_contains(result.stdout, "alsa-lib /var/empty/usr/include/alsa/control.h\n")

test.exit_with_result()
