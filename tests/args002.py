# SPDX-License-Identifier: MIT

import subprocess
import sys

pacmanpp = sys.argv[1]

expected = f"Usage: {pacmanpp} <operation>\noperations:\n  pacmanpp {{-h, --help}}                  Show this help message\n  pacmanpp {{-Q, --query}} [package(s)]    Query installed packages\n"
output = subprocess.run([pacmanpp, "-h"], capture_output=True, text=True).stdout

if output != expected:
    print(f"Expected: {expected}")
    print(f"Output: {output}")
    return_code = 1
else:
    return_code = 0

sys.exit(return_code)
