#!/bin/bash
# Phase 2B: Install GNU MIG from source
# This script builds and installs the Mach Interface Generator (MIG) from GNU sources
# since it's not available in Ubuntu 24.04 repositories.

set -e  # Exit on error
set -u  # Exit on undefined variable

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build/gnu-mig"
INSTALL_PREFIX="/usr/local"
LOG_FILE="${PROJECT_ROOT}/artifacts/logs/phase2b-gnu-mig-install.log"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log() {
    echo -e "${GREEN}[$(date +'%Y-%m-%d %H:%M:%S')]${NC} $1" | tee -a "${LOG_FILE}"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1" | tee -a "${LOG_FILE}"
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1" | tee -a "${LOG_FILE}"
}

# Create log directory
mkdir -p "$(dirname "${LOG_FILE}")"

log "=== Phase 2B: Installing GNU MIG from Source ==="
log "Build directory: ${BUILD_DIR}"
log "Install prefix: ${INSTALL_PREFIX}"

# Check if MIG is already installed
if command -v mig >/dev/null 2>&1; then
    MIG_VERSION=$(mig --version 2>&1 | head -1 || echo "unknown")
    log "MIG already installed: ${MIG_VERSION}"
    log "Location: $(which mig)"
    exit 0
fi

# Install build dependencies
log "Installing build dependencies..."
sudo apt-get update -qq >> "${LOG_FILE}" 2>&1
sudo apt-get install -y \
    build-essential \
    autoconf \
    automake \
    libtool \
    flex \
    bison \
    git \
    >> "${LOG_FILE}" 2>&1

log "Build dependencies installed successfully"

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Clone GNU MIG from Savannah
log "Cloning GNU MIG from git.savannah.gnu.org..."
if [ -d "mig/.git" ]; then
    log "MIG repository already exists, updating..."
    cd mig
    git pull >> "${LOG_FILE}" 2>&1
    cd ..
else
    git clone https://git.savannah.gnu.org/git/hurd/mig.git >> "${LOG_FILE}" 2>&1
fi

cd mig
log "MIG source cloned successfully"
log "Current commit: $(git log -1 --oneline)"

# Bootstrap the build system
log "Bootstrapping build system..."
if [ -f "bootstrap" ]; then
    ./bootstrap >> "${LOG_FILE}" 2>&1
elif [ -f "autogen.sh" ]; then
    ./autogen.sh >> "${LOG_FILE}" 2>&1
else
    autoreconf -fvi >> "${LOG_FILE}" 2>&1
fi

log "Build system bootstrapped"

# Configure
log "Configuring GNU MIG..."
./configure --prefix="${INSTALL_PREFIX}" \
    CFLAGS="-O2 -g" \
    >> "${LOG_FILE}" 2>&1

log "Configuration complete"

# Build
log "Building GNU MIG..."
make -j$(nproc) >> "${LOG_FILE}" 2>&1
log "Build complete"

# Test (if available)
if make -n check >/dev/null 2>&1; then
    log "Running tests..."
    make check >> "${LOG_FILE}" 2>&1 || warn "Some tests failed (this may be expected)"
fi

# Install
log "Installing GNU MIG to ${INSTALL_PREFIX}..."
sudo make install >> "${LOG_FILE}" 2>&1
log "Installation complete"

# Verify installation
if command -v mig >/dev/null 2>&1; then
    MIG_VERSION=$(mig --version 2>&1 | head -1)
    log "✓ MIG installed successfully: ${MIG_VERSION}"
    log "✓ Location: $(which mig)"
    
    # Test MIG with a simple command
    mig --help >/dev/null 2>&1 && log "✓ MIG help command works"
else
    error "MIG installation failed - binary not found in PATH"
    exit 1
fi

log "=== GNU MIG Installation Complete ==="
log "Full build log: ${LOG_FILE}"

# Print summary
echo ""
echo "════════════════════════════════════════"
echo " GNU MIG Installation Summary"
echo "════════════════════════════════════════"
echo " Version: ${MIG_VERSION}"
echo " Binary: $(which mig)"
echo " Install prefix: ${INSTALL_PREFIX}"
echo " Build directory: ${BUILD_DIR}"
echo " Log file: ${LOG_FILE}"
echo "════════════════════════════════════════"
echo ""
