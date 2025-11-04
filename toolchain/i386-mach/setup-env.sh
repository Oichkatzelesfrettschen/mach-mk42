# Mach Toolchain Environment Setup
export PATH="/home/runner/work/mach-mk42/mach-mk42/scripts/../toolchain/i386-mach/bin:$PATH"
export MACH_CC="i386-mach-gcc"
export MACH_CXX="i386-mach-g++"
export MACH_AS="i386-mach-as"
export MACH_LD="i386-mach-ld"
export MACH_AR="i386-mach-ar"
export MACH_TARGET="i386-mach"

echo "Mach toolchain environment configured"
echo "  CC:  $MACH_CC"
echo "  CXX: $MACH_CXX"
echo "  Target: $MACH_TARGET"
