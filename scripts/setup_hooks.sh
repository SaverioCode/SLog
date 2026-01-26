#!/bin/bash

BASE_DIR=$(cd "$(dirname "$0")" && pwd)
PARENT_DIR=${BASE_DIR}/..
HOOKS_DIR="${PARENT_DIR}/.git/hooks"

if [ ! -d "$HOOKS_DIR" ]; then
    echo "CREATING hooks directory at: $HOOKS_DIR"
    mkdir -p "$HOOKS_DIR"
fi

echo "INSTALLING Linux Git Hooks..."

PRE_COMMIT="${HOOKS_DIR}/pre-commit"
POST_MERGE="${HOOKS_DIR}/post-merge"

cat <<EOF > "$PRE_COMMIT"
#!/bin/sh
"${BASE_DIR}/update_badge.sh"
EOF
chmod +x "$PRE_COMMIT"

cat <<EOF > "$POST_MERGE"
#!/bin/sh
"${BASE_DIR}/update_badge.sh"
EOF
chmod +x "$POST_MERGE"

echo "INSTALLED hooks successfully."
