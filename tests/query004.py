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
    ["--root", "/var/empty", "--dbpath", str(db_path), "-Qi", "pacman"]
)

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "Name            : pacman\n")
test.assert_contains(result.stdout, "Version         : 5.2.2-3\n")
test.assert_contains(result.stdout, "Description     : A library-based package manager with dependency support\n")
test.assert_contains(result.stdout, "Architecture    : x86_64\n")
test.assert_contains(result.stdout, "URL             : https://www.archlinux.org/pacman/\n")
test.assert_contains(result.stdout, "Licenses        : GPL\n")
test.assert_contains(result.stdout, "Packager        : Allan McRae <allan@archlinux.org>\n")
test.assert_contains(result.stdout, "Groups          : base-devel\n")
test.assert_contains(result.stdout, "Provides        : libalpm.so\n")
test.assert_contains(result.stdout, "Depends On      : bash  glibc  libarchive  curl  gpgme  pacman-mirrorlist  archlinux-keyring\n")
test.assert_contains(result.stdout, "Required By     : base\n")
test.assert_contains(result.stdout, "Optional For    : None\n")
test.assert_contains(result.stdout, "Optional Deps   : perl-locale-gettext: translation support in makepkg-template\n")
test.assert_contains(result.stdout, "Conflicts With  : None\n")
test.assert_contains(result.stdout, "Replaces        : None\n")
test.assert_contains(result.stdout, "Installed Size  : 4.43 MiB\n")
# TODO: get tests for build and install dates working on github actions (needs some time zone shenanigans)
# test.assert_contains(result.stdout, "Build Date      : Sat 13 Jul 2024 17:31:39 BST\n")
# test.assert_contains(result.stdout, "Install Date    : Sun 22 Jun 2025 17:46:41 BST\n")
test.assert_contains(result.stdout, "Install Reason  : Installed as a dependency for another package\n")
test.assert_contains(result.stdout, "Install Script  : No\n")
test.assert_contains(result.stdout, "Validated By    : Signature\n")

test.exit_with_result()
