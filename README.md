# objc

Objective C experiments. Motivation is to see if a very minimal Objective C runtime can be created for ARM and x86, for embedded systems. Would be good to minimally support autorelease pools.

These are some references for future work:

* <https://github.com/mhjacobson/avr-objc>
* <https://github.com/charlieMonroe/libobjc-kern>

Get toolchain:
https://github.com/ARM-software/LLVM-embedded-toolchain-for-Arm/releases

Build testing on Linux/ARM64:

```bash
brew install --cask gcc-arm-embedded
git clone git@github.com:djthorpe/objc.git
cd objc
git submodule update --init --recursive 
mkdir build
cmake -B build
cmake --build build
```

We are getting segfault, probably because we are not actually calling the alloc method yet...
