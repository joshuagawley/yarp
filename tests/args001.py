# SPDX-License-Identifier: MIT

import subprocess
import sys

pacmanpp = sys.argv[1]

expected = "Error: no operation specified (use -h for help)"
output = subprocess.run([pacmanpp], capture_output=True, text=True).stdout.strip()

if output != expected:
    print(f"Expected: {expected}")
    print(f"Output: {output}")
    return_code = 1
else:
    return_code = 0

sys.exit(return_code)
