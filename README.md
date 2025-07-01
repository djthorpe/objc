# objc runtime

Objective C experiments. Motivation is to see if a very minimal Objective C runtime can be created for ARM and
x86, mostly for embedded systems. Would be good to minimally support autorelease pools.

These are some references for future work:

* <https://github.com/mhjacobson/avr-objc>
* <https://github.com/charlieMonroe/libobjc-kern>

For embedded systems, get the ARM LLVM toolchain:\
<https://github.com/ARM-software/LLVM-embedded-toolchain-for-Arm/releases>

Install this to the `/opt` directory.

```bash
git clone git@github.com:djthorpe/objc.git
cd objc
```

the idea is this will work with both the `clang` and `gcc` compilers. To compile for MacOS, you'll need the `gcc` 
compiler rather than `clang`,

```bash
# Compile with GCC 15 for MacOS and run tests
brew install gcc@15
TOOLCHAIN_PATH=/opt/homebrew CC=gcc-15 make test
```

You can create a static library for the Objective C runtime - say for Pico:

```bash
# Compile for the RP2040 Pico board
CC=clang TARGET=armv6m-none-eabi TOOLCHAIN_PATH=/opt/LLVM-ET-Arm-19.1.5-Darwin-universal make 
```

## Object

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

## NXConstantString

The `NXConstantString` class is used to represent constant strings in Objective C. It is a subclass of `Object` and provides methods for creating and comparing strings.

```objc
@interface NXConstantString : Object {
    const char* data;
    unsigned int length;
}

// Lifecycle
+(id) withCString:(const char* )cStr;

// Methods
-(const char*) cStr;
-(unsigned int) length;
@end
```

## NXLog

The `NXLog` function is used to log messages to the console. Any message is terminated with a newline character, and formatting is currently identical to `printf`.

```objc
void main() {
   NXLog(@"Hello, %s", "Objective C!");
}
```

## NXZone

Represents a memory arena for allocating and deallocating objects. It is used to manage memory for
Objective C objects.

```objc
@interface NXZone : NXObject {
    // ...Private data members for the zone
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

## NXString

Represents a mutable string in Objective C.

```objc
@interface NXString : NXConstantString {
    unsigned int hash;
}

// Lifecycle
+(id) copy:(NXString* )string;

// Methods
-(NXStringCompare) compare:(NXString* )other;
-(unsigned int) hash; // Returns a non-zero hash value for the string

@end
```

## Current status

* [X] Registering classes
* [X] Simple message calling
* [X] `NXConstantString` support
* [X] Resolving super classes and meta classes for message lookup
* [X] Calling methods in super classes - implement `[super init]` for example
* [X] Calling methods in categories
* [ ] `respondsToSelector:` and `+[Class load]`
* [ ] More efficient method implementation lookup
* [ ] Memory management - alloc, dealloc, memory arenas - require malloc in an `NXZone`
* [ ] Memory management - retain, release - reference counting for objects through NXZone
* [ ] String - `NXString` - mutable and immutable strings
* [ ] Array and Map - `NXArray` and `NXMap` - mutable and immutable arrays and maps
* [ ] Application and Runloops - `NXApplication` and `NXRunLoop` 
* [ ] Pico toolchain - integrate with Pico SDK
* [ ] Protocols and `conformsToProtocol:`
* [ ] Threading support
* [ ] `@synchronized` support
* [ ] Exception handling?
