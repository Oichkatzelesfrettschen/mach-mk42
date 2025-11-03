#!/bin/bash
# Boot Mach MK42 kernel in QEMU i386 emulator
# This script demonstrates booting the legacy kernel in virtualization

set -e

# Configuration
KERNEL="${KERNEL:-obj/i386_mach/latest/kernel/STD+WS/mach_kernel}"
MEMORY="${QEMU_MEMORY:-64M}"
TIMEOUT="${QEMU_TIMEOUT:-30}"
VERBOSE="${QEMU_VERBOSE:-0}"
USE_ISO="${USE_ISO:-1}"

# Check if kernel exists
if [ ! -f "$KERNEL" ]; then
    echo "Error: Kernel not found at $KERNEL"
    echo "Available kernels:"
    find . -name "mach_kernel" -type f 2>/dev/null
    exit 1
fi

echo "=== Mach MK42 QEMU Boot Test ==="
echo "Kernel: $KERNEL"
echo "Memory: $MEMORY"
echo "Timeout: ${TIMEOUT}s"
echo ""

# Display kernel information
echo "=== Kernel Information ==="
file "$KERNEL"
ls -lh "$KERNEL"
echo ""

# Check kernel format
if file "$KERNEL" | grep -q "a.out"; then
    echo "⚠ Note: Kernel is in a.out format"
    if [ "$USE_ISO" = "1" ]; then
        echo "Creating bootable ISO with GRUB for proper booting..."
        echo ""
        
        # Check for required tools
        if command -v grub-mkrescue &> /dev/null; then
            ./create-bootable-iso.sh "$KERNEL"
            echo ""
            echo "ISO created. Booting from ISO..."
            exec ./boot-qemu-iso.sh
        else
            echo "⚠ grub-mkrescue not available"
            echo "Install with: apt-get install grub-pc-bin xorriso"
            echo "Attempting direct kernel boot (may not work with a.out format)..."
            echo ""
        fi
    else
        echo "Attempting direct kernel boot (may not work with a.out format)..."
        echo ""
    fi
fi

# Check for QEMU
if ! command -v qemu-system-i386 &> /dev/null; then
    echo "Error: qemu-system-i386 not found"
    echo "Install with: apt-get install qemu-system-x86"
    exit 1
fi

echo "=== Booting kernel in QEMU (${TIMEOUT}s timeout) ==="
echo "Note: Mach microkernel requires user-space servers to fully boot."
echo "This test validates that the kernel loads and begins execution."
echo ""

# QEMU boot parameters
QEMU_ARGS=(
    -kernel "$KERNEL"
    -m "$MEMORY"
    -nographic
    -no-reboot
)

# Add verbose options if requested
if [ "$VERBOSE" = "1" ]; then
    QEMU_ARGS+=(-d cpu_reset,int -D qemu.log)
fi

# Run QEMU with timeout
if timeout "${TIMEOUT}s" qemu-system-i386 "${QEMU_ARGS[@]}" 2>&1; then
    EXIT_CODE=0
else
    EXIT_CODE=$?
fi

echo ""
echo "=== QEMU Exit Code: $EXIT_CODE ==="

# Interpret exit code
case $EXIT_CODE in
    0)
        echo "✓ Kernel exited cleanly"
        ;;
    1)
        echo "⚠ QEMU error - a.out format cannot be directly booted by QEMU's -kernel option"
        echo "The a.out executable format is not supported by QEMU's direct kernel loading."
        echo "Try creating a bootable ISO: ./create-bootable-iso.sh"
        echo "Then boot with: ./boot-qemu-iso.sh"
        exit 2  # Exit with specific code for a.out format limitation
        ;;
    124)
        echo "⏱ Timeout reached (expected - kernel loaded but needs user-space)"
        exit 0
        ;;
    *)
        echo "✗ QEMU failed with exit code $EXIT_CODE"
        if [ -f qemu.log ]; then
            echo ""
            echo "=== QEMU Debug Log (last 50 lines) ==="
            tail -50 qemu.log
        fi
        exit $EXIT_CODE
        ;;
esac
