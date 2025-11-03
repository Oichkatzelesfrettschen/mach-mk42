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
