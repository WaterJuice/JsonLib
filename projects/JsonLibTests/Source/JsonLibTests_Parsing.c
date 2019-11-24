////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibTests
//
//  Unit tests for JsonLib - Parsing
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
//  TEST FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonUnableToParse
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonUnableToParse
    (
        bool            IsJson5,
        char const*     Json,
        size_t          ErrorAtPos
    )
{
    JlDataObject*   object = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;
    JL_STATUS       jlStatus;

    JL_ASSERT_STATUS( jlStatus = JlParseJsonEx( Json, IsJson5, &object, &errorAtPos ), JL_STATUS_INVALID_DATA );
    JL_ASSERT_NULL( object );
    JL_ASSERT( errorAtPos == ErrorAtPos );
    if( errorAtPos != ErrorAtPos )
    {
        printf( "errorAtPos was %zu. Expected %zu\n", errorAtPos, ErrorAtPos );
    }

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonParsesAsStringObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonParsesAsStringObject
    (
        bool            IsJson5,
        char const*     Json,
        char const*     String
    )
{
    JlDataObject*   object = NULL;
    size_t          errorAtPos = 100;
    char const*     stringPtr = NULL;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;

    JL_ASSERT_SUCCESS( JlParseJsonEx( Json, IsJson5, &object, &errorAtPos ) );
    if( NULL == object ) { return TestReturn; }

    JL_ASSERT_NOT_NULL( object );
    JL_ASSERT( errorAtPos == 0 );

    JL_ASSERT_SUCCESS( JlGetObjectString( object, &stringPtr ) );
    if( NULL != stringPtr )
    {
        JL_ASSERT( strcmp( String, stringPtr ) == 0 );
    }
    else
    {
        JL_ASSERT( NULL == String );
    }

    JL_ASSERT_SUCCESS( JlFreeObjectTree( &object ) );
    JL_ASSERT_NULL( object );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonNotString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonNotString
    (
        bool            IsJson5,
        char const*     Json
    )
{
    JlDataObject*   object = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;

    JL_ASSERT_STATUS( JlParseJsonEx( Json, IsJson5, &object, &errorAtPos ), JL_STATUS_SUCCESS );
    JL_ASSERT_NOT_NULL( object );
    JL_ASSERT( JlGetObjectType( object ) != JL_DATA_TYPE_STRING );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &object ) )
    JL_ASSERT_NULL( object );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestStrings
//
//  Tests parsing a json object as a single text string.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestStrings
    (
        bool            IsJson5
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    // Test valid strings
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "   \"SingleString\"   ", "SingleString" ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"SingleString\"", "SingleString" ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"Single'String\"", "Single'String" ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"a\"", "a" ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"\"", NULL ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"\\n\"", "\n" ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\" \\b \\n \\r \\\" \\\\ \\f \\t \\/ \"", " \b \n \r \" \\ \f \t / " ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\" \\u0000 \"", " \xc0\x80 " ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"\\u0000\\u0001\\u0002\\u0003 \\u0004\\u0005\\u0006\\u0007\"", "\xc0\x80\x01\x02\x03 \x04\x05\x06\x07" ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"\\u0008\\u0009\\u000a\\u000b \\u000c\\u000d\\u000e\\u000f\"", "\b\t\n\x0b \x0c\r\x0e\x0f" ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"\\u0010\\u0011\\u0012\\u0013 \\u0014\\u0015\\u0016\\u0017\"", "\x10\x11\x12\x13 \x14\x15\x16\x17" ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"\\u0018\\u0019\\u001a\\u001b \\u001c\\u001d\\u001e\\u001f\"", "\x18\x19\x1a\x1b \x1c\x1d\x1e\x1f" ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"\\ud83d\\ude01\"", "\xF0\x9F\x98\x81" ) );    // smiley face emoji
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"\\uD83D\\ude01\"", "\xF0\x9F\x98\x81" ) );    // smiley face emoji
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"\\ud83d\\uDE01\"", "\xF0\x9F\x98\x81" ) );    // smiley face emoji
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"\\uD83D\\uDE01\"", "\xF0\x9F\x98\x81" ) );    // smiley face emoji
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "null", NULL ) );

    // Test valid objects that are not strings
    JL_ASSERT_SUCCESS( VerifyJsonNotString( IsJson5, "1234" ) );
    JL_ASSERT_SUCCESS( VerifyJsonNotString( IsJson5, "true" ) );
    JL_ASSERT_SUCCESS( VerifyJsonNotString( IsJson5, "false" ) );
    JL_ASSERT_SUCCESS( VerifyJsonNotString( IsJson5, "{ \"Dictionary\" : \"String\" }" ) );
    JL_ASSERT_SUCCESS( VerifyJsonNotString( IsJson5, "[ \"List\" ]" ) );

    // Test valid json5 strings
    if( IsJson5 )
    {
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "'SingleString'", "SingleString" ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "'Single\"String'", "Single\"String" ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"Split\\\nString\"", "SplitString" ) );  // escaped new line in string
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"Split\\\r\\\nString\"", "SplitString" ) );  // fully escaped cr and lf in string
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsStringObject( IsJson5, "\"Windows\\\r\nNewLine\"", "WindowsNewLine" ) );  // windows escaped crlf
    }

    // Test invalid strings
    if( !IsJson5 )
    {
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " 'SingleString' ", 1 ) );
    }
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "\t\n \"SingleString' ", 3 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "    SingleString ", 4 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "\"Single\nString\"", 7 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " \"SingleString'", 1 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " 'SingleString\"", 1 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " \"Noend", 1 ) );

    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "\"Windows\\\n\rCRLF Wrong\"", 10 ) );  // don't allow single escaped LF CR (wrong order)

    // Check doesn't allow a string following a string
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "\"Single\" \"String\"", 8 ) );
    if( IsJson5 )
    {
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "'Single' 'String'", 8 ) );
    }

    // Test invalid escapes
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "\"1234\\a6789\"", 5 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "\"1234\\x11 6789\"", 5 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "\"1234\\Ud83d\\Ude01\"", 5 ) );    // uppercase \U
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "\"1234\\Ud83d\\ude01\"", 5 ) );    // uppercase \U
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "\"1234\\ud83dXX\"", 5 ) ); // Too short for second surrogate
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "\"1234\\ud83dXXXXXXX\"", 5 ) ); // Missing second surrogate
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "\"1234\\ud83d\\ud83dXX\"", 5 ) ); // Second surrogate not low
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "\"1234\\ude01\\ud83dXX\"", 5 ) ); // surrogates swapped (low first)

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonParsesAsNumberObjectUnsigned
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonParsesAsNumberObjectUnsigned
    (
        bool            IsJson5,
        char const*     Json,
        uint64_t        Number
    )
{
    JlDataObject*   object = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;
    uint64_t        num64;

    JL_ASSERT_SUCCESS( JlParseJsonEx( Json, IsJson5, &object, &errorAtPos ) );
    if( NULL != object )
    {
        JL_ASSERT_NOT_NULL( object );
        JL_ASSERT( errorAtPos == 0 );

        JL_ASSERT_SUCCESS( JlGetObjectNumberU64( object, &num64 ) );
        JL_ASSERT( Number == num64 );

        JL_ASSERT_SUCCESS( JlFreeObjectTree( &object ) );
        JL_ASSERT_NULL( object );
    }

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonParsesAsNumberObjectSigned
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonParsesAsNumberObjectSigned
    (
        bool            IsJson5,
        char const*     Json,
        int64_t         Number
    )
{
    JlDataObject*   object = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;
    int64_t        num64;

    JL_ASSERT_SUCCESS( JlParseJsonEx( Json, IsJson5, &object, &errorAtPos ) );
    if( NULL != object )
    {
        JL_ASSERT_NOT_NULL( object );
        JL_ASSERT( errorAtPos == 0 );

        JL_ASSERT_SUCCESS( JlGetObjectNumberS64( object, &num64 ) );
        JL_ASSERT( Number == num64 );

        JL_ASSERT_SUCCESS( JlFreeObjectTree( &object ) );
        JL_ASSERT_NULL( object );
    }

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonParsesAsNumberObjectFloat
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonParsesAsNumberObjectFloat
    (
        bool            IsJson5,
        char const*     Json,
        double          Number
    )
{
    JlDataObject*   object = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;
    double          numfloat;

    JL_ASSERT_SUCCESS( JlParseJsonEx( Json, IsJson5, &object, &errorAtPos ) );
    if( NULL != object )
    {
        JL_ASSERT_NOT_NULL( object );
        JL_ASSERT( errorAtPos == 0 );

        JL_ASSERT_SUCCESS( JlGetObjectNumberF64( object, &numfloat ) );
        JL_ASSERT( Number == numfloat );

        JL_ASSERT_SUCCESS( JlFreeObjectTree( &object ) );
        JL_ASSERT_NULL( object );
    }

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParseJsonAsFloat
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    ParseJsonAsFloat
    (
        bool            IsJson5,
        char const*     Json,
        double*         pNumber
    )
{
    JlDataObject*   object = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;

    *pNumber = 0;

    JL_ASSERT_SUCCESS( JlParseJsonEx( Json, IsJson5, &object, &errorAtPos ) );
    if( NULL != object )
    {
        JL_ASSERT_NOT_NULL( object );
        JL_ASSERT( errorAtPos == 0 );

        JL_ASSERT_SUCCESS( JlGetObjectNumberF64( object, pNumber ) );

        JL_ASSERT_SUCCESS( JlFreeObjectTree( &object ) );
        JL_ASSERT_NULL( object );
    }

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestNumbers
//
//  Tests parsing a json object as a number.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestNumbers
    (
        bool            IsJson5
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    // Test valid numbers
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectUnsigned( IsJson5, "0", 0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectUnsigned( IsJson5, "1", 1 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectUnsigned( IsJson5, " 100", 100 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectUnsigned( IsJson5, " 18446744073709551615 ", UINT64_MAX ) );

    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectSigned( IsJson5, " -1 ", -1 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectSigned( IsJson5, " -9223372036854775808 ", INT64_MIN ) );

    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, " 1.0 ", 1.0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, " -1.0 ", -1.0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, "12345.67891", 12345.67891 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, "123.45e50", 123.45e50 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, "123.45e+50", 123.45e50 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, "-123.45e-50", -123.45e-50 ) );

    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, "1.7976931348623158e308", DBL_MAX ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, "1.7976931348623158e+308", DBL_MAX ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, "2.2250738585072014e-308", DBL_MIN ) );

    // Very large integers will be treated as floating point if they can't be represented in a uint64_t or int64_t.
    // Verify that it gets rounded to correct float
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, "18446744073709551616", 18446744073709552000.0 ) ); // Rounded
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, "-9223372036854775809", -9223372036854775800.0 ) ); // Rounded

    // Test technically valid json numbers but ones our implementation can not handle
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "1e309", 0 ) );      // This will become Inf
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "-1e309", 0 ) );      // This will become -Inf

    if( IsJson5 )
    {
        // Test hex numbers
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectUnsigned( IsJson5, "0x0", 0 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectUnsigned( IsJson5, "0x0000", 0 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectUnsigned( IsJson5, "0x1", 1 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectUnsigned( IsJson5, "0x1234", 0x1234 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectUnsigned( IsJson5, "0X1234", 0X1234 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectUnsigned( IsJson5, "0xabcd", 0xabcd ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectUnsigned( IsJson5, "0xEEFF", 0xeeff ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectUnsigned( IsJson5, "0xffffffffffffffff", 0xffffffffffffffff ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "0x1ffffffffffffffff", 0 ) );
    }

    if( IsJson5 )
    {
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, ".2", 0.2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, "+0.2", 0.2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectFloat( IsJson5, "2.", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsNumberObjectUnsigned( IsJson5, "+2", 2 ) );

        // Test special "numbers"
        double f64 = 0;
        const double a_inf = DBL_MAX * 2.0;
        const double a_ninf = DBL_MAX * -2.0;

        JL_ASSERT_SUCCESS( ParseJsonAsFloat( IsJson5, "Inf", &f64 ) );
        JL_ASSERT( a_inf == f64 );
        JL_ASSERT_SUCCESS( ParseJsonAsFloat( IsJson5, "inf", &f64 ) );
        JL_ASSERT( a_inf == f64 );
        JL_ASSERT_SUCCESS( ParseJsonAsFloat( IsJson5, "+Inf", &f64 ) );
        JL_ASSERT( a_inf == f64 );
        JL_ASSERT_SUCCESS( ParseJsonAsFloat( IsJson5, "+inf", &f64 ) );
        JL_ASSERT( a_inf == f64 );
        JL_ASSERT_SUCCESS( ParseJsonAsFloat( IsJson5, "-Inf", &f64 ) );
        JL_ASSERT( a_ninf == f64 );
        JL_ASSERT_SUCCESS( ParseJsonAsFloat( IsJson5, "-inf", &f64 ) );
        JL_ASSERT( a_ninf == f64 );
        JL_ASSERT_SUCCESS( ParseJsonAsFloat( IsJson5, "NaN", &f64 ) );
        JL_ASSERT( f64 != f64 );        // Tests for NaN
        JL_ASSERT_SUCCESS( ParseJsonAsFloat( IsJson5, "NaN", &f64 ) );
        JL_ASSERT( f64 != f64 );        // Tests for NaN
    }
    else
    {
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, ".2", 0 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "+0.2", 0 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "2.", 0 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "+2", 0 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "Inf", 0 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "+Inf", 0 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "-Inf", 0 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "Nan", 0 ) );
    }

    // Test invalid numbers
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "1.2.3", 0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "E10", 0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "-E10", 0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "1E.10", 0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "11E1.0", 0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "1E1+0", 0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "1E1-0", 0 ) );

    if( !IsJson5 )
    {
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " 0x1234", 1 ) );
    }
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "0x12.34", 0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "0x111G", 0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "0x", 0 ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonParsesAsBool
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonParsesAsBool
    (
        bool            IsJson5,
        char const*     Json,
        bool            BoolValue
    )
{
    JlDataObject*   object = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;
    bool            value;

    JL_ASSERT_SUCCESS( JlParseJsonEx( Json, IsJson5, &object, &errorAtPos ) );
    JL_ASSERT_NOT_NULL( object );
    JL_ASSERT( errorAtPos == 0 );

    JL_ASSERT_SUCCESS( JlGetObjectBool( object, &value ) );
    JL_ASSERT( value == BoolValue );

    JL_ASSERT_SUCCESS( JlFreeObjectTree( &object ) );
    JL_ASSERT_NULL( object );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestBools
//
//  Tests parsing a json object as a bool.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestBools
    (
        bool            IsJson5
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    // Test valid bools
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsBool( IsJson5, "false", false ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsBool( IsJson5, "true", true ) );

    // Test invalid bools
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "TRUE", 0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " FALSE", 1 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " yes", 1 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " no", 1 ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonParsesAsListOfNumbers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonParsesAsListOfNumbers
    (
        bool            IsJson5,
        char const*     Json,
        uint64_t*       NumberArray,
        size_t          NumberArrayCount
    )
{
    JlDataObject*   listObject = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;
    JlListItem*     enumerator = NULL;
    JlDataObject*   object = NULL;

    JL_ASSERT_SUCCESS( JlParseJsonEx( Json, IsJson5, &listObject, &errorAtPos ) );
    if( WJTL_STATUS_SUCCESS == TestReturn )
    {
        JL_ASSERT_NOT_NULL( listObject );
        JL_ASSERT( JlGetObjectType( listObject ) == JL_DATA_TYPE_LIST );
        JL_ASSERT( errorAtPos == 0 );

        JL_ASSERT( JlGetListCount( listObject ) == NumberArrayCount );

        // Verify all elements in list match array
        enumerator = NULL;
        for( size_t i=0; i<NumberArrayCount; i++ )
        {
            uint64_t u64 = UINT64_MAX;
            object = NULL;
            JL_ASSERT_SUCCESS( JlGetObjectListNextItem( listObject, &object, &enumerator ) );
            JL_ASSERT_NOT_NULL( object );
            JL_ASSERT_SUCCESS( JlGetObjectNumberU64( object, &u64 ) );
            JL_ASSERT( u64 == NumberArray[i] );
        }

        // Verify there are no more items in the list
        JL_ASSERT_STATUS( JlGetObjectListNextItem( listObject, &object, &enumerator ), JL_STATUS_END_OF_DATA );

        // Clean up
        JL_ASSERT_SUCCESS( JlFreeObjectTree( &listObject ) );
    }

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonParsesAsListOfStrings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonParsesAsListOfStrings
    (
        bool            IsJson5,
        char const*     Json,
        char const**    StringArray,
        size_t          StringArrayCount
    )
{
    JlDataObject*   listObject = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;
    JlListItem*     enumerator = NULL;
    JlDataObject*   object = NULL;

    JL_ASSERT_SUCCESS( JlParseJsonEx( Json, IsJson5, &listObject, &errorAtPos ) );
    JL_ASSERT_NOT_NULL( listObject );
    JL_ASSERT( JlGetObjectType( listObject ) == JL_DATA_TYPE_LIST );
    JL_ASSERT( errorAtPos == 0 );

    JL_ASSERT( JlGetListCount( listObject ) == StringArrayCount );

    // Verify all elements in list match array
    enumerator = NULL;
    for( size_t i=0; i<StringArrayCount; i++ )
    {
        char const* string = NULL;
        object = NULL;
        JL_ASSERT_SUCCESS( JlGetObjectListNextItem( listObject, &object, &enumerator ) );
        JL_ASSERT_NOT_NULL( object );
        JL_ASSERT_SUCCESS( JlGetObjectString( object, &string ) );
        JL_ASSERT( strcmp( string, StringArray[i] ) == 0 );
    }

    // Verify there are no more items in the list
    JL_ASSERT_STATUS( JlGetObjectListNextItem( listObject, &object, &enumerator ), JL_STATUS_END_OF_DATA );

    // Clean up
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &listObject ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonParsesAsListOfNumbers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonParsesAsListOfListsOfNumbers
    (
        bool            IsJson5,
        char const*     Json,
        size_t*         SubListLengthArray,
        size_t          SubListLengthArrayCount
    )
{
    JlDataObject*   listObject = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;
    JlListItem*     enumerator = NULL;
    JlDataObject*   subListObject = NULL;
    uint64_t        counter = 0;

    JL_ASSERT_SUCCESS( JlParseJsonEx( Json, IsJson5, &listObject, &errorAtPos ) );
    JL_ASSERT_NOT_NULL( listObject );
    JL_ASSERT( JlGetObjectType( listObject ) == JL_DATA_TYPE_LIST );
    JL_ASSERT( errorAtPos == 0 );

    JL_ASSERT( JlGetListCount( listObject ) == SubListLengthArrayCount );

    // Verify all elements in list match array
    enumerator = NULL;
    for( size_t i=0; i<SubListLengthArrayCount; i++ )
    {
        subListObject = NULL;
        JL_ASSERT_SUCCESS( JlGetObjectListNextItem( listObject, &subListObject, &enumerator ) );
        JL_ASSERT_NOT_NULL( subListObject );
        JL_ASSERT( JlGetObjectType( subListObject ) == JL_DATA_TYPE_LIST );
        JL_ASSERT( JlGetListCount( subListObject ) == SubListLengthArray[i] );

        JlListItem*     subEnumerator = NULL;
        JlDataObject*   object = NULL;
        for( size_t x=0; x<SubListLengthArray[i]; x++ )
        {
            uint64_t u64 = UINT64_MAX;
            object = NULL;
            JL_ASSERT_SUCCESS( JlGetObjectListNextItem( subListObject, &object, &subEnumerator ) );
            JL_ASSERT_NOT_NULL( object );
            JL_ASSERT_SUCCESS( JlGetObjectNumberU64( object, &u64 ) );
            counter += 1;
            JL_ASSERT( u64 == counter );
        }

        // Verify there are no more items in the list
        JL_ASSERT_STATUS( JlGetObjectListNextItem( subListObject, &object, &subEnumerator ), JL_STATUS_END_OF_DATA );

    }

    // Verify there are no more items in the list
    JL_ASSERT_STATUS( JlGetObjectListNextItem( listObject, &subListObject, &enumerator ), JL_STATUS_END_OF_DATA );

    // Clean up
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &listObject ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonParsesAsListOfDifferentThingNBS
//
//  List containing a Number, Bool, and String in that order
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonParsesAsListOfDifferentThingNBS
    (
        bool            IsJson5,
        char const*     Json,
        uint64_t        TheNumber,
        bool            TheBool,
        char const*     TheString
    )
{
    JlDataObject*   listObject = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;
    JlListItem*     enumerator = NULL;
    JlDataObject*   object = NULL;
    uint64_t        u64 = 0;
    bool            boolValue = false;
    char const*     stringPtr = NULL;

    JL_ASSERT_SUCCESS( JlParseJsonEx( Json, IsJson5, &listObject, &errorAtPos ) );
    JL_ASSERT_NOT_NULL( listObject );
    JL_ASSERT( JlGetObjectType( listObject ) == JL_DATA_TYPE_LIST );
    JL_ASSERT( errorAtPos == 0 );

    JL_ASSERT( JlGetListCount( listObject ) == 3 );

    // Verify all elements in list match array
    enumerator = NULL;
    JL_ASSERT_SUCCESS( JlGetObjectListNextItem( listObject, &object, &enumerator ) );
    JL_ASSERT_NOT_NULL( object );
    JL_ASSERT_NOT_NULL( enumerator );
    JL_ASSERT( JlGetObjectType( object ) == JL_DATA_TYPE_NUMBER );
    JL_ASSERT_SUCCESS( JlGetObjectNumberU64( object, &u64 ) );
    JL_ASSERT( u64 == TheNumber );

    JL_ASSERT_SUCCESS( JlGetObjectListNextItem( listObject, &object, &enumerator ) );
    JL_ASSERT_NOT_NULL( object );
    JL_ASSERT_NOT_NULL( enumerator );
    JL_ASSERT( JlGetObjectType( object ) == JL_DATA_TYPE_BOOL );
    JL_ASSERT_SUCCESS( JlGetObjectBool( object, &boolValue ) );
    JL_ASSERT( boolValue == TheBool );

    JL_ASSERT_SUCCESS( JlGetObjectListNextItem( listObject, &object, &enumerator ) );
    JL_ASSERT_NOT_NULL( object );
    JL_ASSERT_NOT_NULL( enumerator );
    JL_ASSERT( JlGetObjectType( object ) == JL_DATA_TYPE_STRING );
    JL_ASSERT_SUCCESS( JlGetObjectString( object, &stringPtr ) );
    if(     NULL == TheString
        ||  0 == TheString[0] )
    {
        JL_ASSERT( NULL == stringPtr );
    }
    else
    {
        JL_ASSERT( strcmp( TheString, stringPtr ) == 0 );
    }

    // Verify there are no more items in the list
    JL_ASSERT_STATUS( JlGetObjectListNextItem( listObject, &object, &enumerator ), JL_STATUS_END_OF_DATA );

    // Clean up
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &listObject ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonParsesAsListOfDictionariesOfNumbers
//
//  Takes a list of form [{"num1":1,...{"numn":n}, {"numn+1":numn+1,...{"num2n":num2n} ...]
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonParsesAsListOfDictionariesOfNumbers
    (
        bool            IsJson5,
        char const*     Json,
        uint32_t        ListCount,
        uint32_t        DictionaryCount
    )
{
    JlDataObject*   listObject = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;
    JlListItem*     enumerator = NULL;
    JlDataObject*   dictionaryObject = NULL;
    uint32_t        counter = 0;

    JL_ASSERT_SUCCESS( JlParseJsonEx( Json, IsJson5, &listObject, &errorAtPos ) );
    JL_ASSERT_NOT_NULL( listObject );
    JL_ASSERT( JlGetObjectType( listObject ) == JL_DATA_TYPE_LIST );
    JL_ASSERT( errorAtPos == 0 );

    JL_ASSERT( JlGetListCount( listObject ) == ListCount );

    enumerator = NULL;
    for( uint32_t i=0; i<ListCount; i++ )
    {
        dictionaryObject = NULL;
        JL_ASSERT_SUCCESS( JlGetObjectListNextItem( listObject, &dictionaryObject, &enumerator ) );
        JL_ASSERT_NOT_NULL( dictionaryObject );
        JL_ASSERT( JlGetObjectType( dictionaryObject ) == JL_DATA_TYPE_DICTIONARY );

        for( uint32_t x=0; x<DictionaryCount; x++ )
        {
            JlDataObject* object = NULL;
            uint64_t u64 = 0;

            counter += 1;
            char keyName [32] = {0};
            sprintf( keyName, "num%u", counter );

            JL_ASSERT_SUCCESS( JlGetObjectFromDictionaryByKey( dictionaryObject, keyName, &object ) );
            JL_ASSERT_NOT_NULL( object );
            JL_ASSERT( JlGetObjectType( object ) == JL_DATA_TYPE_NUMBER );
            JL_ASSERT_SUCCESS( JlGetObjectNumberU64( object, &u64 ) );
            JL_ASSERT( u64 == (uint64_t)counter );
        }
    }

    // Verify there are no more items in the list
    JL_ASSERT_STATUS( JlGetObjectListNextItem( listObject, &dictionaryObject, &enumerator ), JL_STATUS_END_OF_DATA );

    // Clean up
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &listObject ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestLists
//
//  Tests parsing a json object as a list.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestLists
    (
        bool            IsJson5
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    // Test lists of numbers
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfNumbers( IsJson5, "[1,2,3,4,5,6,7,8]", (uint64_t[]){1,2,3,4,5,6,7,8}, 8 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfNumbers( IsJson5, "[1]", (uint64_t[]){1}, 1 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfNumbers( IsJson5, "[]",  (uint64_t[]){0}, 0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfNumbers( IsJson5, " [ 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 ] ", (uint64_t[]){1,2,3,4,5,6,7,8}, 8 ) );

    // Test list of strings
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfStrings( IsJson5, " [ \"one string\" ]", (char const*[]){"one string"}, 1 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfStrings( IsJson5, " [ \"one string\" , \"two strings\" ]", (char const*[]){"one string","two strings"}, 2 ) );

    // Test list of lists
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfListsOfNumbers( IsJson5, " [ [1] ]", (size_t[]){1}, 1 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfListsOfNumbers( IsJson5, " [ [1], [2] ]", (size_t[]){1,1}, 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfListsOfNumbers( IsJson5, " [ [1], [2], [3], [4] ]", (size_t[]){1,1,1,1}, 4 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfListsOfNumbers( IsJson5, " [ [1,2], [3,4] ]", (size_t[]){2,2}, 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfListsOfNumbers( IsJson5, " [ [1,2], [3,4,5], [6,7], [8] ]", (size_t[]){2,3,2,1}, 4 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfListsOfNumbers( IsJson5, " [ [1], [2], [3,4,5,6,7,8], [9], [10,11] ]", (size_t[]){1,1,6,1,2}, 5 ) );

    // Test a list of different items
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfDifferentThingNBS( IsJson5, "[100,true,\"string\"]", 100, true, "string" ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfDifferentThingNBS( IsJson5, "[0,false,\"\"]", 0, false, "" ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfDifferentThingNBS( IsJson5, "[0,false,null]", 0, false, "" ) );

    // Test a list of dictionaries
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfDictionariesOfNumbers( IsJson5, "[{\"num1\":1,\"num2\":2},{\"num3\":3,\"num4\":4}]", 2, 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfDictionariesOfNumbers( IsJson5, " [ { \"num1\" : 1 , \"num2\" : 2 , \"num3\" : 3 } , { \"num4\" : 4 , \"num5\" : 5 , \"num6\" : 6 } ]", 2, 3 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfDictionariesOfNumbers( IsJson5, " [ { \"num1\" : 1 } ]", 1, 1 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfDictionariesOfNumbers( IsJson5, " [ { }, { }, { }, { } ]", 4, 0 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfDictionariesOfNumbers( IsJson5, "[{},{},{},{},{}]", 5, 0 ) );

    // Test trailing commas (allowed in json5, not in json1)
    if( IsJson5 )
    {
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfNumbers( IsJson5, "[1,2,]", (uint64_t[]){1,2}, 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfNumbers( IsJson5, "[1,2, ]", (uint64_t[]){1,2}, 2 ) );
    }
    else
    {
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "[1,2,]", 5 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "[1,2, ]", 6 ) );
    }

    // Test invalid lists
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "[1,2,3,4,,6,7,8]", 9 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " [ ,] ", 3 ) );

    if( !IsJson5 )
    {
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " [ [1,2], [3,4], [5,], [7,8] ]", 20 ) );
    }
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " [[[[ * ]]]]", 6 ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonParsesAsDictionaryOfNumbers
//
//  Dictionary must be of form { "item1":1, "item2":2, ... "itemn":n } (order not important)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonParsesAsDictionaryOfNumbers
    (
        bool            IsJson5,
        char const*     Json,
        size_t          NumItems
    )
{
    JlDataObject*   dictionaryObject = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;

    JL_ASSERT_SUCCESS( JlParseJsonEx( Json, IsJson5, &dictionaryObject, &errorAtPos ) );
    if( NULL == dictionaryObject ) { return TestReturn; }

    JL_ASSERT( JlGetObjectType( dictionaryObject ) == JL_DATA_TYPE_DICTIONARY );

    for( size_t num=1; num<=NumItems; num++ )
    {
        JlDataObject* object = NULL;
        uint64_t u64 = 0;
        char itemName [16] = {0};
        sprintf( itemName, "item%u", (uint32_t)num );

        JL_ASSERT_SUCCESS( JlGetObjectFromDictionaryByKey( dictionaryObject, itemName, &object ) );
        JL_ASSERT_SUCCESS( JlGetObjectNumberU64( object, &u64 ) );
        JL_ASSERT( u64 == num );
    }

    // Clean up
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &dictionaryObject ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyJsonParsesAsDictionaryOfStrings
//
//  Dictionary must be of form { "item1":"1", "item2":"2", ... "itemn":"n" } (order not important)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    VerifyJsonParsesAsDictionaryOfStrings
    (
        bool            IsJson5,
        char const*     Json,
        size_t          NumItems
    )
{
    JlDataObject*   dictionaryObject = NULL;
    size_t          errorAtPos = 100;
    WJTL_STATUS     TestReturn = WJTL_STATUS_SUCCESS;

    JL_ASSERT_SUCCESS( JlParseJsonEx( Json, IsJson5, &dictionaryObject, &errorAtPos ) );
    if( NULL != dictionaryObject )
    {
        JL_ASSERT( JlGetObjectType( dictionaryObject ) == JL_DATA_TYPE_DICTIONARY );
        if( JlGetObjectType( dictionaryObject ) == JL_DATA_TYPE_DICTIONARY )
        {
            for( size_t num=1; num<=NumItems; num++ )
            {
                JlDataObject* object = NULL;
                char const* stringPtr = NULL;
                char itemName [16] = {0};
                sprintf( itemName, "item%u", (uint32_t)num );
                char valueStr [16] = {0};
                sprintf( valueStr, "%u", (uint32_t)num );

                JL_ASSERT_SUCCESS( JlGetObjectFromDictionaryByKey( dictionaryObject, itemName, &object ) );
                if( NULL != object )
                {
                    JL_ASSERT_SUCCESS( JlGetObjectString( object, &stringPtr ) );
                    JL_ASSERT( strcmp( stringPtr, valueStr ) == 0 );
                }
            }
        }

        // Clean up
        JL_ASSERT_SUCCESS( JlFreeObjectTree( &dictionaryObject ) );
    }

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestDictionaries
//
//  Tests parsing a json object as a dictionary.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestDictionaries
    (
        bool            IsJson5
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    // Test dictionary of numbers
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, "{\"item1\":1,\"item2\":2,\"item3\":3 }", 3 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, "{\"item3\":3,\"item2\":2,\"item1\":1 }", 3 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { \"item1\" : 1, \"item2\" : 2 }", 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { }", 0 ) );

    // Test dictionary of strings
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfStrings( IsJson5, "{\"item1\":\"1\",\"item2\":\"2\",\"item3\":\"3\" }", 3 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfStrings( IsJson5, "{\"item3\":\"3\",\"item2\":\"2\",\"item1\":\"1\" }", 3 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfStrings( IsJson5, " { \"item1\" : \"1\", \"item2\" : \"2\" }", 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfStrings( IsJson5, " { }", 0 ) );

    // Test dictionary of numbers that also contains other items we are ignoring (and ensuring get freed)
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { \"dummy1\":\"String\", \"item1\" : 1, \"dummy2\":12345, \"item2\" : 2 }", 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { \"dummy1\":{\"sub1\":true,\"sub2\":[{\"aa\":\"bb\"}, 1,2,3,4,5]}, \"item1\":1,  \"item2\":2 }", 2 ) );

    if( IsJson5 )
    {
        // Test Json5 single quoted keys
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, "{'item1':1,'item2':2,'item3':3 }", 3 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, "{'item3':3,'item2':2,'item1':1 }", 3 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { 'item1' : 1, 'item2' : 2 }", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { }", 0 ) );

        // Test dictionary of strings
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfStrings( IsJson5, "{'item1':'1','item2':'2','item3':'3' }", 3 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfStrings( IsJson5, "{'item3':'3','item2':'2','item1':'1' }", 3 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfStrings( IsJson5, " { 'item1' : '1', 'item2' : '2' }", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfStrings( IsJson5, " { }", 0 ) );

        // Test dictionary of numbers that also contains other items we are ignoring (and ensuring get freed)
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { 'dummy1':'String', 'item1' : 1, 'dummy2':12345, 'item2' : 2 }", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { 'dummy1':{'sub1':true,'sub2':[{'aa':'bb'}, 1,2,3,4,5]}, 'item1':1,  'item2':2 }", 2 ) );

        // Test Json5 bareword keys
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, "{item1:1,item2:2,item3:3 }", 3 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, "{item\\u0031:1,it\\u0065m2:2,item3:3 }", 3 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, "{item3:3,item2:2,item1:1 }", 3 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { item1 : 1, item2 : 2 }", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { }", 0 ) );

        // Test dictionary of strings
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfStrings( IsJson5, "{item1:\"1\",item2:\"2\",item3:\"3\" }", 3 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfStrings( IsJson5, "{item3:\"3\",item2:\"2\",item1:\"1\" }", 3 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfStrings( IsJson5, " { item1 : \"1\", item2 : \"2\" }", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfStrings( IsJson5, " { }", 0 ) );

        // Test dictionary of numbers that also contains other items we are ignoring (and ensuring get freed)
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { dummy1:\"String\", item1 : 1, dummy2:12345, item2 : 2 }", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { dummy1:{sub1:true,sub2:[{aa:\"bb\"}, 1,2,3,4,5]}, item1:1,  item2:2 }", 2 ) );
    }

    // Test trailing commas for Json5
    if( IsJson5 )
    {
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, "{\"item1\":1, }", 1 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, "{\"item1\":1 , }", 1 ) );
    }
    else
    {
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{\"item1\":1, }", 12 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{\"item1\":1 , }", 13 ) );
    }

    // Test invalid dictionaries
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ \"item1\":1, \"item2\":2,, \"item3\":3 }", 23 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ ,\"item1\":1, \"item2\":2, \"item3\":3 }", 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ BareWord }", IsJson5 ? 11 : 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ \"string\" }", 11 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ 'string' }", IsJson5 ? 11 : 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ 123 }", 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ [1,2,3] }", 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ \"item1\":1, true }", IsJson5 ? 18 :13 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ \"item1\":1, \"string\", \"item2\":2 }", 21 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ 'key\" : 'value' }", IsJson5 ? 11 : 2 ) );

    if( IsJson5 )
    {
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ bare space: \"value\" }", 7 ) );
    }
    else
    {
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ 'key' : \"value\" }", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ \"key\" : 'value' }", 10 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "{ 'key' : 'value' }", 2 ) );
    }

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestInvalidValues
//
//  Tests it will not parse an invalid object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestInvalidValues
    (
        bool            IsJson5
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  StringWithNoQuotes", 2 ) );
    if( !IsJson5 )
    {
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  +1234", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  .1234", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  1234.", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  inf", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  +inf", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  -inf", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  -nan", 2 ) );
    }
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  1.2.3", 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  $", 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  <>", 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  !", 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  # Comment", 2 ) );
    if( !IsJson5 )
    {
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  // Comment", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, "  /* Comment */", 2 ) );
    }

    // Empty objects
    JlDataObject*   object = NULL;
    size_t          errorAtPos = 100;
    JL_ASSERT_STATUS( JlParseJsonEx( "", IsJson5, &object, &errorAtPos ), JL_STATUS_END_OF_DATA );
    JL_ASSERT_NULL( object );
    JL_ASSERT( 0 == errorAtPos );

    JL_ASSERT_STATUS( JlParseJsonEx( "    ", IsJson5, &object, &errorAtPos ), JL_STATUS_END_OF_DATA );
    JL_ASSERT_NULL( object );
    JL_ASSERT( 0 == errorAtPos );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  GenerateNestedJsonList
//
//  Generates JSON containing a list nested by the specified amount
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    GenerateNestedJsonList
    (
        uint32_t    NestLevel,
        char**      pString
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    char*       stringBuffer = NULL;

    *pString = NULL;

    stringBuffer = WjTestLib_Calloc( (NestLevel*(10+1)) + (1 + 12 + 1), 1 );
    JL_ASSERT_NOT_NULL( stringBuffer );

    for( size_t level=0; level<NestLevel; level++ )
    {
        strcat( stringBuffer, "[12,\"str\"," );
    }
    strcat( stringBuffer, "\"FinalLevel\"" );
    for( size_t level=0; level<NestLevel; level++ )
    {
        strcat( stringBuffer, "]" );
    }

    *pString = stringBuffer;

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  GenerateNestedJsonDictionary
//
//  Generates JSON containing a dictionary nested by the specified amount
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    GenerateNestedJsonDictionary
    (
        uint32_t    NestLevel,
        char**      pString
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    char*       stringBuffer = NULL;
    char const* leftSide = "{\"str\":\"value\", \"dictionary\":";
    char const* finalLevel = "\"FinalLevel\"";
    char const* rightSide = "} ";

    *pString = NULL;

    stringBuffer = WjTestLib_Calloc( (NestLevel*(strlen(leftSide)+strlen(rightSide))) + strlen(finalLevel) + 1, 1 );
    JL_ASSERT_NOT_NULL( stringBuffer );

    for( size_t level=0; level<NestLevel; level++ )
    {
        strcat( stringBuffer, leftSide );
    }
    strcat( stringBuffer, finalLevel );
    for( size_t level=0; level<NestLevel; level++ )
    {
        strcat( stringBuffer, rightSide );
    }

    *pString = stringBuffer;

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestNestedTooDeep
//
//  JSON does not have a limit on how deeply nested a dictionary or list can be, but this implementation does.
//  Verify that this correctly reports an error without memory leak the level expected
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestNestedTooDeep
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    char* jsonText = NULL;
    JlDataObject* objectTree = NULL;

    // Test deep lists

    // Check MAX_JSON_DEPTH levels - The max amount that will work
    JL_ASSERT_SUCCESS( GenerateNestedJsonList( MAX_JSON_DEPTH, &jsonText ) );
    JL_ASSERT_SUCCESS( JlParseJson( jsonText, &objectTree, NULL ) );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &objectTree ) );
    JlFree( jsonText );
    jsonText = NULL;

    // Check one greater than MAX_JSON_DEPTH levels - This should fail. Make sure there is no memory leak
    JL_ASSERT_SUCCESS( GenerateNestedJsonList( MAX_JSON_DEPTH+1, &jsonText ) );
    JL_ASSERT_STATUS( JlParseJson( jsonText, &objectTree, NULL ), JL_STATUS_JSON_NESTING_TOO_DEEP );
    JL_ASSERT_NULL( objectTree );
    JlFree( jsonText );
    jsonText = NULL;

    // Now test dictionaries that are nested deeply

    // Check MAX_JSON_DEPTH level - the max amount that will work
    JL_ASSERT_SUCCESS( GenerateNestedJsonDictionary( MAX_JSON_DEPTH, &jsonText ) );
    JL_ASSERT_SUCCESS( JlParseJson( jsonText, &objectTree, NULL ) );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &objectTree ) );
    JlFree( jsonText );
    jsonText = NULL;

    // Check one greater than MAX_JSON_DEPTH levels - This should fail. Make sure there is no memory leak
    JL_ASSERT_SUCCESS( GenerateNestedJsonDictionary( MAX_JSON_DEPTH+1, &jsonText ) );
    JL_ASSERT_STATUS( JlParseJson( jsonText, &objectTree, NULL ), JL_STATUS_JSON_NESTING_TOO_DEEP );
    JL_ASSERT_NULL( objectTree );
    JlFree( jsonText );
    jsonText = NULL;

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestWhiteSpace
//
//  Tests white space is handled correctly
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestWhiteSpace
    (
        bool            IsJson5
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    // Test ordinary whitespace
    JL_ASSERT_SUCCESS(VerifyJsonParsesAsNumberObjectSigned( IsJson5, " 1 ", 1 ) );          // Space
    JL_ASSERT_SUCCESS(VerifyJsonParsesAsNumberObjectSigned( IsJson5, "\t1\t", 1 ) );        // Tab
    JL_ASSERT_SUCCESS(VerifyJsonParsesAsNumberObjectSigned( IsJson5, "\n1\n", 1 ) );        // Line feed
    JL_ASSERT_SUCCESS(VerifyJsonParsesAsNumberObjectSigned( IsJson5, "\r\n1\r\n", 1 ) );    // Carriage return / Line feed

    // Now test extended json5 whitespace
    if( IsJson5 )
    {
        JL_ASSERT_SUCCESS(VerifyJsonParsesAsNumberObjectSigned( IsJson5, " \v1\v", 1 ) );             // Vertical tab
        JL_ASSERT_SUCCESS(VerifyJsonParsesAsNumberObjectSigned( IsJson5, " \f1\f", 1 ) );             // Form feed
        JL_ASSERT_SUCCESS(VerifyJsonParsesAsNumberObjectSigned( IsJson5, " \xc2\xa0 1 \xc2\xa0", 1 ) );       // Non-breaking space
        JL_ASSERT_SUCCESS(VerifyJsonParsesAsNumberObjectSigned( IsJson5, " \xe2\x80\xa8 1 \xe2\x80\xa8", 1 ) );   // Line separator
        JL_ASSERT_SUCCESS(VerifyJsonParsesAsNumberObjectSigned( IsJson5, " \xe2\x80\xa9 1 \xe2\x80\xa9", 1 ) );   // Paragraph separator
        JL_ASSERT_SUCCESS(VerifyJsonParsesAsNumberObjectSigned( IsJson5, " \xef\xbb\xbf 1 \xef\xbb\xbf", 1 ) );   // Byte order mark
    }
    else
    {
        // Extended white spaces should fail
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " 1\v", 1 ) );             // Vertical tab
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " 1\f", 1 ) );             // Form feed
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " 1\xc2\xa0", 1 ) );       // Non-breaking space
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " 1\xe2\x80\xa8", 1 ) );   // Line separator
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " 1\xe2\x80\xa9", 1 ) );   // Paragraph separator
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " 1\xef\xbb\xbf", 1 ) );   // Byte order mark
    }

    // Now try a more complicated object with all the white spaces used
    #define WSP1 " \t\r\n"
    #define WSP5 " \t\r\n\xc2\xa0 \xe2\x80\xa8 \xe2\x80\xa9 \xef\xbb\xbf"
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { "WSP1"\"item1\" "WSP1": "WSP1"1, "WSP1"\"item2\" "WSP1": "WSP1"2 "WSP1"}", 2 ) );
    JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfNumbers( IsJson5, ""WSP1"["WSP1"1"WSP1","WSP1"2"WSP1"]"WSP1"", (uint64_t[]){1,2}, 2 ) );
    if( IsJson5 )
    {
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsDictionaryOfNumbers( IsJson5, " { "WSP5"\"item1\" "WSP5": "WSP5"1, "WSP5"\"item2\" "WSP5": "WSP5"2 "WSP5"}", 2 ) );
        JL_ASSERT_SUCCESS( VerifyJsonParsesAsListOfNumbers( IsJson5, ""WSP5"["WSP5"1"WSP5","WSP5"2"WSP5"]"WSP5"", (uint64_t[]){1,2}, 2 ) );
    }
    else
    {
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, " { "WSP5"\"item1\" "WSP5": "WSP5"1, "WSP5"\"item2\" "WSP5": "WSP5"2 "WSP5"}", 7 ) );
        JL_ASSERT_SUCCESS( VerifyJsonUnableToParse( IsJson5, ""WSP5"["WSP5"1"WSP5","WSP5"2"WSP5"]"WSP5"", 4 ) );
    }
    #undef WSP5
    #undef WSP5

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestComments
//
//  Tests Comments in JSON 5 parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestComments
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;
    JlDataObject* objectTree = NULL;
    JlDataObject* object = NULL;
    size_t errorAtPos = 100;
    uint64_t u64;
    char const* stringPtr = NULL;

    JL_ASSERT_SUCCESS( JlParseJsonEx( "// Comment\n1234// Comment\n//Comment", true, &objectTree, &errorAtPos ) );
    JL_ASSERT_NOT_NULL( objectTree );
    JL_ASSERT_SUCCESS( JlGetObjectNumberU64( objectTree, &u64 ) );
    JL_ASSERT( 1234 == u64 );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &objectTree ) );

    JL_ASSERT_SUCCESS( JlParseJsonEx( "// Comment\n\"abcd\"// Comment\n//Comment", true, &objectTree, &errorAtPos ) );
    JL_ASSERT_NOT_NULL( objectTree );
    JL_ASSERT_SUCCESS( JlGetObjectString( objectTree, &stringPtr ) );
    JL_ASSERT( strcmp( stringPtr, "abcd" ) == 0 );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &objectTree ) );

    JL_ASSERT_SUCCESS( JlParseJsonEx( "/* Comment*/\n/*Comment*/5678/*Comment*/", true, &objectTree, &errorAtPos ) );
    JL_ASSERT_NOT_NULL( objectTree );
    JL_ASSERT_SUCCESS( JlGetObjectNumberU64( objectTree, &u64 ) );
    JL_ASSERT( 5678 == u64 );
    JL_ASSERT_SUCCESS( JlFreeObjectTree( &objectTree ) );

    // complicated example
    JL_ASSERT_SUCCESS( JlParseJsonEx(
        "{\n"
        "  \"a\" : /* */ 1,\n"
        " //\"b\" : 2,\n"
        " /*       \n"
        "   //     */\n"
        "  //   /* junk\n"
        " /* */ \"c\"/*  *//**/ /*  */:/* */ 3/* */\n"
        "}\n",
        true, &objectTree, &errorAtPos ) );
    JL_ASSERT_NOT_NULL( objectTree );

    JL_ASSERT_SUCCESS( JlGetObjectFromDictionaryByKey( objectTree, "a", &object ) );
    JL_ASSERT_SUCCESS( JlGetObjectNumberU64( object, &u64 ) );
    JL_ASSERT( 1 == u64 );
    JL_ASSERT_STATUS( JlGetObjectFromDictionaryByKey( objectTree, "b", &object ), JL_STATUS_NOT_FOUND );
    JL_ASSERT_SUCCESS( JlGetObjectFromDictionaryByKey( objectTree, "c", &object ) );
    JL_ASSERT_SUCCESS( JlGetObjectNumberU64( object, &u64 ) );
    JL_ASSERT( 3 == u64 );

    JL_ASSERT_SUCCESS( JlFreeObjectTree( &objectTree ) );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  STUBS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static WJTL_STATUS TestStrings_Json1( void ) { return TestStrings( false ); }
static WJTL_STATUS TestStrings_Json5( void ) { return TestStrings( true ); }
static WJTL_STATUS TestNumbers_Json1( void ) { return TestNumbers( false ); }
static WJTL_STATUS TestNumbers_Json5( void ) { return TestNumbers( true ); }
static WJTL_STATUS TestBools_Json1( void ) { return TestBools( false ); }
static WJTL_STATUS TestBools_Json5( void ) { return TestBools( true ); }
static WJTL_STATUS TestLists_Json1( void ) { return TestLists( false ); }
static WJTL_STATUS TestLists_Json5( void ) { return TestLists( true ); }
static WJTL_STATUS TestDictionaries_Json1( void ) { return TestDictionaries( false ); }
static WJTL_STATUS TestDictionaries_Json5( void ) { return TestDictionaries( true ); }
static WJTL_STATUS TestInvalidValues_Json1( void ) { return TestInvalidValues( false ); }
static WJTL_STATUS TestInvalidValues_Json5( void ) { return TestInvalidValues( true ); }
static WJTL_STATUS TestWhiteSpace_Json1( void ) { return TestWhiteSpace( false ); }
static WJTL_STATUS TestWhiteSpace_Json5( void ) { return TestWhiteSpace( true ); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibTests_Parsing_Register
//
//  Registers the tests with WjTestLib
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
    JsonLibTests_Parsing_Register
    (
        void
    )
{
    WjTestLib_NewGroup( "Parsing" );
    WjTestLib_AddTest( TestStrings_Json1, "Strings (Json1)" );
    WjTestLib_AddTest( TestStrings_Json5, "Strings (Json5)" );
    WjTestLib_AddTest( TestNumbers_Json1, "Numbers (Json1)" );
    WjTestLib_AddTest( TestNumbers_Json5, "Numbers (Json5)" );
    WjTestLib_AddTest( TestBools_Json1, "Bools (Json1)" );
    WjTestLib_AddTest( TestBools_Json5, "Bools (Json5)" );
    WjTestLib_AddTest( TestLists_Json1, "Lists (Json1)" );
    WjTestLib_AddTest( TestLists_Json5, "Lists (Json5)" );
    WjTestLib_AddTest( TestDictionaries_Json1, "Dictionaries (Json1)" );
    WjTestLib_AddTest( TestDictionaries_Json5, "Dictionaries (Json5)" );
    WjTestLib_AddTest( TestInvalidValues_Json1, "Invalid values (Json1)" );
    WjTestLib_AddTest( TestInvalidValues_Json5, "Invalid values (Json5)" );
    WjTestLib_AddTest( TestNestedTooDeep, "Nested depth limit" );
    WjTestLib_AddTest( TestWhiteSpace_Json1, "White Space (Json1)" );
    WjTestLib_AddTest( TestWhiteSpace_Json5, "White Space (Json5)" );
    WjTestLib_AddTest( TestComments, "Comments (Json5)" );
}
