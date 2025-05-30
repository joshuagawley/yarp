# SPDX-License-Identifier: MIT

import os
import pathlib
import shutil
import subprocess
import sys
import tempfile
from typing import Any, Dict, List, Optional


class TestResult:
    def __init__(self, returncode: int, stdout: str, stderr: str) -> None:
        self.returncode = returncode
        self.stdout = stdout
        self.stderr = stderr


class TestEnvironment:
    def __enter__(self):
        self.temp_dir = tempfile.mkdtemp(prefix="pacmanpp_test_")
        self.root = pathlib.Path(self.temp_dir)
        self.db_path = self.root / "db"

        # Create directory structure
        self.root.mkdir(parents=True)
        self.db_path.mkdir(parents=True)

        # Create a minimal ALPM database structure
        local_db = self.db_path / "local"
        local_db.mkdir(parents=True)

        (local_db / "ALPM_DB_VERSION").write_text("9\n")

        return self

    def __exit__(self, type, value, traceback):
        if self.temp_dir and os.path.exists(self.temp_dir):
            shutil.rmtree(self.temp_dir)

    def add_package(
        self, name: str, version: str, description: str = "Test package"
    ) -> None:
        pkg_dir = self.db_path / "local" / f"{name}-{version}"
        pkg_dir.mkdir(parents=True)

        (pkg_dir / "desc").write_text(
            f"%NAME%\n{name}\n\n"
            f"%VERSION%\n{version}\n\n"
            f"%DESC%\n{description}\n\n"
        )


class Test:
    def __init__(self, pacmanpp_path: str) -> None:
        self.pacmanpp = pacmanpp_path
        self._failed: bool = False
        self._failure_messages: List[str] = []

    def run(self, args: List[str], env: Optional[Dict[str, str]] = None) -> TestResult:
        cmd = [self.pacmanpp] + args

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
        self.assert_equals(result.stderr, expected_stdout, "Output mismatch")

    def test_error_contains(
        self, args: List[str], expected_error: str, expected_returncode: int = 1
    ) -> None:
        result = self.run(args)
        self.assert_returncode(result, expected_returncode)
        self.assert_contains(result.stderr, expected_error, "Error message mismatch")

    def test_help_output(self, help_flag: str) -> None:
        result = self.run([help_flag])
        self.assert_returncode(result, 0)
        self.assert_contains(result.stdout, f"Usage: {self.pacmanpp} <operation>")
        self.assert_contains(result.stdout, "operations:")
        self.assert_contains(result.stdout, "{-h, --help}")
        self.assert_contains(result.stdout, "{-Q, --query}")
