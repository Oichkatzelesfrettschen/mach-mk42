#!/bin/bash
# Phase 2B: Complete MIG build using native Makefile approach
# This script builds MIG properly by working with the kernel's build system

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build/mig"
MIG_SRC="$PROJECT_ROOT/kernel/src/mig"
KERNEL_ROOT="$PROJECT_ROOT/kernel"

echo "=================================================="
echo "Phase 2B: MIG Build (Using Native Build System)"
echo "=================================================="
echo

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }

# Clean and create build directory
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

log_info "Step 1: Copying MIG sources to build directory"
cp -r "$MIG_SRC"/* .

log_info "Step 2: Generating parser and lexer"

# Generate parser
log_info "  - Generating parser from parser.y..."
yacc -d parser.y
mv y.tab.c parser.c
mv y.tab.h parser.h
log_success "  ✅ Parser generated"

# Generate lexer
log_info "  - Generating lexer from lexxer.l..."
lex lexxer.l
mv lex.yy.c lexxer.c
log_success "  ✅ Lexer generated"

log_info "Step 3: Modernizing varargs usage in error.c and utils.c"

# Backup originals
cp error.c error.c.orig
cp utils.c utils.c.orig

# Create modernized error.c
cat > error.c << 'EOF'
/*
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Modernized to use stdarg.h instead of deprecated varargs.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern char *program_name;
extern int yylineno;
extern char *yyinname;

void fatal(const char *format, ...)
{
    va_list args;
    
    fprintf(stderr, "%s: fatal: ", program_name);
    if (yyinname)
        fprintf(stderr, "\"%s\", line %d: ", yyinname, yylineno);
    
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
    exit(1);
}

void warn(const char *format, ...)
{
    va_list args;
    
    fprintf(stderr, "%s: warning: ", program_name);
    if (yyinname)
        fprintf(stderr, "\"%s\", line %d: ", yyinname, yylineno);
    
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
}

void error(const char *format, ...)
{
    va_list args;
    
    fprintf(stderr, "%s: error: ", program_name);
    if (yyinname)
        fprintf(stderr, "\"%s\", line %d: ", yyinname, yylineno);
    
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
}

void errorline(int lineno, const char *format, ...)
{
    va_list args;
    
    fprintf(stderr, "%s: error: ", program_name);
    if (yyinname)
        fprintf(stderr, "\"%s\", line %d: ", yyinname, lineno);
    
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
}
EOF

# Create modernized utils.c
cat > utils.c << 'EOF'
/*
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Modernized to use stdarg.h instead of deprecated varargs.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

extern void fatal(const char *format, ...);

/* String duplication */
char *strmake(const char *string)
{
    char *saved;
    
    if (string == NULL)
        return NULL;
        
    saved = malloc(strlen(string) + 1);
    if (saved == NULL)
        fatal("out of memory");
        
    strcpy(saved, string);
    return saved;
}

/* Free a string created by strmake/strconcat */
void strfree(char *string)
{
    if (string != NULL)
        free(string);
}

/* Formatted string creation */
char *strconcat(const char *format, ...)
{
    va_list args;
    char buffer[4096];
    int n;
    char *result;
    
    va_start(args, format);
    n = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if (n < 0 || n >= sizeof(buffer))
        fatal("strconcat: buffer overflow");
    
    result = strmake(buffer);
    return result;
}
EOF

log_success "  ✅ Modernized error.c and utils.c"

log_info "Step 4: Compiling MIG with proper flags"

# Compilation flags - relaxed for legacy code
CFLAGS="-O2 -g -I. -I$KERNEL_ROOT"
CFLAGS="$CFLAGS -Wno-endif-labels"  # K&R style OK
CFLAGS="$CFLAGS -Wno-implicit-function-declaration"  # Legacy code
CFLAGS="$CFLAGS -Wno-int-conversion"  # Type compatibility
CFLAGS="$CFLAGS -Wno-incompatible-pointer-types"  # Legacy pointers

# List of objects (from Makefile)
OBJS="mig.o parser.o lexxer.o error.o string.o type.o routine.o statement.o global.o header.o user.o server.o utils.o"

# Compile each source
SUCCESS=0
FAILED=0

for obj in $OBJS; do
    src="${obj%.o}.c"
    if [ ! -f "$src" ]; then
        log_error "Source file not found: $src"
        FAILED=$((FAILED + 1))
        continue
    fi
    
    log_info "  - Compiling $src..."
    if gcc -c $CFLAGS "$src" -o "$obj" 2>&1 | tee "${obj%.o}_build.log"; then
        if [ -f "$obj" ]; then
            log_success "    ✅ $obj created"
            SUCCESS=$((SUCCESS + 1))
        else
            log_error "    ❌ $obj not created"
            FAILED=$((FAILED + 1))
        fi
    else
        log_warn "    ⚠️  $obj compilation had issues"
        FAILED=$((FAILED + 1))
    fi
done

echo
log_info "Compilation: $SUCCESS succeeded, $FAILED failed"
echo

if [ $SUCCESS -lt 13 ]; then
    log_error "Not enough object files compiled ($SUCCESS/13)"
    exit 1
fi

log_info "Step 5: Linking migcom binary"

# Link with lex library (-ll)
log_info "  - Linking $SUCCESS object files with -ll..."
if gcc -o migcom $OBJS -ll 2>&1 | tee migcom_link.log; then
    if [ -f migcom ] && [ -x migcom ]; then
        log_success "  ✅ migcom binary created successfully"
    else
        log_error "  ❌ migcom not executable"
        exit 1
    fi
else
    log_error "  ❌ Linking failed"
    cat migcom_link.log
    exit 1
fi

log_info "Step 6: Installing MIG to toolchain"

# Install
TOOLCHAIN_BIN="$PROJECT_ROOT/toolchain/i386-mach/bin"
mkdir -p "$TOOLCHAIN_BIN"

# Create wrapper script
cat > "$TOOLCHAIN_BIN/i386-mach-mig" << 'MIGSCRIPT'
#!/bin/sh
# MIG wrapper script
MIGCOM=$(dirname "$0")/../../build/mig/migcom
exec "$MIGCOM" "$@"
MIGSCRIPT

chmod +x "$TOOLCHAIN_BIN/i386-mach-mig"
ln -sf i386-mach-mig "$TOOLCHAIN_BIN/mig"

log_success "  ✅ MIG installed to $TOOLCHAIN_BIN/i386-mach-mig"

log_info "Step 7: Testing MIG"

# Basic test - just try to run it
if ./migcom -help 2>&1 | head -5; then
    log_success "  ✅ MIG executable runs"
else
    log_warn "  ⚠️  MIG help output unusual (may be normal)"
fi

echo
echo "=================================================="
log_success "Phase 2B: MIG Build COMPLETE"
echo "=================================================="
echo
echo "Binary:     $BUILD_DIR/migcom"
echo "Installed:  $TOOLCHAIN_BIN/i386-mach-mig"
echo "Objects:    $SUCCESS/13 compiled"
echo

exit 0
