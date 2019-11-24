////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibTests
//
//  Unit tests for JsonLib - RoundTrip
//  Tests json round tripping through the parser and writer
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
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
//  TEST FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestParseJson1AndOutput
//
//  Tests parsing json with JlParseJson and outputting it back with JlOutputJson
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestParseJson1AndOutput
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    JlDataObject* objectTree = NULL;
    char* outputJson = NULL;
    size_t errorPos = 0;

    const char inputJson[] =
        "{ \"Key1\": \"Value1\", \"Key2\" : \"Value2\", \"Key3\":[1,2,3 ,4 , 5  ,6,7],\n"
        "  \"Key4\" : { \"SubKey1\" : true, \"SubKey2\":false, \"SubKey3\":null }, \n"
        "  \"Key5\":{ \"a\":1000, \"b\":2000.5, \"c\":-3000, \"d\":-4e123 },\n"
        "  \"Key6\":{ \"a\":[\"aa\",\"bb\",true,false,1,2,3,4,-5,null,\"\",{\"a\":0}], \"b\":{\"c\":1,\"d\":2} } }\n";

    // Parse as Json1
    JL_ASSERT_SUCCESS( JlParseJsonEx( inputJson, false, &objectTree, &errorPos ) );
    if( NULL == objectTree ) { return TestReturn; }

    // Output as Json1, no formatting
    JL_ASSERT_SUCCESS( JlOutputJson( objectTree, false, &outputJson ) );
    JL_ASSERT( strcmp( outputJson,
        "{\"Key1\":\"Value1\",\"Key2\":\"Value2\",\"Key3\":[1,2,3,4,5,6,7],"
        "\"Key4\":{\"SubKey1\":true,\"SubKey2\":false,\"SubKey3\":null},"
        "\"Key5\":{\"a\":1000,\"b\":2000.5,\"c\":-3000,\"d\":-4e+123},"
        "\"Key6\":{\"a\":[\"aa\",\"bb\",true,false,1,2,3,4,-5,null,null,{\"a\":0}],\"b\":{\"c\":1,\"d\":2}}}" ) == 0 );

    JL_ASSERT_SUCCESS( JlFreeObjectTree( &objectTree ) );
    JL_ASSERT_SUCCESS( JlFreeJsonStringBuffer( &outputJson ) );

    // Parse as Json5
    JL_ASSERT_SUCCESS( JlParseJsonEx( inputJson, true, &objectTree, &errorPos ) );
    if( NULL == objectTree ) { return TestReturn; }

    // Output as Json1, no formatting
    JL_ASSERT_SUCCESS( JlOutputJson( objectTree, false, &outputJson ) );
    JL_ASSERT( strcmp( outputJson,
        "{\"Key1\":\"Value1\",\"Key2\":\"Value2\",\"Key3\":[1,2,3,4,5,6,7],"
        "\"Key4\":{\"SubKey1\":true,\"SubKey2\":false,\"SubKey3\":null},"
        "\"Key5\":{\"a\":1000,\"b\":2000.5,\"c\":-3000,\"d\":-4e+123},"
        "\"Key6\":{\"a\":[\"aa\",\"bb\",true,false,1,2,3,4,-5,null,null,{\"a\":0}],\"b\":{\"c\":1,\"d\":2}}}" ) == 0 );

    JL_ASSERT_SUCCESS( JlFreeJsonStringBuffer( &outputJson ) );

    // Output as Json1, with indenting formatting
    JL_ASSERT_SUCCESS( JlOutputJson( objectTree, true, &outputJson ) );
    JL_ASSERT( strcmp( outputJson,
        "{\n"
        "    \"Key1\": \"Value1\",\n"
        "    \"Key2\": \"Value2\",\n"
        "    \"Key3\": [\n"
        "        1,\n"
        "        2,\n"
        "        3,\n"
        "        4,\n"
        "        5,\n"
        "        6,\n"
        "        7\n"
        "    ],\n"
        "    \"Key4\": {\n"
        "        \"SubKey1\": true,\n"
        "        \"SubKey2\": false,\n"
        "        \"SubKey3\": null\n"
        "    },\n"
        "    \"Key5\": {\n"
        "        \"a\": 1000,\n"
        "        \"b\": 2000.5,\n"
        "        \"c\": -3000,\n"
        "        \"d\": -4e+123\n"
        "    },\n"
        "    \"Key6\": {\n"
        "        \"a\": [\n"
        "            \"aa\",\n"
        "            \"bb\",\n"
        "            true,\n"
        "            false,\n"
        "            1,\n"
        "            2,\n"
        "            3,\n"
        "            4,\n"
        "            -5,\n"
        "            null,\n"
        "            null,\n"
        "            {\n"
        "                \"a\": 0\n"
        "            }\n"
        "        ],\n"
        "        \"b\": {\n"
        "            \"c\": 1,\n"
        "            \"d\": 2\n"
        "        }\n"
        "    }\n"
        "}\n" ) == 0 );

    JL_ASSERT_SUCCESS( JlFreeObjectTree( &objectTree ) );
    JL_ASSERT_SUCCESS( JlFreeJsonStringBuffer( &outputJson ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestParseJson5AndOutput
//
//  Tests parsing json 5 with JlParseJson and outputting it back with JlOutputJson
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestParseJson5AndOutput
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    JlDataObject* objectTree = NULL;
    char* outputJson = NULL;
    size_t errorPos = 0;

    const char inputJson[] =
        "{ 'Key1': \"Value1\", Key2 : 'Value2', Key3:[0x1,2,3 ,4 , 0x5  ,6,7],\n"
        "  'Key 4' : { SubKey1 : true, SubKey2:false, SubKey3:null }, \n"
        "  Key5:{ a:0x1000, b:2000.5, c:-3000, d:-4e123 },\n"
        "  '0Key6':{ a:[\"aa\",\"bb\",true,false,1,2,3,4,-5,null,\"\",{a:0}], b:{c:1,d:2} } }\n";

    // Parse as Json5
    JL_ASSERT_SUCCESS( JlParseJsonEx( inputJson, true, &objectTree, &errorPos ) );
    if( NULL == objectTree ) { return TestReturn; }

    // Output as Json1, no formatting
    JL_ASSERT_SUCCESS( JlOutputJsonEx( objectTree, JL_OUTPUT_FLAGS_NONE, &outputJson ) );
    JL_ASSERT( strcmp( outputJson,
        "{\"Key1\":\"Value1\",\"Key2\":\"Value2\",\"Key3\":[1,2,3,4,5,6,7],"
        "\"Key 4\":{\"SubKey1\":true,\"SubKey2\":false,\"SubKey3\":null},"
        "\"Key5\":{\"a\":4096,\"b\":2000.5,\"c\":-3000,\"d\":-4e+123},"
        "\"0Key6\":{\"a\":[\"aa\",\"bb\",true,false,1,2,3,4,-5,null,null,{\"a\":0}],\"b\":{\"c\":1,\"d\":2}}}" ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeJsonStringBuffer( &outputJson ) );

    // Output as Json5, Allow bare keywords
    JL_ASSERT_SUCCESS( JlOutputJsonEx( objectTree, JL_OUTPUT_FLAGS_J5_USE_BARE_KEYWORDS, &outputJson ) );
    JL_ASSERT( strcmp( outputJson,
        "{Key1:\"Value1\",Key2:\"Value2\",Key3:[1,2,3,4,5,6,7],"
        "\"Key 4\":{SubKey1:true,SubKey2:false,SubKey3:null},"
        "Key5:{a:4096,b:2000.5,c:-3000,d:-4e+123},"
        "\"0Key6\":{a:[\"aa\",\"bb\",true,false,1,2,3,4,-5,null,null,{a:0}],b:{c:1,d:2}}}" ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeJsonStringBuffer( &outputJson ) );

    // Output as Json5, use single quotes instead of double
    JL_ASSERT_SUCCESS( JlOutputJsonEx( objectTree, JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES, &outputJson ) );
    JL_ASSERT( strcmp( outputJson,
        "{'Key1':'Value1','Key2':'Value2','Key3':[1,2,3,4,5,6,7],"
        "'Key 4':{'SubKey1':true,'SubKey2':false,'SubKey3':null},"
        "'Key5':{'a':4096,'b':2000.5,'c':-3000,'d':-4e+123},"
        "'0Key6':{'a':['aa','bb',true,false,1,2,3,4,-5,null,null,{'a':0}],'b':{'c':1,'d':2}}}" ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeJsonStringBuffer( &outputJson ) );

    // Output as Json5, Allow bare keywords and single quotes instead of double
    JL_ASSERT_SUCCESS( JlOutputJsonEx( objectTree, JL_OUTPUT_FLAGS_J5_USE_BARE_KEYWORDS|JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES, &outputJson ) );
    JL_ASSERT( strcmp( outputJson,
        "{Key1:'Value1',Key2:'Value2',Key3:[1,2,3,4,5,6,7],"
        "'Key 4':{SubKey1:true,SubKey2:false,SubKey3:null},"
        "Key5:{a:4096,b:2000.5,c:-3000,d:-4e+123},"
        "'0Key6':{a:['aa','bb',true,false,1,2,3,4,-5,null,null,{a:0}],b:{c:1,d:2}}}" ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeJsonStringBuffer( &outputJson ) );

    // Output as Json5, add trailing commas
    JL_ASSERT_SUCCESS( JlOutputJsonEx( objectTree, JL_OUTPUT_FLAGS_J5_TRAILING_COMMAS, &outputJson ) );
    JL_ASSERT( strcmp( outputJson,
        "{\"Key1\":\"Value1\",\"Key2\":\"Value2\",\"Key3\":[1,2,3,4,5,6,7,],"
        "\"Key 4\":{\"SubKey1\":true,\"SubKey2\":false,\"SubKey3\":null,},"
        "\"Key5\":{\"a\":4096,\"b\":2000.5,\"c\":-3000,\"d\":-4e+123,},"
        "\"0Key6\":{\"a\":[\"aa\",\"bb\",true,false,1,2,3,4,-5,null,null,{\"a\":0,},],\"b\":{\"c\":1,\"d\":2,},},}" ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeJsonStringBuffer( &outputJson ) );

    // Output as Json5, allow hex
    JL_ASSERT_SUCCESS( JlOutputJsonEx( objectTree, JL_OUTPUT_FLAGS_J5_ALLOW_HEX, &outputJson ) );
    JL_ASSERT( strcmp( outputJson,
        "{\"Key1\":\"Value1\",\"Key2\":\"Value2\",\"Key3\":[0x01,2,3,4,0x05,6,7],"
        "\"Key 4\":{\"SubKey1\":true,\"SubKey2\":false,\"SubKey3\":null},"
        "\"Key5\":{\"a\":0x1000,\"b\":2000.5,\"c\":-3000,\"d\":-4e+123},"
        "\"0Key6\":{\"a\":[\"aa\",\"bb\",true,false,1,2,3,4,-5,null,null,{\"a\":0}],\"b\":{\"c\":1,\"d\":2}}}" ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeJsonStringBuffer( &outputJson ) );

    // Output with all options set!
    JL_OUTPUT_FLAGS allFlags =  JL_OUTPUT_FLAGS_INDENT | JL_OUTPUT_FLAGS_J5_USE_BARE_KEYWORDS | JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES
        | JL_OUTPUT_FLAGS_J5_TRAILING_COMMAS | JL_OUTPUT_FLAGS_J5_ALLOW_HEX;
    JL_ASSERT_SUCCESS( JlOutputJsonEx( objectTree, allFlags, &outputJson ) );
    JL_ASSERT( strcmp( outputJson,
    "{\n"
    "    Key1: 'Value1',\n"
    "    Key2: 'Value2',\n"
    "    Key3: [\n"
    "        0x01,\n"
    "        2,\n"
    "        3,\n"
    "        4,\n"
    "        0x05,\n"
    "        6,\n"
    "        7,\n"
    "    ],\n"
    "    'Key 4': {\n"
    "        SubKey1: true,\n"
    "        SubKey2: false,\n"
    "        SubKey3: null,\n"
    "    },\n"
    "    Key5: {\n"
    "        a: 0x1000,\n"
    "        b: 2000.5,\n"
    "        c: -3000,\n"
    "        d: -4e+123,\n"
    "    },\n"
    "    '0Key6': {\n"
    "        a: [\n"
    "            'aa',\n"
    "            'bb',\n"
    "            true,\n"
    "            false,\n"
    "            1,\n"
    "            2,\n"
    "            3,\n"
    "            4,\n"
    "            -5,\n"
    "            null,\n"
    "            null,\n"
    "            {\n"
    "                a: 0,\n"
    "            },\n"
    "        ],\n"
    "        b: {\n"
    "            c: 1,\n"
    "            d: 2,\n"
    "        },\n"
    "    },\n"
    "}\n" ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeJsonStringBuffer( &outputJson ) );

    JL_ASSERT_SUCCESS( JlFreeObjectTree( &objectTree ) );
    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Types
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint64_t    u64;
    uint32_t    u32;
    uint16_t    u16;
    uint8_t     u8;
    int64_t     s64;
    int32_t     s32;
    int16_t     s16;
    int8_t      s8;
    float       f32;
    double      f64;
    uint64_t    hex64;
    uint32_t    hex32;
    char*       string;
    char        fixedString[10];
    bool        b1;
    bool        b2;
    uint8_t     fixedBin [16];
    uint8_t*    varBin;
    uint32_t    varBinSize;
} SubStructType;

typedef struct
{
    SubStructType   subStruct;
    SubStructType   subStructArray[4];
    size_t          subStructArrayCount;
} StructType;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CompareSubStructType
//
//  Compares two SubStructType vars. Returns true if they are same, false if different
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
bool
    CompareSubStructType
    (
        SubStructType const*       subStruct1,
        SubStructType const*       subStruct2
    )
{
    bool same = true;

    do
    {
        if( subStruct1->u64 != subStruct2->u64 ) { same = false; break; }
        if( subStruct1->u32 != subStruct2->u32 ) { same = false; break; }
        if( subStruct1->u16 != subStruct2->u16 ) { same = false; break; }
        if( subStruct1->u8 != subStruct2->u8 ) { same = false; break; }
        if( subStruct1->s64 != subStruct2->s64 ) { same = false; break; }
        if( subStruct1->s32 != subStruct2->s32 ) { same = false; break; }
        if( subStruct1->s16 != subStruct2->s16 ) { same = false; break; }
        if( subStruct1->s8 != subStruct2->s8 ) { same = false; break; }
        if( subStruct1->hex64 != subStruct2->hex64 ) { same = false; break; }
        if( subStruct1->hex32 != subStruct2->hex32 ) { same = false; break; }
        if( NULL == subStruct1->string )
        {
            if( NULL != subStruct2->string ) { same = false; break; }
        }
        else
        {
            if( strcmp( subStruct1->string, subStruct2->string ) != 0 ) { same = false; break; }
        }
        if( strcmp( subStruct1->fixedString, subStruct2->fixedString ) != 0 ) { same = false; break; }
        if( subStruct1->b1 != subStruct2->b1 ) { same = false; break; }
        if( subStruct1->b2 != subStruct2->b2 ) { same = false; break; }
        if( memcmp( subStruct1->fixedBin, subStruct2->fixedBin, sizeof(subStruct1->fixedBin) ) != 0 ) { same = false; break; }
        if( subStruct1->varBinSize != subStruct2->varBinSize ) { same = false; break; }
        if( memcmp( subStruct1->varBin, subStruct2->varBin, subStruct1->varBinSize ) != 0 ) { same = false; break; }
    } while( 0 );

    return same;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CompareStructType
//
//  Compares two StructType vars. Returns true if they are same, false if different
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
bool
    CompareStructType
    (
        StructType const*       struct1,
        StructType const*       struct2
    )
{
    bool same = true;

    if( !CompareSubStructType( &struct1->subStruct, &struct2->subStruct ) ) { same = false; }
    if( struct1->subStructArrayCount != struct2->subStructArrayCount ) { same = false; }
    if( struct1->subStructArrayCount == struct2->subStructArrayCount )
    {
        for( size_t i=0; i<struct1->subStructArrayCount; i++ )
        {
            if( !CompareSubStructType( &struct1->subStructArray[i], &struct2->subStructArray[i] ) ) { same = false; break; }
        }
    }

    return same;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestMarshallUnmarshall
//
//  Tests marshalling a struct into json and then unmarshalling it back to a struct and verifying data is
//  preserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestMarshallUnmarshall
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    JlMarshallElement marshalSubStructType[] =
    {
        JlMarshallUnsigned( SubStructType, u64, "u64" ),
        JlMarshallUnsigned( SubStructType, u32, "u32" ),
        JlMarshallUnsigned( SubStructType, u16, "u16" ),
        JlMarshallUnsigned( SubStructType, u8, "u8" ),
        JlMarshallSigned( SubStructType, s64, "s64" ),
        JlMarshallSigned( SubStructType, s32, "s32" ),
        JlMarshallSigned( SubStructType, s16, "s16" ),
        JlMarshallSigned( SubStructType, s8, "s8" ),
        JlMarshallFloat( SubStructType, f32, "f32" ),
        JlMarshallFloat( SubStructType, f64, "f64" ),
        JlMarshallUnsignedHex( SubStructType, hex64, "hex64" ),
        JlMarshallUnsignedHex( SubStructType, hex32, "hex32" ),
        JlMarshallString( SubStructType, string, "string" ),
        JlMarshallStringFixed( SubStructType, fixedString, "fixedString" ),
        JlMarshallBool( SubStructType, b1, "b1" ),
        JlMarshallBool( SubStructType, b2, "b2" ),
        JlMarshallBinaryFixed( SubStructType, fixedBin, "fixedBin" ),
        JlMarshallBinary( SubStructType, varBin, varBinSize, "varBin" ),
    };

    JlMarshallElement marshalStructType[] =
    {
        JlMarshallStruct( StructType, subStruct, "subStruct", marshalSubStructType, NumElements(marshalSubStructType) ),
        JlMarshallStructFixedArray( StructType, subStructArray, subStructArrayCount, "subStructArray", SubStructType, marshalSubStructType, NumElements(marshalSubStructType) ),
    };

    // Create structure full of data
    StructType theStruct = {{0}};

    theStruct.subStruct.u8 = 12;
    theStruct.subStruct.u16 = 1233;
    theStruct.subStruct.u32 = 54545613;
    theStruct.subStruct.u64 = 3455545654545613;
    theStruct.subStruct.s8 = 22;
    theStruct.subStruct.s16 = -2233;
    theStruct.subStruct.s32 = -654545613;
    theStruct.subStruct.s64 = 8455545654545613;
    theStruct.subStruct.f32 = 100.5;
    theStruct.subStruct.f64 = 21234.5;
    theStruct.subStruct.hex32 = 0x12345678;
    theStruct.subStruct.hex64 = 0x1122334455667788ULL;
    theStruct.subStruct.string = "This is a string to test";
    strcpy( theStruct.subStruct.fixedString, "smallstr" );
    theStruct.subStruct.b1 = true;
    theStruct.subStruct.b2 = false;
    strcpy( (char*)theStruct.subStruct.fixedBin, "X123456" );
    theStruct.subStruct.varBin = (uint8_t*) "12345678901234567890";
    theStruct.subStruct.varBinSize = 20;

    theStruct.subStructArrayCount = 2;

    theStruct.subStructArray[0].u8 = 17;
    theStruct.subStructArray[0].u16 = 878;
    theStruct.subStructArray[0].u32 = 2344613;
    theStruct.subStructArray[0].u64 = 999123;
    theStruct.subStructArray[0].s8 = -5;
    theStruct.subStructArray[0].s16 = -33;
    theStruct.subStructArray[0].s32 = -789545613;
    theStruct.subStructArray[0].s64 = 1234565654545613;
    theStruct.subStructArray[0].f32 = 200.5;
    theStruct.subStructArray[0].f64 = 61234.5;
    theStruct.subStructArray[0].hex32 = 0xA2345678;
    theStruct.subStructArray[0].hex64 = 0xA122334455667788;
    theStruct.subStructArray[0].string = "Also a string TEST";
    strcpy( theStruct.subStructArray[0].fixedString, "tiny" );
    theStruct.subStructArray[0].b1 = true;
    theStruct.subStructArray[0].b2 = false;
    strcpy( (char*)theStruct.subStructArray[0].fixedBin, "ABCDEF" );
    theStruct.subStructArray[0].varBin = (uint8_t*) "AAAAAAAAAA";
    theStruct.subStructArray[0].varBinSize = 10;

    theStruct.subStructArray[1].u8 = 87;
    theStruct.subStructArray[1].u16 = 7678;
    theStruct.subStructArray[1].u32 = 9944613;
    theStruct.subStructArray[1].u64 = 65499123;
    theStruct.subStructArray[1].s8 = -9;
    theStruct.subStructArray[1].s16 = -1233;
    theStruct.subStructArray[1].s32 = -613;
    theStruct.subStructArray[1].s64 = -1234567;
    theStruct.subStructArray[1].f32 = 40000.5;
    theStruct.subStructArray[1].f64 = 6231234.5;
    theStruct.subStructArray[1].hex32 = 0xB234B678;
    theStruct.subStructArray[1].hex64 = 0xB12B33B45B66B788;
    theStruct.subStructArray[1].string = "StringString STRING STRING \"String\"String 'String' String // String";
    strcpy( theStruct.subStructArray[1].fixedString, ":-)" );
    theStruct.subStructArray[1].b1 = true;
    theStruct.subStructArray[1].b2 = false;
    strcpy( (char*)theStruct.subStructArray[1].fixedBin, "......" );
    theStruct.subStructArray[1].varBin = (uint8_t*) "abcdeabcdeabcdeabcdeabcdeabcde";
    theStruct.subStructArray[1].varBinSize = 30;

    // Now marshall and unmarshall via json1 with indent formatting
    char* jsonString = NULL;
    JL_ASSERT_SUCCESS( JlStructToJsonEx(
        &theStruct, marshalStructType, NumElements(marshalStructType), JL_OUTPUT_FLAGS_INDENT, &jsonString ) );
    JL_ASSERT_NOT_NULL( jsonString );

    StructType newStruct = {{0}};
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        jsonString, marshalStructType, NumElements(marshalStructType), &newStruct, NULL ) );
    JL_ASSERT( CompareStructType( &theStruct, &newStruct ) );

    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalStructType, NumElements(marshalStructType), &newStruct ) );
    JlFree( jsonString );

    // Now marshall and unmarshall via json5 with indent formatting
    JL_ASSERT_SUCCESS( JlStructToJsonEx(
        &theStruct, marshalStructType, NumElements(marshalStructType), JL_OUTPUT_FLAGS_JSON5 | JL_OUTPUT_FLAGS_INDENT, &jsonString ) );
    JL_ASSERT_NOT_NULL( jsonString );

    memset( &newStruct, 0, sizeof(newStruct) );
    JL_ASSERT_SUCCESS( JlJsonToStruct(
        jsonString, marshalStructType, NumElements(marshalStructType), &newStruct, NULL ) );
    JL_ASSERT( CompareStructType( &theStruct, &newStruct ) );

    JL_ASSERT_SUCCESS( JlUnmarshallFreeStructAllocs( marshalStructType, NumElements(marshalStructType), &newStruct ) );
    JlFree( jsonString );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibTests_RoundTrip_Register
//
//  Registers the tests with WjTestLib
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
    JsonLibTests_RoundTrip_Register
    (
        void
    )
{
    WjTestLib_NewGroup( "RoundTrip" );
    WjTestLib_AddTest( TestParseJson1AndOutput, "Parse/Output (Json1)" );
    WjTestLib_AddTest( TestParseJson5AndOutput, "Parse/Output (Json5)" );
    WjTestLib_AddTest( TestMarshallUnmarshall, "Marshall/Unmarshall" );
}
