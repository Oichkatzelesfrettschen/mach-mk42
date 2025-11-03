#!/bin/bash
# Phase 2C: Generate IPC stubs for all server components
# Uses MIG to generate Client (User) and Server stubs from .defs files

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
MIG_BIN="$PROJECT_ROOT/toolchain/i386-mach/bin/i386-mach-mig"
INCLUDE_DIR="$PROJECT_ROOT/userland/include"
STUBS_DIR="$PROJECT_ROOT/build/ipc-stubs"

echo "=================================================="
echo "Phase 2C: IPC Stub Generation"
echo "=================================================="
echo

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# Verify MIG is available
if [ ! -x "$MIG_BIN" ]; then
    log_error "MIG not found at $MIG_BIN"
    log_error "Run ./scripts/phase2b-complete-mig.sh first"
    exit 1
fi

log_info "Using MIG: $MIG_BIN"

# Create stubs directory
mkdir -p "$STUBS_DIR"

log_info "Step 1: Generate stubs from kernel .defs files"

# List of kernel .defs files to process
KERNEL_DEFS=(
    "mach/mach.defs:mach"
    "mach/mach_host.defs:mach_host"
    "mach/mach_port.defs:mach_port"
    "mach/memory_object.defs:memory_object"
    "mach/memory_object_default.defs:memory_object_default"
    "mach/default_pager_object.defs:default_pager_object"
    "mach/exc.defs:exc"
    "mach/notify.defs:notify"
)

generated_count=0
total_defs=${#KERNEL_DEFS[@]}

for def_entry in "${KERNEL_DEFS[@]}"; do
    def_file="${def_entry%:*}"
    def_name="${def_entry#*:}"
    full_path="$INCLUDE_DIR/$def_file"
    
    if [ -f "$full_path" ]; then
        log_info "Processing $def_file..."
        
        cd "$STUBS_DIR"
        
        # Generate stubs
        if "$MIG_BIN" -I"$INCLUDE_DIR" "$full_path" 2>&1 | tee "${def_name}_mig.log"; then
            # Check for generated files
            user_stub="${def_name}User.c"
            server_stub="${def_name}Server.c"
            
            if [ -f "$user_stub" ] && [ -f "$server_stub" ]; then
                user_lines=$(wc -l < "$user_stub")
                server_lines=$(wc -l < "$server_stub")
                log_info "✅ Generated ${def_name}User.c ($user_lines lines)"
                log_info "✅ Generated ${def_name}Server.c ($server_lines lines)"
                ((generated_count++))
            else
                log_warn "⚠️  Expected stubs not generated for $def_file"
            fi
        else
            log_error "❌ MIG failed on $def_file (see ${def_name}_mig.log)"
        fi
        
        cd "$PROJECT_ROOT"
    else
        log_warn "⚠️  $def_file not found"
    fi
done

log_info "Step 2: Create custom .defs files for bootstrap server"

cd "$STUBS_DIR"

# Bootstrap server .defs
cat > bootstrap.defs << 'EOF'
/*
 * Bootstrap Server Interface
 */
subsystem bootstrap 1000;

#include <mach/std_types.defs>
#include <mach/mach_types.defs>

import <mach/mach.h>;

/*
 * Register a service with the bootstrap server
 */
routine bootstrap_register(
    bootstrap_port : mach_port_t;
    service_name : name_t;
    service_port : mach_port_t);

/*
 * Look up a service by name
 */
routine bootstrap_look_up(
    bootstrap_port : mach_port_t;
    service_name : name_t;
    out service_port : mach_port_t);

/*
 * Create a new service
 */
routine bootstrap_create_service(
    bootstrap_port : mach_port_t;
    service_name : name_t;
    out service_port : mach_port_t);

/*
 * Check in a service (transfer receive right)
 */
routine bootstrap_check_in(
    bootstrap_port : mach_port_t;
    service_name : name_t;
    out service_port : mach_port_t);

/*
 * Get bootstrap port
 */
routine bootstrap_status(
    bootstrap_port : mach_port_t;
    out status : int);
EOF

log_info "Created custom bootstrap.defs"

if "$MIG_BIN" -I"$INCLUDE_DIR" bootstrap.defs 2>&1 | tee bootstrap_mig.log; then
    if [ -f "bootstrapUser.c" ] && [ -f "bootstrapServer.c" ]; then
        log_info "✅ Generated bootstrapUser.c and bootstrapServer.c"
        ((generated_count++))
    fi
fi

log_info "Step 3: Create custom .defs files for default pager"

cat > default_pager.defs << 'EOF'
/*
 * Default Pager Interface
 */
subsystem default_pager 2000;

#include <mach/std_types.defs>
#include <mach/mach_types.defs>

import <mach/mach.h>;

/*
 * Initialize default pager
 */
routine default_pager_initialize(
    pager_port : mach_port_t;
    max_size : vm_size_t);

/*
 * Create backing object
 */
routine default_pager_object_create(
    pager_port : mach_port_t;
    size : vm_size_t;
    out memory_object : mach_port_t);

/*
 * Get pager info
 */
routine default_pager_info(
    pager_port : mach_port_t;
    out total_size : vm_size_t;
    out free_size : vm_size_t);

/*
 * Page in request
 */
routine default_pager_page_read(
    memory_object : mach_port_t;
    offset : vm_offset_t;
    size : vm_size_t;
    out data : pointer_t);

/*
 * Page out request
 */
routine default_pager_page_write(
    memory_object : mach_port_t;
    offset : vm_offset_t;
    data : pointer_t;
    size : vm_size_t);
EOF

log_info "Created custom default_pager.defs"

if "$MIG_BIN" -I"$INCLUDE_DIR" default_pager.defs 2>&1 | tee default_pager_mig.log; then
    if [ -f "default_pagerUser.c" ] && [ -f "default_pagerServer.c" ]; then
        log_info "✅ Generated default_pagerUser.c and default_pagerServer.c"
        ((generated_count++))
    fi
fi

log_info "Step 4: Compile all generated stubs"

# Create compilation script
cat > compile_stubs.sh << 'EOF'
#!/bin/bash
set -e

INCLUDE_DIR="../userland/include"
# Added -Werror for zero warnings policy on generated stub compilation
CFLAGS="-Wall -Wextra -Werror -I$INCLUDE_DIR -I. -c -m32"

for stub in *User.c *Server.c; do
    if [ -f "$stub" ]; then
        obj="${stub%.c}.o"
        echo "Compiling $stub..."
        if gcc $CFLAGS "$stub" -o "$obj"; then
            echo "✅ $stub → $obj"
        else
            echo "❌ Failed to compile $stub"
        fi
    fi
done

echo "Creating libmach_ipc.a..."
ar rcs libmach_ipc.a *User.o *Server.o
echo "✅ libmach_ipc.a created"
EOF

chmod +x compile_stubs.sh

# Run compilation
log_info "Compiling IPC stubs..."
if ./compile_stubs.sh 2>&1 | tee compile_stubs.log; then
    if [ -f "libmach_ipc.a" ]; then
        # Portable file size detection
        if command -v stat >/dev/null 2>&1; then
            lib_size=$(stat -c%s libmach_ipc.a 2>/dev/null || stat -f%z libmach_ipc.a 2>/dev/null || wc -c < libmach_ipc.a)
        else
            lib_size=$(wc -c < libmach_ipc.a)
        fi
        lib_kb=$((lib_size / 1024))
        log_info "✅ libmach_ipc.a created (${lib_kb}KB)"
    fi
else
    log_warn "⚠️  Some stubs may not have compiled (see compile_stubs.log)"
fi

log_info "Step 5: Create stub usage documentation"

cat > README.md << 'EOF'
# IPC Stubs Directory

This directory contains MIG-generated IPC stubs for the Mach MK42 project.

## Generated Files

### Kernel Stubs
- `machUser.c` / `machServer.c` - Core Mach IPC
- `mach_hostUser.c` / `mach_hostServer.c` - Host operations
- `mach_portUser.c` / `mach_portServer.c` - Port management
- `memory_objectUser.c` / `memory_objectServer.c` - VM object interface
- `memory_object_defaultUser.c` / `memory_object_defaultServer.c` - Default memory object
- `default_pager_objectUser.c` / `default_pager_objectServer.c` - Pager object interface
- `excUser.c` / `excServer.c` - Exception handling
- `notifyUser.c` / `notifyServer.c` - Notification interface

### Custom Stubs
- `bootstrapUser.c` / `bootstrapServer.c` - Bootstrap server interface
- `default_pagerUser.c` / `default_pagerServer.c` - Default pager interface

## Library

`libmach_ipc.a` - Static library containing all compiled stubs

## Usage

### In Client Code (User)
```c
#include "machUser.h"

mach_port_t port;
kern_return_t kr;

kr = task_get_special_port(mach_task_self(), 
                           TASK_BOOTSTRAP_PORT, 
                           &port);
```

### In Server Code
```c
#include "machServer.h"

boolean_t mach_server(mach_msg_header_t *request,
                      mach_msg_header_t *reply)
{
    return mach_server_routine(request, reply);
}
```

## Linking

Link with:
```bash
gcc -o myserver myserver.c -L. -lmach_ipc -m32
```

## Regeneration

To regenerate stubs:
```bash
cd /path/to/project
./scripts/phase2c-generate-stubs.sh
```
EOF

cd "$PROJECT_ROOT"

echo
echo "=================================================="
echo "Phase 2C Summary"
echo "=================================================="
echo "Generated: $generated_count sets of stubs"
echo "Location: $STUBS_DIR"
echo "Library: $STUBS_DIR/libmach_ipc.a"
echo
log_info "✅ IPC stub generation complete"
echo "=================================================="
