#!/usr/bin/env python3

import subprocess
import sys

from pathlib import Path

EXTENSIONS = {".cpp", ".hpp", ".ipp"}
TARGET_DIRS = ["include", "src"]

def main():
    script_dir = Path(__file__).resolve().parent
    project_root = script_dir.parent
    
    try:
        subprocess.run(["clang-format", "--version"], check=True, stdout=subprocess.DEVNULL)
    except FileNotFoundError:
        print("ERROR: clang-format is not installed or not in PATH.")
        sys.exit(1)

    print(f"RUNNING: clang-format on: {', '.join(TARGET_DIRS)}...")
    
    count = 0
    
    for folder_name in TARGET_DIRS:
        folder_path = project_root / folder_name
        
        if not folder_path.exists():
            print(f"SKIPPING missing directory: {folder_name}")
            continue

        for file_path in folder_path.rglob("*"):
            if file_path.is_file() and file_path.suffix in EXTENSIONS:
                print(f"FORMATTING: {file_path.relative_to(project_root)}")
                try:
                    subprocess.run(["clang-format", "-i", str(file_path)], check=True)
                    count += 1
                except subprocess.CalledProcessError as e:
                    print(f"FAILED to format {file_path}: {e}")
                    break

    print(f"DONE. Processed {count} files.")

if __name__ == "__main__":
    main()
