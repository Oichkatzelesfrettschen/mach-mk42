#!/bin/bash
# Extract and prepare userland components from osfmk6.1 and xMach
# This script clones the repositories and extracts relevant userland code

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORK_DIR="${WORK_DIR:-/tmp/userland-extract}"

echo "=== Mach Userland Component Extractor ==="
echo "Working directory: $WORK_DIR"
echo ""

# Create working directory
mkdir -p "$WORK_DIR"
cd "$WORK_DIR"

# Clone repositories if not present
echo "=== Cloning source repositories ==="

if [ ! -d "osfmk6.1" ]; then
    echo "Cloning osfmk6.1..."
    git clone --depth 1 https://github.com/nmartin0/osfmk6.1.git
else
    echo "osfmk6.1 already cloned"
fi

if [ ! -d "xMach" ]; then
    echo "Cloning xMach..."
    git clone --depth 1 https://github.com/nmartin0/xMach.git
else
    echo "xMach already cloned"
fi

echo ""
echo "=== Extracting userland components ==="

# Create userland directory structure
USERLAND_DIR="$SCRIPT_DIR/userland"
mkdir -p "$USERLAND_DIR"

# Extract Default Pager
echo "Extracting default_pager..."
if [ -d "osfmk6.1/src/src/default_pager" ]; then
    mkdir -p "$USERLAND_DIR/default_pager"
    cp -r osfmk6.1/src/src/default_pager/* "$USERLAND_DIR/default_pager/"
    echo "✓ Default pager extracted"
else
    echo "⚠ Default pager not found in osfmk6.1"
fi

# Extract Bootstrap Server
echo "Extracting bootstrap server..."
if [ -d "osfmk6.1/src/src/bootstrap" ]; then
    mkdir -p "$USERLAND_DIR/bootstrap"
    cp -r osfmk6.1/src/src/bootstrap/* "$USERLAND_DIR/bootstrap/"
    echo "✓ Bootstrap server extracted"
else
    echo "⚠ Bootstrap server not found in osfmk6.1"
fi

# Extract Lites components (selective)
echo "Extracting Lites server components..."
if [ -d "xMach/lites" ]; then
    mkdir -p "$USERLAND_DIR/lites"
    
    # Copy key directories
    for dir in server emulator include liblites conf bin; do
        if [ -d "xMach/lites/$dir" ]; then
            cp -r "xMach/lites/$dir" "$USERLAND_DIR/lites/"
            echo "  ✓ Copied $dir"
        fi
    done
    
    # Copy build files
    for file in configure Makeconf Makefile.in; do
        if [ -f "xMach/lites/$file" ]; then
            cp "xMach/lites/$file" "$USERLAND_DIR/lites/"
        fi
    done
    
    echo "✓ Lites server extracted"
else
    echo "⚠ Lites server not found in xMach"
fi

# Create README for userland directory
cat > "$USERLAND_DIR/README.md" << 'EOF'
# Mach Userland Components

This directory contains userland servers and components extracted from:
- **osfmk6.1:** OSF Mach 6.1 (https://github.com/nmartin0/osfmk6.1)
- **xMach:** Mach4 + Lites (https://github.com/nmartin0/xMach)

## Components

### default_pager/
Default paging server - handles virtual memory paging
- Source: osfmk6.1/src/src/default_pager
- Purpose: Provides backing store for VM when no other pager available

### bootstrap/
Bootstrap server - first userland process
- Source: osfmk6.1/src/src/bootstrap
- Purpose: Starts and manages other servers, provides name service

### lites/
Lites single server - BSD Unix compatibility
- Source: xMach/lites
- Purpose: Provides complete BSD 4.4 Unix environment on Mach

## Building

See ../docs/INTEGRATION.md for build instructions.

## Status

- [x] Sources extracted
- [ ] Build infrastructure created
- [ ] Components built for i386
- [ ] Integration with MK42 kernel tested
- [ ] Boot image with userland created

## Notes

These components may require adaptation to work with MK42:
- Include path updates
- MIG interface compatibility
- Binary format matching (a.out vs ELF)

See ../docs/USERLAND.md for architecture details.
EOF

echo ""
echo "=== Extraction complete ==="
echo "Userland components extracted to: $USERLAND_DIR"
echo ""

# Display summary
echo "Summary:"
if [ -d "$USERLAND_DIR/default_pager" ]; then
    FILE_COUNT=$(find "$USERLAND_DIR/default_pager" -type f | wc -l)
    echo "  ✓ default_pager: $FILE_COUNT files"
fi

if [ -d "$USERLAND_DIR/bootstrap" ]; then
    FILE_COUNT=$(find "$USERLAND_DIR/bootstrap" -type f | wc -l)
    echo "  ✓ bootstrap: $FILE_COUNT files"
fi

if [ -d "$USERLAND_DIR/lites" ]; then
    FILE_COUNT=$(find "$USERLAND_DIR/lites" -type f | wc -l)
    echo "  ✓ lites: $FILE_COUNT files"
fi

echo ""
echo "Next steps:"
echo "  1. Review extracted components in $USERLAND_DIR"
echo "  2. Read docs/INTEGRATION.md for build instructions"
echo "  3. Adapt include paths for MK42 kernel"
echo "  4. Create build infrastructure"
echo "  5. Test compilation"
echo ""
echo "To commit to repository:"
echo "  cd $SCRIPT_DIR"
echo "  git add userland/"
echo "  git commit -m 'Add userland components from osfmk6.1 and xMach'"
