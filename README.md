# objc on Raspberry Pi and Pico boards

Objective C experiments. Motivation is to see if a very minimal Objective C runtime can be created for ARM and 
x86, for embedded systems. Would be good to minimally support autorelease pools.

These are some references for future work:

* <https://github.com/mhjacobson/avr-objc>
* <https://github.com/charlieMonroe/libobjc-kern>

Get the ARM LLVM toolchain for Pico:\
<https://github.com/ARM-software/LLVM-embedded-toolchain-for-Arm/releases>

Install this to the `/opt` directory. Checkout all the code and submodules with:

```bash
git clone git@github.com:djthorpe/objc.git
cd objc
make submodule
```

Then you have three options for compiling:

1. Pico target

```bash
LLVM_TOOLCHAIN_PATH=/opt/llvm_toolchain_path PICO_BOARD=pico make # Pico (or other board)
picotool load -x build/src/test/test.uf2 # Load the test program onto the Pico
```

Please see list of possible boards [here](https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards)

2. Raspberry Pi target (or other ARM64 Linux target)

```bash
PICO_BOARD=arm64-linux-gnu make 
./build/src/test/test
```

3. Intel Linux target

```bash
PICO_BOARD=amd64-linux-gnu make
./build/src/test/test
```

In each case, the output is currently as follows:

```text
test
__objc_class_init
   Category AnotherHack
__objc_class_register Test
   Category AnotherHack
__objc_class_register Object
   Category AnotherHack
objc_lookup_class Test
objc_msg_lookup: 0x5567ac02a8 0x5567ac0108
Segmentation fault
```

The segmentation fault is because the `objc_msg_lookup` function is not implemented yet (it's calling `[Test run]`).
Next steps:

* Correctly implement the `objc_lookup_class` and `objc_msg_lookup` so it uses a sparse array in both cases.

That's as far as I got so far, more soon!
