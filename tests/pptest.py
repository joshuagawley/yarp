# SPDX-License-Identifier: MIT

import os
import pathlib
import shutil
import subprocess
import sys
import tempfile
from typing import Any, Dict, List, Optional


class TestChangelogEntry:
    def __init__(self, version: str, changes: List[str]) -> None:
        self.version = version
        self.changes = changes

    def to_changelog_format(self) -> str:
        lines = [f"{self.version}:"]
        lines.extend(self.changes)
        return "\n".join(lines)


class TestPackage:
    def __init__(
        self,
        name: str,
        version: str,
        description: str = "Test package",
    ) -> None:
        self.name = name
        self.version = version
        self.description = description
        self.changelog_entries: List[TestChangelogEntry] = []

    def add_changelog_entry(self, version: str, changes: List[str]) -> None:
        entry = TestChangelogEntry(version, changes)
        self.changelog_entries.append(entry)

    def get_changelog_text(self) -> str:
        header = f"Changelog for {self.name}"
        entries = "\n\n".join(
            entry.to_changelog_format() for entry in self.changelog_entries
        )
        return f"{header}\n{entries}"

    def to_desc_format(self) -> str:
        desc_parts = [
            f"%NAME%\n{self.name}\n",
            f"%VERSION%\n{self.version}\n",
            f"%DESC%\n{self.description}",
        ]

        return "\n".join(desc_parts)


class TestResult:
    def __init__(self, returncode: int, stdout: str, stderr: str) -> None:
        self.returncode = returncode
        self.stdout = stdout
        self.stderr = stderr


class TestEnvironment:
    def __init__(self) -> None:
        self.packages: Dict[str, TestPackage] = {}

    def __enter__(self):
        self.temp_dir = tempfile.mkdtemp(prefix="pacmanpp_test_")
        self.root = pathlib.Path(self.temp_dir) / "root"
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

    def add_package(self, pkg: TestPackage) -> None:
        self.packages[pkg.name] = pkg
        self._write_pkg_to_fs(pkg)

    def create_package(
        self, name: str, version: str, description: str = "Test package"
    ) -> TestPackage:
        pkg = TestPackage(name=name, version=version, description=description)
        self.add_package(pkg)
        return pkg

    def get_package(self, name: str) -> Optional[TestPackage]:
        return self.packages.get(name)

    def remove_package(self, name: str) -> bool:
        if name in self.packages:
            pkg = self.packages[name]
            del self.packages[name]

            pkg_dir = self.db_path / "local" / f"{pkg.name}-{pkg.version}"
            if pkg_dir.exists():
                shutil.rmtree(pkg_dir)
            return True
        return False

    def _write_pkg_to_fs(self, pkg: TestPackage) -> None:
        pkg_dir = self.db_path / "local" / f"{pkg.name}-{pkg.version}"
        pkg_dir.mkdir(parents=True)

        (pkg_dir / "desc").write_text(pkg.to_desc_format())

        if pkg.changelog_entries:
            (pkg_dir / "changelog").write_text(pkg.get_changelog_text())


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
        self.assert_equals(result.stdout, expected_stdout, "Output mismatch")

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

    def test_verbose_output(self, verbose_flag: str) -> None:
        result = self.run([verbose_flag])
        self.assert_returncode(result, 1)
        self.assert_contains(result.stdout, "Root      : /")
        self.assert_contains(result.stdout, "DB Path   : /var/lib/pacman")
