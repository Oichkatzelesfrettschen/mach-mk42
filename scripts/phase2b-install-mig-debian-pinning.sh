#!/bin/bash
set -euo pipefail

echo "=== Phase 2B: Installing MIG via Debian Pinning ==="
echo "Ubuntu doesn't ship MIG - using Debian packages with APT pinning"

# Add Debian Stable sources for MIG
echo "Adding Debian Stable repository..."
sudo tee /etc/apt/sources.list.d/debian-stable.list > /dev/null << 'APT_EOF'
# Debian Stable for MIG package only
deb http://deb.debian.org/debian stable main
deb-src http://deb.debian.org/debian stable main
APT_EOF

# Create APT pinning to prefer Ubuntu but allow specific Debian packages
echo "Configuring APT pinning..."
sudo tee /etc/apt/preferences.d/debian-mig > /dev/null << 'PIN_EOF'
# Prefer Ubuntu packages by default
Package: *
Pin: release o=Ubuntu
Pin-Priority: 700

# Allow Debian packages with lower priority
Package: *
Pin: release o=Debian
Pin-Priority: 100

# Explicitly allow MIG from Debian
Package: mig mig-for-host mig-i686-gnu gnumach-dev
Pin: release o=Debian
Pin-Priority: 600
PIN_EOF

echo "Updating package lists..."
sudo apt-get update

echo "Installing MIG from Debian..."
# Install MIG for i686 target (32-bit Mach)
sudo apt-get install -y mig-i686-gnu gnumach-dev || {
    echo "Note: mig-i686-gnu not available, trying mig-for-host..."
    sudo apt-get install -y mig-for-host gnumach-dev
}

echo "Verifying MIG installation..."
which mig || echo "WARNING: mig not in PATH"
mig --version || echo "WARNING: Could not get MIG version"

echo "MIG installation complete!"
echo "Location: $(which mig 2>/dev/null || echo 'Not in PATH')"
echo "Version: $(mig --version 2>&1 | head -1 || echo 'Unknown')"
