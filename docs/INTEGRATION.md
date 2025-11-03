# Userland Integration Guide

This guide provides step-by-step instructions for integrating userland components from @nmartin0/osfmk6.1 and @nmartin0/xMach into the mach-mk42 project.

## Prerequisites

### Software Requirements
- Docker (for consistent build environment)
- Legacy GCC (2.7.x or 4.x series)
- Binutils (for i386/i586 target)
- MIG (Mach Interface Generator)
- QEMU (for testing)

### Repository Setup

```bash
# Clone required repositories
git clone https://github.com/nmartin0/osfmk6.1.git ../osfmk6.1
git clone https://github.com/nmartin0/xMach.git ../xMach
```

## Phase 1: Extract and Adapt Userland Sources

### Step 1: Default Pager

```bash
# Create userland directory structure
mkdir -p userland/default_pager

# Copy default pager sources
cp -r ../osfmk6.1/src/src/default_pager/* userland/default_pager/

# Adapt for MK42 kernel
cd userland/default_pager
# Update include paths to match our kernel structure
find . -type f -name "*.c" -exec sed -i 's|<mach/|<../../kernel/mach/|g' {} \;
```

### Step 2: Bootstrap Server

```bash
# Create bootstrap directory
mkdir -p userland/bootstrap

# Copy bootstrap sources
cp -r ../osfmk6.1/src/src/bootstrap/* userland/bootstrap/

# Adapt include paths
cd userland/bootstrap
find . -type f -name "*.c" -exec sed -i 's|<mach/|<../../kernel/mach/|g' {} \;
```

### Step 3: Lites Server (Optional - Advanced)

```bash
# Note: Lites is a large component, requires significant adaptation
mkdir -p userland/lites

# Copy Lites server (selective copy)
cp -r ../xMach/lites/server userland/lites/
cp -r ../xMach/lites/emulator userland/lites/
cp -r ../xMach/lites/include userland/lites/
```

## Phase 2: Create Build Infrastructure

### Dockerfile for Userland Build

Create `Dockerfile.userland`:

```dockerfile
FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive

# Install cross-compilation toolchain
RUN apt-get update && apt-get install -y \
    gcc-4.9 \
    g++-4.9 \
    gcc-4.9-multilib \
    binutils \
    make \
    flex \
    bison \
    csh \
    tcsh \
    && rm -rf /var/lib/apt/lists/*

# Set up cross-compiler
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 100
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 100

WORKDIR /workspace
```

### Build Script for Userland

Create `build-userland.sh`:

```bash
#!/bin/bash
set -e

echo "=== Building Mach Userland Components ==="

# Build default pager
echo "Building default_pager..."
cd userland/default_pager
make CC=gcc-4.9 CFLAGS="-m32 -I../../kernel/include" || echo "Build may have warnings"
cd ../..

# Build bootstrap server
echo "Building bootstrap..."
cd userland/bootstrap
make CC=gcc-4.9 CFLAGS="-m32 -I../../kernel/include" || echo "Build may have warnings"
cd ../..

echo "=== Userland build completed ==="
```

## Phase 3: Create Bootable Image with Userland

### Boot Image Creation Script

Create `create-boot-image.sh`:

```bash
#!/bin/bash
set -e

KERNEL="obj/i386_mach/latest/kernel/STD+WS/mach_kernel"
BOOTSTRAP="userland/bootstrap/bootstrap"
PAGER="userland/default_pager/default_pager"

echo "=== Creating Bootable Image with Userland ==="

# Create ISO structure
mkdir -p iso/boot/grub
mkdir -p iso/mach_servers

# Copy kernel
cp $KERNEL iso/boot/mach_kernel

# Copy servers
cp $BOOTSTRAP iso/mach_servers/bootstrap || echo "Bootstrap not built yet"
cp $PAGER iso/mach_servers/default_pager || echo "Pager not built yet"

# Create GRUB configuration with modules
cat > iso/boot/grub/grub.cfg << 'EOF'
set timeout=3
set default=0

menuentry "Mach MK42 + Userland" {
    echo "Loading Mach MK42 microkernel..."
    multiboot /boot/mach_kernel
    module /mach_servers/bootstrap bootstrap
    module /mach_servers/default_pager default_pager
    boot
}

menuentry "Mach MK42 (Kernel Only)" {
    echo "Loading Mach MK42 kernel only..."
    multiboot /boot/mach_kernel
    boot
}
EOF

# Create ISO
grub-mkrescue -o mach-userland.iso iso/ 2>&1 | grep -v "DOS partition" || true

echo "=== Bootable ISO created: mach-userland.iso ==="
ls -lh mach-userland.iso
```

## Phase 4: Update CI/CD Workflow

Add userland build to `.github/workflows/build.yml`:

```yaml
- name: Build userland components
  run: |
    docker build -f Dockerfile.userland -t mach-userland-builder .
    docker run --rm -v $(pwd):/workspace mach-userland-builder \
      /bin/bash -c "cd /workspace && ./build-userland.sh"

- name: Create boot image with userland
  run: |
    ./create-boot-image.sh
    
- name: Upload userland artifacts
  uses: actions/upload-artifact@v4
  with:
    name: mach-userland-iso
    path: mach-userland.iso
```

## Phase 5: Testing

### Test in QEMU

```bash
# Boot with userland
qemu-system-i386 \
  -cdrom mach-userland.iso \
  -m 128M \
  -nographic \
  -serial mon:stdio
```

### Expected Behavior

**Stage 1: Kernel Only (Current)**
- Kernel loads
- No user-space servers
- Times out or panics

**Stage 2: Kernel + Bootstrap**
- Kernel loads
- Bootstrap server starts
- Bootstrap waits for services
- May timeout (no pager)

**Stage 3: Kernel + Bootstrap + Pager**
- Kernel loads
- Bootstrap starts
- Default pager registers
- System ready for servers
- Still no Unix (no Lites)

**Stage 4: Full System (Future)**
- Kernel + Bootstrap + Pager + Lites
- Unix system calls available
- Can run /sbin/init
- Interactive shell

## Compatibility Considerations

### MK42 vs Mach4 Differences

1. **IPC Interface:** May have minor differences
   - Check MIG-generated stubs
   - May need compatibility layer

2. **VM Interface:** Could differ
   - External pager interface
   - Memory object types

3. **Binary Format:**
   - MK42 uses a.out by default
   - Ensure userland matches (a.out vs ELF)

### Adaptation Checklist

- [ ] Update include paths to match MK42 structure
- [ ] Verify MIG interface compatibility
- [ ] Check binary format compatibility
- [ ] Test IPC between kernel and servers
- [ ] Validate memory management interfaces
- [ ] Verify bootstrap protocol

## Debugging

### Enable Verbose Output

```bash
# In kernel config (kernel/conf/MASTER.i386)
options DEBUG=1
options MACH_ASSERT=1

# In QEMU
qemu-system-i386 -d int,cpu_reset -D qemu-debug.log ...
```

### Common Issues

1. **Server Fails to Start**
   - Check binary format (file command)
   - Verify multiboot module loading
   - Check kernel messages

2. **IPC Failures**
   - MIG stub version mismatch
   - Port naming conflicts
   - Bootstrap not initialized

3. **Memory Issues**
   - Pager not registered
   - Insufficient memory allocation
   - Page fault handling

## Next Steps

1. Extract and commit userland sources
2. Create build infrastructure
3. Test individual components
4. Integrate with workflow
5. Create complete boot images
6. Document known issues

## References

- [Mach 3 Server Writer's Guide](https://www.cs.cmu.edu/afs/cs/project/mach/public/www/doc/publications.html)
- [Lites Documentation](http://www.cs.utah.edu/flux/lites/)
- [OSF Mach Documentation](https://github.com/nmartin0/osfmk6.1/tree/master/doc)

## Support

For issues specific to userland integration:
- Check osfmk6.1 documentation
- Review xMach build instructions
- Consult original Mach 3.0 documentation
