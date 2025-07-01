# objc runtime

This is a minimal Objective C runtime written in C, designed to be portable across different platforms,
including ARM and x86 architectures, mostly targeting embedded systems. It uses the "gcc" ABI for the
moment, as that is the most portable across different platforms. You will minimally need the
following tools to build the runtime:

* `make` and `cmake` - for the build system
* `clang` or `gcc` - for compiling the runtime (`clang` is not supported on Apple Silicon). If you want
  to cross-compile for ARM, you will need the ARM LLVM toolchain (see below).

## ARM Toolchain

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
# Compile with GCC 15 for MacOS and run tests
brew install gcc@15
TOOLCHAIN_PATH=/opt/homebrew CC=gcc-15 make test
```

For the RP2040 Pico board, you can use the `clang` compiler with the ARM toolchain. The `TARGET` environment variable should be set to the target architecture, such as `armv6m-none-eabi` for the RP2040 Pico board:

```bash
# Compile for the RP2040 Pico board
CC=clang TARGET=armv6m-none-eabi TOOLCHAIN_PATH=/opt/LLVM-ET-Arm-19.1.5-Darwin-universal make 
```

## Class and Method Reference

### Object

The `Object` class is the root class of the Objective C runtime:

```objc
@interface Object {
    Class isa;
}

+(id) alloc; // Allocate an instance of the class
-(void) dealloc; // Dispose of the instance
-(id) init; // Initialize the instance
-(Class) class; // Returns the class of the instance
+(Class) class; // Returns the class (itself)
-(BOOL) isEqual:(id)anObject; // Returns YES if the instance is equal to another object
@end
```

### NXConstantString

The `NXConstantString` class is used to represent constant strings in Objective C. It is a subclass of `Object` and provides methods for creating and comparing strings.

```objc
@interface NXConstantString : Object {
    // ...
}

// Lifecycle
+(id) withCString:(const char* )cStr;

// Methods
-(const char*) cStr;
-(unsigned int) length;
@end
```

### NXLog

The `NXLog` function is used to log messages to the console. Any message is terminated with a newline character, and formatting is currently identical to `printf`.

```objc
void main() {
   NXLog(@"Hello, %s", "Objective C!");
}
```

### NXZone (TODO)

Represents a memory arena for allocating and deallocating objects. It is used to manage memory for
Objective C objects.

```objc
@interface NXZone : NXObject {
    // ...
}

// Lifecycle
+(id) defaultZone; // Returns the default zone for the application
-(id) initWithSize:(size_t)size; // Creates a new zone with the specified size
-(void) dealloc; // Deallocates the zone

// Methods
-(void* ) alloc:(size_t)size; // Allocates memory of the specified size
-(void) free:(void* )ptr; // Frees the allocated memory
-(void* ) realloc:(void* )ptr size:(size_t)size; // Reallocates memory to the specified size and returns the new pointer, or NULL if the reallocation fails
-(void) reset; // Resets the zone, freeing all allocated memory

@end
```

### NXString (TODO)

Represents a mutable string in Objective C.

```objc
@interface NXString : NXConstantString {
    // ...
}

// Lifecycle
+(id) copy:(NXString* )string;

// Methods
-(NXStringCompare) compare:(NXString* )other;
-(unsigned int) hash; // Returns a non-zero hash value for the string

@end
```

### NXNumber (TODO)

### NXArray (TODO)

### NXDictionary (TODO)

## Current status

* [X] Registering classes
* [X] Simple message calling
* [X] `NXConstantString`
* [X] Resolving super classes and meta classes for message lookup
* [X] Calling methods in super classes - implement `[super init]` for example
* [X] Calling methods in categories
* [ ] clang compatibility
* [ ] `respondsToSelector:` and `+[Class load]`
* [ ] More efficient method implementation lookup
* [ ] Memory management - alloc, dealloc, memory arenas - require malloc in an `NXZone`
* [ ] Memory management - retain, release - reference counting for objects through NXZone
* [ ] String - `NXString` - mutable strings
* [ ] Array and Map - `NXArray` and `NXDictionary`
* [ ] `NXApplication` and `NXRunLoop`
* [ ] Pico toolchain - integrate with Pico SDK
* [ ] Protocols and `conformsToProtocol:`
* [ ] Threading & `@synchronized` support
* [ ] Exception handling?

## References

Here are some references that may be useful for understanding the Objective C runtime and its implementation:

* <https://github.com/mhjacobson/avr-objc>
* <https://github.com/charlieMonroe/libobjc-kern>
