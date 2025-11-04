#!/bin/bash
# Phase 2D: Extract or build libmach_sa.a (Mach standalone library)
# Required for linking bootstrap server and default pager

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
MACH_REPOS="${MACH_REPOS:-$PROJECT_ROOT/../mach-repos}"
TOOLCHAIN_LIB="$PROJECT_ROOT/toolchain/i386-mach/lib"

echo "=================================================="
echo "Phase 2D: libmach_sa.a Extraction/Build"
echo "=================================================="
echo

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }
log_success() { echo -e "${BLUE}[SUCCESS]${NC} $1"; }

# Create library directory
mkdir -p "$TOOLCHAIN_LIB"

log_info "Step 1: Search for libmach_sa.a in cloned repositories"

# Search in all mach repositories
SEARCH_PATHS=(
    "$MACH_REPOS/osfmk6.1"
    "$MACH_REPOS/osfmk7.3"
    "$MACH_REPOS/mach_us"
    "$MACH_REPOS/xMach"
    "$MACH_REPOS/Mach-patched-gcc"
)

FOUND_LIB=""
for path in "${SEARCH_PATHS[@]}"; do
    if [ -d "$path" ]; then
        log_info "Searching in $(basename "$path")..."
        LIB_FOUND=$(find "$path" -name "libmach_sa.a" -o -name "libmach.a" 2>/dev/null | head -1)
        if [ -n "$LIB_FOUND" ]; then
            FOUND_LIB="$LIB_FOUND"
            log_success "Found library: $LIB_FOUND"
            break
        fi
    fi
done

if [ -n "$FOUND_LIB" ]; then
    log_info "Step 2: Copy library to toolchain"
    cp "$FOUND_LIB" "$TOOLCHAIN_LIB/libmach_sa.a"
    log_success "Installed: $TOOLCHAIN_LIB/libmach_sa.a"
    
    log_info "Step 3: Verify library contents"
    echo "Library symbols:"
    nm "$TOOLCHAIN_LIB/libmach_sa.a" | grep -E "mach_msg|mach_port" | head -20
    
    log_success "libmach_sa.a successfully installed!"
    exit 0
fi

log_warn "Library not found in pre-built form"
log_info "Step 2: Attempt to build from source"

# Check for libmach_sa source directory
LIBMACH_SRC=""
for path in "${SEARCH_PATHS[@]}"; do
    if [ -d "$path/lib/libmach_sa" ]; then
        LIBMACH_SRC="$path/lib/libmach_sa"
        log_info "Found source: $LIBMACH_SRC"
        break
    elif [ -d "$path/lib/libmach" ]; then
        LIBMACH_SRC="$path/lib/libmach"
        log_info "Found source: $LIBMACH_SRC"
        break
    fi
done

if [ -z "$LIBMACH_SRC" ]; then
    log_error "Could not find libmach_sa source code"
    log_error "Searched in:"
    for path in "${SEARCH_PATHS[@]}"; do
        echo "  - $path/lib/libmach_sa"
    done
    echo
    log_error "MANUAL INTERVENTION REQUIRED:"
    echo "  1. Locate libmach_sa.a or source in one of the mach repositories"
    echo "  2. Copy to $TOOLCHAIN_LIB/libmach_sa.a"
    echo "  OR"
    echo "  3. Build from source and copy result"
    exit 1
fi

log_info "Attempting to build from $LIBMACH_SRC"

# Try to build using existing Makefile
cd "$LIBMACH_SRC"

if [ -f "Makefile" ]; then
    log_info "Found Makefile, attempting build..."
    
    # Try make with common targets
    if make libmach_sa.a 2>/dev/null || make all 2>/dev/null || make 2>/dev/null; then
        # Find generated library
        BUILT_LIB=$(find . -name "libmach_sa.a" -o -name "libmach.a" | head -1)
        if [ -n "$BUILT_LIB" ]; then
            cp "$BUILT_LIB" "$TOOLCHAIN_LIB/libmach_sa.a"
            log_success "Built and installed: $TOOLCHAIN_LIB/libmach_sa.a"
            
            log_info "Verifying library..."
            nm "$TOOLCHAIN_LIB/libmach_sa.a" | grep -E "mach_msg|mach_port" | head -20
            
            log_success "libmach_sa.a successfully built and installed!"
            exit 0
        fi
    fi
fi

log_warn "Automatic build failed"
log_info "Step 3: Create minimal stub library"

# As last resort, create stub library with required symbols
# This is NOT ideal but allows continuing development
cat > /tmp/libmach_stub.c << 'EOF'
/* Minimal libmach_sa.a stub implementation */
/* WARNING: This is a temporary stub for development only */

#include <mach/message.h>
#include <mach/port.h>

/* Basic Mach IPC functions - stubs only */
mach_msg_return_t mach_msg(
    mach_msg_header_t *msg,
    mach_msg_option_t option,
    mach_msg_size_t send_size,
    mach_msg_size_t rcv_size,
    mach_port_t rcv_name,
    mach_msg_timeout_t timeout,
    mach_port_t notify)
{
    /* TODO: Implement actual system call */
    return MACH_MSG_SUCCESS;
}

kern_return_t mach_port_allocate(
    mach_port_t task,
    mach_port_right_t right,
    mach_port_t *name)
{
    /* TODO: Implement actual system call */
    return KERN_SUCCESS;
}

kern_return_t mach_port_deallocate(
    mach_port_t task,
    mach_port_t name)
{
    /* TODO: Implement actual system call */
    return KERN_SUCCESS;
}

/* Add more functions as needed during server compilation */
EOF

log_warn "Creating stub library (development only)"
gcc -c -m32 -I"$PROJECT_ROOT/userland/include" /tmp/libmach_stub.c -o /tmp/libmach_stub.o
ar rcs "$TOOLCHAIN_LIB/libmach_sa.a" /tmp/libmach_stub.o

log_warn "STUB LIBRARY CREATED"
echo
echo "⚠️  WARNING: This is a minimal stub library!"
echo "    It contains placeholder functions only."
echo "    Server binaries will compile but may not work correctly."
echo
echo "    REQUIRED: Replace with actual libmach_sa.a from:"
echo "      - osfmk6.1/lib/libmach_sa/"
echo "      - mach_us/lib/libmach/"
echo "      - Or build from proper source"
echo

log_info "Stub library installed at: $TOOLCHAIN_LIB/libmach_sa.a"
log_warn "Server compilation can proceed, but functionality limited"

exit 2  # Exit code 2 = stub created (warning)
