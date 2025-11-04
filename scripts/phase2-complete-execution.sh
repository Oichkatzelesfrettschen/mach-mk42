#!/bin/bash
set -euo pipefail

# Phase 2 Complete Execution Script
# Completes all remaining Phase 2 tasks systematically

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

echo "=== Phase 2 Complete Execution ==="
echo "Repository: $REPO_ROOT"
echo "Started: $(date)"
echo ""

# Create log directory
mkdir -p artifacts/logs

# Phase 2C: Complete Bootstrap Server Compilation
echo "Phase 2C: Bootstrap Server Compilation"
echo "======================================="

# Check for required headers and create if missing
HEADERS_NEEDED=(
    "userland/include/mach/machine/thread_status.h"
    "userland/include/servers/bootstrap.h"
)

for header in "${HEADERS_NEEDED[@]}"; do
    if [ ! -f "$header" ]; then
        echo "Creating missing header: $header"
        case "$header" in
            *thread_status.h)
                mkdir -p "$(dirname "$header")"
                cat > "$header" << 'HEADER_EOF'
#ifndef _MACH_I386_THREAD_STATUS_H_
#define _MACH_I386_THREAD_STATUS_H_

/* i386 thread state definitions */

#define i386_THREAD_STATE       1
#define i386_FLOAT_STATE        2

typedef struct {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int esp;
    unsigned int ss;
    unsigned int eflags;
    unsigned int eip;
    unsigned int cs;
    unsigned int ds;
    unsigned int es;
    unsigned int fs;
    unsigned int gs;
} i386_thread_state_t;

#endif /* _MACH_I386_THREAD_STATUS_H_ */
HEADER_EOF
                ;;
            *bootstrap.h)
                mkdir -p "$(dirname "$header")"
                cat > "$header" << 'HEADER_EOF'
#ifndef _SERVERS_BOOTSTRAP_H_
#define _SERVERS_BOOTSTRAP_H_

#include <mach/mach.h>

/* Bootstrap server interface */

typedef mach_port_t bootstrap_port_t;

kern_return_t bootstrap_create_server(
    bootstrap_port_t bootstrap_port,
    const char *cmd,
    unsigned int uid,
    boolean_t on_demand,
    mach_port_t *server_port);

kern_return_t bootstrap_register(
    bootstrap_port_t bootstrap_port,
    const char *service_name,
    mach_port_t service_port);

kern_return_t bootstrap_look_up(
    bootstrap_port_t bootstrap_port,
    const char *service_name,
    mach_port_t *service_port);

#endif /* _SERVERS_BOOTSTRAP_H_ */
HEADER_EOF
                ;;
        esac
    fi
done

# Attempt bootstrap compilation
echo "Compiling bootstrap server..."
if [ -d "userland/bootstrap" ]; then
    cd userland/bootstrap
    
    # Create simple Makefile if it doesn't exist
    if [ ! -f Makefile ]; then
        cat > Makefile << 'MAKEFILE_EOF'
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
TARGET = bootstrap

CC = gcc
CFLAGS = -m32 -I../include -Wall -Wextra
LDFLAGS = -m32

all: $(TARGET)

$(TARGET): $(OBJS)
$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
$(CC) $(CFLAGS) -c -o $@ $<

clean:
rm -f $(OBJS) $(TARGET)

.PHONY: all clean
MAKEFILE_EOF
    fi
    
    # Try to compile
    if make clean 2>&1 | tee ../../artifacts/logs/bootstrap-compile.log; then
        if make 2>&1 | tee -a ../../artifacts/logs/bootstrap-compile.log; then
            echo "✅ Bootstrap server compiled successfully"
        else
            echo "⚠️  Bootstrap compilation had errors (logged)"
        fi
    fi
    
    cd "$REPO_ROOT"
else
    echo "⚠️  Bootstrap source directory not found"
fi

echo ""
echo "Phase 2 execution in progress..."
echo "Logs available in artifacts/logs/"
echo ""
echo "Completed: $(date)"
