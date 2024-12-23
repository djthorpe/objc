# objc

Objective C experiments. Motivation is to see if a very minimal Objective C runtime can be created for ARM and x86, for embedded systems. Would be good to minimally support autorelease pools.

This is a reference for future work: <https://github.com/mhjacobson/avr-objc>

```bash
git clone git@github.com:djthorpe/objc.git
cd objc
cmake --build build
lldb build/src/test/fuse
```

Seg faults on fprintf, but does get to obj_msgSend, so that's something.
