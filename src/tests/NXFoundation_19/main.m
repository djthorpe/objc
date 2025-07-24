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

  [pool release];
  [zone release];
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) { return TestMain("NXFoundation_19", test_nxnumber_methods); }
