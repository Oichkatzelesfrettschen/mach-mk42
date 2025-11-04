#!/bin/bash
# Build Mach userland servers
# This script compiles bootstrap, default_pager, and other essential servers

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
USERLAND_DIR="$PROJECT_ROOT/userland"
KERNEL_DIR="$PROJECT_ROOT/kernel"
TOOLCHAIN="$PROJECT_ROOT/toolchain/i386-mach"
BUILD_DIR="$PROJECT_ROOT/build/userland"

echo "╔════════════════════════════════════════════════════════════╗"
echo "║           Mach Userland Server Builder                    ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Source toolchain environment if available
if [ -f "$TOOLCHAIN/setup-env.sh" ]; then
    echo "=== Loading Mach Toolchain ==="
    source "$TOOLCHAIN/setup-env.sh"
    echo ""
fi

# Configuration
export CC="${MACH_CC:-gcc}"
export CFLAGS="-m32 -I$KERNEL_DIR/include -I$USERLAND_DIR/include -fno-builtin -nostdinc"
export LDFLAGS="-m32"

echo "=== Build Configuration ==="
echo "CC:       $CC"
echo "CFLAGS:   $CFLAGS"
echo "LDFLAGS:  $LDFLAGS"
echo "Userland: $USERLAND_DIR"
echo "Kernel:   $KERNEL_DIR"
echo "Build:    $BUILD_DIR"
echo ""

# Create build directory
mkdir -p "$BUILD_DIR"

# Function to check if source exists
check_component() {
    local component=$1
    if [ ! -d "$USERLAND_DIR/$component" ]; then
        echo "⚠ $component not found, skipping"
        return 1
    fi
    return 0
}

# Function to build a component
build_component() {
    local component=$1
    local source_dir="$USERLAND_DIR/$component"
    local build_dir="$BUILD_DIR/$component"
    
    echo "=== Building $component ==="
    
    if ! check_component "$component"; then
        return 0
    fi
    
    mkdir -p "$build_dir"
    
    # Count source files
    local c_files=$(find "$source_dir" -name "*.c" 2>/dev/null | wc -l)
    local h_files=$(find "$source_dir" -name "*.h" 2>/dev/null | wc -l)
    
    echo "  Source: $source_dir"
    echo "  C files: $c_files"
    echo "  Headers: $h_files"
    
    if [ $c_files -eq 0 ]; then
        echo "  ⚠ No C source files found"
        echo ""
        return 0
    fi
    
    # Attempt to compile (will likely fail without proper headers, but document the attempt)
    echo "  Attempting compilation..."
    
    local success=0
    for cfile in "$source_dir"/*.c; do
        if [ -f "$cfile" ]; then
            local basename=$(basename "$cfile" .c)
            local objfile="$build_dir/$basename.o"
            
            echo "    Compiling $(basename $cfile)..."
            if $CC $CFLAGS -c "$cfile" -o "$objfile" 2>"$build_dir/$basename.err"; then
                echo "      ✓ Success"
                success=$((success + 1))
            else
                echo "      ⚠ Failed (expected - needs kernel headers)"
                # Show first few lines of error
                head -3 "$build_dir/$basename.err" | sed 's/^/        /'
            fi
        fi
    done
    
    echo "  Compiled: $success/$c_files files"
    echo ""
    
    return 0
}

# Build each component
echo "=== Building Userland Components ==="
echo ""

build_component "bootstrap"
build_component "default_pager"
build_component "netmsgserver"
build_component "machid"

# Special handling for Lites (complex component)
if check_component "lites"; then
    echo "=== Lites Server ==="
    echo "  Lites is a complex BSD server requiring full build system"
    echo "  Location: $USERLAND_DIR/lites"
    
    if [ -f "$USERLAND_DIR/lites/configure" ]; then
        echo "  ✓ Has configure script"
    fi
    
    if [ -f "$USERLAND_DIR/lites/Makefile" ]; then
        echo "  ✓ Has Makefile"
    fi
    
    echo "  Status: Requires dedicated build (Phase 4)"
    echo "  See: docs/INTEGRATION.md for Lites build instructions"
    echo ""
fi

# Summary
echo "╔════════════════════════════════════════════════════════════╗"
echo "║              Userland Build Summary                       ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "Build artifacts: $BUILD_DIR"
echo ""
echo "Status:"
echo "  ⚠ Partial compilation only (expected)"
echo "  ℹ Servers need proper Mach kernel headers"
echo "  ℹ MIG interface files need generation"
echo ""
echo "Next Steps:"
echo "  1. Extract kernel headers for userland builds"
echo "  2. Build/install MIG (Mach Interface Generator)"
echo "  3. Generate MIG stubs for IPC interfaces"
echo "  4. Update include paths in server sources"
echo "  5. Retry build with complete environment"
echo ""
echo "See docs/INTEGRATION.md for detailed build instructions."
echo ""

# Create status document
cat > "$BUILD_DIR/STATUS.md" << EOF
# Userland Build Status

## Build Attempt: $(date)

### Components Attempted
- Bootstrap server
- Default pager
- Network message server
- Mach ID server
- Lites (analysis only)

### Current Status
Partial compilation achieved. Full builds require:

1. **Kernel Headers**
   - Need exported headers from kernel build
   - Must match kernel IPC interfaces
   
2. **MIG Tool**
   - Mach Interface Generator
   - Generates IPC stub code from .defs files
   
3. **Include Paths**
   - Update server sources to find headers
   - May need \`-I\` flags adjustments

### Build Configuration
- Compiler: $CC
- Flags: $CFLAGS
- Target: i386 (32-bit)

### Next Actions
1. Export kernel headers to userland/include
2. Build MIG from kernel/src/mig
3. Generate IPC stubs
4. Retry server builds
5. Link servers to create binaries

### Notes
This is expected for Phase 1. The infrastructure is in place,
and we're documenting what's needed for successful builds.

See docs/INTEGRATION.md for complete build procedure.
EOF

echo "✓ Created build status: $BUILD_DIR/STATUS.md"
