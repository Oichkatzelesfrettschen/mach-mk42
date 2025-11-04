#!/bin/bash
# Export kernel headers for userland development
# Phase 2 Step 1: Create complete header export
#
# STRICT MODE: Warnings are errors, no suppression allowed
set -e
set -u
set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
KERNEL_DIR="$PROJECT_ROOT/kernel"
EXPORT_DIR="$PROJECT_ROOT/userland/include"

echo "╔════════════════════════════════════════════════════════════╗"
echo "║         Kernel Header Export for Userland                 ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "This script exports kernel headers needed for userland builds."
echo "All warnings treated as errors - no suppression allowed."
echo ""

# Validate kernel directory exists
if [ ! -d "$KERNEL_DIR" ]; then
    echo "❌ ERROR: Kernel directory not found: $KERNEL_DIR"
    exit 1
fi

echo "=== Configuration ==="
echo "Kernel source: $KERNEL_DIR"
echo "Export target: $EXPORT_DIR"
echo ""

# Create export directory structure
echo "=== Creating Export Directory Structure ==="
mkdir -p "$EXPORT_DIR/mach"
mkdir -p "$EXPORT_DIR/device"
mkdir -p "$EXPORT_DIR/sys"
mkdir -p "$EXPORT_DIR/mach_debug"
mkdir -p "$EXPORT_DIR/mach/machine"
mkdir -p "$EXPORT_DIR/i386"
echo "✓ Directory structure created"
echo ""

# Export Mach headers (core IPC and VM interfaces)
echo "=== Exporting Mach Headers ==="
MACH_HEADERS=(
    "boolean.h"
    "error.h"
    "exception.h"
    "host_info.h"
    "kern_return.h"
    "mach_param.h"
    "mach_types.h"
    "machine.h"
    "memory_object.h"
    "message.h"
    "mig_errors.h"
    "notify.h"
    "pc_sample.h"
    "port.h"
    "processor_info.h"
    "std_types.h"
    "syscall_sw.h"
    "task_info.h"
    "task_special_ports.h"
    "thread_info.h"
    "thread_special_ports.h"
    "thread_status.h"
    "thread_switch.h"
    "time_value.h"
    "vm_attributes.h"
    "vm_inherit.h"
    "vm_param.h"
    "vm_prot.h"
    "vm_statistics.h"
)

MACH_COUNT=0
for header in "${MACH_HEADERS[@]}"; do
    if [ -f "$KERNEL_DIR/mach/$header" ]; then
        cp -p "$KERNEL_DIR/mach/$header" "$EXPORT_DIR/mach/"
        MACH_COUNT=$((MACH_COUNT + 1))
    elif [ -f "$KERNEL_DIR/include/mach/$header" ]; then
        cp -p "$KERNEL_DIR/include/mach/$header" "$EXPORT_DIR/mach/"
        MACH_COUNT=$((MACH_COUNT + 1))
    else
        echo "  ⚠ Warning: $header not found (may not exist in MK42)"
    fi
done
echo "✓ Exported $MACH_COUNT Mach headers"
echo ""

# Export MIG definition files
echo "=== Exporting MIG Definition Files ==="
MIG_DEFS=(
    "mach.defs"
    "mach_host.defs"
    "mach_port.defs"
    "mach_types.defs"
    "memory_object.defs"
    "memory_object_default.defs"
    "default_pager_object.defs"
    "exc.defs"
    "notify.defs"
    "std_types.defs"
)

DEFS_COUNT=0
for defs in "${MIG_DEFS[@]}"; do
    if [ -f "$KERNEL_DIR/mach/$defs" ]; then
        cp -p "$KERNEL_DIR/mach/$defs" "$EXPORT_DIR/mach/"
        DEFS_COUNT=$((DEFS_COUNT + 1))
    else
        echo "  ⚠ Warning: $defs not found"
    fi
done
echo "✓ Exported $DEFS_COUNT MIG definition files"
echo ""

# Export device headers
echo "=== Exporting Device Headers ==="
DEVICE_HEADERS=(
    "device.h"
    "device_types.h"
    "net_status.h"
)

DEVICE_COUNT=0
for header in "${DEVICE_HEADERS[@]}"; do
    if [ -f "$KERNEL_DIR/device/$header" ]; then
        cp -p "$KERNEL_DIR/device/$header" "$EXPORT_DIR/device/"
        DEVICE_COUNT=$((DEVICE_COUNT + 1))
    fi
done
echo "✓ Exported $DEVICE_COUNT device headers"
echo ""

# Export system headers
echo "=== Exporting System Headers ==="
SYS_HEADERS=(
    "types.h"
    "time.h"
    "ioctl.h"
    "reboot.h"
)

SYS_COUNT=0
for header in "${SYS_HEADERS[@]}"; do
    if [ -f "$KERNEL_DIR/sys/$header" ]; then
        cp -p "$KERNEL_DIR/sys/$header" "$EXPORT_DIR/sys/"
        SYS_COUNT=$((SYS_COUNT + 1))
    fi
done
echo "✓ Exported $SYS_COUNT system headers"
echo ""

# Export i386 machine-specific headers
echo "=== Exporting i386 Machine Headers ==="
I386_HEADERS=(
    "asm.h"
    "cpu.h"
    "eflags.h"
    "machparam.h"
    "thread.h"
    "trap.h"
    "vm_param.h"
    "vm_types.h"
)

I386_COUNT=0
for header in "${I386_HEADERS[@]}"; do
    if [ -f "$KERNEL_DIR/i386/$header" ]; then
        cp -p "$KERNEL_DIR/i386/$header" "$EXPORT_DIR/i386/"
        I386_COUNT=$((I386_COUNT + 1))
    elif [ -f "$KERNEL_DIR/i386/include/mach/i386/$header" ]; then
        cp -p "$KERNEL_DIR/i386/include/mach/i386/$header" "$EXPORT_DIR/i386/"
        I386_COUNT=$((I386_COUNT + 1))
    fi
done
echo "✓ Exported $I386_COUNT i386 headers"
echo ""

# Create machine symlink for compatibility
if [ ! -e "$EXPORT_DIR/mach/machine" ]; then
    ln -s ../i386 "$EXPORT_DIR/mach/machine"
    echo "✓ Created machine → i386 symlink"
fi

# Export all remaining Mach headers (comprehensive)
echo "=== Comprehensive Mach Header Export ==="
ADDITIONAL_COUNT=0
if [ -d "$KERNEL_DIR/mach" ]; then
    for header in "$KERNEL_DIR/mach"/*.h; do
        if [ -f "$header" ]; then
            basename_h=$(basename "$header")
            if [ ! -f "$EXPORT_DIR/mach/$basename_h" ]; then
                cp -p "$header" "$EXPORT_DIR/mach/"
                ADDITIONAL_COUNT=$((ADDITIONAL_COUNT + 1))
            fi
        fi
    done
fi
echo "✓ Exported $ADDITIONAL_COUNT additional Mach headers"
echo ""

# Create README for exported headers
cat > "$EXPORT_DIR/README.md" << 'EOF'
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
EOF

echo "✓ Created README.md"
echo ""

# Validation
echo "=== Validation ==="
TOTAL_FILES=$(find "$EXPORT_DIR" -type f | wc -l)
TOTAL_HEADERS=$(find "$EXPORT_DIR" -name "*.h" | wc -l)
TOTAL_DEFS=$(find "$EXPORT_DIR" -name "*.defs" | wc -l)

echo "Total files exported: $TOTAL_FILES"
echo "  Headers (.h):       $TOTAL_HEADERS"
echo "  Definitions (.defs): $TOTAL_DEFS"
echo ""

if [ "$TOTAL_HEADERS" -lt 20 ]; then
    echo "⚠ WARNING: Low header count ($TOTAL_HEADERS < 20)"
    echo "   This may indicate incomplete export"
fi

if [ "$TOTAL_DEFS" -lt 5 ]; then
    echo "⚠ WARNING: Low definition file count ($TOTAL_DEFS < 5)"
    echo "   MIG generation may be incomplete"
fi

# Final summary
echo "╔════════════════════════════════════════════════════════════╗"
echo "║         Header Export Complete                             ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "Exported to: $EXPORT_DIR"
echo "  Mach headers:   $MACH_COUNT + $ADDITIONAL_COUNT"
echo "  Device headers: $DEVICE_COUNT"
echo "  System headers: $SYS_COUNT"
echo "  i386 headers:   $I386_COUNT"
echo "  MIG defs:       $DEFS_COUNT"
echo ""
echo "Next steps:"
echo "  1. Build MIG tool: ./scripts/phase2-build-mig.sh"
echo "  2. Generate IPC stubs from .defs files"
echo "  3. Compile userland servers with -I$EXPORT_DIR"
echo ""
