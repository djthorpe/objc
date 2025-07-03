# objc runtime

This is a minimal Objective C runtime written in C, designed to be portable across different platforms,
including ARM and x86 architectures, mostly targeting embedded systems. It uses the "gcc" ABI for the
moment, as that is the most portable across different platforms.

- [View the list of all classes](annotated.html)
- [View the list of all files](files.html)
- [View the list of all functions](globals_func.html)

## Requirements

You will minimally need the following tools to build the runtime:

- `make` and `cmake` - for the build system
- `clang` or `gcc` - for compiling the runtime (`clang` is not supported on Apple Silicon). If you want
  to cross-compile for ARM, you will need the ARM LLVM toolchain (see below).

### ARM Toolchain

For cross-compilation for embedded systems based on some ARM variant, get the ARM LLVM toolchain:\
<https://github.com/ARM-software/LLVM-embedded-toolchain-for-Arm/releases>

Install this to the `/opt` directory.

## Building the runtime

Download the source code from GitHub:

```bash
git clone git@github.com:djthorpe/objc.git
cd objc
```

The `TOOLCHAIN_PATH` environment variable should point to the directory where the toolchain is installed. For example,

```bash
# Compile with GCC 15 for MacOS
brew install gcc@15
TOOLCHAIN_PATH=/opt/homebrew CC=gcc-15 make
```

Use the `make tests` target to run the unit .tests

For the RP2040 Pico board, you can use the `clang` compiler with the ARM toolchain. The `TARGET` environment variable should be set to the target architecture, such as `armv6m-none-eabi` for the RP2040 Pico board:

```bash
# Compile for the RP2040 Pico board
CC=clang TARGET=armv6m-none-eabi TOOLCHAIN_PATH=/opt/LLVM-ET-Arm-19.1.5-Darwin-universal make 
```

## Current status

- [X] Registering classes
- [X] Simple message calling
- [X] `NXConstantString`
- [X] Resolving super classes and meta classes for message lookup
- [X] Calling methods in super classes - implement `[super init]` for example
- [X] Calling methods in categories
- [ ] clang compatibility
- [ ] `respondsToSelector:` and `+[Class load]`
- [ ] More efficient method implementation lookup
- [ ] Memory management - alloc, dealloc, memory arenas - require malloc in an `NXZone`
- [ ] Memory management - retain, release - reference counting for objects through NXZone
- [ ] String - `NXString` - mutable strings
- [ ] Array and Map - `NXArray` and `NXDictionary`
- [ ] `NXApplication` and `NXRunLoop`
- [ ] Pico toolchain - integrate with Pico SDK
- [ ] Protocols and `conformsToProtocol:`
- [ ] Threading & `@synchronized` support
- [ ] Exception handling?

## References

Here are some references that may be useful for understanding the Objective C runtime and its implementation:

- <https://github.com/mhjacobson/avr-objc>
- <https://github.com/charlieMonroe/libobjc-kern>
- <https://github.com/gnustep/libobjc>
