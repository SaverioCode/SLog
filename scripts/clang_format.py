#!/usr/bin/env python3

import argparse
import subprocess
import sys
from pathlib import Path

EXTENSIONS: set[str] = {".cpp", ".hpp", ".ipp"}
TARGET_DIRS: set[str] = {"include", "src"}

def get_staged_files(project_root: Path) -> list[Path]:
    """Returns absolute paths of files currently in the git index."""
    result = subprocess.run(
        ["git", "diff", "--name-only", "--cached"],
        capture_output=True, text=True, check=True, cwd=project_root
    )
    # Filter empty strings and join with root
    return [project_root / f for f in result.stdout.splitlines() if f]

def get_all_files(project_root: Path) -> list[Path]:
    """Returns absolute paths of all files in TARGET_DIRS."""
    files: list[Path] = []
    for folder in TARGET_DIRS:
        path = project_root / folder
        if path.exists():
            files.extend(path.rglob("*"))
    return files

def check_clang_format() -> None:
    """Check if clang-format is installed."""
    try:
        subprocess.run(["clang-format", "--version"], capture_output=True, check=True)
    except (FileNotFoundError, subprocess.CalledProcessError):
        print("ERROR: clang-format is not installed or not in PATH.")
        sys.exit(1)

def main() -> None:
    check_clang_format()

    parser = argparse.ArgumentParser(description="SLog Clang-Format Wrapper")
    parser.add_argument("--staged", action="store_true", help="Only format files in the git index")
    args = parser.parse_args()

    # Locate project root (one level up from scripts/)
    project_root: Path = Path(__file__).resolve().parent.parent
    # Get raw list of files
    candidates: list[Path] = get_staged_files(project_root) if args.staged else get_all_files(project_root)

    # Filter out non-target files
    to_format: list[Path] = []
    for f in candidates:
        if not f.is_file() or f.suffix not in EXTENSIONS:
            continue
        try:
            # Check relative path to avoid parent folder name collisions
            rel_path: Path = f.relative_to(project_root)
            # rel_path.parts[0] is the top-most folder ('src' or 'include')
            if rel_path.parts and rel_path.parts[0] in TARGET_DIRS:
                to_format.append(f)
        except ValueError:
            continue

    # Format files
    for file_to_format in to_format:
        display_path = file_to_format.relative_to(project_root)
        print(f"Formatting: {display_path}")
        subprocess.run(["clang-format", "-i", str(file_to_format)], check=True)
        # Re-stage if we are in the middle of a commit
        if args.staged:
            subprocess.run(["git", "add", str(file_to_format)], check=True)

    print(f"Done. Processed {len(to_format)} files.")

if __name__ == "__main__":
    main()
