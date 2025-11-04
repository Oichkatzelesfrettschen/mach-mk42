#!/bin/bash
# Initialize complete build directory structure
# Creates all directories referenced by build scripts

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "=================================================="
echo "Build Structure Initialization"
echo "=================================================="
echo

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_create() { echo -e "${BLUE}[CREATE]${NC} $1"; }

cd "$PROJECT_ROOT"

log_info "Creating build directories..."

# Build directories
mkdir -p build/{mig,ipc-stubs,bootstrap,default_pager,servers,kernel}
log_create "build/mig/ - MIG build artifacts"
log_create "build/ipc-stubs/ - Generated IPC stubs"
log_create "build/bootstrap/ - Bootstrap server build"
log_create "build/default_pager/ - Default pager build"
log_create "build/servers/ - Other server builds"
log_create "build/kernel/ - Kernel build artifacts"

# Root filesystem structure
mkdir -p rootfs/{bin,sbin,lib,etc,dev,tmp,var,usr,home,mnt,proc,sys}
mkdir -p rootfs/etc/{init.d,rc.d}
mkdir -p rootfs/var/{log,run,tmp}
mkdir -p rootfs/usr/{bin,sbin,lib,include,share}
mkdir -p rootfs/home/root
mkdir -p rootfs/dev
log_create "rootfs/ - Complete root filesystem structure"

# Artifacts directories
mkdir -p artifacts/{kernels,isos,logs,binaries}
log_create "artifacts/kernels/ - Kernel binaries"
log_create "artifacts/isos/ - Bootable ISO images"
log_create "artifacts/logs/ - Build and test logs"
log_create "artifacts/binaries/ - Server binaries"

# Test directories
mkdir -p tests/{unit,integration,system,performance}
mkdir -p tests/fixtures
log_create "tests/ - Test infrastructure"

# Documentation build
mkdir -p docs/generated
log_create "docs/generated/ - Generated documentation"

# Temporary workspace
mkdir -p tmp/downloads
mkdir -p tmp/workspace
log_create "tmp/ - Temporary build workspace"

echo
log_info "Directory structure created successfully!"
echo
echo "Build Structure:"
echo "├── build/              # All build artifacts"
echo "│   ├── mig/            # MIG tool build"
echo "│   ├── ipc-stubs/      # Generated IPC stubs"
echo "│   ├── bootstrap/      # Bootstrap server build"
echo "│   ├── default_pager/  # Default pager build"
echo "│   └── servers/        # Other servers"
echo "├── rootfs/             # Root filesystem"
echo "│   ├── bin/            # System binaries"
echo "│   ├── lib/            # Libraries"
echo "│   ├── etc/            # Configuration"
echo "│   └── ...             # Standard FHS layout"
echo "├── artifacts/          # Final outputs"
echo "│   ├── kernels/        # Kernel binaries"
echo "│   ├── isos/           # Bootable images"
echo "│   └── logs/           # Build logs"
echo "├── tests/              # Test suite"
echo "│   ├── unit/           # Unit tests"
echo "│   ├── integration/    # Integration tests"
echo "│   └── system/         # System tests"
echo "└── tmp/                # Temporary files"
echo
log_info "Ready for Phase 2 execution!"
