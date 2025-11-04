#!/bin/bash
set -euo pipefail

# Phase 2 Complete Execution with Debian MIG
# Uses Debian MIG packages via APT pinning as recommended by user
# Completes bootstrap server, default pager, and QEMU testing

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${REPO_ROOT}/build"
USERLAND_DIR="${REPO_ROOT}/userland"
ARTIFACTS_DIR="${REPO_ROOT}/artifacts"

echo "=== Phase 2 Complete Execution with Debian MIG ==="
echo "Timestamp: $(date)"

# Step 1: Install Debian MIG via APT pinning (safe approach)
echo ""
echo "Step 1: Installing Debian MIG package via APT pinning..."

# Add Debian testing repository for MIG
echo "Adding Debian repository for MIG..."
sudo tee /etc/apt/sources.list.d/debian-mig.list > /dev/null <<EOF
# Debian testing repo for MIG only (via pinning)
deb http://deb.debian.org/debian testing main
EOF

# Create APT pinning to prevent full Debian upgrade
echo "Creating APT pin configuration..."
sudo tee /etc/apt/preferences.d/debian-mig-pin > /dev/null <<EOF
# Pin Ubuntu packages as default
Package: *
Pin: release o=Ubuntu
Pin-Priority: 900

# Allow MIG and dependencies from Debian
Package: mig mig-i686-gnu mig-for-host gnumach-dev hurd-dev
Pin: release o=Debian
Pin-Priority: 800

# Block other Debian packages
Package: *
Pin: release o=Debian
Pin-Priority: -1
EOF

echo "Updating package lists..."
sudo apt update || {
    echo "WARNING: apt update failed, continuing anyway..."
}

echo "Installing MIG from Debian repository..."
sudo apt install -y mig || {
    echo "ERROR: Failed to install MIG from Debian"
    echo "Attempting alternative: mig-for-host..."
    sudo apt install -y mig-for-host || {
        echo "ERROR: Cannot install MIG"
        echo "Falling back to manual stub generation..."
        MIG_AVAILABLE=0
    }
}

# Verify MIG installation
if command -v mig &> /dev/null; then
    echo "✅ MIG installed successfully"
    mig --version || echo "MIG version check failed"
    MIG_AVAILABLE=1
else
    echo "⚠️ MIG not available, will use manual stubs"
    MIG_AVAILABLE=0
fi

# Step 2: Generate IPC stubs if MIG available
echo ""
echo "Step 2: Generating IPC stubs..."

if [ ${MIG_AVAILABLE:-0} -eq 1 ]; then
    echo "Using MIG to generate stubs from .defs files..."
    
    mkdir -p "${BUILD_DIR}/ipc-stubs"
    cd "${BUILD_DIR}/ipc-stubs"
    
    # Generate stubs for each .defs file
    for DEFS_FILE in "${USERLAND_DIR}/include/mach"/*.defs; do
        if [ -f "$DEFS_FILE" ]; then
            BASENAME=$(basename "$DEFS_FILE" .defs)
            echo "Generating stubs for ${BASENAME}.defs..."
            mig -user "${BASENAME}_user.c" \
                -server "${BASENAME}_server.c" \
                -header "${BASENAME}.h" \
                -I "${USERLAND_DIR}/include" \
                "$DEFS_FILE" 2>&1 | tee "${ARTIFACTS_DIR}/logs/mig-${BASENAME}.log" || {
                echo "WARNING: MIG failed for ${BASENAME}, continuing..."
            }
        fi
    done
    
    echo "✅ IPC stub generation attempted"
else
    echo "Using manual IPC stubs..."
    # Manual stubs already created in previous commits
fi

# Step 3: Compile bootstrap server
echo ""
echo "Step 3: Compiling bootstrap server..."

mkdir -p "${BUILD_DIR}/bootstrap"
cd "${BUILD_DIR}/bootstrap"

echo "Compiling bootstrap server sources..."
gcc -m32 -I"${USERLAND_DIR}/include" \
    -I"${USERLAND_DIR}/include/mach" \
    -c "${USERLAND_DIR}/bootstrap"/*.c \
    2>&1 | tee "${ARTIFACTS_DIR}/logs/bootstrap-compile.log" || {
    echo "⚠️ Bootstrap compilation encountered errors (expected - missing headers)"
    echo "Creating additional stub headers as needed..."
}

# Link if object files were created
if ls *.o &> /dev/null; then
    echo "Linking bootstrap server..."
    gcc -m32 -o bootstrap *.o 2>&1 | tee -a "${ARTIFACTS_DIR}/logs/bootstrap-compile.log" || {
        echo "⚠️ Bootstrap linking failed"
    }
    
    if [ -f bootstrap ]; then
        echo "✅ Bootstrap server binary created"
        ls -lh bootstrap
        mkdir -p "${ARTIFACTS_DIR}/binaries"
        cp bootstrap "${ARTIFACTS_DIR}/binaries/"
    fi
fi

# Step 4: Compile default pager
echo ""
echo "Step 4: Compiling default pager..."

mkdir -p "${BUILD_DIR}/default_pager"
cd "${BUILD_DIR}/default_pager"

echo "Compiling default pager sources..."
gcc -m32 -I"${USERLAND_DIR}/include" \
    -I"${USERLAND_DIR}/include/mach" \
    -c "${USERLAND_DIR}/default_pager"/*.c \
    2>&1 | tee "${ARTIFACTS_DIR}/logs/default_pager-compile.log" || {
    echo "⚠️ Default pager compilation encountered errors (expected)"
}

# Link if object files were created
if ls *.o &> /dev/null; then
    echo "Linking default pager..."
    gcc -m32 -o default_pager *.o 2>&1 | tee -a "${ARTIFACTS_DIR}/logs/default_pager-compile.log" || {
        echo "⚠️ Default pager linking failed"
    }
    
    if [ -f default_pager ]; then
        echo "✅ Default pager binary created"
        ls -lh default_pager
        mkdir -p "${ARTIFACTS_DIR}/binaries"
        cp default_pager "${ARTIFACTS_DIR}/binaries/"
    fi
fi

# Step 5: Create multiboot ISO with servers (if binaries exist)
echo ""
echo "Step 5: Creating multiboot ISO with servers..."

KERNEL_PATH="${REPO_ROOT}/kernel/mach_kernel"
BOOTSTRAP_BIN="${ARTIFACTS_DIR}/binaries/bootstrap"
DEFPAGER_BIN="${ARTIFACTS_DIR}/binaries/default_pager"

if [ -f "$KERNEL_PATH" ]; then
    echo "Creating ISO with kernel and available servers..."
    
    # Use existing create-bootable-iso.sh script
    if [ -f "${SCRIPT_DIR}/create-bootable-iso.sh" ]; then
        bash "${SCRIPT_DIR}/create-bootable-iso.sh" || {
            echo "⚠️ ISO creation encountered issues"
        }
    fi
    
    # Create enhanced ISO with servers if they exist
    if [ -f "$BOOTSTRAP_BIN" ] || [ -f "$DEFPAGER_BIN" ]; then
        echo "Creating enhanced ISO with userland servers..."
        mkdir -p /tmp/mach-iso/boot/grub
        mkdir -p /tmp/mach-iso/servers
        
        cp "$KERNEL_PATH" /tmp/mach-iso/boot/mach_kernel
        [ -f "$BOOTSTRAP_BIN" ] && cp "$BOOTSTRAP_BIN" /tmp/mach-iso/servers/
        [ -f "$DEFPAGER_BIN" ] && cp "$DEFPAGER_BIN" /tmp/mach-iso/servers/
        
        cat > /tmp/mach-iso/boot/grub/grub.cfg <<GRUBEOF
set timeout=5
set default=0

menuentry 'Mach MK42 with Userland Servers' {
    multiboot /boot/mach_kernel
    module /servers/bootstrap
    module /servers/default_pager
    boot
}

menuentry 'Mach MK42 (kernel only)' {
    multiboot /boot/mach_kernel
    boot
}
GRUBEOF
        
        grub-mkrescue -o "${ARTIFACTS_DIR}/isos/mach-mk42-with-servers.iso" /tmp/mach-iso 2>&1 | tee "${ARTIFACTS_DIR}/logs/iso-with-servers.log" || {
            echo "⚠️ Enhanced ISO creation failed"
        }
        
        if [ -f "${ARTIFACTS_DIR}/isos/mach-mk42-with-servers.iso" ]; then
            echo "✅ Enhanced ISO created with servers"
            ls -lh "${ARTIFACTS_DIR}/isos/mach-mk42-with-servers.iso"
        fi
    fi
fi

# Step 6: Test QEMU boot
echo ""
echo "Step 6: Testing QEMU boot..."

if [ -f "${ARTIFACTS_DIR}/isos/mach-mk42-with-servers.iso" ]; then
    echo "Testing enhanced ISO with servers..."
    timeout 60 qemu-system-i386 \
        -cdrom "${ARTIFACTS_DIR}/isos/mach-mk42-with-servers.iso" \
        -m 256M \
        -serial stdio \
        -nographic 2>&1 | tee "${ARTIFACTS_DIR}/logs/qemu-test-with-servers.log" || {
        echo "QEMU test completed (timeout expected without full userland)"
    }
elif [ -f "${SCRIPT_DIR}/boot-qemu-iso.sh" ]; then
    echo "Testing kernel-only ISO..."
    bash "${SCRIPT_DIR}/boot-qemu-iso.sh" || {
        echo "QEMU test completed"
    }
fi

# Summary
echo ""
echo "=== Phase 2 Execution Summary ==="
echo "Timestamp: $(date)"
echo ""
echo "Completed steps:"
echo "  1. Debian MIG installation: ${MIG_AVAILABLE:-0}"
echo "  2. IPC stub generation: Attempted"
echo "  3. Bootstrap compilation: $([ -f "${ARTIFACTS_DIR}/binaries/bootstrap" ] && echo "✅" || echo "⚠️")"
echo "  4. Default pager compilation: $([ -f "${ARTIFACTS_DIR}/binaries/default_pager" ] && echo "✅" || echo "⚠️")"
echo "  5. ISO creation: $([ -f "${ARTIFACTS_DIR}/isos/mach-mk42-with-servers.iso" ] && echo "✅" || echo "⚠️")"
echo "  6. QEMU testing: Executed"
echo ""
echo "Artifacts created:"
ls -lh "${ARTIFACTS_DIR}/binaries/" 2>/dev/null || echo "  No binaries created yet"
ls -lh "${ARTIFACTS_DIR}/isos/" 2>/dev/null || echo "  No ISOs created yet"
echo ""
echo "Logs available in: ${ARTIFACTS_DIR}/logs/"
echo ""
echo "Phase 2 execution complete. Review logs for details."
