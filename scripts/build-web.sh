#!/usr/bin/env bash
set -euo pipefail

if ! command -v em++ >/dev/null 2>&1; then
    echo "Emscripten is required. Install it with: brew install emscripten" >&2
    exit 1
fi

raylib_src="${RAYLIB_SRC:-raylib/src}"
if [[ ! -f "$raylib_src/raylib.h" || ! -f "$raylib_src/libraylib.a" ]]; then
    echo "Web-built raylib was not found in '$raylib_src'." >&2
    echo "Build raylib 5.5 with: make PLATFORM=PLATFORM_WEB -C raylib/src" >&2
    echo "Or set RAYLIB_SRC to the raylib src directory." >&2
    exit 1
fi

mkdir -p dist

em++ src/main.cpp src/levelmanager.cpp src/entities/Player.cpp \
    -std=c++17 \
    -O2 \
    -I"$raylib_src" \
    "$raylib_src/libraylib.a" \
    -DPLATFORM_WEB \
    -s USE_GLFW=3 \
    -s ASYNCIFY \
    -s EXPORTED_RUNTIME_METHODS=HEAPF32 \
    -s ALLOW_MEMORY_GROWTH=1 \
    --preload-file assets@/assets \
    --shell-file web/shell.html \
    -o dist/index.html
