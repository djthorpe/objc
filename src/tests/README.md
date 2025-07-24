# Test Suite Documentation

This directory contains comprehensive test coverage for the Objective-C runtime and NXFoundation framework. Tests can be run with `make tests` from the project root directory.

The tests are organized into three main categories:

## Test Categories

- **NXFoundation Tests** (NXFoundation_01 through NXFoundation_19): Tests for the NXFoundation framework classes and functionality.
- **Runtime Tests** (runtime_01 through runtime_37, excluding runtime_07): Tests for the Objective-C runtime system functionality.
- **System Tests** (sys_01, sys_02, sys_03, sys_04): Tests for low-level system functionality.

---

## NXFoundation Tests

| Test Name | Purpose | Description |
|-----------|---------|-------------|
| NXFoundation_01 | Zone Management | Tests basic zone allocation and deallocation. Creates a zone with `[NXZone zoneWithSize:1024]`, verifies zone becomes the default zone, tests proper zone deallocation. |
| NXFoundation_02 | Basic Object Creation | Tests fundamental object creation and class verification. Creates NXObject instances, verifies class identity with `[object class]`, tests proper object release. |
| NXFoundation_03 | Class Hierarchy Testing | Tests class inheritance and type checking with custom classes. Defines TestA and TestB class hierarchy (TestB inherits from TestA), tests `isKindOfClass:` for inheritance relationships, verifies class identity and hierarchy navigation. |
| NXFoundation_04 | String Operations | Tests NXString class functionality. String creation with `initWithString:` from constant strings, string creation from other NXString instances, C string interoperability and length validation. |
| NXFoundation_05 | Object Description | Tests object description functionality. Object description string generation, string content validation, memory management of description strings. |
| NXFoundation_06 | Autorelease Pool Testing | Tests autorelease pool functionality. Autorelease pool creation and management, adding 100 objects to autorelease pool, pool drainage and cleanup validation. |
| NXFoundation_07 | Thread Sleep Operations | Tests threading functionality. Thread sleep using `[NXThread sleepForTimeInterval:]`, time interval constants (Second). |
| NXFoundation_08 | Application Runner | Tests application framework. Basic application execution with `[NXApplication run]`. |
| NXFoundation_09 | Date and Time Operations | Tests NXDate comprehensive functionality. Current date creation with `[NXDate date]`, date creation with time intervals (past and future), date description and string representation, underlying `sys_time_get_utc` function testing. |
| NXFoundation_10 | Arena Allocator | Tests zone-based memory allocation. Zone creation with specific sizes, memory allocation within zones, zone memory management and cleanup. |
| NXFoundation_11 | Architecture Information | Tests system architecture detection. Architecture bit detection (32/64-bit) with `NXArchBits()`, endianness detection with `NXArchEndian()`, CPU core count detection with `NXArchNumCores()`, consistency validation across multiple calls. |
| NXFoundation_12 | Random Number Generation | Tests random number generation functions. Random 32-bit integer generation with `NXRandInt32()`, random unsigned 32-bit integer generation with `NXRandUnsignedInt32()`, range validation and basic output testing. |
| NXFoundation_13 | Time Interval Constants | Tests NXTimeInterval constants and relationships. Time constant validation (Nanosecond, Millisecond, Second, Minute, Hour, Day), time constant mathematical relationships, precision and calculation accuracy. |
| NXFoundation_14 | Date Edge Cases | Tests NXDate edge cases and overflow handling. Nanosecond overflow in `addTimeInterval:`, negative interval handling, large time interval calculations. |
| NXFoundation_15 | Number Boolean Operations | Tests NXNumber boolean functionality. Boolean number creation (`YES`/`NO`), boolean value retrieval, boolean arithmetic and logic. |
| NXFoundation_16 | Random Number Statistics | Tests random number generation quality. Multiple random number generation calls, duplicate detection and randomness validation, statistical distribution testing. |
| NXFoundation_17 | Null Value Testing | Tests NXNull singleton functionality. Null value creation with `[NXNull nullValue]`, singleton pattern validation (same instance returned), multiple call consistency testing. |
| NXFoundation_18 | String Method Testing | Tests NXString factory methods. String creation with `stringWithCString:`, various string manipulation methods, string object lifecycle management. |
| NXFoundation_19 | Number Comprehensive Testing | Comprehensive test suite for NXNumber class hierarchy. 24 test functions covering: number creation (int64, uint64, int32, uint32, int16, uint16, bool), value handling and conversions, zero singleton optimization, equality testing, description methods, edge cases and boundary conditions, static values, memory management with autorelease pools, class hierarchy validation, random number integration, conversion edge cases, and error condition handling. |

---

## Runtime Tests

| Test Name | Purpose | Description |
|-----------|---------|-------------|
| runtime_01 | Basic Object Introspection | Tests fundamental object and class introspection. Object class name retrieval with `object_getClassName()`, class object retrieval with `object_getClass()`, class name retrieval with `class_getName()`, basic object equality testing. |
| runtime_02 | Custom Class Testing | Tests runtime behavior with custom classes. Custom Test class instantiation and introspection, class name verification for custom classes, object equality and class identity validation. |
| runtime_03 | Constant String Testing | Tests NXConstantString functionality. Constant string creation with `@"string"` syntax, string length validation with `[string length]`, C string conversion with `[string cStr]`, string equality comparison with `isEqual:`. |
| runtime_04 | Getter/Setter Method Testing | Tests getter and setter method functionality. Custom Test class with value instance variable, initialization with value, value modification through setter methods, value retrieval through getter methods. |
| runtime_05 | Instance Variable Access | Tests instance variable access with custom objects. Custom Test1 class with multiple value instance variables, getter and setter method testing for multiple values, object initialization with values, manual memory management. |
| runtime_06 | Category Method Testing | Tests category method functionality. Test class with string initialization, category-provided description method, string comparison and validation. |
| runtime_08 | Method Chaining | Tests method chaining and call tracking. Class methods that return self, call tracking with global variables, chained method invocation `[[Foo foo] bar]`. |
| runtime_09 | Method Type Conflict Resolution | Tests handling of method signature conflicts. Class method vs instance method with conflicting return types, proper method resolution and dispatch. |
| runtime_10 | Class Method Dispatch | Tests calling class methods with type casting. Class method invocation, self redefinition handling, method return type casting. |
| runtime_11 | Root Class Testing | Tests custom root class functionality. Root class with `OBJC_ROOT_CLASS`, class and superclass methods, class lookup with `objc_lookupClass()`, superclass validation (should be Nil for root class). |
| runtime_12 | Root Class with Subclass | Tests root class and simple subclass hierarchy. Root class and single subclass, class lookup and superclass relationships, inheritance validation. |
| runtime_13 | Multiple Subclass Testing | Tests root class with multiple subclasses. Root class with SubClassA and SubClassB, multiple inheritance paths from same root, class hierarchy validation. |
| runtime_14 | Instance Variables and Accessors | Tests root class with subclass containing instance variables. Subclass with `_state` ivar, setter and getter methods, instance variable access and modification. |
| runtime_15 | Method Selector Testing | Tests the hidden `_cmd` argument in method calls. Method that returns its own selector name using `sel_getName(_cmd)`, selector name validation. |
| runtime_16 | Type Encoding Testing | Tests Objective-C type encoding functionality. `@encode` directive testing, type encoding constants validation, various data type encodings. |
| runtime_17 | Enumeration and Bitfields | Tests enumeration types with bitfield instance variables. Color enum with bitfield storage (2 bits), enum value setting and retrieval, bitfield functionality. |
| runtime_18 | Complex Bitfield Structures | Tests complex bitfield structures as instance variables. Struct with multiple bitfield enum members, pointer-based ivar access, complex structure manipulation. |
| runtime_19 | Forward Class Declaration | Tests forward class declarations. `@class` forward declaration, incomplete class type usage, compilation without full class definition. |
| runtime_20 | Function Message Dispatch | Tests function-based message dispatch. Function-to-method conversion, dynamic method creation, call counting and verification. |
| runtime_21 | Informal Protocol Testing | Tests informal protocol (category) functionality. Category method addition to base Object class, method implementation and invocation, call tracking. |
| runtime_22 | Class Initialization | Tests class `+initialize` method behavior. Automatic initialization calls, subclass initialization inheritance, initialization call counting. |
| runtime_23 | Automatic Initialize Call | Tests automatic `+initialize` method invocation. Class variable initialization through `+initialize`, automatic invocation before first class access, class state setup. |
| runtime_24 | Category Method Override | Tests method override behavior with categories. Original class method vs category method, category method precedence, method replacement validation. |
| runtime_25 | Basic Protocol Adoption | Tests protocol definition and class adoption. Protocol with getter/setter methods (`isEnabled`, `setEnabled:`), class implementing protocol methods, protocol conformance validation, instance variable backing methods. |
| runtime_26 | Protocol Type Variables | Tests using protocol-typed variables (`id<Protocol>`). Protocol-typed object variables, method invocation through protocol interface, type safety with protocol conformance. |
| runtime_27 | Multiple Protocol Adoption | Tests class adopting multiple protocols. Multiple protocol conformance (`<Protocol1, Protocol2>`), implementing methods from multiple protocols, multi-protocol type variables. |
| runtime_28 | Protocol Adoption in Category | Tests protocol adoption through categories. Category adopting protocol, protocol method implementation in category, protocol conformance through categories. |
| runtime_29 | Protocol Runtime Access | Tests runtime protocol access and introspection. Protocol object retrieval with `@protocol()`, protocol name access with `proto_getName()`, protocol metadata inspection. |
| runtime_30 | Protocol Definition and Access | Tests protocol definition and runtime access. Protocol definition with `@protocol`, protocol name retrieval with `proto_getName()`, protocol object access. |
| runtime_31 | Protocol Inheritance | Tests protocol inheritance and conformance. Protocol inheritance with `<ParentProtocol>`, multiple protocol conformance checking, inherited protocol method requirements. |
| runtime_32 | Protocol Object Properties | Tests protocol object properties and operations. Protocol object validation, protocol name access, Protocol class properties, protocol equality testing. |
| runtime_33 | Protocol Identity and Equality | Tests protocol identity and comparison operations. Protocol inequality testing, protocol equality validation, Protocol class hierarchy, protocol object uniqueness. |
| runtime_34 | Variadic Method Arguments | Tests methods with variable argument lists. Variadic method definition (`...` syntax), `va_list` argument processing, variable argument method invocation. |
| runtime_35 | Static Variables in Classes | Tests static variables inside class implementations. Static variable declaration within `@implementation`, static variable access through class methods, initialization behavior. |
| runtime_36 | Static Functions in Classes | Tests static functions inside class implementations. Static function definition within `@implementation`, static function invocation from class methods, function encapsulation. |
| runtime_37 | Selector Name Access | Tests selector name retrieval functionality. Selector creation with `@selector()`, selector name access with `sel_getName()`, selector string representation. |

---

## System Tests

| Test Name | Purpose | Description |
|-----------|---------|-------------|
| sys_01 | Printf System Functions | Tests comprehensive printf-style formatting functions. Includes `sys_printf()` and `sys_sprintf()` with extensive format specifier testing: string (`%s`, including NULL), integer (`%d`, `%+d`), hexadecimal (`%x`, `%X`, `%#x`, `%#X`), binary (`%b`, `%#b`), octal (`%o`, `%#o`), unsigned (`%u`), character (`%c`), percent literal (`%`, `%%`), and edge cases. |
| sys_02 | Time System Functions | Tests low-level time management functions. Tests `sys_time_get_utc()` with time structure validation (seconds > 0, nanoseconds in valid range), NULL parameter handling, time consistency testing (sequential calls should be monotonic), time precision and accuracy validation. |
| sys_03 | Thread System Functions | Tests low-level threading system information. Tests `sys_thread_numcores()` CPU core count detection with validation (>= 1, within reasonable bounds), consistency testing across multiple calls, performance testing (function should be fast), boundary testing (should not exceed UINT8_MAX - 1). |
| sys_04 | Random Number Generation | Tests low-level random number generation functions. Tests `sys_random_uint32()` and `sys_random_uint64()` with basic functionality validation, randomness verification (multiple calls should differ), range validation, performance testing (functions should be fast), consistency testing across multiple batches, statistical distribution validation, and 64-bit full range utilization verification. |
