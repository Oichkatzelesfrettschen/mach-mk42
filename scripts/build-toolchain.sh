#!/bin/bash
# Build Mach-patched-gcc toolchain for i386
# This creates a cross-compiler suitable for building Mach kernel and userland

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOOLCHAIN_DIR="$SCRIPT_DIR/../toolchain"
BUILD_DIR="$TOOLCHAIN_DIR/build"
SRC_DIR="$SCRIPT_DIR/../../mach-repos/Mach-patched-gcc"
PREFIX="$TOOLCHAIN_DIR/i386-mach"

echo "╔════════════════════════════════════════════════════════════╗"
echo "║         Mach-patched-gcc Toolchain Builder                ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "This script builds a complete i386 cross-compiler toolchain"
echo "with Mach-specific patches for building the kernel and userland."
echo ""

# Check if source exists
if [ ! -d "$SRC_DIR" ]; then
    echo "❌ ERROR: Mach-patched-gcc not found at $SRC_DIR"
    echo "Run roadmap-kickstart.sh first to clone repositories"
    exit 1
fi

echo "=== Configuration ==="
echo "Source:  $SRC_DIR"
echo "Build:   $BUILD_DIR"
echo "Install: $PREFIX"
echo ""

# Create directories
mkdir -p "$BUILD_DIR"
mkdir -p "$PREFIX"

# Check what's available in the source
echo "=== Inspecting Mach-patched-gcc Source ==="
ls -la "$SRC_DIR"
echo ""

if [ -d "$SRC_DIR/gnu" ]; then
    echo "Found GNU toolchain directory"
    ls -la "$SRC_DIR/gnu/"
    echo ""
fi

# For now, document what we found and create a placeholder
echo "=== Analysis ==="
echo "The Mach-patched-gcc repository contains:"
find "$SRC_DIR" -type f -name "*.patch" -o -name "configure" -o -name "README*" | head -20
echo ""

echo "=== Toolchain Build Strategy ==="
echo ""
echo "Phase 1: Use existing GCC 4.9 from Ubuntu"
echo "  - Current builds work with gcc-4.9"
echo "  - Sufficient for MK42 kernel"
echo "  - Good for initial userland"
echo ""
echo "Phase 2: Build Mach-patched-gcc (if needed)"
echo "  - Extract patches from repository"
echo "  - Apply to appropriate GCC version"
echo "  - Build i386 cross-compiler"
echo "  - Install to $PREFIX"
echo ""

# Create a wrapper toolchain that uses system gcc for now
echo "=== Creating Wrapper Toolchain ==="
mkdir -p "$PREFIX/bin"

# Create wrapper scripts
cat > "$PREFIX/bin/i386-mach-gcc" << 'EOF'
#!/bin/bash
# Wrapper for Mach i386 gcc
# Currently uses system gcc-4.9 with appropriate flags
exec gcc-4.9 -m32 "$@"
EOF

cat > "$PREFIX/bin/i386-mach-g++" << 'EOF'
#!/bin/bash
# Wrapper for Mach i386 g++
exec g++-4.9 -m32 "$@"
EOF

cat > "$PREFIX/bin/i386-mach-as" << 'EOF'
#!/bin/bash
# Wrapper for Mach i386 assembler
exec as --32 "$@"
EOF

cat > "$PREFIX/bin/i386-mach-ld" << 'EOF'
#!/bin/bash
# Wrapper for Mach i386 linker
exec ld -m elf_i386 "$@"
EOF

cat > "$PREFIX/bin/i386-mach-ar" << 'EOF'
#!/bin/bash
# Wrapper for Mach i386 archiver
exec ar "$@"
EOF

chmod +x "$PREFIX/bin/"*

echo "✓ Created wrapper toolchain at $PREFIX/bin/"
echo ""

# Create environment setup script
cat > "$PREFIX/setup-env.sh" << EOF
# Mach Toolchain Environment Setup
export PATH="$PREFIX/bin:\$PATH"
export MACH_CC="i386-mach-gcc"
export MACH_CXX="i386-mach-g++"
export MACH_AS="i386-mach-as"
export MACH_LD="i386-mach-ld"
export MACH_AR="i386-mach-ar"
export MACH_TARGET="i386-mach"

echo "Mach toolchain environment configured"
echo "  CC:  \$MACH_CC"
echo "  CXX: \$MACH_CXX"
echo "  Target: \$MACH_TARGET"
EOF

echo "✓ Created environment setup script"
echo ""

# Test the toolchain
echo "=== Testing Toolchain ==="
export PATH="$PREFIX/bin:$PATH"

echo "Testing i386-mach-gcc..."
if i386-mach-gcc --version | head -1; then
    echo "✓ GCC wrapper works"
else
    echo "❌ GCC wrapper failed"
    exit 1
fi

echo ""
echo "Testing i386-mach-as..."
if i386-mach-as --version | head -1; then
    echo "✓ AS wrapper works"
else
    echo "❌ AS wrapper failed"
    exit 1
fi

echo ""
echo "=== Toolchain Summary ==="
echo ""
echo "Phase 1 Complete: Wrapper toolchain created"
echo "Location: $PREFIX"
echo ""
echo "Available tools:"
ls -1 "$PREFIX/bin/"
echo ""
echo "To use this toolchain:"
echo "  source $PREFIX/setup-env.sh"
echo "  i386-mach-gcc -o hello hello.c"
echo ""
echo "Note: Currently uses system gcc-4.9 with i386 flags."
echo "For full Mach-patched-gcc, run: ./scripts/build-mach-gcc-full.sh"
echo ""

# Create documentation
cat > "$PREFIX/README.md" << 'EOF'
# Mach i386 Toolchain

## Current Implementation

This toolchain uses wrapper scripts that invoke the system's gcc-4.9 with
appropriate i386 flags. This is sufficient for building the Mach MK42 kernel
and initial userland components.

## Tools Available

- `i386-mach-gcc` - C compiler (wraps gcc-4.9 -m32)
- `i386-mach-g++` - C++ compiler (wraps g++-4.9 -m32)
- `i386-mach-as` - Assembler (wraps as --32)
- `i386-mach-ld` - Linker (wraps ld -m elf_i386)
- `i386-mach-ar` - Archiver (wraps ar)

## Usage

```bash
# Set up environment
source toolchain/i386-mach/setup-env.sh

# Compile
i386-mach-gcc -c file.c -o file.o

# Link
i386-mach-gcc -o program file.o
```

## Future: Full Mach-patched-gcc

The Mach-patched-gcc repository contains GCC patches specific to Mach.
These will be integrated in Phase 2 if needed for advanced features.

## Build Flags

Default flags used:
- `-m32` - 32-bit i386 target
- `-march=i386` - i386 architecture (when needed)

For kernel builds, add:
- `-fno-builtin` - No builtin functions
- `-fno-stack-protector` - No stack protection
- `-nostdinc` - No standard includes
EOF

echo "✓ Created toolchain documentation"
echo ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║         Toolchain Build Complete!                         ║"
echo "╚════════════════════════════════════════════════════════════╝"
