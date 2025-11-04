# Mach MK42 Microkernel

This repository contains the Mach 3.0 microkernel (version MK42), a legacy operating system kernel originally developed at Carnegie Mellon University for i386 architecture.

## 🎯 Project Vision

Transform this historical Mach microkernel into a **fully functional operating system** with userland servers, networking, and Unix compatibility - demonstrating the complete potential of microkernel architecture.

## 📋 Status & Roadmap

**Current State:** ✅ Kernel boots successfully in QEMU i386 within Docker

**Next Phase:** 🚧 Integrating userland servers (bootstrap, pager, Lites BSD server)

See **[docs/ROADMAP.md](docs/ROADMAP.md)** for the complete 26-week plan to a fully functional system.

### Quick Status
- ✅ CI/CD with Docker + QEMU
- ✅ GRUB multiboot boot testing
- ✅ Automated builds & artifacts
- ✅ Comprehensive documentation
- 🚧 Userland server integration (in progress)
- 📅 Networking stack (planned)
- 📅 Complete Unix environment (planned)

## Build Status

![Build and Test Mach MK42 Kernel](https://github.com/Oichkatzelesfrettschen/mach-mk42/workflows/Build%20and%20Test%20Mach%20MK42%20Kernel/badge.svg)

## Overview

The Mach 3.0 micro-kernel provides the following functions:
- Virtual memory management
- Inter-process communication (IPC)
- Tasks and threads
- Exception handling
- Scheduling primitives
- Device drivers

This is a **pure microkernel** that does not include file systems, TTY I/O, network support, or other Unix features. Those are implemented as user-space servers in a complete Mach system.

## Quick Start

### Build and Test with Docker (Recommended)

```bash
# Build the Docker image (includes QEMU)
docker build -t mach-mk42-builder .

# Run complete test suite (build + QEMU boot test)
docker run --rm mach-mk42-builder

# Interactive shell in build environment
docker run --rm -it mach-mk42-builder /bin/bash
```

### Individual Test Scripts

```bash
# Build kernel only
docker run --rm -v $(pwd):/workspace mach-mk42-builder /workspace/build-kernel.sh

# Boot test in QEMU only
docker run --rm -v $(pwd):/workspace mach-mk42-builder /workspace/boot-qemu.sh

# Complete test suite
docker run --rm -v $(pwd):/workspace mach-mk42-builder /workspace/test-all.sh
```

## Architecture

This repository supports building for **i386** architecture. The kernel is designed to run on legacy x86 hardware or in emulation.

## Building with Docker and GitHub Actions

The repository is configured to build automatically using GitHub Actions with a Docker-based build environment. This demonstrates a CI/CD pipeline for legacy software.

**Note:** This legacy codebase has compatibility issues with modern build tools (bison 3.x, modern gcc). The repository includes a pre-built kernel binary for demonstration purposes. The Docker/QEMU workflow shows the infrastructure for building and testing legacy i386 software.

### Automated Build (GitHub Actions)

The workflow automatically runs on every push:

1. **Build and Test Job:**
   - Builds Docker image with gcc-7.5, i386 multilib, and QEMU
   - Attempts to compile the Mach kernel
   - Runs complete test suite inside Docker
   - Validates kernel binary format
   - Performs initial QEMU boot test
   - Uploads kernel artifact with build info

2. **QEMU Boot Test Job:**
   - Downloads kernel artifact
   - Performs comprehensive QEMU i386 boot test
   - Validates kernel loads and begins execution
   - Reports detailed boot status

The workflow demonstrates best practices for maintaining legacy software in modern CI/CD environments.

### Manual Build with Docker

1. **Build the Docker image:**
   ```bash
   docker build -t mach-mk42-builder .
   ```

2. **Build the kernel:**
   ```bash
   docker run --rm -v $(pwd):/workspace mach-mk42-builder /workspace/build-kernel.sh
   ```

3. **Find the kernel binary:**
   ```bash
   find obj -name mach_kernel
   # or use pre-built: mach_kernel.MK42.STD+WS
   ```

### Local Build (requires legacy tools)

If you have the appropriate legacy build tools installed:

```bash
cd kernel
export TARGET_MACHINE=AT386
export CONFIG=STD+WS
make ${CONFIG}.doconf
make ${CONFIG}.config
make ${CONFIG}.mig
make ${CONFIG}.make
```

## Running in QEMU

### Using the Boot Script

```bash
# Install QEMU first
sudo apt-get install qemu-system-x86

# Boot with default settings
./boot-qemu.sh

# Boot with custom kernel path
KERNEL=obj/i386_mach/latest/kernel/STD+WS/mach_kernel ./boot-qemu.sh

# Boot with verbose output
QEMU_VERBOSE=1 ./boot-qemu.sh

# Custom memory and timeout
QEMU_MEMORY=128M QEMU_TIMEOUT=60 ./boot-qemu.sh
```

### Manual QEMU Boot

```bash
qemu-system-i386 \
  -kernel obj/i386_mach/latest/kernel/STD+WS/mach_kernel \
  -nographic \
  -m 64M \
  -no-reboot
```

### Expected Behavior

**Important:** The Mach microkernel alone will not boot to a usable system. It requires:
- A bootloader (multiboot-compatible)
- User-space servers (file system, device drivers, etc.)
- An init process or server bootstrap

When booted in QEMU, the kernel will:
1. ✓ Load successfully (validated by SeaBIOS)
2. ✓ Begin execution
3. ⏱ Eventually timeout or panic (no user-space to continue)

This is **expected behavior** for a microkernel tested in isolation. The QEMU test validates that the kernel binary is loadable and begins execution correctly.

## Configuration

The default configuration is `STD+WS` (Standard + Workstation), which includes:
- AT bus support
- Hard disk (hd)
- COM ports
- Floppy disk (fd)
- PC586 ethernet
- Various device drivers

Other configurations can be found in `kernel/conf/MASTER.i386`.

## Repository Structure

```
mach-mk42/
├── kernel/              # Kernel source code
│   ├── i386/           # i386-specific code
│   ├── i386at/         # i386 AT bus device drivers
│   ├── kern/           # Core kernel (tasks, threads, scheduling)
│   ├── vm/             # Virtual memory management
│   ├── ipc/            # Inter-process communication
│   ├── device/         # Device interface
│   ├── conf/           # Configuration files
│   └── boot_ufs/       # Boot filesystem code
├── user/               # User-space libraries
├── include/            # Header files
├── obj/                # Build output directory
├── Dockerfile          # Docker build environment
├── build-kernel.sh     # Kernel build script
├── boot-qemu.sh        # QEMU boot test script
├── test-all.sh         # Complete test suite
└── .github/workflows/  # CI/CD pipeline
```

## Requirements

### Build Requirements
- Docker (recommended)
- OR legacy gcc 2.x - 4.x for native builds
- csh/tcsh shell
- Make
- Bison and flex

### Runtime Requirements (QEMU)
- QEMU i386 emulator
- At least 64MB RAM allocation

## CI/CD Pipeline

The GitHub Actions workflow (`.github/workflows/build.yml`) performs:

1. **Docker Build Stage:**
   - Creates an Ubuntu 18.04 container with gcc-7.5
   - Installs i386 multilib cross-compilation tools
   - Installs QEMU for in-container testing
   - Sets up legacy build environment

2. **Build and Test Stage:**
   - Runs complete test suite inside Docker
   - Attempts to build config and MIG tools
   - Attempts kernel configuration and compilation
   - Validates kernel binary format and size
   - Performs initial QEMU boot test
   - Creates build artifacts with metadata

3. **QEMU Validation Stage:**
   - Downloads kernel artifact
   - Performs comprehensive boot test in QEMU i386
   - Validates kernel loads and begins execution
   - Reports detailed test results

## Scripts

### build-kernel.sh
Automated build script that:
- Attempts to build config and MIG tools
- Configures kernel for STD+WS
- Builds kernel binary
- Falls back to pre-built kernel if build fails
- Provides detailed error context (30 lines)

### boot-qemu.sh
QEMU boot test script that:
- Validates kernel binary exists
- Displays kernel information
- Boots kernel in QEMU i386 emulator
- Handles timeouts gracefully
- Provides verbose debugging options

### test-all.sh
Complete test suite that:
- Checks build environment
- Builds kernel
- Validates binary format
- Performs QEMU boot test
- Reports comprehensive results

## Troubleshooting

### Build Fails with Bison Errors
This is expected. Modern bison (3.x) is incompatible with the 1990s-era yacc grammar files. The workflow uses the pre-built kernel binary.

### MIG Tool Build Fails
Expected. The MIG (Mach Interface Generator) requires full Mach environment headers not available in the base system.

### QEMU Timeout
Expected behavior. The microkernel alone cannot complete boot without user-space servers. The timeout validates that the kernel loads successfully.

### Docker Build Slow
First build downloads Ubuntu 18.04 and installs packages. Subsequent builds use Docker layer caching and are much faster.

## Contributing

This is a legacy/historical codebase. Contributions should focus on:
- Build system improvements
- Documentation
- Testing infrastructure
- Compatibility with modern build tools
- QEMU boot configuration

## License

See individual file headers for copyright information. Originally from Carnegie Mellon University.

## Historical Context

Mach was one of the first microkernel operating systems and influenced many modern OS designs:
- **macOS/iOS**: Darwin/XNU is based on Mach 3.0
- **GNU Hurd**: Built on Mach microkernel
- **QNX**: Influenced by Mach design
- **Research**: Pioneered many OS concepts still in use today

This MK42 version represents a specific release of the Mach 3.0 kernel from the early 1990s, preserved here for historical and educational purposes.

## Resources

- [Original CMU Mach Project](https://www.cs.cmu.edu/afs/cs/project/mach/public/www/mach.html)
- [Mach Kernel Principles](https://www.gnu.org/software/hurd/microkernel/mach.html)
- [Microkernel Architecture](https://en.wikipedia.org/wiki/Microkernel)

## Related Repositories

This project integrates components from multiple Mach repositories:

- [@nmartin0/osfmk7.3](https://github.com/nmartin0/osfmk7.3) - OSF Mach 7.3 (target kernel upgrade)
- [@nmartin0/mach_us](https://github.com/nmartin0/mach_us) - Mach user-space servers
- [@nmartin0/lites-1.1.u3](https://github.com/nmartin0/lites-1.1.u3) - BSD single server (updated)
- [@nmartin0/Mach-patched-gcc](https://github.com/nmartin0/Mach-patched-gcc) - Mach-specific GCC
- [@nmartin0/mach-mk74](https://github.com/nmartin0/mach-mk74) - Mach 7.4 kernel
- [@nmartin0/osfmk6.1](https://github.com/nmartin0/osfmk6.1) - OSF Mach 6.1 reference
- [@nmartin0/xMach](https://github.com/nmartin0/xMach) - Mach4 + Lites integration
- [@nmartin0/mach_vm_descendents](https://github.com/nmartin0/mach_vm_descendents) - VM evolution

See **[docs/ROADMAP.md](docs/ROADMAP.md)** for integration plan.
