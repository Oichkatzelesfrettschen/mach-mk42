# Mach MK42 Microkernel

This repository contains the Mach 3.0 microkernel (version MK42), a legacy operating system kernel originally developed at Carnegie Mellon University.

## Build Status

![Build Mach MK42 Kernel](https://github.com/Oichkatzelesfrettschen/mach-mk42/workflows/Build%20Mach%20MK42%20Kernel/badge.svg)

## Overview

The Mach 3.0 micro-kernel provides the following functions:
- Virtual memory management
- Inter-process communication (IPC)
- Tasks and threads
- Exception handling
- Scheduling primitives
- Device drivers

This is a pure kernel that does not include file systems, TTY I/O, network support, or other Unix features.

## Architecture

This repository supports building for **i386** architecture. The kernel is designed to run on legacy x86 hardware or in emulation.

## Building with Docker and GitHub Actions

The repository is configured to build automatically using GitHub Actions with a Docker-based build environment. This demonstrates a CI/CD pipeline for legacy software.

**Note:** This legacy codebase has compatibility issues with modern build tools (bison 3.x, modern gcc). The repository includes a pre-built kernel binary for demonstration purposes. The Docker/QEMU workflow shows the infrastructure for building and testing legacy i386 software.

### Automated Build (GitHub Actions)

Simply push to the `main` or `master` branch, and the workflow will:
1. Build a Docker image with gcc-7.5 and i386 multilib support
2. Attempt to compile the Mach kernel inside the container
3. Validate existing kernel binary in QEMU i386 emulator
4. Upload the kernel artifact for download

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

To test the kernel in QEMU i386 emulator:

```bash
# Install QEMU
sudo apt-get install qemu-system-x86

# Boot the kernel
qemu-system-i386 \
  -kernel obj/i386_mach/latest/kernel/STD+WS/mach_kernel \
  -nographic \
  -m 64M \
  -serial stdio
```

**Note:** The kernel will attempt to boot but may panic without proper root filesystem and init process. This is expected behavior for a microkernel.

## Configuration

The default configuration is `STD+WS` (Standard + Workstation), which includes:
- AT bus support
- Hard disk (hd)
- COM ports
- Floppy disk (fd)
- PC586 ethernet
- Various device drivers

Other configurations can be found in `kernel/conf/MASTER.i386`.

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

## Directory Structure

- `kernel/` - Kernel source code
  - `i386/` - i386-specific code
  - `i386at/` - i386 AT bus device drivers
  - `kern/` - Core kernel (tasks, threads, scheduling)
  - `vm/` - Virtual memory management
  - `ipc/` - Inter-process communication
  - `device/` - Device interface
  - `conf/` - Configuration files
- `user/` - User-space libraries
- `include/` - Header files
- `obj/` - Build output directory

## CI/CD Pipeline

The GitHub Actions workflow (`.github/workflows/build.yml`) performs:

1. **Docker Build Stage:**
   - Creates an Ubuntu 18.04 container with gcc-7.5
   - Installs i386 multilib cross-compilation tools
   - Sets up legacy build environment

2. **Kernel Build Stage:**
   - Compiles config and mig tools
   - Configures kernel for STD+WS
   - Builds the kernel binary
   - Archives the build artifacts

3. **QEMU Test Stage:**
   - Downloads the kernel artifact
   - Boots kernel in QEMU i386
   - Validates kernel loads correctly

## Contributing

This is a legacy/historical codebase. Contributions should focus on:
- Build system improvements
- Documentation
- Compatibility with modern build tools
- Bug fixes for compilation issues

## License

See individual file headers for copyright information. Originally from Carnegie Mellon University.

## Historical Context

Mach was one of the first microkernel operating systems and influenced many modern OS designs including macOS (Darwin/XNU is based on Mach 3.0).

This MK42 version represents a specific release of the Mach 3.0 kernel from the early 1990s.
