# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run_raw(test.yarp, ["-Qh"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "Usage: yarp {-Q --query} [options] [packages(s)]")
test.assert_contains(result.stdout, "options:")
test.assert_contains(result.stdout, "-b, --dbpath <path>")
test.assert_contains(result.stdout, "-c, --changelog")
test.assert_contains(result.stdout, "-d, --deps")
test.assert_contains(result.stdout, "-e, --explicit")
test.assert_contains(result.stdout, "-g, --groups")
test.assert_contains(result.stdout, "-i, --info")
test.assert_contains(result.stdout, "-k, --check")
test.assert_contains(result.stdout, "-l, --list")
test.assert_contains(result.stdout, "-m, --foreign")
test.assert_contains(result.stdout, "-n, --native")
test.assert_contains(result.stdout, "-o, --owns <file>")
test.assert_contains(result.stdout, "-p, --file <package>")
test.assert_contains(result.stdout, "-r, --root <path>")
test.assert_contains(result.stdout, "-s, --search <regex>")
test.assert_contains(result.stdout, "-t, --unrequired")
test.assert_contains(result.stdout, "-u, --upgrades")
test.assert_contains(result.stdout, "-v, --verbose")

test.exit_with_result()