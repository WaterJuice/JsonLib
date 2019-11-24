////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibTests
//
//  Unit tests for JsonLib - Base64
//  Tests Base 64 handling module
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "JsonLibTests.h"
#include "../Source/JlMemory.h"
#include "../Source/JlBase64.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TEST FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestBase64Encode
//
//  Tests encoding base 64
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestBase64Encode
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    uint8_t data[] = { 1, 2, 3 };
    char* string = NULL;

    JL_ASSERT_SUCCESS( JlBase64Encode( data, 1, &string ) );
    JL_ASSERT_NOT_NULL( string );
    JL_ASSERT( strcmp( string, "AQ" ) == 0 );
    JlFree( string );  string = NULL;

    JL_ASSERT_SUCCESS( JlBase64Encode( data, 2, &string ) );
    JL_ASSERT_NOT_NULL( string );
    JL_ASSERT( strcmp( string, "AQI" ) == 0 );
    JlFree( string );  string = NULL;

    JL_ASSERT_SUCCESS( JlBase64Encode( data, 3, &string ) );
    JL_ASSERT_NOT_NULL( string );
    JL_ASSERT( strcmp( string, "AQID" ) == 0 );
    JlFree( string );  string = NULL;

    uint8_t data2[256*3];
    for( int i=0; i<256; i++ )
    {
        data2[i*3 + 0] = (uint8_t)i;
        data2[i*3 + 1] = (uint8_t)i;
        data2[i*3 + 2] = (uint8_t)i;
    }
    char const* data2Result =
        "AAAAAQEBAgICAwMDBAQEBQUFBgYGBwcHCAgICQkJCgoKCwsLDAwMDQ0NDg4ODw8P"
        "EBAQEREREhISExMTFBQUFRUVFhYWFxcXGBgYGRkZGhoaGxsbHBwcHR0dHh4eHx8f"
        "ICAgISEhIiIiIyMjJCQkJSUlJiYmJycnKCgoKSkpKioqKysrLCwsLS0tLi4uLy8v"
        "MDAwMTExMjIyMzMzNDQ0NTU1NjY2Nzc3ODg4OTk5Ojo6Ozs7PDw8PT09Pj4+Pz8/"
        "QEBAQUFBQkJCQ0NDRERERUVFRkZGR0dHSEhISUlJSkpKS0tLTExMTU1NTk5OT09P"
        "UFBQUVFRUlJSU1NTVFRUVVVVVlZWV1dXWFhYWVlZWlpaW1tbXFxcXV1dXl5eX19f"
        "YGBgYWFhYmJiY2NjZGRkZWVlZmZmZ2dnaGhoaWlpampqa2trbGxsbW1tbm5ub29v"
        "cHBwcXFxcnJyc3NzdHR0dXV1dnZ2d3d3eHh4eXl5enp6e3t7fHx8fX19fn5+f39/"
        "gICAgYGBgoKCg4ODhISEhYWFhoaGh4eHiIiIiYmJioqKi4uLjIyMjY2Njo6Oj4+P"
        "kJCQkZGRkpKSk5OTlJSUlZWVlpaWl5eXmJiYmZmZmpqam5ubnJycnZ2dnp6en5+f"
        "oKCgoaGhoqKio6OjpKSkpaWlpqamp6enqKioqampqqqqq6urrKysra2trq6ur6+v"
        "sLCwsbGxsrKys7OztLS0tbW1tra2t7e3uLi4ubm5urq6u7u7vLy8vb29vr6+v7+/"
        "wMDAwcHBwsLCw8PDxMTExcXFxsbGx8fHyMjIycnJysrKy8vLzMzMzc3Nzs7Oz8/P"
        "0NDQ0dHR0tLS09PT1NTU1dXV1tbW19fX2NjY2dnZ2tra29vb3Nzc3d3d3t7e39/f"
        "4ODg4eHh4uLi4+Pj5OTk5eXl5ubm5+fn6Ojo6enp6urq6+vr7Ozs7e3t7u7u7+/v"
        "8PDw8fHx8vLy8/Pz9PT09fX19vb29/f3+Pj4+fn5+vr6+/v7/Pz8/f39/v7+////";

    JL_ASSERT_SUCCESS( JlBase64Encode( data2, sizeof(data2), &string ) );
    JL_ASSERT_NOT_NULL( string );
    JL_ASSERT( strcmp( string, data2Result) == 0 );

    JlFree( string ); string = NULL;

    uint8_t data3[256];
    for( int i=0; i<256; i++ )
    {
        data3[i] = (uint8_t)i;
    }
    char const* data3Result =
        "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4v"
        "MDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5f"
        "YGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6P"
        "kJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/"
        "wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v"
        "8PHy8/T19vf4+fr7/P3+/w";

    JL_ASSERT_SUCCESS( JlBase64Encode( data3, sizeof(data3), &string ) );
    JL_ASSERT_NOT_NULL( string );
    JL_ASSERT( strcmp( string, data3Result) == 0 );

    JlFree( string ); string = NULL;

    return TestReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestBase64Decode
//
//  Tests decoding base 64
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    TestBase64Decode
    (
        void
    )
{
    WJTL_STATUS TestReturn = WJTL_STATUS_SUCCESS;

    uint8_t* data = NULL;
    size_t dataSize = 0;
    uint8_t pattern[3] = {1,2,3};

    JL_ASSERT_SUCCESS( JlBase64Decode( "AQ", (void**)&data, &dataSize ) );
    JL_ASSERT_NOT_NULL( data );
    JL_ASSERT( 1 == dataSize );
    JL_ASSERT( memcmp( data, pattern, 1 ) == 0 );
    JlFree( data );  data = NULL; dataSize = 0;

    JL_ASSERT_SUCCESS( JlBase64Decode( "AQI", (void**)&data, &dataSize ) );
    JL_ASSERT_NOT_NULL( data );
    JL_ASSERT( 2 == dataSize );
    JL_ASSERT( memcmp( data, pattern, 2 ) == 0 );
    JlFree( data );  data = NULL; dataSize = 0;

    JL_ASSERT_SUCCESS( JlBase64Decode( "AQID", (void**)&data, &dataSize ) );
    JL_ASSERT_NOT_NULL( data );
    JL_ASSERT( 3 == dataSize );
    JL_ASSERT( memcmp( data, pattern, 3 ) == 0 );
    JlFree( data );  data = NULL; dataSize = 0;

    uint8_t pattern2[256*3];
    for( int i=0; i<256; i++ )
    {
        pattern2[i*3 + 0] = (uint8_t)i;
        pattern2[i*3 + 1] = (uint8_t)i;
        pattern2[i*3 + 2] = (uint8_t)i;
    }
    char const* data2Base64 =
        "AAAAAQEBAgICAwMDBAQEBQUFBgYGBwcHCAgICQkJCgoKCwsLDAwMDQ0NDg4ODw8P"
        "EBAQEREREhISExMTFBQUFRUVFhYWFxcXGBgYGRkZGhoaGxsbHBwcHR0dHh4eHx8f"
        "ICAgISEhIiIiIyMjJCQkJSUlJiYmJycnKCgoKSkpKioqKysrLCwsLS0tLi4uLy8v"
        "MDAwMTExMjIyMzMzNDQ0NTU1NjY2Nzc3ODg4OTk5Ojo6Ozs7PDw8PT09Pj4+Pz8/"
        "QEBAQUFBQkJCQ0NDRERERUVFRkZGR0dHSEhISUlJSkpKS0tLTExMTU1NTk5OT09P"
        "UFBQUVFRUlJSU1NTVFRUVVVVVlZWV1dXWFhYWVlZWlpaW1tbXFxcXV1dXl5eX19f"
        "YGBgYWFhYmJiY2NjZGRkZWVlZmZmZ2dnaGhoaWlpampqa2trbGxsbW1tbm5ub29v"
        "cHBwcXFxcnJyc3NzdHR0dXV1dnZ2d3d3eHh4eXl5enp6e3t7fHx8fX19fn5+f39/"
        "gICAgYGBgoKCg4ODhISEhYWFhoaGh4eHiIiIiYmJioqKi4uLjIyMjY2Njo6Oj4+P"
        "kJCQkZGRkpKSk5OTlJSUlZWVlpaWl5eXmJiYmZmZmpqam5ubnJycnZ2dnp6en5+f"
        "oKCgoaGhoqKio6OjpKSkpaWlpqamp6enqKioqampqqqqq6urrKysra2trq6ur6+v"
        "sLCwsbGxsrKys7OztLS0tbW1tra2t7e3uLi4ubm5urq6u7u7vLy8vb29vr6+v7+/"
        "wMDAwcHBwsLCw8PDxMTExcXFxsbGx8fHyMjIycnJysrKy8vLzMzMzc3Nzs7Oz8/P"
        "0NDQ0dHR0tLS09PT1NTU1dXV1tbW19fX2NjY2dnZ2tra29vb3Nzc3d3d3t7e39/f"
        "4ODg4eHh4uLi4+Pj5OTk5eXl5ubm5+fn6Ojo6enp6urq6+vr7Ozs7e3t7u7u7+/v"
        "8PDw8fHx8vLy8/Pz9PT09fX19vb29/f3+Pj4+fn5+vr6+/v7/Pz8/f39/v7+////";

    JL_ASSERT_SUCCESS( JlBase64Decode( data2Base64, (void**)&data, &dataSize ) );
    JL_ASSERT_NOT_NULL( data );
    JL_ASSERT( sizeof(pattern2) == dataSize );
    JL_ASSERT( memcmp( data, pattern2, sizeof(pattern2) ) == 0 );
    JlFree( data );  data = NULL; dataSize = 0;

    uint8_t pattern3[256];
    for( int i=0; i<256; i++ )
    {
        pattern3[i] = (uint8_t)i;
    }
    char const* data3Base64 =
        "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4v"
        "MDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5f"
        "YGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6P"
        "kJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/"
        "wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v"
        "8PHy8/T19vf4+fr7/P3+/w";
    JL_ASSERT_SUCCESS( JlBase64Decode( data3Base64, (void**)&data, &dataSize ) );
    JL_ASSERT_NOT_NULL( data );
    JL_ASSERT( sizeof(pattern3) == dataSize );
    JL_ASSERT( memcmp( data, pattern3, sizeof(pattern3) ) == 0 );
    JlFree( data );  data = NULL; dataSize = 0;

    // Test with padding character
    JL_ASSERT_SUCCESS( JlBase64Decode( "AQ==", (void**)&data, &dataSize ) );
    JL_ASSERT_NOT_NULL( data );
    JL_ASSERT( 1 == dataSize );
    JL_ASSERT( memcmp( data, pattern, 1 ) == 0 );
    JlFree( data );  data = NULL; dataSize = 0;

    JL_ASSERT_SUCCESS( JlBase64Decode( "AQ=", (void**)&data, &dataSize ) );
    JL_ASSERT_NOT_NULL( data );
    JL_ASSERT( 1 == dataSize );
    JL_ASSERT( memcmp( data, pattern, 1 ) == 0 );
    JlFree( data );  data = NULL; dataSize = 0;

    JL_ASSERT_SUCCESS( JlBase64Decode( "AQ=Random stuff, after the padding", (void**)&data, &dataSize ) );
    JL_ASSERT_NOT_NULL( data );
    JL_ASSERT( 1 == dataSize );
    JL_ASSERT( memcmp( data, pattern, 1 ) == 0 );
    JlFree( data );  data = NULL; dataSize = 0;

    // Test invalid base64
    JL_ASSERT_STATUS( JlBase64Decode( "AQIDB", (void**)&data, &dataSize ), JL_STATUS_INVALID_DATA );       // next block too short
    JL_ASSERT_NULL( data );
    JL_ASSERT( 0 == dataSize );

    JL_ASSERT_STATUS( JlBase64Decode( "AQID$QUID", (void**)&data, &dataSize ), JL_STATUS_INVALID_DATA );   // invalid character
    JL_ASSERT_NULL( data );
    JL_ASSERT( 0 == dataSize );

    return TestReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibTests_Base64_Register
//
//  Registers the tests with WjTestLib
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
    JsonLibTests_Base64_Register
    (
        void
    )
{
    WjTestLib_NewGroup( "Base64" );
    WjTestLib_AddTest( TestBase64Encode, "Base64 Encoding" );
    WjTestLib_AddTest( TestBase64Decode, "Base64 Decoding" );
}
