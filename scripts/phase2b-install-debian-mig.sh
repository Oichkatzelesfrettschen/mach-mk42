#!/bin/bash
set -euo pipefail

# Phase 2B: Install MIG from Debian repositories for i686-gnu (32-bit Mach) target
# Based on user-provided information that MIG is available in Debian but not Ubuntu

echo "=== Phase 2B: Installing MIG from Debian repositories ==="

# Add Debian sources for MIG
echo "Adding Debian Sid sources for MIG packages..."
sudo tee /etc/apt/sources.list.d/debian-mig.list > /dev/null << 'EOF'
# Debian Sid for MIG (GNU Mach Interface Generator)
deb http://deb.debian.org/debian sid main
EOF

# Create APT preferences to only allow MIG packages from Debian
echo "Creating APT pinning preferences..."
sudo tee /etc/apt/preferences.d/debian-mig > /dev/null << 'EOF'
# Prefer Ubuntu packages by default
Package: *
Pin: release o=Ubuntu
Pin-Priority: 500

# Allow MIG and dependencies from Debian Sid
Package: mig mig-i686-gnu mig-x86-64-gnu gnumach-common mig-for-host
Pin: release o=Debian
Pin-Priority: 600
EOF

# Update package lists
echo "Updating package lists..."
sudo apt update

# Install MIG for i686-gnu target (32-bit Mach)
echo "Installing mig-i686-gnu from Debian..."
sudo apt install -y mig-i686-gnu || {
    echo "Trying mig-for-host as fallback..."
    sudo apt install -y mig-for-host || {
        echo "Trying generic mig package..."
        sudo apt install -y mig
    }
}

# Verify installation
echo ""
echo "=== MIG Installation Verification ==="
which mig && mig --version || echo "MIG binary not found in PATH"
which i686-gnu-mig && i686-gnu-mig --version || echo "i686-gnu-mig not found (may not exist)"

# Check installed packages
dpkg -l | grep mig || echo "No MIG packages found"

echo ""
echo "=== Phase 2B: MIG Installation Complete ==="
