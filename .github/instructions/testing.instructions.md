---
applyTo: '**'
---

# Testing Instructions

In order to compile and run all the tests on a Mac, run the following command in the repository root:

```bash
TOOLCHAIN_PATH=/opt/homebrew CC=gcc-15 RELEASE=1 make tests
```

Once you've done this, you can then compile and run an individual tests.
Additional debugging information can be included by removing the `RELEASE=1` flag,
which will enable debug symbols and other debugging features. 

Use the following command to test an individual target:

```bash
cmake --build build --target <test_target>
```

You can then run the test with with the following command, to get more detailed output:

```bash
ctest --output-on-failure --test-dir build --tests-regex <test_target>
```

When you've updated a test with more test coverage, make sure to update the file in `src/tests/README.md`
to keep the summary of tests up to date with the same name as the target, and include the new test 
cases in that file.

## Cross-Compilation - PICO

To cross-compile the libraries for the pico, this is the command to run:

```bash
CC=clang TARGET=armv6m-none-eabi TOOLCHAIN_PATH=/opt/LLVM-ET-Arm-19.1.5-Darwin-universal make
```


Then the tests can be compiled as before, and loaded onto a Pico with the following command:

```bash
cmake --build build --target <test_target> && picotool load -x build/src/tests/<test_target>/<test_target>.uf2
```

## Cross-Compilation - Linux

And for Linux, you can use the following command:

```bash
docker run --rm -i -v $(pwd):/root bookworm-builder bash -c "cd /root && make clean && CC=gcc make"
```

And then individual test targets can be run with:

```bash
docker run --rm -i -v $(pwd):/root bookworm-builder bash -c "ctest --output-on-failure --test-dir build --tests-regex <test_target>"
```
