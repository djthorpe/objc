# objc

Objective C experiments. Motivation is to see if a very minimal Objective C runtime can be created for ARM and x86, for embedded systems. Would be good to minimally support autorelease pools.

These are some references for future work:

* <https://github.com/mhjacobson/avr-objc>
* <https://github.com/charlieMonroe/libobjc-kern>

Build testing on Linux/ARM64:

```bash
git clone git@github.com:djthorpe/objc.git
cd objc
mkdir build
cmake -DCMAKE_C_COMPILER=/opt/homebrew/Cellar/llvm@14/14.0.6/bin/clang  -B build
cmake --build build
lldb build/src/test/test
```

```text
objc: objc_init
objc_class_load: Test
  Test is a meta class
  Test superclass => 
objc_class_load: Object
  Object is a meta class
  Object is a root class
  Object superclass => 
objc_class_load: Object
  Object is a root class
  Method: -[Object init]
00 00 00 00 D1 '#' 'U' 00 00 00 00 00 08 '%' 'U' 00 '(' '%' 'U' 00 00 00 00 00 00 00 00 00 00 00 00 00 '`' 00 00 00 'H' '%' 'U' 00 00 00 00 00 00 00 00 00 '=' '#' 'U' 00 C6 '#' 'U' 00 90 '#' 'U' 00 8C '#' 'U' 00   Object protocol: <(null)> 
  Object ivar: <isa>
  Object superclass <= 
  Method: +[Object alloc]
00 00 00 00 D1 '#' 'U' 00 00 00 00 00 08 '%' 'U' 00 '(' '%' 'U' 00 00 00 00 00 00 00 00 00 00 00 00 00 '`' 00 00 00 'H' '%' 'U' 00 00 00 00 00 00 00 00 00 '=' '#' 'U' 00 C6 '#' 'U' 00 90 '#' 'U' 00 8C '#' 'U' 00   Object protocol: <(null)> 
  Test superclass <= 
  Method: +[Test classMethod]
00 00 00 00 83 '#' 'U' 00 00 00 00 00 'p' '%' 'U' 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 '`' 00 00 00 90 '%' 'U' 00 00 00 00 00 00 00 00 00 C0 '%' 'U' 00 00 00 00 00 FA '%' 'U' 00 00 00 00 00   Test protocol: <(null)> 
objc_msgSend selector alloc
```

We are getting segfault, probably because we are not actually calling the alloc method yet...
