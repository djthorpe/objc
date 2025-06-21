# objc

Objective C experiments. Motivation is to see if a very minimal Objective C runtime can be created for ARM and x86, for embedded systems. Would be good to minimally support autorelease pools.

These are some references for future work:

* <https://github.com/mhjacobson/avr-objc>
* <https://github.com/charlieMonroe/libobjc-kern>

Get the ARM toolchain:
https://github.com/ARM-software/LLVM-embedded-toolchain-for-Arm/releases

You than set the cmake to link to the toolchain:

```cmake
set(PICO_COMPILER "pico_arm_clang")
set(PICO_TOOLCHAIN_PATH "/Applications/LLVM-ET-Arm-19.1.5-Darwin-universal/bin")
```

Build testing on Linux/ARM64:

```bash
git clone git@github.com:djthorpe/objc.git
cd objc
git submodule update --init --recursive 
cmake -B build
cmake --build build
```

That's as far as I got so far, more soon!
