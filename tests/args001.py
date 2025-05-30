# SPDX-License-Identifier: MIT

# import subprocess
# import sys

# pacmanpp = sys.argv[1]

# expected = "Error: no operation specified (use -h for help)"
# output = subprocess.run([pacmanpp], capture_output=True, text=True).stdout.strip()

# if output != expected:
#     print(f"Expected: {expected}")
#     print(f"Output: {output}")
#     return_code = 1
# else:
#     return_code = 0

# sys.exit(return_code)

import pptest
import sys

test = pptest.Test(sys.argv[1])
result = test.run([])

test.test_exact_output(
    [], "Error: no operation specified (use -h for help)\n", expected_returncode=1
)
test.exit_with_result()
