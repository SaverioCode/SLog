#!/usr/bin/env python3

import argparse
import subprocess
import sys

from pathlib import Path

DOCKER_SLOG_DIR : str = "/home/dev/slog"

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run the slog docker container.")
    parser.add_argument(
        "tag", 
        type=str,
        help="The docker image tag to run (e.g., latest)"
    )
    return parser.parse_args()

def main():
    args = parse_args()

    script_dir = Path(__file__).resolve().parent
    project_root = script_dir.parent
    
    mounts = [
        ("benchmark", "benchmark"),
        ("cmake", "cmake"),
        ("examples", "examples"),
        ("include", "include"),
        ("scripts", "scripts"),
        ("src", "src"),
        ("tests", "tests"),
        (".clang-format", ".clang-format"),
        (".gitignore", ".gitignore"),
        ("CMakeLists.txt", "CMakeLists.txt"),
        ("CMakePresets.json", "CMakePresets.json"),
        ("conanfile.py", "conanfile.py"),
        ("LICENSE", "LICENSE"),
        ("README.md", "README.md"),
    ]

    cmd = ["docker", "run", "-it", "--rm"]
    
    for src, dst in mounts:
        cmd += ["-v", f"{project_root / src}:{DOCKER_SLOG_DIR}/{dst}"]

    cmd.append(f"slog:{args.tag}")
    cmd.append("bash")

    print(f"RUNNING: {' '.join(str(x) for x in cmd)}")
    
    try:
        subprocess.run(cmd, check=True)
    except subprocess.CalledProcessError as e:
        sys.exit(e.returncode)
    except KeyboardInterrupt:
        sys.exit(130)

if __name__ == "__main__":
    main()
