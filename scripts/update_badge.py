#!/usr/bin/env python3

import subprocess
import sys
import re
from pathlib import Path

def get_current_branch():
    try:
        result = subprocess.run(
            ["git", "branch", "--show-current"],
            capture_output=True, text=True, check=True
        )
        return result.stdout.strip()
    except subprocess.CalledProcessError:
        return None

def main():
    script_dir = Path(__file__).resolve().parent
    project_root = script_dir.parent
    readme_path = project_root / "README.md"

    if not readme_path.exists():
        print(f"ERROR: {readme_path} not found.")
        sys.exit(1)

    branch = get_current_branch()
    if not branch:
        print("WARNING: No branch detected. Skipping badge update.")
        sys.exit(0)

    content = readme_path.read_text(encoding="utf-8")
    
    # Regex matches ?branch= followed by anything up to &, space, or )
    # This covers various badge URL formats
    pattern = r"(\?branch=)[^& \)]+"
    new_content = re.sub(pattern, rf"\1{branch}", content)

    if content != new_content:
        # write_text defaults to current OS line endings, which is usually what's wanted
        readme_path.write_text(new_content, encoding="utf-8")
        try:
            subprocess.run(["git", "add", str(readme_path)], check=True)
            print(f"UPDATED README badges to branch: {branch}")
        except subprocess.CalledProcessError as e:
            print(f"FAILED to stage README.md: {e}")
            sys.exit(1)
    else:
        print(f"Badges already up to date for branch: {branch}")

if __name__ == "__main__":
    main()
