////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibTests
//
//  Unit tests for JsonLib - Output
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
//  TEST FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyOutputUnsigned
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyOutputUnsigned
    (
        uint64_t    Number,
        char const* StringMatch
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    JlDataObject* numberObject = NULL;
    char* jsonString = NULL;

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_NUMBER, &numberObject ) );
    JL_ASSERT_SUCCESS( JlSetObjectNumberU64( numberObject, Number ) );
    JL_ASSERT_SUCCESS( JlOutputJson( numberObject, false, &jsonString ) );
    JL_ASSERT( strcmp( jsonString, StringMatch ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &numberObject ) );
    JlFree( jsonString );
    jsonString = NULL;

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyOutputSigned
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyOutputSigned
    (
        int64_t     Number,
        char const* StringMatch
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    JlDataObject* numberObject = NULL;
    char* jsonString = NULL;

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_NUMBER, &numberObject ) );
    JL_ASSERT_SUCCESS( JlSetObjectNumberS64( numberObject, Number ) );
    JL_ASSERT_SUCCESS( JlOutputJson( numberObject, false, &jsonString ) );
    JL_ASSERT( strcmp( jsonString, StringMatch ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &numberObject ) );
    JlFree( jsonString );
    jsonString = NULL;

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyOutputFloat
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyOutputFloat
    (
        double      Number,
        char const* StringMatch
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    JlDataObject* numberObject = NULL;
    char* jsonString = NULL;

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_NUMBER, &numberObject ) );
    JL_ASSERT_SUCCESS( JlSetObjectNumberF64( numberObject, Number ) );
    JL_ASSERT_SUCCESS( JlOutputJson( numberObject, false, &jsonString ) );
    JL_ASSERT( strcmp( jsonString, StringMatch ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &numberObject ) );
    JlFree( jsonString );
    jsonString = NULL;

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestNumbers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestNumbers
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    JL_ASSERT_SUCCESS( VerifyOutputUnsigned( 10, "10" ) );
    JL_ASSERT_SUCCESS( VerifyOutputUnsigned( 0, "0" ) );
    JL_ASSERT_SUCCESS( VerifyOutputUnsigned( UINT64_MAX, "18446744073709551615" ) );

    JL_ASSERT_SUCCESS( VerifyOutputSigned( 10, "10" ) );
    JL_ASSERT_SUCCESS( VerifyOutputSigned( -10, "-10" ) );

    JL_ASSERT_SUCCESS( VerifyOutputFloat( 0.0, "0" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 10.1, "10.1" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.34e+125, "2.34e+125" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 234e123, "2.34e+125" ) );

    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.1, "2.1" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.11, "2.11" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.111, "2.111" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.1111, "2.1111" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.11111, "2.11111" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.111111, "2.111111" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.1111111, "2.1111111" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.11111111, "2.11111111" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.111111111, "2.111111111" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.1111111111, "2.1111111111" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.11111111111, "2.11111111111" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.111111111111, "2.111111111111" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.1111111111111, "2.1111111111111" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.11111111111111, "2.11111111111111" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( 2.111111111111111, "2.111111111111111" ) );

    // Note there is slight rounding from the value of DBL_MAX (1.7976931348623158e+308)
    // and DBL_MIN (2.2250738585072014e-308)
    // This is because we don't output the full precision of the float. If we did then simple values
    // such as 2.1 would end up as 2.1000000000000001 due to 2.1 not being perfectly representable in floating format.
    JL_ASSERT_SUCCESS( VerifyOutputFloat( DBL_MAX, "1.797693134862316e+308" ) );
    JL_ASSERT_SUCCESS( VerifyOutputFloat( DBL_MIN, "2.225073858507201e-308" ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyOutputBool
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyOutputBool
    (
        bool        BoolValue,
        char const* StringMatch
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    JlDataObject* boolObject = NULL;
    char* jsonString = NULL;

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_BOOL, &boolObject ) );
    JL_ASSERT_SUCCESS( JlSetObjectBool( boolObject, BoolValue ) );
    JL_ASSERT_SUCCESS( JlOutputJson( boolObject, false, &jsonString ) );
    JL_ASSERT( strcmp( jsonString, StringMatch ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &boolObject ) );
    JlFree( jsonString );
    jsonString = NULL;

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestBools
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestBools
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    JL_ASSERT_SUCCESS( VerifyOutputBool( false, "false" ) );
    JL_ASSERT_SUCCESS( VerifyOutputBool( true, "true" ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyOutputString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyOutputString
    (
        char const* String,
        char const* StringMatch,
        bool        IsJson5
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    JlDataObject* stringObject = NULL;
    char* jsonString = NULL;
    JL_OUTPUT_FLAGS outputFlags = IsJson5 ? JL_OUTPUT_FLAGS_JSON5 : JL_OUTPUT_FLAGS_NONE;

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_STRING, &stringObject ) );
    JL_ASSERT_SUCCESS( JlSetObjectString( stringObject, String ) );
    JL_ASSERT_SUCCESS( JlOutputJsonEx( stringObject, outputFlags, &jsonString ) );
    JL_ASSERT( strcmp( jsonString, StringMatch ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &stringObject ) );
    JlFree( jsonString );
    jsonString = NULL;

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyInvalidString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyInvalidString
    (
        char const* String
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    JlDataObject* stringObject = NULL;
    char* jsonString = NULL;

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_STRING, &stringObject ) );
    JL_ASSERT_SUCCESS( JlSetObjectString( stringObject, String ) );
    JL_ASSERT_STATUS( JlOutputJson( stringObject, false, &jsonString ), JL_STATUS_INVALID_DATA );
    JL_ASSERT_NULL( jsonString );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &stringObject ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestStrings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestStrings
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    JL_ASSERT_SUCCESS( VerifyOutputString( "string", "\"string\"", false ) );
    JL_ASSERT_SUCCESS( VerifyOutputString( "", "null", false ) );
    JL_ASSERT_SUCCESS( VerifyOutputString( NULL, "null", false ) );

    JL_ASSERT_SUCCESS( VerifyOutputString( " string ", "\" string \"", false ) );
    JL_ASSERT_SUCCESS( VerifyOutputString( " ", "\" \"", false ) );
    JL_ASSERT_SUCCESS( VerifyOutputString( "it's", "\"it's\"", false ) );
    JL_ASSERT_SUCCESS( VerifyOutputString( "\n", "\"\\n\"", false ) );
    JL_ASSERT_SUCCESS( VerifyOutputString( "\x01\x02\x03\x04\x05\x06\x07\x08", "\"\\u0001\\u0002\\u0003\\u0004\\u0005\\u0006\\u0007\\b\"", false ) );
    JL_ASSERT_SUCCESS( VerifyOutputString( "\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10", "\"\\t\\n\\u000b\\f\\r\\u000e\\u000f\\u0010\"", false ) );
    JL_ASSERT_SUCCESS( VerifyOutputString( "\x11\x12\x13\x14\x15\x16\x17\x18", "\"\\u0011\\u0012\\u0013\\u0014\\u0015\\u0016\\u0017\\u0018\"", false ) );
    JL_ASSERT_SUCCESS( VerifyOutputString( "\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20", "\"\\u0019\\u001a\\u001b\\u001c\\u001d\\u001e\\u001f \"", false ) );

    JL_ASSERT_SUCCESS( VerifyOutputString( "_\"_", "\"_\\\"_\"", false ) );
    JL_ASSERT_SUCCESS( VerifyOutputString( "_\\_", "\"_\\\\_\"", false ) );
    JL_ASSERT_SUCCESS( VerifyOutputString( "\xF0\x9F\x98\x81", "\"\xF0\x9F\x98\x81\"", false ) );    // smiley face emoji

    JL_ASSERT_SUCCESS( VerifyInvalidString( "\xF0\x9F\x98Truncated" ) );    // utf8 sequence truncated

    // Test Json5 handling of single quote in a string when using single quotes for string
    JL_ASSERT_SUCCESS( VerifyOutputString( "it's", "'it\\'s'", true ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CreateJsonAndVerify
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    CreateJsonAndVerify
    (
        JlDataObject const* ObjectRoot,
        char const*         StringMatch
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    char* jsonString = NULL;

    JL_ASSERT_SUCCESS( JlOutputJson( ObjectRoot, false, &jsonString ) );
    JL_ASSERT( strcmp( jsonString, StringMatch ) == 0 );
    JlFree( jsonString );
    jsonString = NULL;
    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CreateJsonFormatedAndVerify
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    CreateJsonFormatedAndVerify
    (
        JlDataObject const* ObjectRoot,
        char const*         StringMatch
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    char* jsonString = NULL;

    JL_ASSERT_SUCCESS( JlOutputJson( ObjectRoot, true, &jsonString ) );
    JL_ASSERT( strcmp( jsonString, StringMatch ) == 0 );
    JlFree( jsonString );
    jsonString = NULL;
    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CreateJsonAndVerifyEx
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    CreateJsonAndVerifyEx
    (
        JlDataObject const* ObjectRoot,
        JL_OUTPUT_FLAGS     OutputFlags,
        char const*         StringMatch
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    char* jsonString = NULL;

    JL_ASSERT_SUCCESS( JlOutputJsonEx( ObjectRoot, OutputFlags, &jsonString ) );
    JL_ASSERT( strcmp( jsonString, StringMatch ) == 0 );
    JlFree( jsonString );
    jsonString = NULL;
    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestLists
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestLists
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    JlDataObject* listObject = NULL;
    JlDataObject* subListObject = NULL;
    JlDataObject* dictionaryObject = NULL;

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_LIST, &listObject ) );
    JL_ASSERT_SUCCESS( JlAddStringToListObject( listObject, "string1" ) );
    JL_ASSERT_SUCCESS( JlAddStringToListObject( listObject, "string2" ) );
    JL_ASSERT_SUCCESS( JlAddStringToListObject( listObject, "string3" ) );
    JL_ASSERT_SUCCESS( CreateJsonAndVerify( listObject, "[\"string1\",\"string2\",\"string3\"]" ) );
    JL_ASSERT_SUCCESS( CreateJsonFormatedAndVerify( listObject, "[\n    \"string1\",\n    \"string2\",\n    \"string3\"\n]\n" ) );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &listObject ) );

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_LIST, &listObject ) );
    JL_ASSERT_SUCCESS( JlAddStringToListObject( listObject, "string1" ) );
    JL_ASSERT_SUCCESS( JlAddNumberU64ToListObject( listObject, 1234 ) );
    JL_ASSERT_SUCCESS( JlAddBoolToListObject( listObject, true ) );
    JL_ASSERT_SUCCESS( JlAddNumberU64ToListObject( listObject, 5678 ) );
    JL_ASSERT_SUCCESS( JlAddBoolToListObject( listObject, false ) );
    JL_ASSERT_SUCCESS( JlAddNumberS64ToListObject( listObject, -999 ) );
    JL_ASSERT_SUCCESS( JlAddNumberF64ToListObject( listObject, 1.234 ) );
    JL_ASSERT_SUCCESS( JlAddStringToListObject( listObject, "string2" ) );
    JL_ASSERT_SUCCESS( CreateJsonAndVerify( listObject, "[\"string1\",1234,true,5678,false,-999,1.234,\"string2\"]" ) );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &listObject ) );

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_LIST, &listObject ) );
    JL_ASSERT_SUCCESS( JlAddStringToListObject( listObject, "string1" ) );
    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_DICTIONARY, &dictionaryObject ) );
    JL_ASSERT_SUCCESS( JlAttachObjectToListObject( listObject, dictionaryObject ) );
    JL_ASSERT_SUCCESS( JlAddStringToDictionaryObject( dictionaryObject, "str1", "stringA" ) );
    JL_ASSERT_SUCCESS( JlAddNumberU64ToDictionaryObject( dictionaryObject, "num1", 10000 ) );
    JL_ASSERT_SUCCESS( JlAddNumberS64ToDictionaryObject( dictionaryObject, "num2", -10000 ) );
    JL_ASSERT_SUCCESS( JlAddNumberF64ToDictionaryObject( dictionaryObject, "num3", 1.1 ) );
    JL_ASSERT_SUCCESS( JlAddBoolToDictionaryObject( dictionaryObject, "bool1", true ) );
    JL_ASSERT_SUCCESS( JlAddStringToListObject( listObject, "string2" ) );
    JL_ASSERT_SUCCESS( CreateJsonAndVerify( listObject, "[\"string1\",{\"str1\":\"stringA\",\"num1\":10000,\"num2\":-10000,\"num3\":1.1,\"bool1\":true},\"string2\"]" ) );
    JL_ASSERT_SUCCESS( CreateJsonFormatedAndVerify( listObject, "[\n    \"string1\",\n    {\n        \"str1\": \"stringA\",\n        \"num1\": 10000,\n        \"num2\": -10000,\n        \"num3\": 1.1,\n        \"bool1\": true\n    },\n    \"string2\"\n]\n" ) );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &listObject ) );

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_LIST, &listObject ) );
    JL_ASSERT_SUCCESS( JlAddStringToListObject( listObject, "string1" ) );
    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_LIST, &subListObject ) );
    JL_ASSERT_SUCCESS( JlAttachObjectToListObject( listObject, subListObject ) );
    JL_ASSERT_SUCCESS( JlAddNumberU64ToListObject( subListObject, 1 ) );
    JL_ASSERT_SUCCESS( JlAddNumberU64ToListObject( subListObject, 2 ) );
    JL_ASSERT_SUCCESS( JlAddNumberU64ToListObject( subListObject, 3 ) );
    JL_ASSERT_SUCCESS( CreateJsonAndVerify( listObject, "[\"string1\",[1,2,3]]" ) );
    JL_ASSERT_SUCCESS( CreateJsonFormatedAndVerify( listObject, "[\n    \"string1\",\n    [\n        1,\n        2,\n        3\n    ]\n]\n" ) );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &listObject ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestDictionaries
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestDictionaries
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    JlDataObject* dictionaryObject = NULL;
    JlDataObject* listObject = NULL;
    JlDataObject* subDictionaryObject = NULL;

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_DICTIONARY, &dictionaryObject ) );
    JL_ASSERT_SUCCESS( JlAddStringToDictionaryObject( dictionaryObject, "key1", "string1" ) );
    JL_ASSERT_SUCCESS( JlAddStringToDictionaryObject( dictionaryObject, "key2", "string2" ) );
    JL_ASSERT_SUCCESS( JlAddStringToDictionaryObject( dictionaryObject, "key3", "string3" ) );
    JL_ASSERT_SUCCESS( CreateJsonAndVerify( dictionaryObject, "{\"key1\":\"string1\",\"key2\":\"string2\",\"key3\":\"string3\"}" ) );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &dictionaryObject ) );

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_DICTIONARY, &dictionaryObject ) );
    JL_ASSERT_SUCCESS( JlAddStringToDictionaryObject( dictionaryObject, "key1", "string1" ) );
    JL_ASSERT_SUCCESS( JlAddNumberU64ToDictionaryObject( dictionaryObject, "num1", 111 ) );
    JL_ASSERT_SUCCESS( JlAddNumberS64ToDictionaryObject( dictionaryObject, "num2", -111 ) );
    JL_ASSERT_SUCCESS( JlAddNumberF64ToDictionaryObject( dictionaryObject, "num3", 111.1 ) );
    JL_ASSERT_SUCCESS( JlAddBoolToDictionaryObject( dictionaryObject, "bool1", true ) );
    JL_ASSERT_SUCCESS( JlAddBoolToDictionaryObject( dictionaryObject, "bool2", false ) );
    JL_ASSERT_SUCCESS( CreateJsonAndVerify( dictionaryObject, "{\"key1\":\"string1\",\"num1\":111,\"num2\":-111,\"num3\":111.1,\"bool1\":true,\"bool2\":false}" ) );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &dictionaryObject ) );

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_DICTIONARY, &dictionaryObject ) );
    JL_ASSERT_SUCCESS( JlAddStringToDictionaryObject( dictionaryObject, "key1", "string1" ) );
    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_LIST, &listObject ) );
    JL_ASSERT_SUCCESS( JlAttachObjectToDictionaryObject( dictionaryObject, "list", listObject ) );
    JL_ASSERT_SUCCESS( JlAddNumberU64ToListObject( listObject, 1 ) );
    JL_ASSERT_SUCCESS( JlAddNumberU64ToListObject( listObject, 2 ) );
    JL_ASSERT_SUCCESS( JlAddNumberU64ToListObject( listObject, 3 ) );
    JL_ASSERT_SUCCESS( JlAddStringToDictionaryObject( dictionaryObject, "key2", "string2" ) );
    JL_ASSERT_SUCCESS( CreateJsonAndVerify( dictionaryObject, "{\"key1\":\"string1\",\"list\":[1,2,3],\"key2\":\"string2\"}" ) );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &dictionaryObject ) );

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_DICTIONARY, &dictionaryObject ) );
    JL_ASSERT_SUCCESS( JlAddStringToDictionaryObject( dictionaryObject, "key1", "string1" ) );
    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_DICTIONARY, &subDictionaryObject ) );
    JL_ASSERT_SUCCESS( JlAttachObjectToDictionaryObject( dictionaryObject, "dictionary", subDictionaryObject ) );
    JL_ASSERT_SUCCESS( JlAddNumberU64ToDictionaryObject( subDictionaryObject, "num1", 1 ) );
    JL_ASSERT_SUCCESS( JlAddNumberU64ToDictionaryObject( subDictionaryObject, "num2", 2 ) );
    JL_ASSERT_SUCCESS( JlAddNumberU64ToDictionaryObject( subDictionaryObject, "num3", 3 ) );
    JL_ASSERT_SUCCESS( JlAddStringToDictionaryObject( dictionaryObject, "key2", "string2" ) );
    JL_ASSERT_SUCCESS( CreateJsonAndVerify( dictionaryObject, "{\"key1\":\"string1\",\"dictionary\":{\"num1\":1,\"num2\":2,\"num3\":3},\"key2\":\"string2\"}" ) );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &dictionaryObject ) );

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestOutputFormats
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestOutputFormats
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    JlDataObject* dictionaryObject = NULL;
    JlDataObject* numObject = NULL;

    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_DICTIONARY, &dictionaryObject ) );
    JL_ASSERT_SUCCESS( JlAddStringToDictionaryObject( dictionaryObject, "quotechars", "\"'" ) );
    JL_ASSERT_SUCCESS( JlAddStringToDictionaryObject( dictionaryObject, "emoji", "\xF0\x9F\x98\x81" ) );
    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_NUMBER, &numObject ) );
    JL_ASSERT_SUCCESS( JlSetObjectNumberHex( numObject, 0x1000 ) );
    JL_ASSERT_SUCCESS( JlAttachObjectToDictionaryObject( dictionaryObject, "hex", numObject ) );

    JL_ASSERT_SUCCESS( CreateJsonAndVerifyEx( dictionaryObject, JL_OUTPUT_FLAGS_NONE,
        "{\"quotechars\":\"\\\"'\",\"emoji\":\"\xF0\x9F\x98\x81\",\"hex\":4096}" ) );

    JL_ASSERT_SUCCESS( CreateJsonAndVerifyEx( dictionaryObject, JL_OUTPUT_FLAGS_INDENT,
        "{\n"
        "    \"quotechars\": \"\\\"'\",\n"
        "    \"emoji\": \"\xF0\x9F\x98\x81\",\n"
        "    \"hex\": 4096\n"
        "}\n" ) );

    JL_ASSERT_SUCCESS( CreateJsonAndVerifyEx( dictionaryObject, JL_OUTPUT_FLAGS_INDENT | JL_OUTPUT_FLAGS_J5_ALLOW_HEX,
        "{\n"
        "    \"quotechars\": \"\\\"'\",\n"
        "    \"emoji\": \"\xF0\x9F\x98\x81\",\n"
        "    \"hex\": 0x1000\n"
        "}\n" ) );

    JL_ASSERT_SUCCESS( CreateJsonAndVerifyEx( dictionaryObject, JL_OUTPUT_FLAGS_INDENT | JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES,
        "{\n"
        "    'quotechars': '\\\"\\'',\n"
        "    'emoji': '\xF0\x9F\x98\x81',\n"
        "    'hex': 4096\n"
        "}\n" ) );

    JL_ASSERT_SUCCESS( CreateJsonAndVerifyEx( dictionaryObject, JL_OUTPUT_FLAGS_INDENT | JL_OUTPUT_FLAGS_J5_USE_BARE_KEYWORDS,
        "{\n"
        "    quotechars: \"\\\"'\",\n"
        "    emoji: \"\xF0\x9F\x98\x81\",\n"
        "    hex: 4096\n"
        "}\n" ) );

    JL_ASSERT_SUCCESS( CreateJsonAndVerifyEx( dictionaryObject, JL_OUTPUT_FLAGS_INDENT | JL_OUTPUT_FLAGS_J5_TRAILING_COMMAS,
        "{\n"
        "    \"quotechars\": \"\\\"'\",\n"
        "    \"emoji\": \"\xF0\x9F\x98\x81\",\n"
        "    \"hex\": 4096,\n"
        "}\n" ) );

    JL_ASSERT_SUCCESS( CreateJsonAndVerifyEx( dictionaryObject, JL_OUTPUT_FLAGS_INDENT | JL_OUTPUT_FLAGS_JSON5,
        "{\n"
        "    quotechars: '\\\"\\'',\n"
        "    emoji: '\xF0\x9F\x98\x81',\n"
        "    hex: 0x1000\n"
        "}\n" ) );

    JL_ASSERT_SUCCESS( CreateJsonAndVerifyEx( dictionaryObject, JL_OUTPUT_FLAGS_INDENT | JL_OUTPUT_FLAGS_ASCII,
        "{\n"
        "    \"quotechars\": \"\\\"'\",\n"
        "    \"emoji\": \"\\u003d\\u0201\",\n"
        "    \"hex\": 4096\n"
        "}\n" ) );

    JL_ASSERT_SUCCESS( JlFreeObjectTree( &dictionaryObject ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibTests_Output_Register
//
//  Registers the tests with WjTestLib
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
    JsonLibTests_Output_Register
    (
        void
    )
{
    WjTestLib_NewGroup( "Output" );
    WjTestLib_AddTest( TestNumbers, "Numbers" );
    WjTestLib_AddTest( TestBools, "Bools" );
    WjTestLib_AddTest( TestStrings, "Strings" );
    WjTestLib_AddTest( TestLists, "Lists" );
    WjTestLib_AddTest( TestDictionaries, "Dictionaries" );
    WjTestLib_AddTest( TestOutputFormats, "OutputFormats" );
}
