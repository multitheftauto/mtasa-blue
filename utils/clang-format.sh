#!/bin/bash

# cd to parent location
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

# download clang-format if needed
CLANG_FORMAT_URL="https://github.com/multitheftauto/llvm-project/releases/download/llvmorg-21.1.8/clang-format-armv7a-linux-gnueabihf"
BIN_DIR="$REPO_ROOT/Build/tmp"
CLANG_FORMAT_PATH="$BIN_DIR/clang-format"
EXPECTED_HASH="b77927335fd4caece863ea0c09ec8c4dfbdab545510c9091205ccfb528d5abf2"

SHOULD_DOWNLOAD=true
if [ -f "$CLANG_FORMAT_PATH" ]; then
    CURRENT_HASH=$(sha256sum "$CLANG_FORMAT_PATH" | awk '{print $1}')
    if [ "$CURRENT_HASH" = "$EXPECTED_HASH" ]; then
        SHOULD_DOWNLOAD=false
    else
        echo "clang-format hash mismatch, re-downloading"
    fi
fi

if [ "$SHOULD_DOWNLOAD" = true ]; then
    echo "Downloading clang-format..."
    mkdir -p "$BIN_DIR"

    if ! curl -L -o "$CLANG_FORMAT_PATH" "$CLANG_FORMAT_URL"; then
        echo "Error: Failed to download clang-format" >&2
        exit 1
    fi

    DOWNLOADED_HASH=$(sha256sum "$CLANG_FORMAT_PATH" | awk '{print $1}')
    if [ "$DOWNLOADED_HASH" != "$EXPECTED_HASH" ]; then
        echo "Error: SHA256 hash mismatch! Expected: $EXPECTED_HASH, Got: $DOWNLOADED_HASH" >&2
        exit 1
    fi

    chmod +x "$CLANG_FORMAT_PATH"
    echo "clang-format downloaded successfully"
fi

# compute list of files to format
SEARCH_FOLDERS=("Client" "Server" "Shared")
TMP_FILE=$(mktemp)

for folder in "${SEARCH_FOLDERS[@]}"; do
    find "$folder" -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.h" -o -name "*.hh" -o -name "*.hpp" \) >> "$TMP_FILE"
done

# clang-format in place, clean up temp file
"$CLANG_FORMAT_PATH" -i --files="$TMP_FILE"
rm "$TMP_FILE"
