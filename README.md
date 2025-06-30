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

-(const char*) cStr;
-(unsigned int) length;
@end
```

## NSLog

The `NSLog` function is used to log messages to the console. Any message is terminated with a newline character, and formatting is currently identical to `printf`.

```objc
void main() {
   NSLog(@"Hello, %s", "Objective C!");
}
```

## Current status

* [X] Registering classes
* [X] Simple message calling
* [X] `NXConstantString` support
* [X] Resolving super classes and meta classes for message lookup
* [ ] Calling methods in super classes - implement `[super init]` for example
* [ ] Calling methods in categories
* [ ] `respondsToSelector:`
* [ ] More efficient method implementation lookup
* [ ] Memory management - alloc, dealloc, memory arenas - require malloc in an `NXMemoryZone`
* [ ] Memory management - retain, release - reference counting for objects
* [ ] String - `NXString` - mutable and immutable strings
* [ ] Array and Map - `NXArray` and `NXMap` - mutable and immutable arrays and maps
* [ ] Application and Runloops - `NXApplication` and `NXRunLoop` 
* [ ] Pico toolchain - integrate with Pico SDK
* [ ] Protocols and `conformsToProtocol:`
* [ ] Threading support
* [ ] `@synchronized` support
* [ ] Exception handling?
