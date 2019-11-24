////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibTests
//
//  Unit tests for JsonLib - Marshall
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MakeJsonAndVerify
//
//  Marshalls struct to json and verifies result
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    MakeJsonAndVerify
    (
        void const*                 Structure,                      // [in]
        JlMarshallElement const*    StructDescription,              // [in]
        uint32_t                    StructDescriptionCount,         // [in]
        char const*                 ExpectedJson                    // [in]
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    char* jsonString = NULL;
    JL_ASSERT_SUCCESS( JlStructToJson(Structure, StructDescription,StructDescriptionCount, false, &jsonString ) );
    JL_ASSERT( strcmp( jsonString, ExpectedJson ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeJsonStringBuffer( &jsonString ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MakeJson5AndVerify
//
//  Marshalls struct to json5 and verifies result
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    MakeJson5AndVerify
    (
        void const*                 Structure,                      // [in]
        JlMarshallElement const*    StructDescription,              // [in]
        uint32_t                    StructDescriptionCount,         // [in]
        char const*                 ExpectedJson                    // [in]
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    char* jsonString = NULL;
    JL_ASSERT_SUCCESS( JlStructToJsonEx(Structure, StructDescription,StructDescriptionCount, JL_OUTPUT_FLAGS_JSON5, &jsonString ) );
    if( NULL != jsonString )
    {
        JL_ASSERT( strcmp( jsonString, ExpectedJson ) == 0 );
        JL_ASSERT_SUCCESS( JlFreeJsonStringBuffer( &jsonString ) );
    }

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestMarshallStrings
//
//  Test marshalling strings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestMarshallStrings
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

    StringStruct theStruct = {{0}};
    char* jsonString = NULL;

    // Check marshalling null strings
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                                          "{\"fixedStr\":null,\"strPtr\":null}" ) );

    // Check with strings
    strcpy( theStruct.fixedStr, "Hello" );
    theStruct.strPtr = "There";
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                                          "{\"fixedStr\":\"Hello\",\"strPtr\":\"There\"}" ) );

    // try with escaped characters
    theStruct.fixedStr[0] = 0;
    theStruct.strPtr = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\0xf";
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
        "{\"fixedStr\":null,\"strPtr\":\"\\u0001\\u0002\\u0003\\u0004\\u0005\\u0006\\u0007\\b\\t\\n\\u000b\\f\\r\\u000e\"}" ) );

    theStruct.strPtr = "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f";
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
        "{\"fixedStr\":null,\"strPtr\":\"\\u0010\\u0011\\u0012\\u0013\\u0014\\u0015\\u0016\\u0017"
        "\\u0018\\u0019\\u001a\\u001b\\u001c\\u001d\\u001e\\u001f\"}" ) );

    theStruct.strPtr = "_\"_";
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                       "{\"fixedStr\":null,\"strPtr\":\"_\\\"_\"}" ) );

    theStruct.strPtr = "_\\_";
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                                          "{\"fixedStr\":null,\"strPtr\":\"_\\\\_\"}" ) );

    theStruct.strPtr = "\xF0\x9F\x98\x81";       // smiley face emoji
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                                          "{\"fixedStr\":null,\"strPtr\":\"\xF0\x9F\x98\x81\"}" ) );

    // Test invalid utf8 values (truncated)
    theStruct.strPtr = "\xF0\x9F\x98Truncated";
    JL_ASSERT_STATUS( JlStructToJson( &theStruct, marshalStringStruct, NumElements(marshalStringStruct), false, &jsonString ),
                      JL_STATUS_INVALID_DATA );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestMarshallArrayStrings
//
//  Test marshalling arrays of strings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestMarshallArrayStrings
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct
    {
        char* ArrayStrFixed[4];
        size_t ArrayStrFixedCount;
        char** StrArray;
        size_t StrArrayCount;
    } StringStruct;

    JlMarshallElement marshalStringStruct[] =
    {
        JlMarshallStringFixedArray( StringStruct, ArrayStrFixed, ArrayStrFixedCount, "ArrayStrFixed" ),
        JlMarshallStringArray( StringStruct, StrArray, StrArrayCount, "StrArray" ),
    };

    StringStruct theStruct = {{0}};
    char* someStrings[10] = {0};
//    char* jsonString = NULL;

    // Check marshalling empty arrays
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                                          "{\"ArrayStrFixed\":[],\"StrArray\":[]}" ) );

    // Now try with arrays containing only a single NULL string
    theStruct.ArrayStrFixed[0] = NULL;
    theStruct.ArrayStrFixedCount = 1;
    theStruct.StrArray = someStrings;
    theStruct.StrArrayCount = 1;

    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                                          "{\"ArrayStrFixed\":[null],\"StrArray\":[null]}" ) );

    // Now try with arrays containing only a single empty string
    theStruct.ArrayStrFixed[0] = "";
    theStruct.ArrayStrFixedCount = 1;
    someStrings[0] = "";
    theStruct.StrArray = someStrings;
    theStruct.StrArrayCount = 1;

    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                                          "{\"ArrayStrFixed\":[null],\"StrArray\":[null]}" ) );

    // Now try regular strings
    theStruct.ArrayStrFixed[0] = "a";
    theStruct.ArrayStrFixed[1] = "b";
    theStruct.ArrayStrFixed[2] = "c";
    theStruct.ArrayStrFixedCount = 3;
    someStrings[0] = "A";
    someStrings[1] = "B";
    someStrings[2] = "C";
    theStruct.StrArray = someStrings;
    theStruct.StrArrayCount = 3;

    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                                          "{\"ArrayStrFixed\":[\"a\",\"b\",\"c\"],\"StrArray\":[\"A\",\"B\",\"C\"]}" ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestMarshallNumbers
//
//  Test marshalling numbers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestMarshallNumbers
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

    NumberStruct theStruct = {0};

    // Check numbers
    theStruct.u8 = 100;
    theStruct.u16 = 1000;
    theStruct.u32 = 100000;
    theStruct.u64 = 10000000000;
    theStruct.u8h = 0x77;
    theStruct.u16h = 0x7777;
    theStruct.u32h = 0x77777777;
    theStruct.u64h = 0x7777777777777777;
    theStruct.s8 = -100;
    theStruct.s16 = -1000;
    theStruct.s32 = -100000;
    theStruct.s64 = -10000000000;
    theStruct.f32 = (float)100.5;
    theStruct.f64 = 2.2;
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalNumberStruct, NumElements(marshalNumberStruct),
        "{\"u8\":100,\"u16\":1000,\"u32\":100000,\"u64\":10000000000,"
        "\"u8h\":119,\"u16h\":30583,\"u32h\":2004318071,\"u64h\":8608480567731124087,"
        "\"s8\":-100,\"s16\":-1000,\"s32\":-100000,\"s64\":-10000000000,\"f32\":100.5,\"f64\":2.2}" ) );

    // Now Json5
    JL_ASSERT_SUCCESS( MakeJson5AndVerify( &theStruct, marshalNumberStruct, NumElements(marshalNumberStruct),
        "{u8:100,u16:1000,u32:100000,u64:10000000000,"
        "u8h:0x77,u16h:0x7777,u32h:0x77777777,u64h:0x7777777777777777,"
        "s8:-100,s16:-1000,s32:-100000,s64:-10000000000,f32:100.5,f64:2.2}" ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestMarshallArrayNumbers
//
//  Test marshalling arrays of numbers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestMarshallArrayNumbers
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct
    {
        uint16_t u16sFixed[4];
        size_t u16sFixedCount;
        uint64_t* u64sVar;
        size_t u64sVarCount;
        uint32_t u32sHexFixed[4];
        size_t u32sHexFixedCount;
        uint32_t* u32sHexVar;
        size_t u32sHexVarCount;
        int32_t s32sFixed[4];
        size_t s32sFixedCount;
        int8_t* s8sVar;
        size_t s8sVarCount;
        float f32sFixed[4];
        size_t f32sFixedCount;
        double* f64sVar;
        size_t f64sVarCount;
    } StringStruct;

    JlMarshallElement marshalStringStruct[] =
    {
        JlMarshallUnsignedFixedArray( StringStruct, u16sFixed, u16sFixedCount, "u16sFixed" ),
        JlMarshallUnsignedArray( StringStruct, u64sVar, u64sVarCount, "u64sVar" ),
        JlMarshallUnsignedHexFixedArray( StringStruct, u32sHexFixed, u32sHexFixedCount, "u32sHexFixed" ),
        JlMarshallUnsignedHexArray( StringStruct, u32sHexVar, u32sHexVarCount, "u32sHexVar" ),
        JlMarshallSignedFixedArray( StringStruct, s32sFixed, s32sFixedCount, "s32sFixed" ),
        JlMarshallSignedArray( StringStruct, s8sVar, s8sVarCount, "s8sVar" ),
        JlMarshallFloatFixedArray( StringStruct, f32sFixed, f32sFixedCount, "f32sFixed" ),
        JlMarshallFloatArray( StringStruct, f64sVar, f64sVarCount, "f64sVar" ),
    };

    StringStruct theStruct = {{0}};

    // Check marshalling empty arrays
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                                          "{\"u16sFixed\":[],\"u64sVar\":[],"
                                          "\"u32sHexFixed\":[],\"u32sHexVar\":[],"
                                          "\"s32sFixed\":[],\"s8sVar\":[],"
                                          "\"f32sFixed\":[],\"f64sVar\":[]}" ) );

    // Now with numbers
    theStruct.u16sFixed[0] = 1;
    theStruct.u16sFixed[1] = 2;
    theStruct.u16sFixedCount = 2;
    uint64_t u64Array[4] = { 3, 4 };
    theStruct.u64sVar = u64Array;
    theStruct.u64sVarCount = 2;

    theStruct.u32sHexFixed[0] = 0xabc;
    theStruct.u32sHexFixed[1] = 0xdef;
    theStruct.u32sHexFixedCount = 2;
    uint32_t u32sHexArray[4] = { 0x12345, 0x6789a };
    theStruct.u32sHexVar = u32sHexArray;
    theStruct.u32sHexVarCount = 2;

    theStruct.s32sFixed[0] = -5;
    theStruct.s32sFixed[1] = 6;
    theStruct.s32sFixedCount = 2;
    int8_t s8Array[4] = { -7, 8 };
    theStruct.s8sVar = s8Array;
    theStruct.s8sVarCount = 2;

    theStruct.f32sFixed[0] = 9.5;
    theStruct.f32sFixed[1] = 10.5;
    theStruct.f32sFixedCount = 2;
    double f64Array[4] = { 11.5, 12.5 };
    theStruct.f64sVar = f64Array;
    theStruct.f64sVarCount = 2;

    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                                          "{\"u16sFixed\":[1,2],\"u64sVar\":[3,4],"
                                          "\"u32sHexFixed\":[2748,3567],\"u32sHexVar\":[74565,424090],"
                                          "\"s32sFixed\":[-5,6],\"s8sVar\":[-7,8],"
                                          "\"f32sFixed\":[9.5,10.5],\"f64sVar\":[11.5,12.5]}" ) );

    JL_ASSERT_SUCCESS( MakeJson5AndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                                          "{u16sFixed:[1,2],u64sVar:[3,4],"
                                          "u32sHexFixed:[0x0abc,0x0def],u32sHexVar:[0x00012345,0x0006789a],"
                                          "s32sFixed:[-5,6],s8sVar:[-7,8],"
                                          "f32sFixed:[9.5,10.5],f64sVar:[11.5,12.5]}" ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestMarshallBools
//
//  Test marshalling Bools
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestMarshallBools
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct
    {
        bool BoolValue;
    } BoolStruct;

    JlMarshallElement marshalBoolStruct[] =
    {
        JlMarshallBool( BoolStruct, BoolValue, "b" ),
    };

    BoolStruct theStruct = {0};

    theStruct.BoolValue = true;
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalBoolStruct, NumElements(marshalBoolStruct), "{\"b\":true}" ) );

    theStruct.BoolValue = false;
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalBoolStruct, NumElements(marshalBoolStruct), "{\"b\":false}" ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestMarshallArrayBools
//
//  Test marshalling arrays of bools
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestMarshallArrayBools
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct
    {
        bool fixedArray[5];
        uint16_t fixedArrayCount;
        bool* varArray;
        uint16_t varArrayCount;
    } StringStruct;

    JlMarshallElement marshalStringStruct[] =
    {
        JlMarshallBoolFixedArray( StringStruct, fixedArray, fixedArrayCount, "fixedArray" ),
        JlMarshallBoolArray( StringStruct, varArray, varArrayCount, "varArray" ),
    };

    StringStruct theStruct = {{0}};

    // Check marshalling empty arrays
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                                          "{\"fixedArray\":[],\"varArray\":[]}" ) );

    // Now values
    theStruct.fixedArray[0] = true;
    theStruct.fixedArray[1] = false;
    theStruct.fixedArrayCount = 2;
    bool boolArray[4] = { false, true };
    theStruct.varArray = boolArray;
    theStruct.varArrayCount = 2;

    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalStringStruct, NumElements(marshalStringStruct),
                                          "{\"fixedArray\":[true,false],\"varArray\":[false,true]}" ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestMarshallStructs
//
//  Test marshalling structs within structs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestMarshallStructs
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct
    {
        uint64_t    Number64;
    } InnerStruct1;

    typedef struct
    {
        uint16_t    Number16;
    } InnerStruct2;

    typedef struct
    {
        InnerStruct1    struct1;
        InnerStruct2    struct2;
    } OuterStruct;

    JlMarshallElement marshalStruct1[] =
    {
        JlMarshallUnsigned( InnerStruct1, Number64, "num64" ),
    };
    JlMarshallElement marshalStruct2[] =
    {
        JlMarshallUnsigned( InnerStruct2, Number16, "num16" ),
    };
    JlMarshallElement marshalOuter[] =
    {
        JlMarshallStruct( OuterStruct, struct1, "struct1", marshalStruct1, NumElements(marshalStruct1) ),
        JlMarshallStruct( OuterStruct, struct2, "struct2", marshalStruct2, NumElements(marshalStruct2) ),
    };

    OuterStruct theStruct = {{0}};

    theStruct.struct1.Number64 = 100;
    theStruct.struct2.Number16 = 200;
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStruct, marshalOuter, NumElements(marshalOuter),
                                          "{\"struct1\":{\"num64\":100},\"struct2\":{\"num16\":200}}" ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestMarshallArrayStructs
//
//  Test marshalling arrays of structs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestMarshallArrayStructs
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    typedef struct
    {
        uint64_t    Number64;
    } InnerStructX1;

    typedef struct
    {
        uint16_t    Number16;
    } InnerStructX2;

    typedef struct
    {
        InnerStructX1   struct1Fixed[4];
        size_t          struct1FixedCount;
        InnerStructX1*  struct1Var;
        size_t          struct1VarCount;
        InnerStructX2   struct2Fixed[4];
        size_t          struct2FixedCount;
        InnerStructX2*  struct2Var;
        size_t          struct2VarCount;
    } OuterStructX;

    JlMarshallElement marshalStructX1[] =
    {
        JlMarshallUnsigned( InnerStructX1, Number64, "num64" ),
    };
    JlMarshallElement marshalStructX2[] =
    {
        JlMarshallUnsigned( InnerStructX2, Number16, "num16" ),
    };
    JlMarshallElement marshalOuterX[] =
    {
        JlMarshallStructFixedArray( OuterStructX, struct1Fixed, struct1FixedCount, "struct1Fixed", InnerStructX1, marshalStructX1, NumElements(marshalStructX1) ),
        JlMarshallStructArray( OuterStructX, struct1Var, struct1VarCount, "struct1Var", InnerStructX1, marshalStructX1, NumElements(marshalStructX1) ),
        JlMarshallStructFixedArray( OuterStructX, struct2Fixed, struct2FixedCount, "struct2Fixed", InnerStructX2, marshalStructX2, NumElements(marshalStructX2) ),
        JlMarshallStructArray( OuterStructX, struct2Var, struct2VarCount, "struct2Var", InnerStructX2, marshalStructX2, NumElements(marshalStructX2) ),
    };

    OuterStructX theStructX = {{{0}}};

    // Test empty
    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStructX, marshalOuterX, NumElements(marshalOuterX),
                                          "{\"struct1Fixed\":[],\"struct1Var\":[],"
                                          "\"struct2Fixed\":[],\"struct2Var\":[]}" ) );

    // Now with data
    theStructX.struct1Fixed[0].Number64 = 1;
    theStructX.struct1Fixed[1].Number64 = 2;
    theStructX.struct1Fixed[2].Number64 = 3;
    theStructX.struct1FixedCount = 3;
    InnerStructX1 inner1 [4] = {{0}};
    inner1[0].Number64 = 4;
    inner1[1].Number64 = 5;
    theStructX.struct1Var = inner1;
    theStructX.struct1VarCount = 2;

    theStructX.struct2Fixed[0].Number16 = 6;
    theStructX.struct2Fixed[1].Number16 = 7;
    theStructX.struct2Fixed[2].Number16 = 8;
    theStructX.struct2FixedCount = 3;
    InnerStructX2 inner2 [4] = {{0}};
    inner2[0].Number16 = 9;
    inner2[1].Number16 = 10;
    theStructX.struct2Var = inner2;
    theStructX.struct2VarCount = 2;

    JL_ASSERT_SUCCESS( MakeJsonAndVerify( &theStructX, marshalOuterX, NumElements(marshalOuterX),
                                          "{\"struct1Fixed\":[{\"num64\":1},{\"num64\":2},{\"num64\":3}],"
                                          "\"struct1Var\":[{\"num64\":4},{\"num64\":5}],"
                                          "\"struct2Fixed\":[{\"num16\":6},{\"num16\":7},{\"num16\":8}],"
                                          "\"struct2Var\":[{\"num16\":9},{\"num16\":10}]}" ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestMarshallBinary
//
//  Test marshalling arbitrary binary into base64 strings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestMarshallBinary
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

    // Check marshalling zero values
    JL_ASSERT_SUCCESS( MakeJson5AndVerify( &theStruct, marshalTestStruct, NumElements(marshalTestStruct),
                                          "{FixedBlob:'AAAAAAAAAAAAAA',BinaryBlob2:'AAAAAAAAAAA'}" ) );

    // Check other values
    memcpy( theStruct.FixedBlob, "ThisIsTest", 10 );
    theStruct.BinaryBlob2 = 0x123456789ABCDEF0ULL;
    JL_ASSERT_SUCCESS( MakeJson5AndVerify( &theStruct, marshalTestStruct, NumElements(marshalTestStruct),
                                          "{FixedBlob:'VGhpc0lzVGVzdA',BinaryBlob2:'8N68mnhWNBI'}" ) );

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
    theStruct2.VariableBlob1 = (uint8_t*) "1234567890";
    theStruct2.VariableBlob1Size = 10;
    theStruct2.VariableBlob2 = "1234567890AAAABBBBCCCCDDDD";
    theStruct2.VariableBlob2Size = 26;

    JL_ASSERT_SUCCESS( MakeJson5AndVerify(
        &theStruct2, marshalTestStruct2, NumElements(marshalTestStruct2),
        "{VariableBlob1:'MTIzNDU2Nzg5MA',VariableBlob2:'MTIzNDU2Nzg5MEFBQUFCQkJCQ0NDQ0REREQ',VariableBlob3:null}" ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibTests_Marshall_Register
//
//  Registers the tests with WjTestLib
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
    JsonLibTests_Marshall_Register
    (
        void
    )
{
    WjTestLib_NewGroup( "Marshall" );
    WjTestLib_AddTest( TestMarshallStrings, "Strings" );
    WjTestLib_AddTest( TestMarshallArrayStrings, "ArrayStrings" );
    WjTestLib_AddTest( TestMarshallNumbers, "Numbers" );
    WjTestLib_AddTest( TestMarshallArrayNumbers, "ArrayNumbers" );
    WjTestLib_AddTest( TestMarshallBools, "Bools" );
    WjTestLib_AddTest( TestMarshallArrayBools, "ArrayBools" );
    WjTestLib_AddTest( TestMarshallStructs, "Structs" );
    WjTestLib_AddTest( TestMarshallArrayStructs, "ArrayStructs" );
    WjTestLib_AddTest( TestMarshallBinary, "BinaryData" );
}
