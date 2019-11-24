////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibTests
//
//  Unit tests for JsonLib - Unmarshall
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include "JsonLib.h"
#include "JsonLibTests.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  SETTINGS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
    #pragma warning( disable : 4221 )
    #pragma warning( disable : 4204 )
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NumElements( Array )    ( sizeof(Array) / sizeof((Array)[0]) )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  DoublesAreEqual
//
//  Returns true if the two Values are close enough.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
bool
    FloatsAreEqual
    (
        double      Value1,             // [in]
        double      Value2              // [in]
    )
{
    double diff = fabs( Value1 - Value2 );
    double reldiff = diff / fabs( Value1 );

    // Note using FLT_EPSILON not DBL_EPSILON because floats may be passed to this function
    if( reldiff <= FLT_EPSILON )
    {
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TEST FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallNumbers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallNumbers
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct
    {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        uint8_t u8h;
        uint16_t u16h;
        uint32_t u32h;
        uint64_t u64h;
        int8_t s8;
        int16_t s16;
        int32_t s32;
        int64_t s64;
        float f32;
        double f64;
    } NumberStruct;

    JlMarshallElement marshalNumberStruct[] =
    {
        JlMarshallUnsigned( NumberStruct, u8, "u8" ),
        JlMarshallUnsigned( NumberStruct, u16, "u16" ),
        JlMarshallUnsigned( NumberStruct, u32, "u32" ),
        JlMarshallUnsigned( NumberStruct, u64, "u64" ),
        JlMarshallUnsignedHex( NumberStruct, u8h, "u8h" ),
        JlMarshallUnsignedHex( NumberStruct, u16h, "u16h" ),
        JlMarshallUnsignedHex( NumberStruct, u32h, "u32h" ),
        JlMarshallUnsignedHex( NumberStruct, u64h, "u64h" ),
        JlMarshallSigned( NumberStruct, s8, "s8" ),
        JlMarshallSigned( NumberStruct, s16, "s16" ),
        JlMarshallSigned( NumberStruct, s32, "s32" ),
        JlMarshallSigned( NumberStruct, s64, "s64" ),
        JlMarshallFloat( NumberStruct, f32, "f32" ),
        JlMarshallFloat( NumberStruct, f64, "f64" ),
    };
    uint32_t const marshalNumberStructCount = sizeof(marshalNumberStruct) / sizeof(marshalNumberStruct[0]);

    NumberStruct struct1 = {0};
    size_t errorAtPos = 0;

    // Try a set of numbers that would fit any type
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{\"u8\":100,\"u16\":101,\"u32\":102,\"u64\":103,\"s8\":104,\"s16\":105,\"s32\":106,\"s64\":107,\"f32\":108,\"f64\":109}",
        marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 100 == struct1.u8 );
    JL_ASSERT( 101 == struct1.u16 );
    JL_ASSERT( 102 == struct1.u32 );
    JL_ASSERT( 103 == struct1.u64 );
    JL_ASSERT( 104 == struct1.s8 );
    JL_ASSERT( 105 == struct1.s16 );
    JL_ASSERT( 106 == struct1.s32 );
    JL_ASSERT( 107 == struct1.s64 );
    JL_ASSERT( 108.0 == struct1.f32 );
    JL_ASSERT( 109.0 == struct1.f64 );

    // Try a set that goes to the extremes of each type
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{\"u8\":255,\"u16\":65535,\"u32\":4294967295,\"u64\":18446744073709551615,\"s8\":-127,\"s16\":-32767,\"s32\":-2147483647,\"s64\":-9223372036854775807,\"f32\":1e+37,\"f64\":1e+307}",
        marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 255 == struct1.u8 );
    JL_ASSERT( 65535 == struct1.u16 );
    JL_ASSERT( 4294967295 == struct1.u32 );
    JL_ASSERT( 18446744073709551615ULL == struct1.u64 );
    JL_ASSERT( -127 == struct1.s8 );
    JL_ASSERT( -32767 == struct1.s16 );
    JL_ASSERT( -2147483647 == struct1.s32 );
    JL_ASSERT( -9223372036854775807 == struct1.s64 );
    JL_ASSERT( FloatsAreEqual( 1e37, struct1.f32 ) );
    JL_ASSERT( FloatsAreEqual( 1e307, struct1.f64 ) );

    // Try numbers too big for their containers
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u8\": 256 }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u8\": -1 }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u8\": 1.1 }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u16\": 65536 }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u32\": 18446744073709551616 }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s8\": 128 }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s8\": -129 }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s16\": 32768 }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s32\": 9223372036854775807 }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s64\": 2e+308 }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_INVALID_DATA );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s64\": -2e+308 }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_INVALID_DATA );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s32\": 4e+38 }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s32\": -4e+38 }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;

    // Try entries that aren't numbers
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u32\": \"Not a number\" }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_WRONG_TYPE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u32\": null }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_WRONG_TYPE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u32\": true }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_WRONG_TYPE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u32\": false }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_WRONG_TYPE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u32\": { \"a\":1 } }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_WRONG_TYPE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u32\": [ 1 ] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_WRONG_TYPE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;

    // Try Json5 with hex
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{u8:100,u16:101,u32:102,u64:103,u8h:0x10,u16h:0x101,u32h:0x102,u64h:0x103}",
        marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 100 == struct1.u8 );
    JL_ASSERT( 101 == struct1.u16 );
    JL_ASSERT( 102 == struct1.u32 );
    JL_ASSERT( 103 == struct1.u64 );
    JL_ASSERT( 0x10 == struct1.u8h );
    JL_ASSERT( 0x101 == struct1.u16h );
    JL_ASSERT( 0x102 == struct1.u32h );
    JL_ASSERT( 0x103 == struct1.u64h );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallArraysOfNumbers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallArraysOfNumbers
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct
    {
        uint8_t* ArrayU8;
        size_t ArrayU8Count;
        uint16_t* ArrayU16;
        size_t ArrayU16Count;
        uint32_t* ArrayU32;
        size_t ArrayU32Count;
        uint64_t* ArrayU64;
        size_t ArrayU64Count;
        int8_t* ArrayS8;
        size_t ArrayS8Count;
        int16_t* ArrayS16;
        size_t ArrayS16Count;
        int32_t* ArrayS32;
        size_t ArrayS32Count;
        int64_t* ArrayS64;
        size_t ArrayS64Count;
        float* ArrayF32;
        size_t ArrayF32Count;
        double* ArrayF64;
        size_t ArrayF64Count;
    } NumberStruct;

    JlMarshallElement marshalNumberStruct[] =
    {
        JlMarshallUnsignedArray( NumberStruct, ArrayU8, ArrayU8Count, "u8" ),
        JlMarshallUnsignedArray( NumberStruct, ArrayU16, ArrayU16Count, "u16" ),
        JlMarshallUnsignedArray( NumberStruct, ArrayU32, ArrayU32Count, "u32" ),
        JlMarshallUnsignedArray( NumberStruct, ArrayU64, ArrayU64Count, "u64" ),
        JlMarshallSignedArray( NumberStruct, ArrayS8, ArrayS8Count, "s8" ),
        JlMarshallSignedArray( NumberStruct, ArrayS16, ArrayS16Count, "s16" ),
        JlMarshallSignedArray( NumberStruct, ArrayS32, ArrayS32Count, "s32" ),
        JlMarshallSignedArray( NumberStruct, ArrayS64, ArrayS64Count, "s64" ),
        JlMarshallFloatArray( NumberStruct, ArrayF32, ArrayF32Count, "f32" ),
        JlMarshallFloatArray( NumberStruct, ArrayF64, ArrayF64Count, "f64" ),
    };
    uint32_t const marshalNumberStructCount = sizeof(marshalNumberStruct) / sizeof(marshalNumberStruct[0]);

    NumberStruct struct1 = {0};
    size_t errorAtPos = 0;

    // Try a set of numbers that would fit any type
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{\"u8\":[100,101],\"u16\":[102,103],\"u32\":[104,105],\"u64\":[106,107],\"s8\":[108,109],\"s16\":[110,111],\"s32\":[112,113],\"s64\":[114,115],\"f32\":[116,117],\"f64\":[118,119]}",
        marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 2 == struct1.ArrayU8Count );
    JL_ASSERT( 100 == struct1.ArrayU8[0] );
    JL_ASSERT( 101 == struct1.ArrayU8[1] );
    JL_ASSERT( 2 == struct1.ArrayU16Count );
    JL_ASSERT( 102 == struct1.ArrayU16[0] );
    JL_ASSERT( 103 == struct1.ArrayU16[1] );
    JL_ASSERT( 2 == struct1.ArrayU32Count );
    JL_ASSERT( 104 == struct1.ArrayU32[0] );
    JL_ASSERT( 105 == struct1.ArrayU32[1] );
    JL_ASSERT( 2 == struct1.ArrayU64Count );
    JL_ASSERT( 106 == struct1.ArrayU64[0] );
    JL_ASSERT( 107 == struct1.ArrayU64[1] );
    JL_ASSERT( 2 == struct1.ArrayS8Count );
    JL_ASSERT( 108 == struct1.ArrayS8[0] );
    JL_ASSERT( 109 == struct1.ArrayS8[1] );
    JL_ASSERT( 2 == struct1.ArrayS16Count );
    JL_ASSERT( 110 == struct1.ArrayS16[0] );
    JL_ASSERT( 111 == struct1.ArrayS16[1] );
    JL_ASSERT( 2 == struct1.ArrayS32Count );
    JL_ASSERT( 112 == struct1.ArrayS32[0] );
    JL_ASSERT( 113 == struct1.ArrayS32[1] );
    JL_ASSERT( 2 == struct1.ArrayS64Count );
    JL_ASSERT( 114 == struct1.ArrayS64[0] );
    JL_ASSERT( 115 == struct1.ArrayS64[1] );
    JL_ASSERT( 2 == struct1.ArrayF32Count );
    JL_ASSERT( 116 == struct1.ArrayF32[0] );
    JL_ASSERT( 117 == struct1.ArrayF32[1] );
    JL_ASSERT( 2 == struct1.ArrayF64Count );
    JL_ASSERT( 118 == struct1.ArrayF64[0] );
    JL_ASSERT( 119 == struct1.ArrayF64[1] );
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalNumberStruct, marshalNumberStructCount, &struct1 ) );

    // Try a set of numbers that wouldn't fit in a smaller type
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{\"u8\":[100],\"u16\":[1000],\"u32\":[100000],\"u64\":[5000000000],\"s8\":[-100],\"s16\":[-1000],\"s32\":[-100000],\"s64\":[-5000000000],\"f32\":[1.12345],\"f64\":[1.12345678912345]}",
        marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 1 == struct1.ArrayU8Count );
    JL_ASSERT( 100 == struct1.ArrayU8[0] );
    JL_ASSERT( 1 == struct1.ArrayU16Count );
    JL_ASSERT( 1000 == struct1.ArrayU16[0] );
    JL_ASSERT( 1 == struct1.ArrayU32Count );
    JL_ASSERT( 100000 == struct1.ArrayU32[0] );
    JL_ASSERT( 1 == struct1.ArrayU64Count );
    JL_ASSERT( 5000000000 == struct1.ArrayU64[0] );
    JL_ASSERT( 1 == struct1.ArrayS8Count );
    JL_ASSERT( -100 == struct1.ArrayS8[0] );
    JL_ASSERT( 1 == struct1.ArrayS16Count );
    JL_ASSERT( -1000 == struct1.ArrayS16[0] );
    JL_ASSERT( 1 == struct1.ArrayS32Count );
    JL_ASSERT( -100000 == struct1.ArrayS32[0] );
    JL_ASSERT( 1 == struct1.ArrayS64Count );
    JL_ASSERT( -5000000000 == struct1.ArrayS64[0] );
    JL_ASSERT( 1 == struct1.ArrayF32Count );
    JL_ASSERT( FloatsAreEqual( struct1.ArrayF32[0], 1.12345 ) );
    JL_ASSERT( 1 == struct1.ArrayF64Count );
    JL_ASSERT( FloatsAreEqual( struct1.ArrayF64[0], 1.12345678912345 ) );
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalNumberStruct, marshalNumberStructCount, &struct1 ) );

    // Try numbers too big for their containers
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u8\":[256] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u8\":[-1] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u8\":[1.1] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u16\":[65536] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u32\":[18446744073709551616] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s8\":[128] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s8\":[-129] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s16\":[32768] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s32\":[9223372036854775807] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s64\":[2e+308] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_INVALID_DATA );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s64\":[-2e+308] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_INVALID_DATA );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s32\":[4e+38] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s32\":[-4e+38] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallFixedArraysOfNumbers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallFixedArraysOfNumbers
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct
    {
        uint8_t ArrayU8[2];
        size_t ArrayU8Count;
        uint16_t ArrayU16[2];
        size_t ArrayU16Count;
        uint32_t ArrayU32[2];
        size_t ArrayU32Count;
        uint64_t ArrayU64[2];
        size_t ArrayU64Count;
        int8_t ArrayS8[2];
        size_t ArrayS8Count;
        int16_t ArrayS16[2];
        size_t ArrayS16Count;
        int32_t ArrayS32[2];
        size_t ArrayS32Count;
        int64_t ArrayS64[2];
        size_t ArrayS64Count;
        float ArrayF32[2];
        size_t ArrayF32Count;
        double ArrayF64[2];
        size_t ArrayF64Count;
    } NumberStruct;

    JlMarshallElement marshalNumberStruct[] =
    {
        JlMarshallUnsignedFixedArray( NumberStruct, ArrayU8, ArrayU8Count, "u8" ),
        JlMarshallUnsignedFixedArray( NumberStruct, ArrayU16, ArrayU16Count, "u16" ),
        JlMarshallUnsignedFixedArray( NumberStruct, ArrayU32, ArrayU32Count, "u32" ),
        JlMarshallUnsignedFixedArray( NumberStruct, ArrayU64, ArrayU64Count, "u64" ),
        JlMarshallSignedFixedArray( NumberStruct, ArrayS8, ArrayS8Count, "s8" ),
        JlMarshallSignedFixedArray( NumberStruct, ArrayS16, ArrayS16Count, "s16" ),
        JlMarshallSignedFixedArray( NumberStruct, ArrayS32, ArrayS32Count, "s32" ),
        JlMarshallSignedFixedArray( NumberStruct, ArrayS64, ArrayS64Count, "s64" ),
        JlMarshallFloatFixedArray( NumberStruct, ArrayF32, ArrayF32Count, "f32" ),
        JlMarshallFloatFixedArray( NumberStruct, ArrayF64, ArrayF64Count, "f64" ),
    };
    uint32_t const marshalNumberStructCount = sizeof(marshalNumberStruct) / sizeof(marshalNumberStruct[0]);

    NumberStruct struct1 = {{0}};
    size_t errorAtPos = 0;

    // Try just float numbers
    JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"f32\":[116,117]}", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 116 == struct1.ArrayF32[0] );
    JL_ASSERT( 117 == struct1.ArrayF32[1] );

    // Try a set of numbers that would fit any type
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{\"u8\":[100,101],\"u16\":[102,103],\"u32\":[104,105],\"u64\":[106,107],\"s8\":[108,109],\"s16\":[110,111],\"s32\":[112,113],\"s64\":[114,115],\"f32\":[116,117],\"f64\":[118,119]}",
        marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 2 == struct1.ArrayU8Count );
    JL_ASSERT( 100 == struct1.ArrayU8[0] );
    JL_ASSERT( 101 == struct1.ArrayU8[1] );
    JL_ASSERT( 2 == struct1.ArrayU16Count );
    JL_ASSERT( 102 == struct1.ArrayU16[0] );
    JL_ASSERT( 103 == struct1.ArrayU16[1] );
    JL_ASSERT( 2 == struct1.ArrayU32Count );
    JL_ASSERT( 104 == struct1.ArrayU32[0] );
    JL_ASSERT( 105 == struct1.ArrayU32[1] );
    JL_ASSERT( 2 == struct1.ArrayU64Count );
    JL_ASSERT( 106 == struct1.ArrayU64[0] );
    JL_ASSERT( 107 == struct1.ArrayU64[1] );
    JL_ASSERT( 2 == struct1.ArrayS8Count );
    JL_ASSERT( 108 == struct1.ArrayS8[0] );
    JL_ASSERT( 109 == struct1.ArrayS8[1] );
    JL_ASSERT( 2 == struct1.ArrayS16Count );
    JL_ASSERT( 110 == struct1.ArrayS16[0] );
    JL_ASSERT( 111 == struct1.ArrayS16[1] );
    JL_ASSERT( 2 == struct1.ArrayS32Count );
    JL_ASSERT( 112 == struct1.ArrayS32[0] );
    JL_ASSERT( 113 == struct1.ArrayS32[1] );
    JL_ASSERT( 2 == struct1.ArrayS64Count );
    JL_ASSERT( 114 == struct1.ArrayS64[0] );
    JL_ASSERT( 115 == struct1.ArrayS64[1] );
    JL_ASSERT( 2 == struct1.ArrayF32Count );
    JL_ASSERT( 116 == struct1.ArrayF32[0] );
    JL_ASSERT( 117 == struct1.ArrayF32[1] );
    JL_ASSERT( 2 == struct1.ArrayF64Count );
    JL_ASSERT( 118 == struct1.ArrayF64[0] );
    JL_ASSERT( 119 == struct1.ArrayF64[1] );
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalNumberStruct, marshalNumberStructCount, &struct1 ) );

    // Try a set of numbers that wouldn't fit in a smaller type
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{\"u8\":[100],\"u16\":[1000],\"u32\":[100000],\"u64\":[5000000000],\"s8\":[-100],\"s16\":[-1000],\"s32\":[-100000],\"s64\":[-5000000000],\"f32\":[1.12345],\"f64\":[1.12345678912345]}",
        marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 1 == struct1.ArrayU8Count );
    JL_ASSERT( 100 == struct1.ArrayU8[0] );
    JL_ASSERT( 1 == struct1.ArrayU16Count );
    JL_ASSERT( 1000 == struct1.ArrayU16[0] );
    JL_ASSERT( 1 == struct1.ArrayU32Count );
    JL_ASSERT( 100000 == struct1.ArrayU32[0] );
    JL_ASSERT( 1 == struct1.ArrayU64Count );
    JL_ASSERT( 5000000000 == struct1.ArrayU64[0] );
    JL_ASSERT( 1 == struct1.ArrayS8Count );
    JL_ASSERT( -100 == struct1.ArrayS8[0] );
    JL_ASSERT( 1 == struct1.ArrayS16Count );
    JL_ASSERT( -1000 == struct1.ArrayS16[0] );
    JL_ASSERT( 1 == struct1.ArrayS32Count );
    JL_ASSERT( -100000 == struct1.ArrayS32[0] );
    JL_ASSERT( 1 == struct1.ArrayS64Count );
    JL_ASSERT( -5000000000 == struct1.ArrayS64[0] );
    JL_ASSERT( 1 == struct1.ArrayF32Count );
    JL_ASSERT( FloatsAreEqual( struct1.ArrayF32[0], 1.12345 ) );
    JL_ASSERT( 1 == struct1.ArrayF64Count );
    JL_ASSERT( FloatsAreEqual( struct1.ArrayF64[0], 1.12345678912345 ) );
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalNumberStruct, marshalNumberStructCount, &struct1 ) );

    // Try numbers too big for their containers
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u8\":[256] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u8\":[-1] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u8\":[1.1] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u16\":[65536] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"u32\":[18446744073709551616] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s8\":[128] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s8\":[-129] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 8 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s16\":[32768] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s32\":[9223372036854775807] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s64\":[2e+308] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_INVALID_DATA );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s64\":[-2e+308] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_INVALID_DATA );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s32\":[4e+38] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;
    JL_ASSERT_STATUS( JlJsonToStruct( "{ \"s32\":[-4e+38] }", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_VALUE_OUT_OF_RANGE );
    JL_ASSERT( 9 == errorAtPos );  errorAtPos = 0;

    // Test lists too big for fixed arrays.
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"u8\":[1,1,1]}", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_TOO_MANY_ITEMS );
    JL_ASSERT( 6 == errorAtPos );
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"u16\":[1,1,1]}", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_TOO_MANY_ITEMS );
    JL_ASSERT( 7 == errorAtPos );
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"u32\":[1,1,1]}", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_TOO_MANY_ITEMS );
    JL_ASSERT( 7 == errorAtPos );
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"u64\":[1,1,1]}", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_TOO_MANY_ITEMS );
    JL_ASSERT( 7 == errorAtPos );
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"s8\":[1,1,1]}", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_TOO_MANY_ITEMS );
    JL_ASSERT( 6 == errorAtPos );
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"s16\":[1,1,1]}", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_TOO_MANY_ITEMS );
    JL_ASSERT( 7 == errorAtPos );
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"s32\":[1,1,1]}", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_TOO_MANY_ITEMS );
    JL_ASSERT( 7 == errorAtPos );
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"s64\":[1,1,1]}", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_TOO_MANY_ITEMS );
    JL_ASSERT( 7 == errorAtPos );
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"f32\":[1,1,1]}", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_TOO_MANY_ITEMS );
    JL_ASSERT( 7 == errorAtPos );
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"f64\":[1,1,1]}", marshalNumberStruct, marshalNumberStructCount, &struct1, &errorAtPos ), JL_STATUS_TOO_MANY_ITEMS );
    JL_ASSERT( 7 == errorAtPos );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallBools
//
//  Tests Bools and Arrays of bools
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallBools
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct
    {
        bool b1;
        bool b2;
        bool* allocatedBoolArray;
        size_t allocatedBoolArrayCount;
        bool fixedBoolArray[3];
        size_t fixedBoolArrayCount;
    } BoolStruct;

    JlMarshallElement marshalBoolStruct[] =
    {
        JlMarshallBool( BoolStruct, b1, "b1" ),
        JlMarshallBool( BoolStruct, b2, "b2" ),
        JlMarshallBoolArray( BoolStruct, allocatedBoolArray, allocatedBoolArrayCount, "bools1" ),
        JlMarshallBoolFixedArray( BoolStruct, fixedBoolArray, fixedBoolArrayCount, "bools2" ),
    };
    uint32_t const marshalBoolStructCount = sizeof(marshalBoolStruct) / sizeof(marshalBoolStruct[0]);

    BoolStruct struct1 = {0};
    size_t errorAtPos = 0;

    // Try success
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{\"b1\":true,\"b2\":false,\"bools1\":[true,false,true],\"bools2\":[true,false,true]}",
        marshalBoolStruct, marshalBoolStructCount, &struct1, &errorAtPos ) );
    JL_ASSERT( true == struct1.b1 );
    JL_ASSERT( false == struct1.b2 );
    JL_ASSERT_NOT_NULL( struct1.allocatedBoolArray );
    JL_ASSERT( 3 == struct1.allocatedBoolArrayCount );
    JL_ASSERT( true == struct1.allocatedBoolArray[0] );
    JL_ASSERT( false == struct1.allocatedBoolArray[1] );
    JL_ASSERT( true == struct1.allocatedBoolArray[2] );
    JL_ASSERT( 3 == struct1.fixedBoolArrayCount );
    JL_ASSERT( true == struct1.fixedBoolArray[0] );
    JL_ASSERT( false == struct1.fixedBoolArray[1] );
    JL_ASSERT( true == struct1.fixedBoolArray[2] );
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalBoolStruct, marshalBoolStructCount, &struct1 ) );

    // Try invalid values
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"b1\":null}", marshalBoolStruct, marshalBoolStructCount, &struct1, &errorAtPos ), JL_STATUS_WRONG_TYPE );
    JL_ASSERT( 6 == errorAtPos );
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"b1\":0}", marshalBoolStruct, marshalBoolStructCount, &struct1, &errorAtPos ), JL_STATUS_WRONG_TYPE );
    JL_ASSERT( 6 == errorAtPos );
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"b1\":[true]}", marshalBoolStruct, marshalBoolStructCount, &struct1, &errorAtPos ), JL_STATUS_WRONG_TYPE );
    JL_ASSERT( 6 == errorAtPos );

    // Try invalid value in list
    struct1 = (BoolStruct){0};
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"bools1\":[true, 0]}", marshalBoolStruct, marshalBoolStructCount, &struct1, &errorAtPos ), JL_STATUS_WRONG_TYPE );
    JL_ASSERT( 17 == errorAtPos );

    // Try a list too big for the fixed array
    struct1 = (BoolStruct){0};
    JL_ASSERT_STATUS( JlJsonToStruct( "{\"bools2\":[true,true,true,true]}", marshalBoolStruct, marshalBoolStructCount, &struct1, &errorAtPos ), JL_STATUS_TOO_MANY_ITEMS );
    JL_ASSERT( 10 == errorAtPos );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallStrings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallStrings
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct
    {
        char fixedStr[10];
        char* strPtr;
    } StringStruct;

    JlMarshallElement marshalStringStruct[] =
    {
        JlMarshallStringFixed( StringStruct, fixedStr, "fixedStr" ),
        JlMarshallString( StringStruct, strPtr, "strPtr" ),
    };
    uint32_t const marshalStringStructCount = sizeof(marshalStringStruct) / sizeof(marshalStringStruct[0]);

    StringStruct struct1 = {{0}};
    size_t errorAtPos = 0;

    // Test success
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{\"fixedStr\": \"abcdef\", \"strPtr\": \"ghijklm\" }",
        marshalStringStruct, marshalStringStructCount, &struct1, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( strcmp( struct1.fixedStr, "abcdef" ) == 0 );
    JL_ASSERT_NOT_NULL( struct1.strPtr );
    JL_ASSERT( strcmp( struct1.strPtr, "ghijklm" ) == 0 );

    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalStringStruct, marshalStringStructCount, &struct1 ) );

    // Test no strings
    struct1 = (StringStruct){{0}};
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{ \"unrelated\" : 1234 }",
        marshalStringStruct, marshalStringStructCount, &struct1, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 0 == struct1.fixedStr[0] );
    JL_ASSERT_NULL( struct1.strPtr );

    // Test string too large for fixed container
    JL_ASSERT_STATUS( JlJsonToStruct(
        "{\"fixedStr\": \"12345678901234567890\" }",
        marshalStringStruct, marshalStringStructCount, &struct1, &errorAtPos ), JL_STATUS_BUFFER_TOO_SMALL );
    JL_ASSERT( 13 == errorAtPos );
    JL_ASSERT( 0 == struct1.fixedStr[0] );
    JL_ASSERT_NULL( struct1.strPtr );

    // Test that when string is rejected there aren't already other items stored in struct
    struct1 = (StringStruct){{0}};
    JL_ASSERT_STATUS( JlJsonToStruct(
        "{\"strPtr\":\"a\", \"fixedStr\": \"12345678901234567890\" }",
        marshalStringStruct, marshalStringStructCount, &struct1, &errorAtPos ), JL_STATUS_BUFFER_TOO_SMALL );
    JL_ASSERT( 27 == errorAtPos );
    JL_ASSERT( 0 == struct1.fixedStr[0] );
    JL_ASSERT_NULL( struct1.strPtr );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallArraysOfStrings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallArraysOfStrings
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct
    {
        char* StringFixedArray1[4];
        uint16_t StringFixedArray1Count;
        char* StringFixedArray2[4];
        uint32_t StringFixedArray2Count;
        char** StringArray;
        uint64_t StringArrayCount;
    } StringStruct;

    JlMarshallElement marshalStringStruct[] =
    {
        JlMarshallStringFixedArray( StringStruct, StringFixedArray1, StringFixedArray1Count, "StrsF1" ),
        JlMarshallStringFixedArray( StringStruct, StringFixedArray2, StringFixedArray2Count, "StrsF2" ),
        JlMarshallStringArray( StringStruct, StringArray, StringArrayCount, "Strs" ),
    };
    uint32_t const marshalStringStructCount = sizeof(marshalStringStruct) / sizeof(marshalStringStruct[0]);

    StringStruct struct1 = {{0}};
    size_t errorAtPos = 0;

    // Test success
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{\"StrsF1\": [ \"abc\", \"def\", \"ghi\" ], \"StrsF2\":[\"aa\",\"bb\",\"cc\",\"dd\"], \"Strs\":[\"a\",\"b\",\"c\",\"d\",\"e\"] }",
        marshalStringStruct, marshalStringStructCount, &struct1, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 3 == struct1.StringFixedArray1Count );
    JL_ASSERT( strcmp( struct1.StringFixedArray1[0], "abc" ) == 0 );
    JL_ASSERT( strcmp( struct1.StringFixedArray1[1], "def" ) == 0 );
    JL_ASSERT( strcmp( struct1.StringFixedArray1[2], "ghi" ) == 0 );
    JL_ASSERT_NULL( struct1.StringFixedArray1[3] );

    JL_ASSERT( 4 == struct1.StringFixedArray2Count );
    JL_ASSERT( strcmp( struct1.StringFixedArray2[0], "aa" ) == 0 );
    JL_ASSERT( strcmp( struct1.StringFixedArray2[1], "bb" ) == 0 );
    JL_ASSERT( strcmp( struct1.StringFixedArray2[2], "cc" ) == 0 );
    JL_ASSERT( strcmp( struct1.StringFixedArray2[3], "dd" ) == 0 );

    JL_ASSERT( 5 == struct1.StringArrayCount );
    JL_ASSERT_NOT_NULL( struct1.StringArray );
    JL_ASSERT( strcmp( struct1.StringArray[0], "a" ) == 0 );
    JL_ASSERT( strcmp( struct1.StringArray[1], "b" ) == 0 );
    JL_ASSERT( strcmp( struct1.StringArray[2], "c" ) == 0 );
    JL_ASSERT( strcmp( struct1.StringArray[3], "d" ) == 0 );
    JL_ASSERT( strcmp( struct1.StringArray[4], "e" ) == 0 );

    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalStringStruct, marshalStringStructCount, &struct1 ) );

    // Test too many strings for fixed array
    JL_ASSERT_STATUS( JlJsonToStruct(
        "{\"StrsF1\": [ \"abc\", \"def\", \"ghi\", \"jkl\", \"mno\" ] }",
        marshalStringStruct, marshalStringStructCount, &struct1, &errorAtPos ), JL_STATUS_TOO_MANY_ITEMS );
    JL_ASSERT( 11 == errorAtPos );

    // Test no strings. We are still providing the arrays (empty). This should wipe out the existing ones in the
    // struct, so we first fill them
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{\"StrsF1\": [ \"a\",\"a\",\"a\",\"a\" ], \"StrsF2\":[\"a\",\"a\",\"a\",\"a\"], \"Strs\":[\"a\",\"a\",\"a\",\"a\",\"a\"] }",
        marshalStringStruct, marshalStringStructCount, &struct1, &errorAtPos ) );

    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{\"StrsF1\": [ ], \"StrsF2\":[ ], \"Strs\":[ ] }",
        marshalStringStruct, marshalStringStructCount, &struct1, &errorAtPos ) );
    JL_ASSERT( 0 == struct1.StringFixedArray1Count );
    JL_ASSERT_NULL( struct1.StringFixedArray1[0] );
    JL_ASSERT_NULL( struct1.StringFixedArray1[1] );
    JL_ASSERT_NULL( struct1.StringFixedArray1[2] );
    JL_ASSERT_NULL( struct1.StringFixedArray1[3] );
    JL_ASSERT( 0 == struct1.StringFixedArray2Count );
    JL_ASSERT_NULL( struct1.StringFixedArray2[0] );
    JL_ASSERT_NULL( struct1.StringFixedArray2[1] );
    JL_ASSERT_NULL( struct1.StringFixedArray2[2] );
    JL_ASSERT_NULL( struct1.StringFixedArray2[3] );
    JL_ASSERT( 0 == struct1.StringArrayCount );
    JL_ASSERT_NULL( struct1.StringArray );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallArraysCounts1
//
//  Verify that the count field is handled correctly at every size.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallArraysCounts1
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct { uint8_t Bytes[8]; } Bytes8;

    // Note we put each Count field in a a union with a 64bit field. This allows us to verify that only bytes
    // of the count field are updated and do not overwrite past their element.
    typedef struct
    {
        char*       FA1[4];
        union{
          uint8_t     FA1Count;
          Bytes8      VerifyFA1Count; };
        char*       FA2[4];
        union{
          uint16_t    FA2Count;
          Bytes8      VerifyFA2Count; };
        char*       FA3[4];
        union{
          uint32_t    FA3Count;
          Bytes8      VerifyFA3Count; };
        char*       FA4[4];
        uint64_t    FA4Count;

        char**      VA1;
        union{
          uint8_t     VA1Count;
          Bytes8      VerifyVA1Count; };
        char**      VA2;
        union{
          uint16_t    VA2Count;
          Bytes8      VerifyVA2Count; };
        char**      VA3;
        union{
          uint32_t    VA3Count;
          Bytes8      VerifyVA3Count; };
        char**      VA4;
        uint64_t    VA4Count;
    } TheStruct;

    JlMarshallElement marshalTheStruct[] =
    {
        JlMarshallStringFixedArray( TheStruct, FA1, FA1Count, "FA1" ),
        JlMarshallStringFixedArray( TheStruct, FA2, FA2Count, "FA2" ),
        JlMarshallStringFixedArray( TheStruct, FA3, FA3Count, "FA3" ),
        JlMarshallStringFixedArray( TheStruct, FA4, FA4Count, "FA4" ),
        JlMarshallStringArray( TheStruct, VA1, VA1Count, "VA1" ),
        JlMarshallStringArray( TheStruct, VA2, VA2Count, "VA2" ),
        JlMarshallStringArray( TheStruct, VA3, VA3Count, "VA3" ),
        JlMarshallStringArray( TheStruct, VA4, VA4Count, "VA4" ),
    };
    uint32_t const marshalTheStructCount = sizeof(marshalTheStruct) / sizeof(marshalTheStruct[0]);

    TheStruct struct1 = {{0}};
    size_t errorAtPos = 0;

    // Set entire struct to ff. To verify that zeros are written in as counts and they fill the entire count fields
    memset( &struct1, 0xff, sizeof(struct1) );

    // Test success
    JL_ASSERT_SUCCESS( JlJsonToStructEx(
        "{\"FA1\":[],\"FA2\":[],\"FA3\":[],\"FA4\":[],\"VA1\":[],\"VA2\":[],\"VA3\":[],\"VA4\":[]}",
        marshalTheStruct, marshalTheStructCount, true, false, &struct1, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 0 == struct1.FA1Count );
    JL_ASSERT( 0xff == struct1.VerifyFA1Count.Bytes[1] );
    JL_ASSERT( 0xff == struct1.VerifyFA1Count.Bytes[2] );
    JL_ASSERT( 0xff == struct1.VerifyFA1Count.Bytes[3] );
    JL_ASSERT( 0xff == struct1.VerifyFA1Count.Bytes[4] );
    JL_ASSERT( 0xff == struct1.VerifyFA1Count.Bytes[5] );
    JL_ASSERT( 0xff == struct1.VerifyFA1Count.Bytes[6] );
    JL_ASSERT( 0xff == struct1.VerifyFA1Count.Bytes[7] );
    JL_ASSERT( 0 == struct1.FA2Count );
    JL_ASSERT( 0xff == struct1.VerifyFA2Count.Bytes[2] );
    JL_ASSERT( 0xff == struct1.VerifyFA2Count.Bytes[3] );
    JL_ASSERT( 0xff == struct1.VerifyFA2Count.Bytes[4] );
    JL_ASSERT( 0xff == struct1.VerifyFA2Count.Bytes[5] );
    JL_ASSERT( 0xff == struct1.VerifyFA2Count.Bytes[6] );
    JL_ASSERT( 0xff == struct1.VerifyFA2Count.Bytes[7] );
    JL_ASSERT( 0 == struct1.FA3Count );
    JL_ASSERT( 0xff == struct1.VerifyFA3Count.Bytes[4] );
    JL_ASSERT( 0xff == struct1.VerifyFA3Count.Bytes[5] );
    JL_ASSERT( 0xff == struct1.VerifyFA3Count.Bytes[6] );
    JL_ASSERT( 0xff == struct1.VerifyFA3Count.Bytes[7] );
    JL_ASSERT( 0 == struct1.FA4Count );

    JL_ASSERT( 0 == struct1.VA1Count );
    JL_ASSERT( 0xff == struct1.VerifyVA1Count.Bytes[2] );
    JL_ASSERT( 0xff == struct1.VerifyVA1Count.Bytes[3] );
    JL_ASSERT( 0xff == struct1.VerifyVA1Count.Bytes[4] );
    JL_ASSERT( 0xff == struct1.VerifyVA1Count.Bytes[5] );
    JL_ASSERT( 0xff == struct1.VerifyVA1Count.Bytes[6] );
    JL_ASSERT( 0xff == struct1.VerifyVA1Count.Bytes[7] );
    JL_ASSERT( 0 == struct1.VA2Count );
    JL_ASSERT( 0xff == struct1.VerifyVA2Count.Bytes[2] );
    JL_ASSERT( 0xff == struct1.VerifyVA2Count.Bytes[3] );
    JL_ASSERT( 0xff == struct1.VerifyVA2Count.Bytes[4] );
    JL_ASSERT( 0xff == struct1.VerifyVA2Count.Bytes[5] );
    JL_ASSERT( 0xff == struct1.VerifyVA2Count.Bytes[6] );
    JL_ASSERT( 0xff == struct1.VerifyVA2Count.Bytes[7] );
    JL_ASSERT( 0 == struct1.VA3Count );
    JL_ASSERT( 0xff == struct1.VerifyVA3Count.Bytes[4] );
    JL_ASSERT( 0xff == struct1.VerifyVA3Count.Bytes[5] );
    JL_ASSERT( 0xff == struct1.VerifyVA3Count.Bytes[6] );
    JL_ASSERT( 0xff == struct1.VerifyVA3Count.Bytes[7] );
    JL_ASSERT( 0 == struct1.VA4Count );

    // Check all pointers are NULL too in the arrays
    JL_ASSERT_NULL( struct1.FA1[0] );
    JL_ASSERT_NULL( struct1.FA1[1] );
    JL_ASSERT_NULL( struct1.FA1[2] );
    JL_ASSERT_NULL( struct1.FA1[3] );
    JL_ASSERT_NULL( struct1.FA2[0] );
    JL_ASSERT_NULL( struct1.FA2[1] );
    JL_ASSERT_NULL( struct1.FA2[2] );
    JL_ASSERT_NULL( struct1.FA2[3] );
    JL_ASSERT_NULL( struct1.FA3[0] );
    JL_ASSERT_NULL( struct1.FA3[1] );
    JL_ASSERT_NULL( struct1.FA3[2] );
    JL_ASSERT_NULL( struct1.FA3[3] );
    JL_ASSERT_NULL( struct1.FA4[0] );
    JL_ASSERT_NULL( struct1.FA4[1] );
    JL_ASSERT_NULL( struct1.FA4[2] );
    JL_ASSERT_NULL( struct1.FA4[3] );

    JL_ASSERT_NULL( struct1.VA1 );
    JL_ASSERT_NULL( struct1.VA2 );
    JL_ASSERT_NULL( struct1.VA3 );
    JL_ASSERT_NULL( struct1.VA4 );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallArraysCounts2
//
//  Now test that it will reject arrays containing more elements than we can represent in the count field.
//  Note this is only practical for 8 bit and 16 bit count fields. We can not feasibly create an array containing
//  more than 4.3 billion elements. So we will just have to trust that code works for 32 bit count fields.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallArraysCounts2
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    size_t errorAtPos = 0;

    typedef struct
    {
        uint8_t*    NumArray;
        uint8_t     NumArrayCountU8;
        uint16_t    NumArrayCountU16;
        uint32_t    NumArrayCountU32;
    } StructWithNumArray;

    JlMarshallElement marshalStructWithNumArrayU8[1] = {
        JlMarshallUnsignedArray( StructWithNumArray, NumArray, NumArrayCountU8, "NumArray" ) };
    JlMarshallElement marshalStructWithNumArrayU16[1] = {
        JlMarshallUnsignedArray( StructWithNumArray, NumArray, NumArrayCountU16, "NumArray" ) };
    JlMarshallElement marshalStructWithNumArrayU32[1] = {
        JlMarshallUnsignedArray( StructWithNumArray, NumArray, NumArrayCountU32, "NumArray" ) };

    (void) marshalStructWithNumArrayU16;
    (void) marshalStructWithNumArrayU32;

    StructWithNumArray numArrayStruct = {0};
    char* jsonString = WjTestLib_MallocAndZero( 20 + (2 * (UINT16_MAX+2) ) );

    // Create Json with 257 elements. Verify that the U8 can't handle it but U16 can.
    strcpy( jsonString, "{\"NumArray\":[1" );
    for( int i=0; i<256; i++ ) { strcat( jsonString, ",1" ); }
    strcat( jsonString, "]}" );

    // Attempt to unmarshall with U8 count field.
    JL_ASSERT_STATUS( JlJsonToStruct( jsonString, marshalStructWithNumArrayU8, 1, &numArrayStruct, &errorAtPos ), JL_STATUS_COUNT_FIELD_TOO_SMALL );
    JL_ASSERT( 12 == errorAtPos );
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalStructWithNumArrayU8, 1, &numArrayStruct ) );

    // Attempt to unmarshall with U16 count field. This should work
    JL_ASSERT_SUCCESS( JlJsonToStruct( jsonString, marshalStructWithNumArrayU16, 1, &numArrayStruct, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalStructWithNumArrayU8, 1, &numArrayStruct ) );

    // Attempt to unmarshall with U32 count field. This should work
    JL_ASSERT_SUCCESS( JlJsonToStruct( jsonString, marshalStructWithNumArrayU32, 1, &numArrayStruct, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalStructWithNumArrayU8, 1, &numArrayStruct ) );

    // Now try with one that will fit in 32bit but not 16bit

    // Create Json with 65536 elements. Verify that the U8 and U16 can't handle it but U32 can
    strcpy( jsonString, "{\"NumArray\":[1" );
    for( int i=0; i<65536; i++ ) { strcat( jsonString, ",1" ); }
    strcat( jsonString, "]}" );

    // Attempt to unmarshall with U8 count field. This should NOT work
    JL_ASSERT_STATUS( JlJsonToStruct( jsonString, marshalStructWithNumArrayU8, 1, &numArrayStruct, &errorAtPos ), JL_STATUS_COUNT_FIELD_TOO_SMALL );
    JL_ASSERT( 12 == errorAtPos );
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalStructWithNumArrayU8, 1, &numArrayStruct ) );

    // Attempt to unmarshall with U16 count field. This should NOT work
    JL_ASSERT_STATUS( JlJsonToStruct( jsonString, marshalStructWithNumArrayU16, 1, &numArrayStruct, &errorAtPos ), JL_STATUS_COUNT_FIELD_TOO_SMALL );
    JL_ASSERT( 12 == errorAtPos );
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalStructWithNumArrayU8, 1, &numArrayStruct ) );

    // Attempt to unmarshall with U32 count field. This should work
    JL_ASSERT_SUCCESS( JlJsonToStruct( jsonString, marshalStructWithNumArrayU32, 1, &numArrayStruct, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalStructWithNumArrayU8, 1, &numArrayStruct ) );

    WjTestLib_Free( jsonString );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallSubDictionaries
//
//  Tests dictionaries within dictionaries
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallSubDictionaries
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    size_t errorAtPos = 0;

    typedef struct
    {
        char*       String1;
        uint32_t    Number1;
    } InnerStruct1;

    typedef struct
    {
        char*           String2;
        uint32_t        Number2;
        InnerStruct1    Inner;
    } InnerStruct2;

    typedef struct
    {
        InnerStruct1    Inner1;
        InnerStruct2    Inner2;
    } OuterStruct;

    JlMarshallElement marshalInnerStruct1[] = {
        JlMarshallString( InnerStruct1, String1, "str1" ),
        JlMarshallUnsigned( InnerStruct1, Number1, "num1" ),
    };

    JlMarshallElement marshalInnerStruct2[] = {
        JlMarshallString( InnerStruct2, String2, "str2" ),
        JlMarshallUnsigned( InnerStruct2, Number2, "num2" ),
        JlMarshallStruct( InnerStruct2, Inner, "inner", marshalInnerStruct1, NumElements(marshalInnerStruct1) ),
    };
    JlMarshallElement marshalOuterStruct[] = {
        JlMarshallStruct( OuterStruct, Inner1, "inner1", marshalInnerStruct1, NumElements(marshalInnerStruct1) ),
        JlMarshallStruct( OuterStruct, Inner2, "inner2", marshalInnerStruct2, NumElements(marshalInnerStruct2) ),
    };

    OuterStruct theStruct = {{0}};

    // Verify working
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{ \"inner1\": { \"str1\":\"String1\", \"num1\":100 }, \"inner2\": { \"str2\":\"String2\", \"num2\":200, \"inner\":{\"str1\":\"String111\",\"num1\":111} } } }",
         marshalOuterStruct, NumElements(marshalOuterStruct), &theStruct, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( strcmp( theStruct.Inner1.String1, "String1" ) == 0 );
    JL_ASSERT( 100 == theStruct.Inner1.Number1 );
    JL_ASSERT( strcmp( theStruct.Inner2.String2, "String2" ) == 0 );
    JL_ASSERT( 200 == theStruct.Inner2.Number2 );
    JL_ASSERT( strcmp( theStruct.Inner2.Inner.String1, "String111" ) == 0 );
    JL_ASSERT( 111 == theStruct.Inner2.Inner.Number1 );

    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalOuterStruct, NumElements(marshalOuterStruct), &theStruct ) );

    // Verify wrong type in a sub dictionary
    JL_ASSERT_STATUS( JlJsonToStruct(
        "{ \"inner1\": { \"str1\": \"String1\", \"num1\":100 }, \"inner2\": { \"str2\":true, \"num2\":200, \"inner\":{\"str1\":\"String111\",\"num1\":111} } } }",
         marshalOuterStruct, NumElements(marshalOuterStruct), &theStruct, &errorAtPos ), JL_STATUS_WRONG_TYPE );
    JL_ASSERT( 66 == errorAtPos );

    // Verify wrong type in base
    JL_ASSERT_STATUS( JlJsonToStruct(
        "{ \"inner1\": { \"str1\": \"String1\", \"num1\":100 }, \"inner2\": 100 } }",
         marshalOuterStruct, NumElements(marshalOuterStruct), &theStruct, &errorAtPos ), JL_STATUS_WRONG_TYPE );
    JL_ASSERT( 57 == errorAtPos );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallPartialFills
//
//  Tests parsing json into structs that already have data. This should only change the values we have data for.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallPartialFills
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    size_t errorAtPos = 0;

    typedef struct
    {
        uint64_t    XNum;
        char*       XStr;
    } MiniStruct;

    typedef struct
    {
        char        FixStr[10];
        char*       VarStr;
        uint32_t    Num;
        bool        Bool1;
        bool        Bool2;
        char**      StrArray;
        size_t      StrArrayCount;
        char*       StrFixArray[4];
        size_t      StrFixArrayCount;
        uint32_t*   NumVarArray;
        size_t      NumVarArrayCount;
        uint32_t    NumFixArray[4];
        size_t      NumFixArrayCount;
        bool*       BoolVarArray;
        size_t      BoolVarArrayCount;
        bool        BoolFixArray[4];
        size_t      BoolFixArrayCount;
        MiniStruct* StructVarArray;
        size_t      StructVarArrayCount;
        MiniStruct  StructFixArray[4];
        size_t      StructFixArrayCount;
    } TheStruct;

    JlMarshallElement marshalMiniStruct[] = {
        JlMarshallUnsigned( MiniStruct, XNum, "XNum" ),
        JlMarshallString( MiniStruct, XStr, "XStr" ),
    };

    JlMarshallElement marshalTheStruct[] = {
        JlMarshallStringFixed( TheStruct, FixStr, "FixStr" ),
        JlMarshallString( TheStruct, VarStr, "VarStr" ),
        JlMarshallUnsigned( TheStruct, Num, "Num" ),
        JlMarshallBool( TheStruct, Bool1, "Bool1" ),
        JlMarshallBool( TheStruct, Bool2, "Bool2" ),
        JlMarshallStringArray( TheStruct, StrArray, StrArrayCount, "StrVarList" ),
        JlMarshallStringFixedArray( TheStruct, StrFixArray, StrFixArrayCount, "StrFixList" ),
        JlMarshallUnsignedArray( TheStruct, NumVarArray, NumVarArrayCount, "NumVarList" ),
        JlMarshallUnsignedFixedArray( TheStruct, NumFixArray, NumFixArrayCount, "NumFixList" ),
        JlMarshallBoolArray( TheStruct, BoolVarArray, BoolVarArrayCount, "BoolVarList" ),
        JlMarshallBoolFixedArray( TheStruct, BoolFixArray, BoolFixArrayCount, "BoolFixList" ),
        JlMarshallStructArray( TheStruct, StructVarArray, StructVarArrayCount, "MiniVarList", MiniStruct, marshalMiniStruct, NumElements(marshalMiniStruct) ),
        JlMarshallStructFixedArray( TheStruct, StructFixArray, StructFixArrayCount, "MiniFixList", MiniStruct, marshalMiniStruct, NumElements(marshalMiniStruct) ),
    };

    // Test replacing each element individually with new json and verify rest of struct does not get changed
    for( size_t element=1; element<=12; element++ )
    {
        TheStruct theStruct = {{0}};

        // Fill in struct with values
        JL_ASSERT_SUCCESS( JlJsonToStruct(
            "{ \"FixStr\":\"FixStr\", \"VarStr\":\"VarStr\", \"Num\":1234, \"Bool1\":true, \"Bool2\":false,"
            " \"StrVarList\":[\"StrVarList1\",\"StrVarList2\"], \"StrFixList\":[\"StrFixList1\",\"StrFixList2\"],"
            " \"NumVarList\":[101,102], \"NumFixList\":[201,202], \"BoolVarList\":[true,false,true],"
            " \"BoolFixList\":[true,false,true],"
            " \"MiniVarList\": [ {\"XNum\":301,\"XStr\":\"301\"}, {\"XNum\":401,\"XStr\":\"401\"} ],"
            " \"MiniFixList\": [ {\"XNum\":501,\"XStr\":\"501\"}, {\"XNum\":601,\"XStr\":\"601\"} ] }",
             marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
        JL_ASSERT( 0 == errorAtPos );

        switch( element )
        {
        case 1:
            JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"FixStr\":\"XXXX\"}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
            JL_ASSERT( strcmp( theStruct.FixStr, "XXXX" ) == 0 );
            break;
        case 2:
            JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"VarStr\":\"XXXX\"}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
            JL_ASSERT( strcmp( theStruct.VarStr, "XXXX" ) == 0 );
            break;
        case 3:
            JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"Num\":9999}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
            JL_ASSERT( 9999 == theStruct.Num );
            break;
        case 4:
            JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"Bool1\":false,\"Bool2\":true}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
            JL_ASSERT( !theStruct.Bool1 );
            JL_ASSERT( theStruct.Bool2 );
            break;
        case 5:
            JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"StrVarList\":[\"a\",\"b\",\"c\"]}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
            JL_ASSERT( 3 == theStruct.StrArrayCount );
            JL_ASSERT( strcmp( theStruct.StrArray[0], "a" ) == 0 );
            JL_ASSERT( strcmp( theStruct.StrArray[1], "b" ) == 0 );
            JL_ASSERT( strcmp( theStruct.StrArray[2], "c" ) == 0 );
            break;
        case 6:
            JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"StrFixList\":[\"a\",\"b\",\"c\"]}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
            JL_ASSERT( 3 == theStruct.StrFixArrayCount );
            JL_ASSERT( strcmp( theStruct.StrFixArray[0], "a" ) == 0 );
            JL_ASSERT( strcmp( theStruct.StrFixArray[1], "b" ) == 0 );
            JL_ASSERT( strcmp( theStruct.StrFixArray[2], "c" ) == 0 );
            break;
        case 7:
            JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"NumVarList\":[10,20,30]}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
            JL_ASSERT( 3 == theStruct.NumVarArrayCount );
            JL_ASSERT( 10 == theStruct.NumVarArray[0] );
            JL_ASSERT( 20 == theStruct.NumVarArray[1] );
            JL_ASSERT( 30 == theStruct.NumVarArray[2] );
            break;
        case 8:
            JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"NumFixList\":[10,20,30]}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
            JL_ASSERT( 3 == theStruct.NumFixArrayCount );
            JL_ASSERT( 10 == theStruct.NumFixArray[0] );
            JL_ASSERT( 20 == theStruct.NumFixArray[1] );
            JL_ASSERT( 30 == theStruct.NumFixArray[2] );
            break;
        case 9:
            JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"BoolVarList\":[false,true,false,true]}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
            JL_ASSERT( 4 == theStruct.BoolVarArrayCount );
            JL_ASSERT( !theStruct.BoolVarArray[0] );
            JL_ASSERT( theStruct.BoolVarArray[1] );
            JL_ASSERT( !theStruct.BoolVarArray[2] );
            JL_ASSERT( theStruct.BoolVarArray[3] );
            break;
        case 10:
            JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"BoolFixList\":[false,true,false,true]}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
            JL_ASSERT( 4 == theStruct.BoolFixArrayCount );
            JL_ASSERT( !theStruct.BoolFixArray[0] );
            JL_ASSERT( theStruct.BoolFixArray[1] );
            JL_ASSERT( !theStruct.BoolFixArray[2] );
            JL_ASSERT( theStruct.BoolFixArray[3] );
            break;
        case 11:
            JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"MiniVarList\": [ {\"XNum\":777,\"XStr\":\"777\"}, {\"XNum\":888,\"XStr\":\"888\"}, {\"XNum\":999,\"XStr\":\"999\"} ]}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
            JL_ASSERT( 3 == theStruct.StructVarArrayCount );
            JL_ASSERT( 777 == theStruct.StructVarArray[0].XNum );
            JL_ASSERT( strcmp( theStruct.StructVarArray[0].XStr, "777" ) == 0 );
            JL_ASSERT( 888 == theStruct.StructVarArray[1].XNum );
            JL_ASSERT( strcmp( theStruct.StructVarArray[1].XStr, "888" ) == 0 );
            JL_ASSERT( 999 == theStruct.StructVarArray[2].XNum );
            JL_ASSERT( strcmp( theStruct.StructVarArray[2].XStr, "999" ) == 0 );
            break;
        case 12:
            JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"MiniFixList\": [ {\"XNum\":777,\"XStr\":\"777\"}, {\"XNum\":888,\"XStr\":\"888\"}, {\"XNum\":999,\"XStr\":\"999\"} ]}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
            JL_ASSERT( 3 == theStruct.StructFixArrayCount );
            JL_ASSERT( 777 == theStruct.StructFixArray[0].XNum );
            JL_ASSERT( strcmp( theStruct.StructFixArray[0].XStr, "777" ) == 0 );
            JL_ASSERT( 888 == theStruct.StructFixArray[1].XNum );
            JL_ASSERT( strcmp( theStruct.StructFixArray[1].XStr, "888" ) == 0 );
            JL_ASSERT( 999 == theStruct.StructFixArray[2].XNum );
            JL_ASSERT( strcmp( theStruct.StructFixArray[2].XStr, "999" ) == 0 );
            break;
        default:
            JL_ASSERT( false );
            break;
        }

        // Verify every other element is still there
        if( 1 != element ) { JL_ASSERT( strcmp( theStruct.FixStr, "FixStr" ) == 0 ); }
        if( 2 != element ) { JL_ASSERT( strcmp( theStruct.VarStr, "VarStr" ) == 0 ); }
        if( 3 != element ) { JL_ASSERT( 1234 == theStruct.Num ); }
        if( 4 != element ) {
            JL_ASSERT( theStruct.Bool1 );
            JL_ASSERT( !theStruct.Bool2 );
        }
        if( 5 != element ) {
            JL_ASSERT( 2 == theStruct.StrArrayCount );
            JL_ASSERT( strcmp( theStruct.StrArray[0], "StrVarList1" ) == 0 );
            JL_ASSERT( strcmp( theStruct.StrArray[1], "StrVarList2" ) == 0 );
        }
        if( 6 != element ) {
            JL_ASSERT( 2 == theStruct.StrFixArrayCount );
            JL_ASSERT( strcmp( theStruct.StrFixArray[0], "StrFixList1" ) == 0 );
            JL_ASSERT( strcmp( theStruct.StrFixArray[1], "StrFixList2" ) == 0 );
        }
        if( 7 != element ) {
            JL_ASSERT( 2 == theStruct.NumVarArrayCount );
            JL_ASSERT( 101 == theStruct.NumVarArray[0] );
            JL_ASSERT( 102 == theStruct.NumVarArray[1] );
        }
        if( 8 != element ) {
            JL_ASSERT( 2 == theStruct.NumFixArrayCount );
            JL_ASSERT( 201 == theStruct.NumFixArray[0] );
            JL_ASSERT( 202 == theStruct.NumFixArray[1] );
        }
        if( 9 != element ) {
            JL_ASSERT( 3 == theStruct.BoolVarArrayCount );
            JL_ASSERT( theStruct.BoolVarArray[0] );
            JL_ASSERT( !theStruct.BoolVarArray[1] );
            JL_ASSERT( theStruct.BoolVarArray[2] );
        }
        if( 10 != element ) {
            JL_ASSERT( 3 == theStruct.BoolFixArrayCount );
            JL_ASSERT( theStruct.BoolFixArray[0] );
            JL_ASSERT( !theStruct.BoolFixArray[1] );
            JL_ASSERT( theStruct.BoolFixArray[2] );
        }
        if( 11 != element ) {
            JL_ASSERT( 2 == theStruct.StructVarArrayCount );
            JL_ASSERT( 301 == theStruct.StructVarArray[0].XNum );
            JL_ASSERT( strcmp( theStruct.StructVarArray[0].XStr, "301" ) == 0 );
            JL_ASSERT( 401 == theStruct.StructVarArray[1].XNum );
            JL_ASSERT( strcmp( theStruct.StructVarArray[1].XStr, "401" ) == 0 );
        }
        if( 12 != element ) {
            JL_ASSERT( 2 == theStruct.StructFixArrayCount );
            JL_ASSERT( 501 == theStruct.StructFixArray[0].XNum );
            JL_ASSERT( strcmp( theStruct.StructFixArray[0].XStr, "501" ) == 0 );
            JL_ASSERT( 601 == theStruct.StructFixArray[1].XNum );
            JL_ASSERT( strcmp( theStruct.StructFixArray[1].XStr, "601" ) == 0 );
        }

        JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalTheStruct, NumElements(marshalTheStruct), &theStruct ) );
    }

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallStructFixArray
//
//  Specific test for testing that a fixed array of structs does not stray out of bounds. This test was written for
//  a specific situation and is now covered in TestUnmarshallPartialFills. The test is left here anyway.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallStructFixArray
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    size_t errorAtPos = 0;

    typedef struct
    {
        uint64_t    XNum;
        char*       XStr;
    } MiniStruct;

    typedef struct
    {
        MiniStruct  StructFixArray[4];
        size_t      StructFixArrayCount;
    } TheStruct;

    JlMarshallElement marshalMiniStruct[] = {
        JlMarshallUnsigned( MiniStruct, XNum, "XNum" ),
        JlMarshallString( MiniStruct, XStr, "XStr" ),
    };

    JlMarshallElement marshalTheStruct[] = {
        JlMarshallStructFixedArray( TheStruct, StructFixArray, StructFixArrayCount, "MiniFixList", MiniStruct, marshalMiniStruct, NumElements(marshalMiniStruct) ),
    };

    TheStruct theStruct = {{{0}}};

    // Fill in struct with values
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{ \"MiniFixList\": [ {\"XNum\":501,\"XStr\":\"501\"}, {\"XNum\":601,\"XStr\":\"601\"} ] }",
            marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );

    JL_ASSERT( 2 == theStruct.StructFixArrayCount );
    JL_ASSERT( 501 == theStruct.StructFixArray[0].XNum );
    JL_ASSERT( strcmp( theStruct.StructFixArray[0].XStr, "501" ) == 0 );
    JL_ASSERT( 601 == theStruct.StructFixArray[1].XNum );
    JL_ASSERT( strcmp( theStruct.StructFixArray[1].XStr, "601" ) == 0 );

    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalTheStruct, NumElements(marshalTheStruct), &theStruct ) );

    // Try with too much data
    JL_ASSERT_STATUS( JlJsonToStruct(
        "{ \"MiniFixList\": [ {\"XNum\":501,\"XStr\":\"501\"}, {\"XNum\":601,\"XStr\":\"601\"},"
        "{\"XNum\":701,\"XStr\":\"701\"}, {\"XNum\":801,\"XStr\":\"801\"},{\"XNum\":901,\"XStr\":\"901\"} ] }",
            marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ), JL_STATUS_TOO_MANY_ITEMS );
    JL_ASSERT( 17 == errorAtPos );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallListAlreadyHasContents
//
//  Specific test for testing that a list in a struct gets deallocated if it already contains contents when it is
//  being unmarshalled.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallListAlreadyHasContents
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    size_t errorAtPos = 0;

    typedef struct
    {
        uint32_t*   NumArray;
        size_t      NumArrayCount;
    } TheStruct;

    JlMarshallElement marshalTheStruct[] = {
        JlMarshallUnsignedArray( TheStruct, NumArray, NumArrayCount, "NumList" )
    };

    TheStruct theStruct = {0};

    // Fill in struct with values
    JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"NumList\":[1,2]}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 2 == theStruct.NumArrayCount );
    JL_ASSERT( 1 == theStruct.NumArray[0] );
    JL_ASSERT( 2 == theStruct.NumArray[1] );

    // Now unmarshall again when struct already has a list with contents.
    JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"NumList\":[3]}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 1 == theStruct.NumArrayCount );
    JL_ASSERT( 3 == theStruct.NumArray[0] );

    // Free and make sure no memory leak
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalTheStruct, NumElements(marshalTheStruct), &theStruct ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallListAlreadyHasContentsStrFixedArray
//
//  Specific test for testing that a list in a struct gets deallocated if it already contains contents when it is
//  being unmarshalled.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallListAlreadyHasContentsStrFixedArray
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    size_t errorAtPos = 0;

    typedef struct
    {
        char*       StrFixList [4];
        size_t      StrFixListCount;
    } TheStruct;

    JlMarshallElement marshalTheStruct[] = {
        JlMarshallStringFixedArray( TheStruct, StrFixList, StrFixListCount, "Strs" )
    };

    TheStruct theStruct = {{0}};

    // Fill in struct with values
    JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"Strs\":[\"aaa\",\"bbb\"]}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 2 == theStruct.StrFixListCount );
    JL_ASSERT( strcmp( theStruct.StrFixList[0], "aaa" ) == 0 );
    JL_ASSERT( strcmp( theStruct.StrFixList[1], "bbb" ) == 0 );

    // Now unmarshall again when struct already has a list with contents.
    JL_ASSERT_SUCCESS( JlJsonToStruct( "{\"Strs\":[\"ccc\"]}", marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 1 == theStruct.StrFixListCount );
    JL_ASSERT( strcmp( theStruct.StrFixList[0], "ccc" ) == 0 );

    // Free and make sure no memory leak
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalTheStruct, NumElements(marshalTheStruct), &theStruct ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallLists
//
//  Test unmarshalling lists
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallLists
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    size_t errorAtPos = 0;

    typedef struct
    {
        uint32_t        Num;
    } SubSubStruct;

    typedef struct
    {
        SubSubStruct   SubSubFixList [4];
        size_t         SubSubFixListCount;
        SubSubStruct*  SubSubVarList;
        size_t         SubSubVarListCount;
    } SubStruct;

    typedef struct
    {
        SubStruct   SubFixList [4];
        size_t      SubFixListCount;
        SubStruct*  SubVarList;
        size_t      SubVarListCount;
    } TheStruct;

    JlMarshallElement marshalSubSubStruct[] = {
        JlMarshallUnsigned( SubSubStruct, Num, "Num" )
    };

    JlMarshallElement marshalSubStruct[] = {
        JlMarshallStructFixedArray( SubStruct, SubSubFixList, SubSubFixListCount, "SubSubFixList", SubSubStruct, marshalSubSubStruct, NumElements(marshalSubSubStruct) ),
        JlMarshallStructArray( SubStruct, SubSubVarList, SubSubVarListCount, "SubSubVarList", SubSubStruct, marshalSubSubStruct, NumElements(marshalSubSubStruct) )
    };

    JlMarshallElement marshalTheStruct[] = {
        JlMarshallStructFixedArray( TheStruct, SubFixList, SubFixListCount, "SubFixList", SubStruct, marshalSubStruct, NumElements(marshalSubStruct) ),
        JlMarshallStructArray( TheStruct, SubVarList, SubVarListCount, "SubVarList", SubStruct, marshalSubStruct, NumElements(marshalSubStruct) )
    };

    TheStruct theStruct = {{{{{0}}}}};

    // Fill in struct with values
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{"
        "  \"SubFixList\":[ "
        "    { \"SubSubFixList\": [{\"Num\":1},{\"Num\":2}], \"SubSubVarList\": [{\"Num\":3},{\"Num\":4}] },"
        "    { \"SubSubFixList\": [{\"Num\":5},{\"Num\":6}], \"SubSubVarList\": [{\"Num\":7},{\"Num\":8}] }"
        "  ],"
        "  \"SubVarList\":[ "
        "    { \"SubSubFixList\": [{\"Num\":9},{\"Num\":10}], \"SubSubVarList\": [{\"Num\":11},{\"Num\":12}] },"
        "    { \"SubSubFixList\": [{\"Num\":13},{\"Num\":14}], \"SubSubVarList\": [{\"Num\":15},{\"Num\":16}] }"
        "  ]"
        "}",
        marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );

    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 2 == theStruct.SubFixListCount );
      JL_ASSERT( 2 == theStruct.SubFixList[0].SubSubFixListCount );
        JL_ASSERT( 1 == theStruct.SubFixList[0].SubSubFixList[0].Num );
        JL_ASSERT( 2 == theStruct.SubFixList[0].SubSubFixList[1].Num );
      JL_ASSERT( 2 == theStruct.SubFixList[0].SubSubVarListCount );
        JL_ASSERT( 3 == theStruct.SubFixList[0].SubSubVarList[0].Num );
        JL_ASSERT( 4 == theStruct.SubFixList[0].SubSubVarList[1].Num );
      JL_ASSERT( 2 == theStruct.SubFixList[1].SubSubFixListCount );
        JL_ASSERT( 5 == theStruct.SubFixList[1].SubSubFixList[0].Num );
        JL_ASSERT( 6 == theStruct.SubFixList[1].SubSubFixList[1].Num );
      JL_ASSERT( 2 == theStruct.SubFixList[1].SubSubVarListCount );
        JL_ASSERT( 7 == theStruct.SubFixList[1].SubSubVarList[0].Num );
        JL_ASSERT( 8 == theStruct.SubFixList[1].SubSubVarList[1].Num );

      JL_ASSERT( 2 == theStruct.SubVarList[0].SubSubFixListCount );
        JL_ASSERT( 9 == theStruct.SubVarList[0].SubSubFixList[0].Num );
        JL_ASSERT( 10 == theStruct.SubVarList[0].SubSubFixList[1].Num );
      JL_ASSERT( 2 == theStruct.SubVarList[0].SubSubVarListCount );
        JL_ASSERT( 11 == theStruct.SubVarList[0].SubSubVarList[0].Num );
        JL_ASSERT( 12 == theStruct.SubVarList[0].SubSubVarList[1].Num );
      JL_ASSERT( 2 == theStruct.SubVarList[1].SubSubFixListCount );
        JL_ASSERT( 13 == theStruct.SubVarList[1].SubSubFixList[0].Num );
        JL_ASSERT( 14 == theStruct.SubVarList[1].SubSubFixList[1].Num );
      JL_ASSERT( 2 == theStruct.SubVarList[1].SubSubVarListCount );
        JL_ASSERT( 15 == theStruct.SubVarList[1].SubSubVarList[0].Num );
        JL_ASSERT( 16 == theStruct.SubVarList[1].SubSubVarList[1].Num );

    // Partial replace
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{"
        "  \"SubFixList\":[ "
        "    { \"SubSubFixList\": [{\"Num\":41},{\"Num\":42}], \"SubSubVarList\": [{\"Num\":43},{\"Num\":44}] },"
        "    { \"SubSubFixList\": [{\"Num\":45},{\"Num\":46}], \"SubSubVarList\": [{\"Num\":47},{\"Num\":48}] }"
        "  ]"
        "}",
        marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );

    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 2 == theStruct.SubFixListCount );
      JL_ASSERT( 2 == theStruct.SubFixList[0].SubSubFixListCount );
        JL_ASSERT( 41 == theStruct.SubFixList[0].SubSubFixList[0].Num );
        JL_ASSERT( 42 == theStruct.SubFixList[0].SubSubFixList[1].Num );
      JL_ASSERT( 2 == theStruct.SubFixList[0].SubSubVarListCount );
        JL_ASSERT( 43 == theStruct.SubFixList[0].SubSubVarList[0].Num );
        JL_ASSERT( 44 == theStruct.SubFixList[0].SubSubVarList[1].Num );
      JL_ASSERT( 2 == theStruct.SubFixList[1].SubSubFixListCount );
        JL_ASSERT( 45 == theStruct.SubFixList[1].SubSubFixList[0].Num );
        JL_ASSERT( 46 == theStruct.SubFixList[1].SubSubFixList[1].Num );
      JL_ASSERT( 2 == theStruct.SubFixList[1].SubSubVarListCount );
        JL_ASSERT( 47 == theStruct.SubFixList[1].SubSubVarList[0].Num );
        JL_ASSERT( 48 == theStruct.SubFixList[1].SubSubVarList[1].Num );
      // Rest should be same from before
      JL_ASSERT( 2 == theStruct.SubVarList[0].SubSubFixListCount );
        JL_ASSERT( 9 == theStruct.SubVarList[0].SubSubFixList[0].Num );
        JL_ASSERT( 10 == theStruct.SubVarList[0].SubSubFixList[1].Num );
      JL_ASSERT( 2 == theStruct.SubVarList[0].SubSubVarListCount );
        JL_ASSERT( 11 == theStruct.SubVarList[0].SubSubVarList[0].Num );
        JL_ASSERT( 12 == theStruct.SubVarList[0].SubSubVarList[1].Num );
      JL_ASSERT( 2 == theStruct.SubVarList[1].SubSubFixListCount );
        JL_ASSERT( 13 == theStruct.SubVarList[1].SubSubFixList[0].Num );
        JL_ASSERT( 14 == theStruct.SubVarList[1].SubSubFixList[1].Num );
      JL_ASSERT( 2 == theStruct.SubVarList[1].SubSubVarListCount );
        JL_ASSERT( 15 == theStruct.SubVarList[1].SubSubVarList[0].Num );
        JL_ASSERT( 16 == theStruct.SubVarList[1].SubSubVarList[1].Num );

    // Replace with some values
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{"
        "  \"SubFixList\":[ "
        "    { \"SubSubFixList\": [{\"Num\":20}], \"SubSubVarList\": [{\"Num\":21}] }"
        "  ],"
        "  \"SubVarList\":[ "
        "    { \"SubSubFixList\": [{\"Num\":22}], \"SubSubVarList\": [{\"Num\":23}] }"
        "  ]"
        "}",
        marshalTheStruct, NumElements(marshalTheStruct), &theStruct, &errorAtPos ) );

    JL_ASSERT( 0 == errorAtPos );
    JL_ASSERT( 1 == theStruct.SubFixListCount );
      JL_ASSERT( 1 == theStruct.SubFixList[0].SubSubFixListCount );
        JL_ASSERT( 20 == theStruct.SubFixList[0].SubSubFixList[0].Num );
      JL_ASSERT( 1 == theStruct.SubFixList[0].SubSubVarListCount );
        JL_ASSERT( 21 == theStruct.SubFixList[0].SubSubVarList[0].Num );

      JL_ASSERT( 1 == theStruct.SubVarList[0].SubSubFixListCount );
        JL_ASSERT( 22 == theStruct.SubVarList[0].SubSubFixList[0].Num );
      JL_ASSERT( 1 == theStruct.SubVarList[0].SubSubVarListCount );
        JL_ASSERT( 23 == theStruct.SubVarList[0].SubSubVarList[0].Num );

    // Free and make sure no memory leak
    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalTheStruct, NumElements(marshalTheStruct), &theStruct ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestUnmarshallBinary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestUnmarshallBinary
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct
    {
        uint8_t     FixedBlob[10];      // Treat as 80bit binary
        uint64_t    BinaryBlob2;        // Treat as 64bit binary
    } TestStruct;

    JlMarshallElement marshalTestStruct[] =
    {
        JlMarshallBinaryFixed( TestStruct, FixedBlob, "FixedBlob" ),
        JlMarshallBinaryFixed( TestStruct, BinaryBlob2, "BinaryBlob2" ),
    };

    TestStruct theStruct = {{0}};
    size_t errorAtPos = 0;

    // Test success
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{FixedBlob:'VGhpc0lzVGVzdA',BinaryBlob2:'8N68mnhWNBI'}",
        marshalTestStruct, NumElements(marshalTestStruct), &theStruct, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );

    JL_ASSERT( memcmp( theStruct.FixedBlob, "ThisIsTest", 10 ) == 0 );
    JL_ASSERT( 0x123456789ABCDEF0ULL == theStruct.BinaryBlob2 );

    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalTestStruct, NumElements(marshalTestStruct), &theStruct ) );

    // Now Test with variable sized binary
    typedef struct
    {
        uint8_t*    VariableBlob1;
        size_t      VariableBlob1Size;
        void*       VariableBlob2;
        size_t      VariableBlob2Size;
        uint32_t*   VariableBlob3;
        size_t      VariableBlob3Size;
    } TestStruct2;

    JlMarshallElement marshalTestStruct2[] =
    {
        JlMarshallBinary( TestStruct2, VariableBlob1, VariableBlob1Size, "VariableBlob1" ),
        JlMarshallBinary( TestStruct2, VariableBlob2, VariableBlob2Size, "VariableBlob2" ),
        JlMarshallBinary( TestStruct2, VariableBlob3, VariableBlob3Size, "VariableBlob3" ),
    };

    TestStruct2 theStruct2 = {0};

    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{VariableBlob1:'MTIzNDU2Nzg5MA',VariableBlob2:'MTIzNDU2Nzg5MEFBQUFCQkJCQ0NDQ0REREQ',VariableBlob3:null}",
        marshalTestStruct2, NumElements(marshalTestStruct2), &theStruct2, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );

    JL_ASSERT_NOT_NULL( theStruct2.VariableBlob1 );
    JL_ASSERT( 10 == theStruct2.VariableBlob1Size );
    JL_ASSERT( memcmp( theStruct2.VariableBlob1, "1234567890", 10 ) == 0 );

    JL_ASSERT_NOT_NULL( theStruct2.VariableBlob2 );
    JL_ASSERT( 26 == theStruct2.VariableBlob2Size );
    JL_ASSERT( memcmp( theStruct2.VariableBlob2, "1234567890AAAABBBBCCCCDDDD", 26 ) == 0 );

    JL_ASSERT_NULL( theStruct2.VariableBlob3 );
    JL_ASSERT( 0 == theStruct2.VariableBlob3Size );

    // Now test loading in different data and making sure no memory leak
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{VariableBlob1:'AAAAAAAAAAAAAA'}",
        marshalTestStruct2, NumElements(marshalTestStruct2), &theStruct2, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );

    JL_ASSERT( 10 == theStruct2.VariableBlob1Size );
    uint8_t zero10[10] = {0};
    JL_ASSERT( memcmp( theStruct2.VariableBlob1, zero10, 10 ) == 0 );

    JL_ASSERT_NOT_NULL( theStruct2.VariableBlob2 );
    JL_ASSERT( 26 == theStruct2.VariableBlob2Size );
    JL_ASSERT( memcmp( theStruct2.VariableBlob2, "1234567890AAAABBBBCCCCDDDD", 26 ) == 0 );

    JL_ASSERT_NULL( theStruct2.VariableBlob3 );
    JL_ASSERT( 0 == theStruct2.VariableBlob3Size );

    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{VariableBlob1:'MTIzNDU2Nzg5MA',VariableBlob2:null,VariableBlob3:'MTIzNDU2Nzg5MA'}",
        marshalTestStruct2, NumElements(marshalTestStruct2), &theStruct2, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );

    JL_ASSERT_NOT_NULL( theStruct2.VariableBlob1 );
    JL_ASSERT( 10 == theStruct2.VariableBlob1Size );
    JL_ASSERT( memcmp( theStruct2.VariableBlob1, "1234567890", 10 ) == 0 );

    JL_ASSERT_NULL( theStruct2.VariableBlob2 );
    JL_ASSERT( 0 == theStruct2.VariableBlob2Size );

    JL_ASSERT_NOT_NULL( theStruct2.VariableBlob3 );
    JL_ASSERT( 10 == theStruct2.VariableBlob3Size );
    JL_ASSERT( memcmp( theStruct2.VariableBlob3, "1234567890", 10 ) == 0 );

    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalTestStruct2, NumElements(marshalTestStruct2), &theStruct2 ) );

    // Now test invalid values. Fixed buffers have to have the exact sized base 64
    JL_ASSERT_STATUS( JlJsonToStruct(
        "{BinaryBlob2:'8N68mnhWNBI',FixedBlob:'c0lzVGVzdA'}",
        marshalTestStruct, NumElements(marshalTestStruct), &theStruct, &errorAtPos ), JL_STATUS_INVALID_DATA );
    JL_ASSERT( 37 == errorAtPos );

    // Now test invalid values. Fixed buffers have to have the exact sized base 64
    JL_ASSERT_STATUS( JlJsonToStruct(
        "{BinaryBlob2:'8N68mnhWNBI',FixedBlob:'VGhpVGhpc0lzVGVzdA'}",
        marshalTestStruct, NumElements(marshalTestStruct), &theStruct, &errorAtPos ), JL_STATUS_INVALID_DATA );
    JL_ASSERT( 37 == errorAtPos );

    // Now test invalid base 64
    JL_ASSERT_STATUS( JlJsonToStruct(
        "{BinaryBlob2:'8N68mnhWNBI',FixedBlob:'VGhp%0lzVGVzdA'}",
        marshalTestStruct, NumElements(marshalTestStruct), &theStruct, &errorAtPos ), JL_STATUS_INVALID_DATA );
    JL_ASSERT( 37 == errorAtPos );

    // Now test padding character
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        "{FixedBlob:'VGhpc0lzVGVzdA===',BinaryBlob2:'8N68mnhWNBI==Anything ignored after padding'}",
        marshalTestStruct, NumElements(marshalTestStruct), &theStruct, &errorAtPos ) );
    JL_ASSERT( 0 == errorAtPos );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibTests_Unmarshall_Register
//
//  Registers the tests with WjTestLib
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
    JsonLibTests_Unmarshall_Register
    (
        void
    )
{
    WjTestLib_NewGroup( "Unmarshal" );
    WjTestLib_AddTest( TestUnmarshallNumbers, "Numbers" );
    WjTestLib_AddTest( TestUnmarshallStrings, "Strings" );
    WjTestLib_AddTest( TestUnmarshallArraysOfNumbers, "ArraysNumbers" );
    WjTestLib_AddTest( TestUnmarshallFixedArraysOfNumbers, "FixedArraysNumbers" );
    WjTestLib_AddTest( TestUnmarshallBools, "Bools" );
    WjTestLib_AddTest( TestUnmarshallArraysOfStrings, "ArraysStrings" );
    WjTestLib_AddTest( TestUnmarshallArraysCounts1, "ArrayCounts1" );
    WjTestLib_AddTest( TestUnmarshallArraysCounts2, "ArrayCounts2" );
    WjTestLib_AddTest( TestUnmarshallSubDictionaries, "SubDictionaries" );
    WjTestLib_AddTest( TestUnmarshallStructFixArray, "StructFixArray" );
    WjTestLib_AddTest( TestUnmarshallListAlreadyHasContents, "ListAlreadyHasContents" );
    WjTestLib_AddTest( TestUnmarshallListAlreadyHasContentsStrFixedArray, "ListAlreadyHasStrFixedArray" );
    WjTestLib_AddTest( TestUnmarshallPartialFills, "PartialFills" );
    WjTestLib_AddTest( TestUnmarshallLists, "Lists" );
    WjTestLib_AddTest( TestUnmarshallBinary, "BinaryData" );
}
