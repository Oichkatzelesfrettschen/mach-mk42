# Mach Microkernel Userland Components

## Overview

The Mach microkernel requires userland servers to provide a complete operating system. This document explains the userland architecture and how to integrate components from @nmartin0/osfmk6.1 and @nmartin0/xMach repositories.

## What is Userland?

In a microkernel architecture like Mach, the kernel itself only provides:
- Virtual memory management
- Inter-process communication (IPC)
- Task and thread management
- Low-level scheduling

All other operating system services run as **user-space servers**:
- File systems
- Device drivers
- Network stack
- POSIX/Unix emulation

## Key Userland Components

### 1. Bootstrap Server
**Purpose:** First userland process, starts other servers

**Source:** `osfmk6.1/src/src/bootstrap/`

**Functionality:**
- Registers and manages other servers
- Provides name service for IPC ports
- Essential for system startup

**Files:**
- `bootstrap.c` - Main bootstrap server implementation
- `bootstrap.h` - Interface definitions

### 2. Default Pager
**Purpose:** Handles virtual memory paging when no other pager is available

**Source:** `osfmk6.1/src/src/default_pager/`

**Functionality:**
- Manages swap space
- Provides backing store for VM
- Critical for memory management

**Files:**
- `default_pager.c` - Pager implementation
- `mach/default_pager_object.defs` - MIG interface definitions
- `mach/default_pager_types.h` - Type definitions

### 3. Lites (Lite Server)
**Purpose:** BSD Unix single server providing POSIX compatibility

**Source:** `xMach/lites/`

**Functionality:**
- Complete BSD 4.4 Lite server
- Provides Unix system calls
- File system support (UFS, FFS)
- Network stack (BSD sockets)
- TTY and device management
- Process management (fork, exec, wait)

**Major Components:**
- `server/` - Core server implementation
- `emulator/` - User process emulation
- `liblites/` - Runtime library
- `bin/` - Utilities

## Architecture Diagram

```
┌─────────────────────────────────────────────────┐
│           User Applications                      │
│  (sh, ls, gcc, etc. - standard Unix tools)      │
└────────────────┬────────────────────────────────┘
                 │ System Calls (POSIX API)
┌────────────────┴────────────────────────────────┐
│      Lites Server (BSD Single Server)           │
│  ┌──────────────────────────────────────────┐  │
│  │ File System │ Network │ TTY │ Processes │  │
│  └──────────────────────────────────────────┘  │
└────────────────┬────────────────────────────────┘
                 │ Mach IPC
┌────────────────┴────────────────────────────────┐
│         Mach Services Layer                      │
│  ┌──────────────────────────────────────────┐  │
│  │Bootstrap│ Pager │Name Server│Auth Server│  │
│  └──────────────────────────────────────────┘  │
└────────────────┬────────────────────────────────┘
                 │ Mach IPC
┌────────────────┴────────────────────────────────┐
│          Mach Microkernel (MK42)                 │
│  ┌──────────────────────────────────────────┐  │
│  │ VM │ IPC │ Tasks │ Threads │ Scheduling │  │
│  └──────────────────────────────────────────┘  │
└──────────────────────────────────────────────────┘
```

## Required Components for Minimal Boot

To achieve a fully booting system, you need (in order):

1. **Mach Kernel** (mach_kernel) - Already present ✓
2. **Bootstrap Server** - Starts the system
3. **Default Pager** - Handles paging
4. **Lites Server** - Provides Unix environment
5. **Init Process** - First user process (/sbin/init)
6. **Shell** - Interactive command line
7. **Basic Utilities** - ls, cat, etc.

## References

- [@nmartin0/osfmk6.1](https://github.com/nmartin0/osfmk6.1) - OSF Mach kernel 6.1 with userland servers
- [@nmartin0/xMach](https://github.com/nmartin0/xMach) - Mach4 kernel + Lites server (BSD single server)
- **Lites:** Lite Server - BSD 4.4 Unix server for Mach
- **Mach IPC:** Inter-Process Communication for client-server architecture

See [INTEGRATION.md](INTEGRATION.md) for detailed integration instructions.
