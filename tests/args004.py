# SPDX-License-Identifier: MIT

import subprocess
import sys

pacmanpp = sys.argv[1]

expected = "pacman 7.0.0.r6.gc685ae6-2\n"
output = subprocess.run(
    [pacmanpp, "-Q", "pacman"], capture_output=True, text=True
).stdout

if output != expected:
    print(f"Expected: {expected}")
    print(f"Output: {output}")
    return_code = 1
else:
    return_code = 0

sys.exit(return_code)
