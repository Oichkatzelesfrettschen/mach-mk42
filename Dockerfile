# Dockerfile for building legacy Mach MK42 kernel
# Uses Ubuntu 18.04 with gcc-7.5 and i386 multilib support
# Includes QEMU for in-container boot testing

FROM ubuntu:18.04

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary build tools and dependencies for i386 cross-compilation
# Also install QEMU and GRUB for kernel boot testing
RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y \
    gcc \
    g++ \
    gcc-multilib \
    g++-multilib \
    binutils \
    make \
    bison \
    flex \
    csh \
    tcsh \
    patch \
    build-essential \
    file \
    bc \
    qemu-system-x86 \
    grub-pc-bin \
    xorriso \
    mtools \
    libc6-dev:i386 \
    libc6-dev \
    && rm -rf /var/lib/apt/lists/*

# Set up working directory
WORKDIR /workspace

# Copy the repository contents
COPY . /workspace

# Set environment variables for i386 build
ENV TARGET_MACHINE=AT386
ENV CONFIG=STD+WS
ENV OBJECTDIR=/workspace/obj/i386_mach/latest/kernel/STD+WS

# Make scripts executable
RUN chmod +x /workspace/*.sh 2>/dev/null || true

# Default command runs the complete test suite
CMD ["/workspace/test-all.sh"]
