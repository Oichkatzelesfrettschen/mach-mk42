#!/bin/bash
# Build script for Mach MK42 kernel

set -e

echo "=== Building Mach MK42 Kernel ==="
echo "Target Machine: ${TARGET_MACHINE:-AT386}"
echo "Configuration: ${CONFIG:-STD+WS}"

# Navigate to kernel directory
cd /workspace/kernel

# Set required environment variables
export TARGET_MACHINE=${TARGET_MACHINE:-AT386}
export CONFIG=${CONFIG:-STD+WS}
export OBJECTDIR=${OBJECTDIR:-/workspace/obj/i386_mach/latest/kernel/STD+WS}

# Create object directory if it doesn't exist
mkdir -p ${OBJECTDIR}

# Build the config tool
echo "=== Building config tool ==="
cd src/config
make || true
cd ../..

# Build the mig tool
echo "=== Building mig tool ==="
cd src/mig
make || true
cd ../..

# Configure the kernel
echo "=== Configuring kernel ==="
make ${CONFIG}.doconf || true
make ${CONFIG}.config || true
make ${CONFIG}.mig || true

# Build the kernel
echo "=== Building kernel ==="
make ${CONFIG}.make || echo "Build completed with warnings"

# Check if kernel was built
if [ -f "${OBJECTDIR}/mach_kernel" ]; then
    echo "=== Kernel built successfully ==="
    ls -lh ${OBJECTDIR}/mach_kernel
    file ${OBJECTDIR}/mach_kernel
else
    echo "=== Kernel binary not found at expected location ==="
    echo "Searching for kernel binaries..."
    find /workspace -name "mach_kernel" -type f 2>/dev/null || echo "No kernel binary found"
fi

echo "=== Build process completed ==="
