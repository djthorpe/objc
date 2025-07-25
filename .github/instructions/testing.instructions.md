---
applyTo: '**'
---
Provide project context and coding guidelines that AI should follow when generating code, answering questions, or reviewing changes.---
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
