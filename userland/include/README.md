# Kernel Headers for Userland

This directory contains exported kernel headers for building userland servers.

## Structure

- `mach/` - Core Mach IPC and VM interfaces
- `device/` - Device driver interfaces
- `sys/` - System headers
- `i386/` - i386-specific definitions
- `mach/machine` - Symlink to i386 for compatibility

## Usage

When compiling userland servers, use:
```bash
gcc -I/path/to/userland/include -c file.c
```

## MIG Definition Files

The `mach/*.defs` files are MIG (Mach Interface Generator) definitions.
These are processed by MIG to generate IPC stub code:

```bash
mig -user file_user.c -server file_server.c -header file.h mach/interface.defs
```

## Exported Interfaces

### Core Mach
- Task management
- Thread management  
- Virtual memory
- IPC (ports, messages)
- Exception handling

### Device
- Device protocol
- Network status

### System
- Standard types
- Time definitions
- I/O control

## Notes

These headers are exported from the MK42 kernel and match the kernel's
IPC interfaces. Any changes to kernel interfaces require re-export.
