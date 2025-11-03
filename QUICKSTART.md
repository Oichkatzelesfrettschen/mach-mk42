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
