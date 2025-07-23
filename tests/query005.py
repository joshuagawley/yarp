# SPDX-License-Identifier: MIT

import pptest
import sys

test = pptest.Test(sys.argv[1])

result = test.run(["-Ql", "alsa-lib"])

test.assert_returncode(result, 0)
test.assert_contains(result.stdout, "alsa-lib /var/empty/usr/bin/aserver\n")
test.assert_contains(result.stdout, "alsa-lib /var/empty/usr/include/alsa/asoundlib.h\n")
test.assert_contains(result.stdout, "alsa-lib /var/empty/usr/include/alsa/conf.h\n")
test.assert_contains(result.stdout, "alsa-lib /var/empty/usr/include/alsa/control.h\n")

test.exit_with_result()
