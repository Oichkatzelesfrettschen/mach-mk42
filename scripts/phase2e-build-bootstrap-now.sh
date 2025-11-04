#!/bin/bash
# Phase 2E: Build Bootstrap Server (Immediate Execution)
# This script attempts to compile the bootstrap server with available resources

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${REPO_ROOT}/build/bootstrap"
SOURCE_DIR="${REPO_ROOT}/userland/bootstrap"
INCLUDE_DIR="${REPO_ROOT}/userland/include"

echo "=== Phase 2E: Building Bootstrap Server ==="

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Compiler flags
CFLAGS="-m32 -I${INCLUDE_DIR} -I${INCLUDE_DIR}/mach -I${SOURCE_DIR} -g -O2"

echo "[1/4] Compiling bootstrap.c..."
gcc-13 ${CFLAGS} -c "${SOURCE_DIR}/bootstrap.c" -o bootstrap.o 2>&1 || {
    echo "WARNING: bootstrap.c compilation failed, continuing..."
}

echo "[2/4] Compiling service.c..."
gcc-13 ${CFLAGS} -c "${SOURCE_DIR}/service.c" -o service.o 2>&1 || {
    echo "WARNING: service.c compilation failed, continuing..."
}

echo "[3/4] Compiling file_io.c..."
gcc-13 ${CFLAGS} -c "${SOURCE_DIR}/file_io.c" -o file_io.o 2>&1 || {
    echo "WARNING: file_io.c compilation failed, continuing..."
}

echo "[4/4] Compiling load.c..."
gcc-13 ${CFLAGS} -c "${SOURCE_DIR}/load.c" -o load.o 2>&1 || {
    echo "WARNING: load.c compilation failed, continuing..."
}

echo ""
echo "=== Build Summary ==="
ls -lh *.o 2>/dev/null || echo "No object files created"
echo ""
echo "Build attempted. Check output above for details."
