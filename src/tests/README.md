# Test Suite Documentation

This directory contains test coverage for the Objective-C runtime and NXFoundation framework. Tests can be run with `make tests` from the project root directory.

The tests are designed to work correctly in both debug and release builds (`RELEASE=1`), with proper handling of assertions and error conditions.

The tests are organized into three main categories:

## Test Categories

- **NXFoundation Tests** (NXFoundation_01 through NXFoundation_22): Tests for the NXFoundation framework classes and functionality.
- **Runtime Tests** (runtime_01 through runtime_37, excluding runtime_07): Tests for the Objective-C runtime system functionality.
- **System Tests** (sys_01, sys_02, sys_03, sys_04, sys_05, sys_06): Tests for low-level system functionality.

---

## NXFoundation Tests

| Test Name | Purpose | Description |
|-----------|---------|-------------|
| NXFoundation_01 | Zone Management | Tests zone allocation, default zone setup, and deallocation. |
| NXFoundation_02 | Basic Object Creation | Tests object creation, class verification, and release. |
| NXFoundation_03 | Class Hierarchy Testing | Tests inheritance and type checking with TestA/TestB hierarchy. |
| NXFoundation_04 | String Operations | Tests NXString creation, C string interoperability, and length validation. |
| NXFoundation_05 | Object Description | Tests object description generation and memory management. |
| NXFoundation_06 | Autorelease Pool Testing | Tests autorelease pool creation, management, and cleanup. |
| NXFoundation_07 | Thread Sleep Operations | Tests threading with `[NXThread sleepForTimeInterval:]`. |
| NXFoundation_08 | Application Runner | Tests application execution with `[NXApplication run]`. |
| NXFoundation_09 | Date and Time Operations | Tests NXDate creation, intervals, descriptions, and comparisons. |
| NXFoundation_10 | Arena Allocator | Tests zone-based memory allocation and cleanup. |
| NXFoundation_11 | Architecture Information | Tests system architecture detection (bits, endianness, cores). |
| NXFoundation_12 | Random Number Generation | Tests random 32-bit integer generation and validation. |
| NXFoundation_13 | Time Interval Operations | Tests time constants, arithmetic, conversions, and formatting. |
| NXFoundation_14 | Date Edge Cases | Tests NXDate overflow and edge cases with large intervals. |
| NXFoundation_15 | Number Boolean Operations | Tests NXNumber boolean functionality and arithmetic. |
| NXFoundation_16 | Random Number Statistics | Tests random number quality, duplicates, and distribution. |
| NXFoundation_17 | Null Value Testing | Tests NXNull singleton functionality and consistency. |
| NXFoundation_18 | String Method Testing | Tests NXString formatting, comparison, operations, and JSON escaping. |
| NXFoundation_19 | Number Testing | Tests NXNumber types, conversions, edge cases, and memory management. |
| NXFoundation_20 | JSON Protocol Testing | Tests JSONProtocol conformance for core types with Base64 serialization. |
| NXFoundation_21 | Array Testing | Tests NXArray creation, access, JSON serialization, and mutable operations. |
| NXFoundation_22 | Data Operations | Tests NXData storage, encoding, append operations, and equality comparisons. |

---

## Runtime Tests

| Test Name | Purpose | Description |
|-----------|---------|-------------|
| runtime_01 | Basic Object Introspection | Tests object/class introspection: `object_getClassName()`, `object_getClass()`, equality. |
| runtime_02 | Custom Class Testing | Tests runtime behavior with custom classes and introspection. |
| runtime_03 | Constant String Testing | Tests NXConstantString: creation, length, C string conversion, equality. |
| runtime_04 | Getter/Setter Method Testing | Tests getter/setter methods with custom Test class. |
| runtime_05 | Instance Variable Access | Tests instance variable access with custom objects. |
| runtime_06 | Category Method Testing | Tests category method functionality with description methods. |
| runtime_08 | Method Chaining | Tests method chaining and `[[Foo foo] bar]` patterns. |
| runtime_09 | Method Type Conflict Resolution | Tests method signature conflicts between class/instance methods. |
| runtime_10 | Class Method Dispatch | Tests class method invocation with type casting. |
| runtime_11 | Root Class Testing | Tests custom root class with `OBJC_ROOT_CLASS`. |
| runtime_12 | Root Class with Subclass | Tests root class inheritance with subclass hierarchy. |
| runtime_13 | Multiple Subclass Testing | Tests root class with multiple subclasses. |
| runtime_14 | Instance Variables and Accessors | Tests root class subclass with instance variables and accessors. |
| runtime_15 | Method Selector Testing | Tests hidden `_cmd` argument and `sel_getName()`. |
| runtime_16 | Type Encoding Testing | Tests `@encode` directive and type encoding constants. |
| runtime_17 | Enumeration and Bitfields | Tests enumeration types with bitfield instance variables. |
| runtime_18 | Complex Bitfield Structures | Tests complex bitfield structures as instance variables. |
| runtime_19 | Forward Class Declaration | Tests `@class` forward declarations and incomplete types. |
| runtime_20 | Function Message Dispatch | Tests function-based message dispatch and dynamic methods. |
| runtime_21 | Informal Protocol Testing | Tests informal protocols (categories) with method addition. |
| runtime_22 | Class Initialization | Tests `+initialize` method behavior and inheritance. |
| runtime_23 | Automatic Initialize Call | Tests automatic `+initialize` invocation and class state setup. |
| runtime_24 | Category Method Override | Tests method override behavior with category precedence. |
| runtime_25 | Basic Protocol Adoption | Tests protocol definition, adoption, and conformance validation. |
| runtime_26 | Protocol Type Variables | Tests protocol-typed variables (`id<Protocol>`) and method invocation. |
| runtime_27 | Multiple Protocol Adoption | Tests multiple protocol conformance and implementation. |
| runtime_28 | Protocol Adoption in Category | Tests protocol adoption through categories. |
| runtime_29 | Protocol Runtime Access | Tests runtime protocol access with `@protocol()`. |
| runtime_30 | Protocol Definition and Access | Tests protocol definition and runtime access with `proto_getName()`. |
| runtime_31 | Protocol Inheritance | Tests protocol inheritance and conformance checking. |
| runtime_32 | Protocol Object Properties | Tests protocol object validation and operations. |
| runtime_33 | Protocol Identity and Equality | Tests protocol identity, comparison, and uniqueness. |
| runtime_34 | Variadic Method Arguments | Tests variadic methods with `va_list` and variable arguments. |
| runtime_35 | Static Variables in Classes | Tests static variables within class implementations. |
| runtime_36 | Static Functions in Classes | Tests static functions within class implementations. |
| runtime_37 | Selector Name Access | Tests selector creation and name retrieval with `sel_getName()`. |

---

## System Tests

| Test Name | Purpose | Description |
|-----------|---------|-------------|
| sys_00 | Memory System Functions | Tests `sys_malloc()`, `sys_free()`, `sys_memset()`, `sys_memcpy()`, `sys_memmove()`, `sys_memcmp()`. |
| sys_01 | Printf System Functions | Tests `sys_printf()`, `sys_sprintf()`, and `sys_vsprintf()` with 102 tests covering format specifiers, width modifiers, and buffer overflow. |
| sys_02 | Time System Functions | Tests `sys_time_get_utc()` with validation and NULL handling. |
| sys_03 | Thread System Functions | Tests `sys_thread_numcores()` with validation and boundary checks. |
| sys_04 | Random Number Generation | Tests `sys_random_uint32()` and `sys_random_uint64()` with distribution testing. |
| sys_05 | Hash Function Testing | Tests `sys_hash_*` functions with MD5/SHA-256 algorithms and test vectors. |
| sys_06 | Timer System Functions | Tests `sys_timer_*` functions with 19 tests covering initialization, callbacks, one-shot/periodic behavior, timing precision, error handling, and edge cases. |
