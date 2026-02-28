#!/usr/bin/env python3

import subprocess
import sys
from pathlib import Path

def get_python_command():
    """
    Find which command (py, python3, python) works on this machine.
    """
    for cmd in ["py", "python3", "python"]:
        try:
            result = subprocess.run(
                [cmd, "-c", "import sys; print(sys.version_info.major)"],
                capture_output=True,
                text=True,
                timeout=2
            )
            if result.returncode == 0 and result.stdout.strip() == "3":
                return cmd
        except (FileNotFoundError, subprocess.TimeoutExpired, subprocess.SubprocessError):
            continue

    # Fallback to the literal path of the interpreter running this script
    # Convert \ to / for shell compatibility in git hooks
    return f'"{sys.executable.replace("\\", "/")}"'

def main():
    """
    Install git hooks for the project.
    """
    script_dir = Path(__file__).resolve().parent
    project_root = script_dir.parent
    hooks_dir = project_root / ".git" / "hooks"

    if not hooks_dir.exists():
        print(f"CREATING hooks directory at: {hooks_dir}")
        hooks_dir.mkdir(parents=True, exist_ok=True)

    py_cmd = get_python_command()
    print(f"INSTALLING Git Hooks (System: {platform.system()}, Python: {py_cmd})...")

    hook_content = (
        "#!/bin/sh\n\n"
        f"{py_cmd} scripts/clang_format.py --staged\n"
        f"{py_cmd} scripts/update_badge.py\n"
    )

    for hook in ["pre-commit", "post-merge"]:
        hook_path = hooks_dir / hook
        print(f"Writing {hook} hook...")
        # Use LF for hooks as they are shell scripts
        hook_path.write_text(hook_content, encoding="utf-8", newline="\n")
        
        if platform.system() != "Windows":
            # Make executable on Unix-like systems
            os.chmod(hook_path, 0o755)

    print("INSTALLED hooks successfully.")

if __name__ == "__main__":
    main()
