#!/bin/bash
# Create a bootable ISO image for Mach MK42 kernel using GRUB

set -e

KERNEL="${1:-obj/i386_mach/latest/kernel/STD+WS/mach_kernel}"
ISO_DIR="iso_build"
ISO_FILE="mach-mk42.iso"

echo "=== Creating Bootable ISO for Mach MK42 ==="
echo "Kernel: $KERNEL"
echo ""

# Check if kernel exists
if [ ! -f "$KERNEL" ]; then
    echo "Error: Kernel not found at $KERNEL"
    exit 1
fi

# Check for required tools
if ! command -v grub-mkrescue &> /dev/null; then
    echo "Error: grub-mkrescue not found"
    echo "Install with: apt-get install grub-pc-bin xorriso"
    exit 1
fi

# Create ISO structure
echo "Creating ISO directory structure..."
rm -rf "$ISO_DIR"
mkdir -p "$ISO_DIR/boot/grub"

# Copy kernel
echo "Copying kernel..."
cp "$KERNEL" "$ISO_DIR/boot/mach_kernel"

# Create GRUB configuration
echo "Creating GRUB configuration..."
cat > "$ISO_DIR/boot/grub/grub.cfg" << 'EOF'
set timeout=3
set default=0

menuentry "Mach MK42 Kernel" {
    echo "Loading Mach MK42 microkernel..."
    multiboot /boot/mach_kernel
    boot
}

menuentry "Mach MK42 Kernel (Verbose)" {
    echo "Loading Mach MK42 microkernel (verbose)..."
    multiboot /boot/mach_kernel -v
    boot
}
EOF

# Create ISO
echo "Creating bootable ISO..."
if grub-mkrescue -o "$ISO_FILE" "$ISO_DIR" 2>&1 | tee iso-build.log | grep -v "DOS partition"; then
    rm -f iso-build.log
else
    echo "Warning: ISO creation had errors. Check iso-build.log for details."
fi

# Cleanup
rm -rf "$ISO_DIR"

echo ""
echo "=== ISO Created Successfully ==="
ls -lh "$ISO_FILE"
echo ""
echo "Boot with QEMU:"
echo "  qemu-system-i386 -cdrom $ISO_FILE -m 64M -nographic"
echo ""
echo "Or use boot-qemu-iso.sh script"
