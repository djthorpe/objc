# objc runtime

This is a minimal Objective C runtime written in C, designed to be portable across different platforms, including ARM and x86 architectures, mostly targeting embedded systems. It uses the "gcc" ABI for the moment, as that is the most portable across different platforms.

- [View the list of all classes](https://djthorpe.github.io/objc/annotated.html)
- [View the list of all files](https://djthorpe.github.io/objc/files.html)
- [View the list of all functions](https://djthorpe.github.io/objc/globals_func.html)

## Requirements

You will minimally need the following tools to build the runtime:

- **Build system** `make` and `cmake` - for the build system
- **Compiler** `clang` or `gcc` - for compiling the runtime (`clang` is not supported on Apple Silicon). You can use the environment variable `CC` to specify the compiler, e.g. `CC=clang` or `CC=gcc`.
- **Documentation** `docker` is needed for generating the documentation from the source code.
- **Cross-Compilation** For cross-compilation for embedded systems based on some ARM variant, get the ARM LLVM toolchain: <https://github.com/ARM-software/LLVM-embedded-toolchain-for-Arm/releases>. Install this to the `/opt` directory. You can use the environment variable `TOOLCHAIN_PATH` to specify the path to the toolchain, e.g. `TOOLCHAIN_PATH=/opt/LLVM-ET-Arm-19.1.5-Darwin-universal`.

## Building the libraries

Three static libraries are currently provided:

- `objc-gcc` - the Objective C runtime library using the ancient GCC ABI
- `NXFoundation` - a minimal set of classes, to support memory management and basic types such as string, array, dictionary, date, time and number.
- `sys` - a minimal set of system functions, needed to bind the runtime to the underlying system, on a per-platform basis.

Download the source code from GitHub:

```bash
git clone git@github.com:djthorpe/objc.git
cd objc
```

The `TOOLCHAIN_PATH` environment variable should point to the directory where the toolchain is installed.
For Macintosh, you can use Homebrew to install the GCC toolchain, which is required for compiling the runtime on MacOS:

```bash
# Compile with GCC 15 for MacOS
brew install gcc@15
TOOLCHAIN_PATH=/opt/homebrew RELEASE=1 CC=gcc-15 make
```

Once you've made the libraries, use the `make tests` target to run the unit tests.

You can target different architectures by setting the `TARGET` environment variable. For a RP2040-based board, you can use the `clang` compiler with the ARM toolchain. The `TARGET` environment variable should be set to the target architecture, such as `armv6m-none-eabi` for the RP2040 Pico board:

```bash
# Compile for the RP2040 Pico board
CC=clang TARGET=armv6m-none-eabi TOOLCHAIN_PATH=/opt/LLVM-ET-Arm-19.1.5-Darwin-universal RELEASE=1 make 
```

See the list of supported targets in the [cmake](https://github.com/djthorpe/objc/tree/main/cmake) directory.
You can exclude the environment variable `RELEASE=1` to build debugging versions of the libraries.

## Installing the libraries

TODO: the libraries should be installed under a prefix path:

```bash
PREFIX=/opt/objc make install
```

- /opt/objc/lib/armv6m-none-eabi/libobjc-gcc.a
- /opt/objc/lib/armv6m-none-eabi/libsys.a
- /opt/objc/lib/armv6m-none-eabi/libFoundation.a
- /opt/objc/include/objc
- /opt/objc/include/sys
- /opt/objc/include/NXFoundation
- /opt/objc/doc/...

These can subsequently be used to make your executables!

## Documentation

To build the API documentation, you will need to have `docker` installed. The documentation is built using `doxygen` through `docker` so you don't need to have it installed directly.

```bash
make docs
open docs/index.html 
```

The documentation is also published [here](https://djthorpe.github.io/objc/).

## Current status

- [X] Registering classes
- [X] Simple message calling
- [X] `NXConstantString`
- [X] Resolving super classes and meta classes for message lookup
- [X] Calling methods in super classes - implement `[super init]` for example
- [X] Calling methods in categories
- [X] Memory management - alloc, dealloc, memory arenas - require malloc in an `NXZone`
- [X] Printf support - `vsprintf` - printf-like function which can be used by NXLog, NXString and sys_panicf
- [X] Memory management - retain, release - reference counting for objects through NXZone
- [X] Date and Time - `NXDate` - mutable date and time
- [X] `NXNumber` with booleans
- [X] `NXNull` - singleton for null objects that can be inserted into collections
- [ ] Number - `NXNumber` with `NXNumberInt16` and `NXNumberUnsignedInt16`
- [ ] Number - `NXNumber` with `NXNumberInt32` and `NXNumberUnsignedInt32`
- [ ] Number - `NXNumber` with `NXNumberInt64` and `NXNumberUnsignedInt64`
- [ ] Number - `NXNumber` with `NXNumberFloat` and `NXNumberDouble`
- [ ] `NXLog` and `NXString stringWithFormat:` support - `%@` format specifier for logging objects and `[Object description]`
- [ ] `NXString` - mutable strings - append, appendFormat, substring, split, join, etc.
- [ ] `NXArray` - ordered collections of objects, with methods for adding, removing, and accessing objects
- [ ] `NXMap` - unordered collections with string-keys (what about `NXDictionary`?)
- [ ] `NXData` - mutable data creating hashes
- [ ] `make install` will compile the libraries and install them to a prefix path
- [ ] Fix linking categories in static libraries (see test NXFoundation_05)
- [ ] clang compatibility (probably fix with selectors)
- [ ] `respondsToSelector:` (see test `runtime_14`)
- [ ] More efficient method implementation lookup
- [ ] `NXCoder` - JSON / Binary marshalling and unmarshalling
- [ ] `NXScanner` - scanning and tokenizing
- [ ] Calling `+[initialise]` for categories
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
