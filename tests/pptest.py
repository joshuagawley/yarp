# SPDX-License-Identifier: MIT

import os
import subprocess
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional


class TestResult:
    def __init__(self, returncode: int, stdout: str, stderr: str) -> None:
        self.returncode = returncode
        self.stdout = stdout
        self.stderr = stderr


class Test:
    def __init__(self, yarp_path: str) -> None:
        self.yarp = yarp_path
        self._failed: bool = False
        self._failure_messages: List[str] = []

        # yarp binary is located in [build-dir]/src/yarp
        build_dir = Path(yarp_path).parent.parent
        self.test_data_dir = build_dir / "tests" / "test-data"
        self.config_path = self.test_data_dir / "pacman.conf"
        self.db_path = self.test_data_dir / "db"

        self.mock_db_args = ["--root", "/var/empty", "--dbpath", str(self.db_path)]

    def run(self, args: List[str], env: Optional[Dict[str, str]] = None) -> TestResult:
        return self.run_raw(self.yarp, self.mock_db_args + args, env)

    def run_pacman(self, args: List[str], env: Optional[Dict[str, str]] = None) -> TestResult:
        return self.run_raw("pacman", self.mock_db_args + args, env)

    def run_raw(self, command: str, args: List[str], env: Optional[Dict[str, str]] = None) -> TestResult:
        """Run pacman or yarp without mock database arguments (for args tests)"""
        cmd = [command] + args

        result = subprocess.run(
            cmd, capture_output=True, text=True, env=env or os.environ.copy()
        )

        return TestResult(
            returncode=result.returncode, stdout=result.stdout, stderr=result.stderr
        )

    def assert_equals(self, actual: Any, expected: Any, message: str = "") -> None:
        if actual != expected:
            self._failed = True
            msg = f"Expected: {repr(expected)}\nActual: {repr(actual)}"
            if message:
                msg = f"{message}\n{msg}"
            self._failure_messages.append(msg)

    def assert_contains(self, haystack: str, needle: str, message: str = "") -> None:
        if needle not in haystack:
            self._failed = True
            msg = f"Expected to find: {repr(needle)}\nIn: {repr(haystack)}"
            if message:
                msg = f"{message}\n{msg}"
            self._failure_messages.append(msg)

    def assert_not_contains(
            self, haystack: str, needle: str, message: str = ""
    ) -> None:
        if needle in haystack:
            self._failed = True
            msg = f"Expected NOT to find: {repr(needle)}\nIn: {repr(haystack)}"
            if message:
                msg = f"{message}\n{msg}"
            self._failure_messages.append(msg)

    def assert_returncode(self, result: TestResult, expected: int) -> None:
        self.assert_equals(result.returncode, expected, "Return code mismatch")

    def fail(self, message: str) -> None:
        self._failed = True
        self._failure_messages.append(message)

    def exit_with_result(self) -> None:
        if self._failed:
            for msg in self._failure_messages:
                print(msg, file=sys.stderr)
            sys.exit(1)
        sys.exit(0)

    def test_exact_output(
            self, args: List[str], expected_stdout: str, expected_returncode: int = 0
    ) -> None:
        result = self.run(args)
        self.assert_returncode(result, expected_returncode)
        self.assert_equals(result.stdout, expected_stdout, "Output mismatch")

    def test_error_contains(
            self, args: List[str], expected_error: str, expected_returncode: int = 1
    ) -> None:
        result = self.run_raw(args)
        self.assert_returncode(result, expected_returncode)
        self.assert_contains(result.stderr, expected_error, "Error message mismatch")

    def test_help_output(self, help_flag: str) -> None:
        result = self.run_raw(self.yarp, [help_flag])
        self.assert_returncode(result, 0)
        self.assert_contains(result.stdout, f"Usage: yarp <operation>")
        self.assert_contains(result.stdout, "operations:")
        self.assert_contains(result.stdout, "  yarp {-h, --help}")
        self.assert_contains(result.stdout, "  yarp {-V, --version}")
        self.assert_contains(result.stdout, "  yarp {-Q, --query} [options] [package(s)]")
        self.assert_contains(result.stdout, "  yarp {-S, --sync}  [options] [package(s)]")
        self.assert_contains(result.stdout, "Use 'yarp' {-h --help} with an operation for available options")

    def test_verbose_output(self, verbose_flag: str) -> None:
        result = self.run_raw(self.yarp, [verbose_flag])
        self.assert_returncode(result, 1)
        self.assert_contains(result.stdout, "Root       : /")
        self.assert_contains(result.stdout, "DB Path    : /var/lib/pacman")
        self.assert_contains(result.stdout, "Cache Dirs : /var/cache/pacman/pkg/")
        self.assert_contains(result.stdout, "Hook Dirs  : /usr/share/libalpm/hooks/ /etc/pacman.d/hooks/")
        self.assert_contains(result.stdout, "Log File   : /var/log/pacman.log/")
        self.assert_contains(result.stdout, "GPG Dir    : /etc/pacman.d/gnupg/")
        self.assert_contains(result.stdout, "Targets    : None")

    def test_version_output(self, version_flag: str) -> None:
        result = self.run_raw(self.yarp, [version_flag])
        self.assert_returncode(result, 0)
        self.assert_equals(result.stdout, "yarp version 0.0.0\n")
