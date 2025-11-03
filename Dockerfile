# Dockerfile for building legacy Mach MK42 kernel
# Uses Debian Buster with gcc-4.9 for i386 architecture

FROM debian:buster

# Install necessary build tools and dependencies for i386 cross-compilation
RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y \
    gcc-4.9 \
    g++-4.9 \
    binutils \
    make \
    bison \
    flex \
    csh \
    tcsh \
    patch \
    build-essential \
    libc6-dev:i386 \
    libgcc-8-dev:i386 \
    && rm -rf /var/lib/apt/lists/*

# Set gcc-4.9 as the default gcc
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 100

# Set up working directory
WORKDIR /workspace

# Copy the repository contents
COPY . /workspace

# Set environment variables for i386 build
ENV TARGET_MACHINE=AT386
ENV CONFIG=STD+WS
ENV OBJECTDIR=/workspace/obj/i386_mach/latest/kernel/STD+WS

# Default command
CMD ["/bin/bash"]
