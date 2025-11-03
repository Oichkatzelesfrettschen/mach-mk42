# Component Inventory

## Status: Initial Setup

### Kernel
- [x] MK42 (current) - i386 a.out format
- [ ] OSFMK 7.3 (target) - evaluation pending
- [ ] MK74 (alternative) - evaluation pending

### Userland Servers
- [ ] Bootstrap server - not extracted
- [ ] Default pager - not extracted
- [ ] Network message server - not extracted
- [ ] Mach ID server - not extracted
- [ ] Lites server - not extracted

### Toolchain
- [x] GCC 7.5 (current) - for basic builds
- [ ] Mach-patched-gcc - not built
- [ ] MIG - need to extract and build

### Root Filesystem
- [ ] Init system - not implemented
- [ ] Shell - not cross-compiled
- [ ] Basic utilities - not available
- [ ] Libraries - not built

## Next Actions
1. Run scripts/extract-userland.sh
2. Evaluate kernel versions
3. Build Mach-patched-gcc toolchain
4. Extract and build servers

Updated: $(date)
