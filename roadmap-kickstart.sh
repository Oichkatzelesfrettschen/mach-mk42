#!/bin/bash
# Mach MK42 Roadmap Kickstarter
# This script sets up the infrastructure for the complete system build

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "╔════════════════════════════════════════════════════════════╗"
echo "║     Mach MK42 Complete System - Roadmap Kickstarter       ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "This script will prepare the repository for full system development"
echo "following the roadmap in docs/ROADMAP.md"
echo ""

# Step 1: Create directory structure
echo "=== Step 1: Creating Directory Structure ==="
mkdir -p userland/{bootstrap,default_pager,lites,netmsgserver,machid}
mkdir -p toolchain
mkdir -p rootfs/{bin,sbin,etc,dev,lib,usr/{bin,sbin,lib},var,tmp,proc}
mkdir -p scripts
mkdir -p tests
mkdir -p build/{kernel,userland,rootfs}
echo "✓ Directory structure created"
echo ""

# Step 2: Move existing scripts to scripts/
echo "=== Step 2: Organizing Scripts ==="
for script in build-kernel.sh build-userland.sh boot-qemu.sh boot-qemu-iso.sh create-bootable-iso.sh test-all.sh extract-userland.sh; do
    if [ -f "$script" ]; then
        mv "$script" scripts/ 2>/dev/null || cp "$script" scripts/
        echo "  Moved $script to scripts/"
    fi
done
echo "✓ Scripts organized"
echo ""

# Step 3: Clone required repositories
echo "=== Step 3: Cloning Required Repositories (to ../mach-repos/) ==="
mkdir -p ../mach-repos
cd ../mach-repos

REPOS=(
    "osfmk7.3:High priority - Target kernel"
    "mach_us:High priority - User-space servers"
    "lites-1.1.u3:High priority - BSD server"
    "Mach-patched-gcc:High priority - Toolchain"
    "mach-mk74:Medium priority - Alternative kernel"
    "osfmk6.1:Medium priority - Reference"
    "xMach:Medium priority - Lites reference"
    "mach_vm_descendents:Low priority - Documentation"
)

for repo_info in "${REPOS[@]}"; do
    repo="${repo_info%%:*}"
    desc="${repo_info#*:}"
    
    if [ ! -d "$repo" ]; then
        echo "Cloning $repo ($desc)..."
        git clone --depth 1 "https://github.com/nmartin0/$repo.git" || echo "  ⚠ Clone failed, skipping"
    else
        echo "  ✓ $repo already cloned"
    fi
done

cd "$SCRIPT_DIR"
echo "✓ Repositories cloned to ../mach-repos/"
echo ""

# Step 4: Create initial inventory
echo "=== Step 4: Creating Component Inventory ==="
cat > docs/INVENTORY.md << 'EOF'
# Component Inventory

## Status: Initial Setup

### Kernel
- [x] MK42 (current) - i386 a.out format
- [ ] OSFMK 7.3 (target) - evaluation pending
- [ ] MK74 (alternative) - evaluation pending

### Userland Servers
- [ ] Bootstrap server - not extracted
- [ ] Default pager - not extracted
- [ ] Network message server - not extracted
- [ ] Mach ID server - not extracted
- [ ] Lites server - not extracted

### Toolchain
- [x] GCC 7.5 (current) - for basic builds
- [ ] Mach-patched-gcc - not built
- [ ] MIG - need to extract and build

### Root Filesystem
- [ ] Init system - not implemented
- [ ] Shell - not cross-compiled
- [ ] Basic utilities - not available
- [ ] Libraries - not built

## Next Actions
1. Run scripts/extract-userland.sh
2. Evaluate kernel versions
3. Build Mach-patched-gcc toolchain
4. Extract and build servers

Updated: $(date)
EOF
echo "✓ Created docs/INVENTORY.md"
echo ""

# Step 5: Update .gitignore
echo "=== Step 5: Updating .gitignore ==="
cat >> .gitignore << 'EOF'

# Roadmap build artifacts
../mach-repos/
build/
toolchain/bin/
toolchain/lib/
rootfs.img
*.tar.gz

# Downloaded sources
downloads/
EOF
echo "✓ Updated .gitignore"
echo ""

# Step 6: Create quick reference guide
echo "=== Step 6: Creating Quick Reference ==="
cat > QUICKSTART.md << 'EOF'
# Quick Start Guide

## What We Have
- ✅ Mach MK42 kernel (boots in QEMU)
- ✅ Docker build environment
- ✅ CI/CD with GitHub Actions
- ✅ GRUB bootable ISO creation

## What's Next (Phase 1)
1. **Evaluate Kernels:** Compare MK42 vs OSFMK7.3 vs MK74
2. **Build Toolchain:** Extract and build Mach-patched-gcc
3. **Extract Servers:** Get bootstrap, pager, and other servers

## Quick Commands

```bash
# Run complete test (current state)
docker build -t mach-mk42-builder .
docker run --rm mach-mk42-builder

# Extract userland components
./scripts/extract-userland.sh

# Create bootable ISO
./scripts/create-bootable-iso.sh

# Boot in QEMU
./scripts/boot-qemu-iso.sh
```

## Directory Structure
```
mach-mk42/
├── kernel/          # Mach kernel source
├── userland/        # User-space servers (to be populated)
├── toolchain/       # Mach-patched-gcc (to be built)
├── rootfs/          # Root filesystem (to be created)
├── scripts/         # Build and test scripts
├── docs/            # Documentation
│   ├── ROADMAP.md        # 26-week complete plan
│   ├── USERLAND.md       # Userland architecture
│   ├── INTEGRATION.md    # Integration guide
│   └── INVENTORY.md      # Component status
└── .github/workflows/    # CI/CD
```

## See Full Roadmap
Read **[docs/ROADMAP.md](docs/ROADMAP.md)** for the complete 26-week plan.

## Current Phase
**Phase 1: Foundation Enhancement** (Weeks 1-2)
- Evaluate kernel options
- Build Mach-specific toolchain
- Plan migration strategy
EOF
echo "✓ Created QUICKSTART.md"
echo ""

# Step 7: Summary
echo "╔════════════════════════════════════════════════════════════╗"
echo "║                  Setup Complete!                           ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "Directory structure created:"
echo "  📁 userland/     - For user-space servers"
echo "  📁 toolchain/    - For Mach-patched-gcc"
echo "  📁 rootfs/       - For root filesystem"
echo "  📁 scripts/      - Build and test scripts"
echo "  📁 build/        - Build artifacts"
echo ""
echo "Repositories cloned to:"
echo "  📁 ../mach-repos/ - All Mach-related repositories"
echo ""
echo "Documentation created:"
echo "  📄 docs/ROADMAP.md    - Complete 26-week plan"
echo "  📄 docs/INVENTORY.md  - Component status"
echo "  📄 QUICKSTART.md      - Quick reference"
echo ""
echo "Next steps:"
echo "  1. Review docs/ROADMAP.md for the complete plan"
echo "  2. Read QUICKSTART.md for immediate actions"
echo "  3. Run: ./scripts/extract-userland.sh"
echo "  4. Start Phase 1: Kernel evaluation"
echo ""
echo "To commit these changes:"
echo "  git add ."
echo "  git commit -m 'Initialize roadmap infrastructure'"
echo ""
