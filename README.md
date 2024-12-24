# objc

Objective C experiments. Motivation is to see if a very minimal Objective C runtime can be created for ARM and x86, for embedded systems. Would be good to minimally support autorelease pools.

These are some references for future work:

* <https://github.com/mhjacobson/avr-objc>
* <https://github.com/charlieMonroe/libobjc-kern>

Build:

```bash
git clone git@github.com:djthorpe/objc.git
cd objc
mkdir build
cmake -B build
lldb build/src/test/fuse
```

Seg faults on fprintf, but does get to obj_msgSend, so that's something.
