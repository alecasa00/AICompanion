#!/usr/bin/env python3
"""Reject common credential files and secret values from the Git index."""

from __future__ import annotations

import re
import subprocess
import sys
from pathlib import PurePosixPath


SENSITIVE_NAMES = {"secrets.h", ".env"}
SENSITIVE_SUFFIXES = {".pem", ".key", ".p12", ".pfx", ".jks", ".keystore"}
PLACEHOLDERS = ("your_", "change_me", "changeme", "placeholder", "example", "dummy", "<", "todo")

VALUE_ASSIGNMENT = re.compile(
    r"(?im)^\s*(?:#\s*define\s+)?(?:WIFI_SSID|WIFI_PASSWORD|GEMINI_API_KEY)"
    r"\s*(?:=|\s)\s*[\"']?([^\"'\s;]+)"
)
PRIVATE_KEY = re.compile(r"-----BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY-----")
KNOWN_TOKENS = re.compile(
    r"(?:AIza[0-9A-Za-z_-]{30,}|\bAKIA[0-9A-Z]{16}\b|"
    r"\bgh[pousr]_[A-Za-z0-9]{30,}\b|\bgithub_pat_[A-Za-z0-9_]{30,}\b)"
)


def run_git(*args: str) -> bytes:
    return subprocess.check_output(["git", *args])


def is_sensitive_path(path: str) -> bool:
    name = PurePosixPath(path).name.lower()
    if name == "secrets.example.h" or name.startswith("credentials.example."):
        return False
    return (
        name in SENSITIVE_NAMES
        or name.startswith(".env.")
        or name.startswith("credentials.")
        or PurePosixPath(name).suffix in SENSITIVE_SUFFIXES
    )


def has_real_assignment(text: str) -> bool:
    for match in VALUE_ASSIGNMENT.finditer(text):
        value = match.group(1).strip().lower()
        if value and not any(marker in value for marker in PLACEHOLDERS):
            return True
    return False


def main() -> int:
    try:
        paths = [p.decode("utf-8", "surrogateescape") for p in run_git(
            "diff", "--cached", "--name-only", "--diff-filter=ACMR", "-z"
        ).split(b"\0") if p]
    except (OSError, subprocess.CalledProcessError):
        print("Secret check failed: could not read staged files; commit stopped.", file=sys.stderr)
        return 2

    blocked = []
    for path in paths:
        if is_sensitive_path(path):
            blocked.append(path)
            continue
        try:
            content = run_git("show", f":{path}").decode("utf-8", "replace")
        except subprocess.CalledProcessError:
            continue
        if PRIVATE_KEY.search(content) or KNOWN_TOKENS.search(content) or has_real_assignment(content):
            blocked.append(path)

    if blocked:
        print("Commit stopped: possible secrets detected in staged file(s):", file=sys.stderr)
        for path in blocked:
            print(f"  {path}", file=sys.stderr)
        print(
            "Remove the credential from the index, keep it in a local ignored file, "
            "and stage the safe version. The detected value was not printed.",
            file=sys.stderr,
        )
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
