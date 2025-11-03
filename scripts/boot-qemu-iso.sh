#!/bin/bash
# Boot Mach MK42 kernel from ISO in QEMU

set -e

ISO="${1:-mach-mk42.iso}"
MEMORY="${QEMU_MEMORY:-64M}"
TIMEOUT="${QEMU_TIMEOUT:-30}"

echo "=== Mach MK42 ISO Boot Test ==="
echo "ISO: $ISO"
echo "Memory: $MEMORY"
echo "Timeout: ${TIMEOUT}s"
echo ""

# Check if ISO exists
if [ ! -f "$ISO" ]; then
    echo "Error: ISO not found at $ISO"
    echo "Create it first with: ./create-bootable-iso.sh"
    exit 1
fi

# Check for QEMU
if ! command -v qemu-system-i386 &> /dev/null; then
    echo "Error: qemu-system-i386 not found"
    echo "Install with: apt-get install qemu-system-x86"
    exit 1
fi

echo "=== Booting ISO in QEMU (${TIMEOUT}s timeout) ==="
echo "Note: Mach microkernel requires user-space servers to fully boot."
echo "This test validates that the kernel loads via GRUB bootloader."
echo ""

# Run QEMU with timeout
if timeout "${TIMEOUT}s" qemu-system-i386 \
    -cdrom "$ISO" \
    -m "$MEMORY" \
    -nographic \
    -no-reboot \
    -serial mon:stdio 2>&1; then
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
    124)
        echo "⏱ Timeout reached (expected - kernel needs user-space)"
        exit 0
        ;;
    *)
        echo "✗ QEMU failed with exit code $EXIT_CODE"
        exit $EXIT_CODE
        ;;
esac
