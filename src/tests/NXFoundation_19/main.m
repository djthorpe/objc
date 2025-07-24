#include <NXFoundation/NXFoundation.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

int test_nxnumber_methods(void);

///////////////////////////////////////////////////////////////////////////////
// TEST IMPLEMENTATIONS

/**
 * @brief Test NXNumber creation methods
 */
void test_nxnumber_creation(void) {
  // Test creation with various int64 values
  NXNumber *num1 = [NXNumber numberWithInt64:42];
  test_assert(num1 != nil);
  test_assert([num1 int64Value] == 42);

  NXNumber *num2 = [NXNumber numberWithInt64:-100];
  test_assert(num2 != nil);
  test_assert([num2 int64Value] == -100);

  NXNumber *num3 = [NXNumber numberWithInt64:0];
  test_assert(num3 != nil);
  test_assert([num3 int64Value] == 0);

  // Test creation with unsigned int64 values
  NXNumber *unum1 = [NXNumber numberWithUnsignedInt64:42U];
  test_assert(unum1 != nil);
  test_assert([unum1 unsignedInt64Value] == 42U);

  NXNumber *unum2 =
      [NXNumber numberWithUnsignedInt64:18446744073709551615ULL]; // UINT64_MAX
  test_assert(unum2 != nil);
  test_assert([unum2 unsignedInt64Value] == 18446744073709551615ULL);

  NXNumber *unum3 = [NXNumber numberWithUnsignedInt64:0U];
  test_assert(unum3 != nil);
  test_assert([unum3 unsignedInt64Value] == 0U);

  // Test creation with boolean values
  NXNumber *numTrue = [NXNumber numberWithBool:YES];
  test_assert(numTrue != nil);
  test_assert([numTrue boolValue] == YES);

  NXNumber *numFalse = [NXNumber numberWithBool:NO];
  test_assert(numFalse != nil);
  test_assert([numFalse boolValue] == NO);
}

/**
 * @brief Test int64 value handling
 */
void test_nxnumber_int64_values(void) {
  // Test large positive values
  int64_t largePos = 9223372036854775807LL; // MAX_INT64
  NXNumber *numLargePos = [NXNumber numberWithInt64:largePos];
  test_assert([numLargePos int64Value] == largePos);

  // Test large negative values
  int64_t largeNeg = -9223372036854775807LL - 1; // MIN_INT64
  NXNumber *numLargeNeg = [NXNumber numberWithInt64:largeNeg];
  test_assert([numLargeNeg int64Value] == largeNeg);

  // Test regular values
  int64_t values[] = {1, -1, 100, -100, 1000, -1000, 123456789, -987654321};
  int i;
  for (i = 0; i < 8; i++) {
    NXNumber *num = [NXNumber numberWithInt64:values[i]];
    test_assert([num int64Value] == values[i]);
  }
}

/**
 * @brief Test unsigned int64 value handling
 */
void test_nxnumber_unsigned_int64_values(void) {
  // Test maximum unsigned value
  uint64_t maxUnsigned = 18446744073709551615ULL; // UINT64_MAX
  NXNumber *numMaxUnsigned = [NXNumber numberWithUnsignedInt64:maxUnsigned];
  test_assert([numMaxUnsigned unsignedInt64Value] == maxUnsigned);

  // Test large unsigned values
  uint64_t largeUnsigned =
      9223372036854775808ULL; // 2^63, larger than max signed int64
  NXNumber *numLargeUnsigned = [NXNumber numberWithUnsignedInt64:largeUnsigned];
  test_assert([numLargeUnsigned unsignedInt64Value] == largeUnsigned);

  // Test regular unsigned values
  uint64_t values[] = {1U,
                       100U,
                       1000U,
                       1000000U,
                       4294967295U,
                       4294967296ULL,
                       1234567890123456789ULL};
  int i;
  for (i = 0; i < 7; i++) {
    NXNumber *num = [NXNumber numberWithUnsignedInt64:values[i]];
    test_assert([num unsignedInt64Value] == values[i]);
  }

  // Test zero value
  NXNumber *numZero = [NXNumber numberWithUnsignedInt64:0U];
  test_assert([numZero unsignedInt64Value] == 0U);
  test_assert([numZero boolValue] == NO);
}

/**
 * @brief Test boolean conversion
 */
void test_nxnumber_bool_conversion(void) {
  // Test non-zero signed values convert to YES
  NXNumber *num1 = [NXNumber numberWithInt64:1];
  test_assert([num1 boolValue] == YES);

  NXNumber *num2 = [NXNumber numberWithInt64:-1];
  test_assert([num2 boolValue] == YES);

  NXNumber *num3 = [NXNumber numberWithInt64:42];
  test_assert([num3 boolValue] == YES);

  // Test non-zero unsigned values convert to YES
  NXNumber *unum1 = [NXNumber numberWithUnsignedInt64:1U];
  test_assert([unum1 boolValue] == YES);

  NXNumber *unum2 = [NXNumber numberWithUnsignedInt64:42U];
  test_assert([unum2 boolValue] == YES);

  NXNumber *unum3 =
      [NXNumber numberWithUnsignedInt64:18446744073709551615ULL]; // UINT64_MAX
  test_assert([unum3 boolValue] == YES);

  // Test zero converts to NO (both signed and unsigned)
  NXNumber *numZero = [NXNumber numberWithInt64:0];
  test_assert([numZero boolValue] == NO);

  NXNumber *unumZero = [NXNumber numberWithUnsignedInt64:0U];
  test_assert([unumZero boolValue] == NO);

  // Test boolean numbers
  NXNumber *numTrue = [NXNumber numberWithBool:YES];
  test_assert([numTrue boolValue] == YES);

  NXNumber *numFalse = [NXNumber numberWithBool:NO];
  test_assert([numFalse boolValue] == NO);
}

/**
 * @brief Test zero singleton behavior
 */
void test_nxnumber_zero_singleton(void) {
  // Test signed int64 zero singleton
  NXNumber *zero1 = [NXNumber numberWithInt64:0];
  NXNumber *zero2 = [NXNumber numberWithInt64:0];

  // Both should be the same singleton instance
  test_assert(zero1 == zero2);
  test_assert([zero1 int64Value] == 0);
  test_assert([zero2 int64Value] == 0);
  test_assert([zero1 boolValue] == NO);

  // Test unsigned int64 zero singleton
  NXNumber *uzero1 = [NXNumber numberWithUnsignedInt64:0U];
  NXNumber *uzero2 = [NXNumber numberWithUnsignedInt64:0U];

  // Both should be the same singleton instance
  test_assert(uzero1 == uzero2);
  test_assert([uzero1 unsignedInt64Value] == 0U);
  test_assert([uzero2 unsignedInt64Value] == 0U);
  test_assert([uzero1 boolValue] == NO);

  // Note: signed and unsigned zero might be different instances
  // since they're implemented by different classes
}

/**
 * @brief Test number equality
 */
void test_nxnumber_equality(void) {
  // Test signed int64 equality
  NXNumber *num1 = [NXNumber numberWithInt64:42];
  NXNumber *num2 = [NXNumber numberWithInt64:42];
  NXNumber *num3 = [NXNumber numberWithInt64:43];

  // Test equality with same values
  test_assert([num1 isEqual:num2]);
  test_assert([num2 isEqual:num1]);

  // Test inequality with different values
  test_assert(![num1 isEqual:num3]);
  test_assert(![num3 isEqual:num1]);

  // Test self-equality
  test_assert([num1 isEqual:num1]);

  // Test nil comparison
  test_assert(![num1 isEqual:nil]);

  // Test unsigned int64 equality
  NXNumber *unum1 = [NXNumber numberWithUnsignedInt64:42U];
  NXNumber *unum2 = [NXNumber numberWithUnsignedInt64:42U];
  NXNumber *unum3 = [NXNumber numberWithUnsignedInt64:43U];

  // Test equality with same unsigned values
  test_assert([unum1 isEqual:unum2]);
  test_assert([unum2 isEqual:unum1]);

  // Test inequality with different unsigned values
  test_assert(![unum1 isEqual:unum3]);
  test_assert(![unum3 isEqual:unum1]);

  // Test self-equality for unsigned
  test_assert([unum1 isEqual:unum1]);

  // Test large unsigned value equality
  NXNumber *large1 = [NXNumber numberWithUnsignedInt64:18446744073709551615ULL];
  NXNumber *large2 = [NXNumber numberWithUnsignedInt64:18446744073709551615ULL];
  test_assert([large1 isEqual:large2]);

  // Test cross-class equality (signed vs unsigned)
  NXNumber *signed42 = [NXNumber numberWithInt64:42];
  NXNumber *unsigned42 = [NXNumber numberWithUnsignedInt64:42U];

  // Test that positive signed and unsigned numbers with same value are equal
  test_assert([signed42 isEqual:unsigned42]);
  test_assert([unsigned42 isEqual:signed42]);

  // Test zero equality across types
  NXNumber *signedZero = [NXNumber numberWithInt64:0];
  NXNumber *unsignedZero = [NXNumber numberWithUnsignedInt64:0U];
  test_assert([signedZero isEqual:unsignedZero]);
  test_assert([unsignedZero isEqual:signedZero]);

  // Test that negative signed numbers don't equal positive unsigned numbers
  NXNumber *signedNegative = [NXNumber numberWithInt64:-42];
  NXNumber *unsignedPositive = [NXNumber numberWithUnsignedInt64:42U];
  test_assert(![signedNegative isEqual:unsignedPositive]);
  test_assert(![unsignedPositive isEqual:signedNegative]);

  // Test different positive values across types
  NXNumber *signed100 = [NXNumber numberWithInt64:100];
  NXNumber *unsigned200 = [NXNumber numberWithUnsignedInt64:200U];
  test_assert(![signed100 isEqual:unsigned200]);
  test_assert(![unsigned200 isEqual:signed100]);

  // Test boolean cross-class equality
  NXNumber *boolTrue = [NXNumber numberWithBool:YES];
  NXNumber *signed1 = [NXNumber numberWithInt64:1];
  NXNumber *unsigned1 = [NXNumber numberWithUnsignedInt64:1U];

  // Boolean true should equal numeric 1 across types
  test_assert([boolTrue isEqual:signed1]);
  test_assert([signed1 isEqual:boolTrue]);
  test_assert([boolTrue isEqual:unsigned1]);
  test_assert([unsigned1 isEqual:boolTrue]);

  NXNumber *boolFalse = [NXNumber numberWithBool:NO];
  // Boolean false should equal numeric 0 across types
  test_assert([boolFalse isEqual:signedZero]);
  test_assert([signedZero isEqual:boolFalse]);
  test_assert([boolFalse isEqual:unsignedZero]);
  test_assert([unsignedZero isEqual:boolFalse]);
}

/**
 * @brief Test string description
 */
void test_nxnumber_description(void) {
  // Test signed int64 descriptions
  NXNumber *num1 = [NXNumber numberWithInt64:42];
  NXString *desc1 = [num1 description];
  test_assert(desc1 != nil);
  test_assert(strlen([desc1 cStr]) > 0);

  NXNumber *num2 = [NXNumber numberWithInt64:-123];
  NXString *desc2 = [num2 description];
  test_assert(desc2 != nil);

  NXNumber *zero = [NXNumber numberWithInt64:0];
  NXString *descZero = [zero description];
  test_assert(descZero != nil);

  // Test unsigned int64 descriptions
  NXNumber *unum1 = [NXNumber numberWithUnsignedInt64:42U];
  NXString *udesc1 = [unum1 description];
  test_assert(udesc1 != nil);
  test_assert(strlen([udesc1 cStr]) > 0);

  NXNumber *unum2 =
      [NXNumber numberWithUnsignedInt64:18446744073709551615ULL]; // UINT64_MAX
  NXString *udesc2 = [unum2 description];
  test_assert(udesc2 != nil);
  test_assert(strlen([udesc2 cStr]) > 0);

  NXNumber *uzero = [NXNumber numberWithUnsignedInt64:0U];
  NXString *udescZero = [uzero description];
  test_assert(udescZero != nil);
}

/**
 * @brief Test edge cases and error conditions
 */
void test_nxnumber_edge_cases(void) {
  // Test unsigned conversion with positive signed values
  NXNumber *numPos = [NXNumber numberWithInt64:42];
  uint64_t unsignedVal = [numPos unsignedInt64Value];
  test_assert(unsignedVal == 42);

  // Test unsigned conversion with zero
  NXNumber *numZero = [NXNumber numberWithInt64:0];
  uint64_t unsignedZero = [numZero unsignedInt64Value];
  test_assert(unsignedZero == 0);

  // Test signed conversion from unsigned values
  NXNumber *unumSmall = [NXNumber numberWithUnsignedInt64:42U];
  int64_t signedVal = [unumSmall int64Value];
  test_assert(signedVal == 42);

  // Test conversion between signed and unsigned with same bit pattern
  NXNumber *unumMax = [NXNumber
      numberWithUnsignedInt64:9223372036854775807ULL]; // INT64_MAX as unsigned
  int64_t signedFromUnsigned = [unumMax int64Value];
  test_assert(signedFromUnsigned == 9223372036854775807LL);

  // Test boundary values
  NXNumber *unumBoundary = [NXNumber
      numberWithUnsignedInt64:9223372036854775808ULL]; // INT64_MAX + 1
  test_assert([unumBoundary unsignedInt64Value] == 9223372036854775808ULL);
  test_assert([unumBoundary boolValue] == YES);

  // Test UINT64_MAX
  NXNumber *unumMaxVal =
      [NXNumber numberWithUnsignedInt64:18446744073709551615ULL];
  test_assert([unumMaxVal unsignedInt64Value] == 18446744073709551615ULL);
  test_assert([unumMaxVal boolValue] == YES);

  // Note: Testing negative to unsigned conversion would trigger assertion
  // so we don't test that case as it's designed to fail
}

/**
 * @brief Test NXNumberInt32 creation and basic functionality
 */
void test_nxnumber_int32_creation(void) {
  // Test basic creation
  NXNumber *num1 = [NXNumber numberWithInt32:42];
  test_assert(num1 != nil);
  test_assert([num1 int32Value] == 42);
  test_assert([num1 int64Value] == 42);

  // Test negative values
  NXNumber *num2 = [NXNumber numberWithInt32:-100];
  test_assert(num2 != nil);
  test_assert([num2 int32Value] == -100);
  test_assert([num2 int64Value] == -100);

  // Test zero
  NXNumber *num3 = [NXNumber numberWithInt32:0];
  test_assert(num3 != nil);
  test_assert([num3 int32Value] == 0);
  test_assert([num3 int64Value] == 0);

  // Test boundary values
  NXNumber *numMax = [NXNumber numberWithInt32:INT32_MAX];
  test_assert(numMax != nil);
  test_assert([numMax int32Value] == INT32_MAX);
  test_assert([numMax int64Value] == INT32_MAX);

  NXNumber *numMin = [NXNumber numberWithInt32:INT32_MIN];
  test_assert(numMin != nil);
  test_assert([numMin int32Value] == INT32_MIN);
  test_assert([numMin int64Value] == INT32_MIN);
}

/**
 * @brief Test NXNumberUnsignedInt32 creation and basic functionality
 */
void test_nxnumber_unsigned_int32_creation(void) {
  // Test basic creation
  NXNumber *num1 = [NXNumber numberWithUnsignedInt32:42U];
  test_assert(num1 != nil);
  test_assert([num1 unsignedInt32Value] == 42U);
  test_assert([num1 unsignedInt64Value] == 42U);

  // Test zero
  NXNumber *num2 = [NXNumber numberWithUnsignedInt32:0U];
  test_assert(num2 != nil);
  test_assert([num2 unsignedInt32Value] == 0U);
  test_assert([num2 unsignedInt64Value] == 0U);

  // Test boundary values
  NXNumber *numMax = [NXNumber numberWithUnsignedInt32:UINT32_MAX];
  test_assert(numMax != nil);
  test_assert([numMax unsignedInt32Value] == UINT32_MAX);
  test_assert([numMax unsignedInt64Value] == UINT32_MAX);
}

/**
 * @brief Test cross-class equality between 32-bit and 64-bit number classes
 */
void test_nxnumber_32bit_equality(void) {
  // Test int32 vs int64 equality
  NXNumber *int32_42 = [NXNumber numberWithInt32:42];
  NXNumber *int64_42 = [NXNumber numberWithInt64:42];
  test_assert([int32_42 isEqual:int64_42]);
  test_assert([int64_42 isEqual:int32_42]);

  // Test uint32 vs uint64 equality
  NXNumber *uint32_42 = [NXNumber numberWithUnsignedInt32:42U];
  NXNumber *uint64_42 = [NXNumber numberWithUnsignedInt64:42U];
  test_assert([uint32_42 isEqual:uint64_42]);
  test_assert([uint64_42 isEqual:uint32_42]);

  // Test int32 vs uint32 equality (positive values)
  test_assert([int32_42 isEqual:uint32_42]);
  test_assert([uint32_42 isEqual:int32_42]);

  // Test int32 vs uint64 equality (positive values)
  test_assert([int32_42 isEqual:uint64_42]);
  test_assert([uint64_42 isEqual:int32_42]);

  // Test inequality
  NXNumber *int32_43 = [NXNumber numberWithInt32:43];
  test_assert(![int32_42 isEqual:int32_43]);
  test_assert(![int32_43 isEqual:int32_42]);

  // Test negative int32 vs positive uint32 (should not be equal)
  NXNumber *int32_neg = [NXNumber numberWithInt32:-1];
  NXNumber *uint32_pos = [NXNumber numberWithUnsignedInt32:1U];
  test_assert(![int32_neg isEqual:uint32_pos]);
  test_assert(![uint32_pos isEqual:int32_neg]);
}

/**
 * @brief Test 32-bit number string descriptions
 */
void test_nxnumber_32bit_description(void) {
  // Test int32 descriptions
  NXNumber *int32_pos = [NXNumber numberWithInt32:123];
  NXString *desc1 = [int32_pos description];
  test_assert(desc1 != nil);
  test_assert(strcmp([desc1 cStr], "123") == 0);

  NXNumber *int32_neg = [NXNumber numberWithInt32:-456];
  NXString *desc2 = [int32_neg description];
  test_assert(desc2 != nil);
  test_assert(strcmp([desc2 cStr], "-456") == 0);

  // Test uint32 descriptions
  NXNumber *uint32_val = [NXNumber numberWithUnsignedInt32:789U];
  NXString *desc3 = [uint32_val description];
  test_assert(desc3 != nil);
  test_assert(strcmp([desc3 cStr], "789") == 0);

  NXNumber *uint32_max = [NXNumber numberWithUnsignedInt32:UINT32_MAX];
  NXString *desc4 = [uint32_max description];
  test_assert(desc4 != nil);
  test_assert(strcmp([desc4 cStr], "4294967295") == 0);
}

/**
 * @brief Test NXNumberInt16 creation and basic functionality
 */
void test_nxnumber_int16_creation(void) {
  // Test basic creation
  NXNumber *num1 = [NXNumber numberWithInt16:42];
  test_assert(num1 != nil);
  test_assert([num1 int16Value] == 42);
  test_assert([num1 int32Value] == 42);
  test_assert([num1 int64Value] == 42);

  // Test negative values
  NXNumber *num2 = [NXNumber numberWithInt16:-100];
  test_assert(num2 != nil);
  test_assert([num2 int16Value] == -100);
  test_assert([num2 int32Value] == -100);
  test_assert([num2 int64Value] == -100);

  // Test zero
  NXNumber *num3 = [NXNumber numberWithInt16:0];
  test_assert(num3 != nil);
  test_assert([num3 int16Value] == 0);
  test_assert([num3 int32Value] == 0);
  test_assert([num3 int64Value] == 0);

  // Test boundary values
  NXNumber *numMax = [NXNumber numberWithInt16:INT16_MAX];
  test_assert(numMax != nil);
  test_assert([numMax int16Value] == INT16_MAX);
  test_assert([numMax int32Value] == INT16_MAX);
  test_assert([numMax int64Value] == INT16_MAX);

  NXNumber *numMin = [NXNumber numberWithInt16:INT16_MIN];
  test_assert(numMin != nil);
  test_assert([numMin int16Value] == INT16_MIN);
  test_assert([numMin int32Value] == INT16_MIN);
  test_assert([numMin int64Value] == INT16_MIN);
}

/**
 * @brief Test NXNumberUnsignedInt16 creation and basic functionality
 */
void test_nxnumber_unsigned_int16_creation(void) {
  // Test basic creation
  NXNumber *num1 = [NXNumber numberWithUnsignedInt16:42U];
  test_assert(num1 != nil);
  test_assert([num1 unsignedInt16Value] == 42U);
  test_assert([num1 unsignedInt32Value] == 42U);
  test_assert([num1 unsignedInt64Value] == 42U);

  // Test zero
  NXNumber *num2 = [NXNumber numberWithUnsignedInt16:0U];
  test_assert(num2 != nil);
  test_assert([num2 unsignedInt16Value] == 0U);
  test_assert([num2 unsignedInt32Value] == 0U);
  test_assert([num2 unsignedInt64Value] == 0U);

  // Test boundary values
  NXNumber *numMax = [NXNumber numberWithUnsignedInt16:UINT16_MAX];
  test_assert(numMax != nil);
  test_assert([numMax unsignedInt16Value] == UINT16_MAX);
  test_assert([numMax unsignedInt32Value] == UINT16_MAX);
  test_assert([numMax unsignedInt64Value] == UINT16_MAX);
}

/**
 * @brief Test cross-class equality between 16-bit and other number classes
 */
void test_nxnumber_16bit_equality(void) {
  // Test int16 vs int32 equality
  NXNumber *int16_42 = [NXNumber numberWithInt16:42];
  NXNumber *int32_42 = [NXNumber numberWithInt32:42];
  test_assert([int16_42 isEqual:int32_42]);
  test_assert([int32_42 isEqual:int16_42]);

  // Test int16 vs int64 equality
  NXNumber *int64_42 = [NXNumber numberWithInt64:42];
  test_assert([int16_42 isEqual:int64_42]);
  test_assert([int64_42 isEqual:int16_42]);

  // Test uint16 vs uint32 equality
  NXNumber *uint16_42 = [NXNumber numberWithUnsignedInt16:42U];
  NXNumber *uint32_42 = [NXNumber numberWithUnsignedInt32:42U];
  test_assert([uint16_42 isEqual:uint32_42]);
  test_assert([uint32_42 isEqual:uint16_42]);

  // Test uint16 vs uint64 equality
  NXNumber *uint64_42 = [NXNumber numberWithUnsignedInt64:42U];
  test_assert([uint16_42 isEqual:uint64_42]);
  test_assert([uint64_42 isEqual:uint16_42]);

  // Test int16 vs uint16 equality (positive values)
  test_assert([int16_42 isEqual:uint16_42]);
  test_assert([uint16_42 isEqual:int16_42]);

  // Test inequality
  NXNumber *int16_43 = [NXNumber numberWithInt16:43];
  test_assert(![int16_42 isEqual:int16_43]);
  test_assert(![int16_43 isEqual:int16_42]);

  // Test negative vs positive (should not be equal)
  NXNumber *int16_neg = [NXNumber numberWithInt16:-1];
  test_assert(![int16_42 isEqual:int16_neg]);
  test_assert(![int16_neg isEqual:int16_42]);
}

/**
 * @brief Test 16-bit number string descriptions
 */
void test_nxnumber_16bit_description(void) {
  // Test int16 descriptions
  NXNumber *int16_pos = [NXNumber numberWithInt16:123];
  NXString *desc1 = [int16_pos description];
  test_assert(desc1 != nil);
  test_assert(strcmp([desc1 cStr], "123") == 0);

  NXNumber *int16_neg = [NXNumber numberWithInt16:-456];
  NXString *desc2 = [int16_neg description];
  test_assert(desc2 != nil);
  test_assert(strcmp([desc2 cStr], "-456") == 0);

  // Test uint16 descriptions
  NXNumber *uint16_val = [NXNumber numberWithUnsignedInt16:789U];
  NXString *desc3 = [uint16_val description];
  test_assert(desc3 != nil);
  test_assert(strcmp([desc3 cStr], "789") == 0);

  NXNumber *uint16_max = [NXNumber numberWithUnsignedInt16:UINT16_MAX];
  NXString *desc4 = [uint16_max description];
  test_assert(desc4 != nil);
  test_assert(strcmp([desc4 cStr], "65535") == 0);

  // Test boundary values
  NXNumber *int16_max = [NXNumber numberWithInt16:INT16_MAX];
  NXString *desc5 = [int16_max description];
  test_assert(desc5 != nil);
  test_assert(strcmp([desc5 cStr], "32767") == 0);

  NXNumber *int16_min = [NXNumber numberWithInt16:INT16_MIN];
  NXString *desc6 = [int16_min description];
  test_assert(desc6 != nil);
  test_assert(strcmp([desc6 cStr], "-32768") == 0);
}

/**
 * @brief Test NXNumberZero functionality
 */
void test_nxnumber_zero(void) {
  // Test zero value creation
  NXNumber *zero = [NXNumber zeroValue];
  test_assert(zero != nil);

  // Test that multiple calls return the same singleton
  NXNumber *zero2 = [NXNumber zeroValue];
  test_assert(zero == zero2);

  // Test all accessor methods return 0
  test_assert([zero boolValue] == NO);
  test_assert([zero int16Value] == 0);
  test_assert([zero unsignedInt16Value] == 0);
  test_assert([zero int32Value] == 0);
  test_assert([zero unsignedInt32Value] == 0);
  test_assert([zero int64Value] == 0);
  test_assert([zero unsignedInt64Value] == 0);

  // Test string description
  NXString *desc = [zero description];
  test_assert(desc != nil);
  test_assert(strcmp([desc cStr], "0") == 0);

  // Test equality with other zero representations
  NXNumber *int16_zero = [NXNumber numberWithInt16:0];
  NXNumber *int32_zero = [NXNumber numberWithInt32:0];
  NXNumber *int64_zero = [NXNumber numberWithInt64:0];
  NXNumber *uint16_zero = [NXNumber numberWithUnsignedInt16:0];
  NXNumber *uint32_zero = [NXNumber numberWithUnsignedInt32:0];
  NXNumber *uint64_zero = [NXNumber numberWithUnsignedInt64:0];

  test_assert([zero isEqual:int16_zero]);
  test_assert([zero isEqual:int32_zero]);
  test_assert([zero isEqual:int64_zero]);
  test_assert([zero isEqual:uint16_zero]);
  test_assert([zero isEqual:uint32_zero]);
  test_assert([zero isEqual:uint64_zero]);

  // Test reverse equality
  test_assert([int16_zero isEqual:zero]);
  test_assert([int32_zero isEqual:zero]);
  test_assert([int64_zero isEqual:zero]);
  test_assert([uint16_zero isEqual:zero]);
  test_assert([uint32_zero isEqual:zero]);
  test_assert([uint64_zero isEqual:zero]);

  // Test inequality with non-zero values
  NXNumber *one = [NXNumber numberWithInt32:1];
  test_assert(![zero isEqual:one]);
  test_assert(![one isEqual:zero]);

  NXNumber *negOne = [NXNumber numberWithInt32:-1];
  test_assert(![zero isEqual:negOne]);
  test_assert(![negOne isEqual:zero]);
}

/**
 * @brief Test static value methods (trueValue, falseValue)
 */
void test_nxnumber_static_values(void) {
  // Test trueValue
  NXNumber *trueVal = [NXNumber trueValue];
  test_assert(trueVal != nil);
  test_assert([trueVal boolValue] == YES);
  test_assert([trueVal int64Value] == 1);
  test_assert([trueVal unsignedInt64Value] == 1);

  // Test multiple calls return same instance
  NXNumber *trueVal2 = [NXNumber trueValue];
  test_assert(trueVal == trueVal2);

  // Test falseValue
  NXNumber *falseVal = [NXNumber falseValue];
  test_assert(falseVal != nil);
  test_assert([falseVal boolValue] == NO);
  test_assert([falseVal int64Value] == 0);
  test_assert([falseVal unsignedInt64Value] == 0);

  // Test multiple calls return same instance
  NXNumber *falseVal2 = [NXNumber falseValue];
  test_assert(falseVal == falseVal2);

  // Test that falseValue and zeroValue are logically equal but may be different
  // instances
  NXNumber *zeroVal = [NXNumber zeroValue];
  test_assert([falseVal isEqual:zeroVal]); // Logically equal
  test_assert([zeroVal isEqual:falseVal]); // Reverse equality
  // Note: falseVal and zeroVal may be different singleton instances

  // Test equality
  test_assert([trueVal isEqual:[NXNumber numberWithBool:YES]]);
  test_assert([falseVal isEqual:[NXNumber numberWithBool:NO]]);
  test_assert([trueVal isEqual:[NXNumber numberWithInt64:1]]);
  test_assert([falseVal isEqual:[NXNumber numberWithInt64:0]]);

  // Test string descriptions
  NXString *trueDesc = [trueVal description];
  test_assert(trueDesc != nil);
  test_assert(strcmp([trueDesc cStr], "true") == 0);

  NXString *falseDesc = [falseVal description];
  test_assert(falseDesc != nil);
  test_assert(strcmp([falseDesc cStr], "false") == 0);
}

/**
 * @brief Test zero optimization singleton behavior across all factory methods
 */
void test_nxnumber_zero_optimization(void) {
  // Get the singleton zero reference
  NXNumber *zeroSingleton = [NXNumber zeroValue];

  // Test that all zero factory methods return the same singleton
  NXNumber *int16Zero = [NXNumber numberWithInt16:0];
  NXNumber *int32Zero = [NXNumber numberWithInt32:0];
  NXNumber *int64Zero = [NXNumber numberWithInt64:0];
  NXNumber *uint16Zero = [NXNumber numberWithUnsignedInt16:0];
  NXNumber *uint32Zero = [NXNumber numberWithUnsignedInt32:0];
  NXNumber *uint64Zero = [NXNumber numberWithUnsignedInt64:0];
  NXNumber *boolFalse = [NXNumber numberWithBool:NO];

  // Verify all zero instances are the same object (singleton optimization)
  test_assert(int16Zero == zeroSingleton);
  test_assert(int32Zero == zeroSingleton);
  test_assert(int64Zero == zeroSingleton);
  test_assert(uint16Zero == zeroSingleton);
  test_assert(uint32Zero == zeroSingleton);
  test_assert(uint64Zero == zeroSingleton);

  // Note: boolFalse may be a different singleton (NXNumberBool vs NXNumberZero)
  // but should be logically equal
  test_assert([boolFalse isEqual:zeroSingleton]);
  test_assert([zeroSingleton isEqual:boolFalse]);

  // Test that non-zero values create different instances
  NXNumber *one = [NXNumber numberWithInt32:1];
  NXNumber *anotherOne = [NXNumber numberWithInt32:1];
  test_assert(one != zeroSingleton);
  test_assert(anotherOne != zeroSingleton);
  test_assert(one != anotherOne); // Different instances for non-zero values
}

/**
 * @brief Test memory management behavior
 */
void test_nxnumber_memory_management(void) {
  // Test retain/release of singleton zero values
  NXNumber *zero = [NXNumber zeroValue];
  NXNumber *retainedZero = [zero retain];
  test_assert(retainedZero == zero); // Should return same instance
  [retainedZero release];            // Should be safe

  // Test retain/release of regular numbers
  NXNumber *num = [NXNumber numberWithInt32:42];
  NXNumber *retainedNum = [num retain];
  test_assert(retainedNum == num);
  [retainedNum release];

  // Note: Skipping autorelease tests to avoid double-autorelease issues
  // since factory methods already return autoreleased objects
}

/**
 * @brief Test class hierarchy and inheritance
 */
void test_nxnumber_class_hierarchy(void) {
  // Test base NXNumber class methods
  test_class_has_superclass("NXNumber", "NXObject");
  test_class_has_superclass("NXNumberBool", "NXNumber");
  test_class_has_superclass("NXNumberInt16", "NXNumber");
  test_class_has_superclass("NXNumberInt32", "NXNumber");
  test_class_has_superclass("NXNumberInt64", "NXNumber");
  test_class_has_superclass("NXNumberUnsignedInt16", "NXNumber");
  test_class_has_superclass("NXNumberUnsignedInt32", "NXNumber");
  test_class_has_superclass("NXNumberUnsignedInt64", "NXNumber");
  test_class_has_superclass("NXNumberZero", "NXNumber");

  // Test isKindOfClass behavior
  NXNumber *num = [NXNumber numberWithInt32:42];
  test_assert([num isKindOfClass:[NXNumber class]]);
  test_assert([num isKindOfClass:[NXObject class]]);

  NXNumber *zero = [NXNumber zeroValue];
  test_assert([zero isKindOfClass:[NXNumber class]]);
  test_assert([zero isKindOfClass:[NXObject class]]);

  // Test class identity
  test_assert([num class] != [zero class]); // Different concrete classes
}

/**
 * @brief Test random number generation functions
 */
void test_nxnumber_random_functions(void) {
  // Test NXRandInt32 - generate several values and verify they're in range
  for (int i = 0; i < 10; i++) {
    int32_t randInt = NXRandInt32();
    // Can't test for specific value, but can test it's a valid int32
    NXNumber *randNum = [NXNumber numberWithInt32:randInt];
    test_assert([randNum int32Value] == randInt);
  }

  // Test NXRandUnsignedInt32 - generate several values and verify they're in
  // range
  for (int i = 0; i < 10; i++) {
    uint32_t randUInt = NXRandUnsignedInt32();
    // Can't test for specific value, but can test it's a valid uint32
    NXNumber *randNum = [NXNumber numberWithUnsignedInt32:randUInt];
    test_assert([randNum unsignedInt32Value] == randUInt);
  }

  // Test that consecutive calls generally produce different values
  int32_t val1 = NXRandInt32();
  int32_t val2 = NXRandInt32();
  int32_t val3 = NXRandInt32();

  // Very unlikely that all three are the same (but not impossible)
  test_assert(!(val1 == val2 && val2 == val3));
}

/**
 * @brief Test comprehensive value type conversions and edge cases
 */
void test_nxnumber_conversion_edge_cases(void) {
  // Test signed to unsigned conversion edge cases
  NXNumber *maxInt32 = [NXNumber numberWithInt32:INT32_MAX];
  test_assert([maxInt32 unsignedInt32Value] == (uint32_t)INT32_MAX);
  test_assert([maxInt32 unsignedInt64Value] == (uint64_t)INT32_MAX);

  // Test unsigned to signed conversion edge cases
  NXNumber *maxUInt32 = [NXNumber numberWithUnsignedInt32:UINT32_MAX];
  test_assert([maxUInt32 unsignedInt64Value] == UINT32_MAX);
  test_assert([maxUInt32 int64Value] == (int64_t)UINT32_MAX);

  // Test value truncation from larger to smaller types (using values within
  // range)
  NXNumber *int64InRange =
      [NXNumber numberWithInt64:1000]; // Within int16 range
  int32_t truncated32 = [int64InRange int32Value];
  int16_t truncated16 = [int64InRange int16Value];
  test_assert(truncated32 == 1000);
  test_assert(truncated16 == 1000);

  // Test unsigned value truncation (using values within range)
  NXNumber *uint64InRange =
      [NXNumber numberWithUnsignedInt64:50000]; // Within uint16 range
  uint32_t utruncated32 = [uint64InRange unsignedInt32Value];
  uint16_t utruncated16 = [uint64InRange unsignedInt16Value];
  test_assert(utruncated32 == 50000);
  test_assert(utruncated16 == 50000);

  // Test boolean conversion edge cases
  test_assert([int64InRange boolValue] == YES);
  test_assert([uint64InRange boolValue] == YES);
  test_assert([[NXNumber numberWithInt32:-1] boolValue] == YES);
  test_assert([[NXNumber numberWithInt32:0] boolValue] == NO);

  // Test boundary values that should work
  NXNumber *int32Max = [NXNumber numberWithInt32:INT32_MAX];
  test_assert([int32Max int64Value] == INT32_MAX);

  NXNumber *uint32Max = [NXNumber numberWithUnsignedInt32:UINT32_MAX];
  test_assert([uint32Max unsignedInt64Value] == UINT32_MAX);
}

/**
 * @brief Test error handling and boundary conditions
 */
void test_nxnumber_error_conditions(void) {
  // Test nil equality handling
  NXNumber *num = [NXNumber numberWithInt32:42];
  test_assert(![num isEqual:nil]);

  // Test equality with non-number objects
  NXString *str = [NXString stringWithCString:"42"];
  test_assert(![num isEqual:str]);

  // Test self-equality for all types
  test_assert([num isEqual:num]);

  NXNumber *zero = [NXNumber zeroValue];
  test_assert([zero isEqual:zero]);

  // Test description methods don't return nil
  test_assert([num description] != nil);
  test_assert([zero description] != nil);
  test_assert([[NXNumber numberWithBool:YES] description] != nil);
  test_assert([[NXNumber numberWithBool:NO] description] != nil);
}

/**
 * @brief Main test function for NXNumber functionality
 */
int test_nxnumber_methods(void) {
  // Create a zone and autorelease pool first
  NXZone *zone = [NXZone zoneWithSize:1024 * 1024]; // 1MB zone
  if (zone == nil) {
    sys_panicf("Failed to create zone");
    return 1;
  }

  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];
  if (pool == nil) {
    sys_panicf("Failed to create autorelease pool");
    return 1;
  }

  test_nxnumber_creation();
  test_nxnumber_int64_values();
  test_nxnumber_unsigned_int64_values();
  test_nxnumber_bool_conversion();
  test_nxnumber_zero_singleton();
  test_nxnumber_equality();
  test_nxnumber_description();
  test_nxnumber_edge_cases();

  // Test 32-bit number classes
  test_nxnumber_int32_creation();
  test_nxnumber_unsigned_int32_creation();
  test_nxnumber_32bit_equality();
  test_nxnumber_32bit_description();

  // Test 16-bit number classes
  test_nxnumber_int16_creation();
  test_nxnumber_unsigned_int16_creation();
  test_nxnumber_16bit_equality();
  test_nxnumber_16bit_description();

  // Test zero value class
  test_nxnumber_zero();

  // New comprehensive tests
  test_nxnumber_static_values();
  test_nxnumber_zero_optimization();
  test_nxnumber_memory_management();
  test_nxnumber_class_hierarchy();
  test_nxnumber_random_functions();
  test_nxnumber_conversion_edge_cases();
  test_nxnumber_error_conditions();

  [pool release];
  [zone release];
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) { return TestMain("NXFoundation_19", test_nxnumber_methods); }
