# objc runtime

This is a minimal Objective C runtime written in C, designed to be portable across different platforms, including ARM and x86 architectures, mostly targeting embedded systems. It uses the "gcc" ABI for the moment, as that is the most portable across different platforms.

- [View the list of all classes](https://djthorpe.github.io/objc/annotated.html)
- [View the list of all files](https://djthorpe.github.io/objc/files.html)
- [View the list of all functions](https://djthorpe.github.io/objc/globals_func.html)

## Requirements

You will minimally need the following tools to build the runtime:

- **Build system** `make` and `cmake` - for the build system
- **Compiler** `clang` or `gcc` - for compiling the runtime (`clang` is not supported on Apple Silicon). You can use the environment variable `CC` to specify the compiler, e.g. `CC=clang` or `CC=gcc`.
- **Library Dependencies** `openssl` for Linux and Darwin - for the hash functions, which are used in the runtime and Foundation framework.
- **Documentation** `docker` is needed for generating the documentation from the source code.
- **Cross-Compilation** For cross-compilation for embedded systems based on some ARM variant, get the ARM LLVM toolchain: <https://github.com/ARM-software/LLVM-embedded-toolchain-for-Arm/releases>. Install this to the `/opt` directory. You can use the environment variable `TOOLCHAIN_PATH` to specify the path to the toolchain, e.g. `TOOLCHAIN_PATH=/opt/LLVM-ET-Arm-19.1.5-Darwin-universal`.

## Building the libraries

Several static libraries are currently built:

- `objc-gcc` - the Objective C runtime library using the ancient GCC ABI
- `runtime-sys` - a minimal set of system functions, needed to bind the runtime to the underlying system, on a per-platform basis. Includes cryptographic hash functions (MD5, SHA-256).
- `runtime-hw` - hardware interfaces like SPI, I2C, GPIO. These are stubs for Linux and Darwin, but I guess some of these interfaces couple be implemented later.
- `runtime-fs` - organizing files, directories and volumes which can either be volatile (in-memory) or non-volatile (file-based or in flash)
- `drivers` and `runtime-pix` have a long way to go but should provide hardware driver and display support eventually.
- `Foundation` - a minimal set of classes, to support memory management and basic types such as string, array, dictionary, date, time and number.
- `Application` - the application framework, which provides a runloop and handles input, display, power and sensors.

A lot of this code is in development still. See the "examples" or documentation on what's been implemented so far.

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

Once you've made the libraries, use the `make tests` target to run the unit tests. There is information about test coverage in the [tests directory](src/tests/README.md).

### Running Tests

On macOS, build and run all tests:

```bash
TOOLCHAIN_PATH=/opt/homebrew CC=gcc-15 RELEASE=1 make tests
```

Run an individual test target (replace `<test_target>` with e.g. `fs_09`):

```bash
cmake --build build --target <test_target>
ctest --output-on-failure --test-dir build --tests-regex <test_target>
```

See the [tests README](src/tests/README.md) for a catalog of tests (fs_01..fs_09 and others) and details.

You can target different architectures by setting the `TARGET` environment variable. For a RP2040-based board, you can use the `clang` compiler with the ARM toolchain. The `TARGET` environment variable should be set to the target architecture, such as `armv6m-none-eabi` for the RP2040 Pico board:

```bash
# Compile for the RP2040 Pico board
CC=clang TARGET=armv6m-none-eabi TOOLCHAIN_PATH=/opt/LLVM-ET-Arm-19.1.5-Darwin-universal RELEASE=1 make 
```

See the list of supported targets in the [cmake](https://github.com/djthorpe/objc/tree/main/cmake) directory.
You can exclude the environment variable `RELEASE=1` to build debugging versions of the libraries.

For Pico cross-compilation, after building, you can load a specific test onto a Pico W board (example for `<test_target>`):

```bash
cmake --build build --target <test_target> && picotool load -x build/src/tests/<test_target>/<test_target>.uf2
```

For Linux cross-compilation using Docker (Bookworm):

```bash
docker run --rm -i -v $(pwd):/root bookworm-builder bash -c "cd /root && make clean && CC=gcc make"
docker run --rm -i -v $(pwd):/root bookworm-builder bash -c "ctest --output-on-failure --test-dir build --tests-regex <test_target>"
```

## Installing the libraries

The libraries should be installed under a prefix path:

```bash
PREFIX=/opt/picofuse make install
```

- /opt/objc/lib/pkgconfig/picofuse-linux-arm64.pc
- /opt/objc/lib/linux-arm64/libobjc-gcc.a
- /opt/objc/lib/linux-arm64/libruntime-sys.a
- /opt/objc/lib/linux-arm64/libFoundation.a
- /opt/objc/include/objc
- /opt/objc/include/sys
- /opt/objc/include/Foundation
- /opt/objc/bin/...

These can subsequently be used in projects to make executables for any combination of platform.

## Documentation

To build the API documentation, you will need to have `docker` installed. The documentation is built using `doxygen` through `docker` so you don't need to have it installed directly.

```bash
make docs
open docs/index.html 
```

The documentation is also published on the [project documentation site](https://djthorpe.github.io/objc/).

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
- [X] Protocols and `conformsTo:`
- [X] Number - `NXNumber` with `NXNumberInt16` and `NXNumberUnsignedInt16`
- [X] Number - `NXNumber` with `NXNumberInt32` and `NXNumberUnsignedInt32`
- [X] Number - `NXNumber` with `NXNumberInt64` and `NXNumberUnsignedInt64`
- [X] Fix linking categories in static libraries (see test NXFoundation_05)
- [X] `NXString` - mutable strings - append, appendFormat
- [X] `NXArray` - ordered collections of objects, with methods for adding, removing, and accessing objects
- [X] `NXData` - mutable binary data with Base64/hex encoding and append operations
- [X] Hash functions - MD5 and SHA-256 hash computation through `sys_hash_*` API
- [X] Pico toolchain - integrate with Pico SDK
- [X] printf - `%@` format specifier for logging objects and `[Object description]`
- [X] printf - `%T` format specifier for time intervals
- [X] `NXArray` - string methods - join
- [X] Robust sys_hash functions which hash some void* data against an arbitrary key, and a hashing function that can be used
- [X] `NXMap` - unordered collections with string-keys (what about `NXDictionary`?)
- [X] `NXMap` - arbitary key sizes
- [X] clang compatibility
- [X] runtime-hw SPI support
- [X] runtime-hw watchdog support
- [X] runtime-hw PWM support - make tying to a GPIO pin a function, focus on slices
- [X] runtime-hw Power support - callbacks for low battery, battery level, power on/off, etc.
- [X] runtime-hw Power support - measure voltage on VSYS to get %age and also whether connected to USB or battery
- [X] `GPIO` uses static instances, and `<GPIODelegate>` protocol to handle GPIO events which are pushed from the runloop
- [ ] mqtt make it asynchronous for publish, subscribe, unsubscribe and new message
- [ ] MQTT objective-c client
- [ ] runtime-hw LED support - blink and fade re-work
- [ ] `NXInputManager` - input manager for handling keyboard, mouse, GPIO and other input devices with single click, double click and so forth.
- [ ] Wire GPIO into NXInputManager
- [ ] `NXApplication` and `NXRunLoop` running on both cores
- [ ] Shared instances need to be finalized on exit
- [ ] `Power` class and `<PowerDelegate>` for managing power, resets, uptime, etc.
- [ ] `@synchronized` support - use fixed-size table to store locks for objects, no allocations
- [ ] NXNumber - `NXNumberUnsignedInt8` and `NXNumberInt8`
- [ ] Make all NX classes thread-safe, so that they can be used in multi-threaded applications
- [ ] `NXScanner`, `ReaderProtocol` - scanning, parsing and tokenizing - needs unicode work
- [ ] `NXData` - fix hexString encoding and decoding (see test NXFoundation_22)?
- [ ] `NXArray` sortWithFunction: sortedArrayWithFunction: reverse: and reversedArray:
- [ ] `NXArray` filterWithFunction: filteredArrayWithFunction:
- [ ] Pico - timer alarm pool should be on both cores, not just core 0, and then use the right pool for the core that the timer is running on
- [ ] Pico - when building RELEASE=1 builds it includes stdout and printf, which is not needed. Same for cyw43 and lwip.
- [ ] `NXRange` and `NXString` - substringWithRange and substringWithRange:options:
- [ ] `NXString` - rangeOfSubstring and rangeOfSubstring:options:
- [ ] `NXArray` - `subarrayWithRange:` and `subarrayWithRange:options:`
- [ ] `NXString` - array methods - componentsSeparatedByString and componentsSeparatedByByte
- [ ] Number - `NXNumber` with `NXNumberFloat` and `NXNumberDouble`
- [ ] printf - `%f and %lf` format specifier for floats and doubles
- [ ] More efficient method implementation lookup
- [ ] `make install` will compile the libraries and install them to a prefix path - third-party repositories can compile against it
- [ ] `respondsToSelector:` (see test `runtime_14`) and lots of tests - might be working?
- [ ] USB Support for keyboards & mice and input manager - some way of knowing when USB device inserted and removed
- [ ] Touchscreen support for Pimironi device
- [ ] Remote decoding of IR protocols and wire into NXInputManager
- [ ] runtime-pix with SDL devices to start with
- [ ] runtime-pix with eInk devices
- [ ] runtime-pix with LED devices
- [ ] runtime-pix with Pimironi device
- [ ] runtime-pix vector drawing
- [ ] runtime-pix 7x5 fonts
- [ ] runtime-pix vector fonts
- [ ] runtime-fs Low-level flash block device
- [ ] runtime-fs Low-level SD card block device
- [ ] runtime-fs littlefs filesystem - register block devices, format, etc.
- [ ] `NXSensorManager` - sensor manager for handling sensors such as accelerometer, gyroscope, magnetometer, temperature, humidity, pressure, etc.
- [ ] `NXWirelessManager` - keeps track of wireless networks, connections, disconnections
- [ ] runtime-pix load/save fonts and images
- [ ] low-level http client
- [ ] `NXURL` class - URL/filepath parsing and manipulation
- [ ] `NXFile` - represents an open file
- [ ] `NXFileManager` - manages files and directories on a volume
- [ ] `NXFileDirectory` - represents a directory on a volume
- [ ] Classes have a unique number so we can do NXCoder to serialize and deserialize objects from binary data?
- [ ] `NXCoder` - JSON / Binary marshalling and unmarshalling
- [ ] Calling `+[initialise]` for categories
- [ ] Exception handling?
- [ ] Raspberry Pi support - GPIO, SPI, I2C, PWM, ADC, etc.

## References

Here are some references that may be useful for understanding the Objective C runtime and its implementation:

- <https://github.com/mhjacobson/avr-objc>
- <https://github.com/charlieMonroe/libobjc-kern>
- <https://github.com/gnustep/libobjc>
