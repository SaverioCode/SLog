#!/bin/bash

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
PROJECT_ROOT="${SCRIPT_DIR}/.."
README_FILE="${PROJECT_ROOT}/README.md"

BRANCH=$(git branch --show-current)

if [ -z "$BRANCH" ]; then
    echo "No branch detected."
    exit 0
fi

sed -i -E "s/(\?branch=)[^&]+/\1$BRANCH/g" "$README_FILE"

if ! git diff --quiet "$README_FILE"; then
    git add "$README_FILE"
    echo "UPDATED README badges to branch: $BRANCH"
fi
