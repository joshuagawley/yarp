# SPDX-License-Identifier: MIT

import subprocess
import sys

pacmanpp = sys.argv[1]

expected = "Error: package firefox not found\n"
output = subprocess.run(
    [pacmanpp, "-Q", "firefox"], capture_output=True, text=True
).stdout

if output != expected:
    print(f"Expected: {expected}")
    print(f"Output: {output}")
    return_code = 1
else:
    return_code = 0

sys.exit(return_code)
