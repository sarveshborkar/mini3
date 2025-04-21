#!/usr/bin/env bash

# Grab the first CLI argument
TARGET_ARCH="$1"

# Default to 'arm64' if user did not specify an argument
if [ -z "$TARGET_ARCH" ]; then
  TARGET_ARCH="arm64"
fi

BUILD_DIR="build"
mkdir -p "$BUILD_DIR"

# Use if-else to choose the correct toolchain file
if [ "$TARGET_ARCH" = "arm64" ]; then
  cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_TOOLCHAIN_FILE=arch/toolchain_arm64.cmake \
    -G "Unix Makefiles"
else
  cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_TOOLCHAIN_FILE=arch/toolchain_x86_64.cmake \
    -G "Unix Makefiles"
fi

# Build
cmake --build "$BUILD_DIR"