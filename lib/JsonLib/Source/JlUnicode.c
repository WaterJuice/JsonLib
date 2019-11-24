////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//  This module handles internal unicode conversions
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JlUnicode.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlUnicodeValueToUtf8
//
//  Converts the UnicodeValue into UTF8 bytes.
//  *pNumBytesWritten will contain the number of bytes used.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlUnicodeValueToUtf8
    (
        uint32_t        UnicodeValue,
        char*           OutputUtf8,
        size_t*         pNumBytesWritten
    )
{
    JL_STATUS jlStatus;

    if( 0 == UnicodeValue )
    {
        // Special case, When we have unicode 0 value we allow this by using outputting modified utf8 for 0 (0xc0 0x80)
        // We do not want to put a 0 directly in the string or it will be treated as a null terminator
        OutputUtf8[0] = 0xc0;
        OutputUtf8[1] = 0x80;
        *pNumBytesWritten = 2;
        jlStatus = JL_STATUS_SUCCESS;
    }
    else if( UnicodeValue <= 0x7f )
    {
        OutputUtf8[0] = (uint8_t)UnicodeValue;
        *pNumBytesWritten = 1;
        jlStatus = JL_STATUS_SUCCESS;
    }
    else if( UnicodeValue <=0x7ff)
    {
        OutputUtf8[0] = ((uint8_t)(UnicodeValue >> 6) & 0x1f ) | 0xc0;
        OutputUtf8[1] = ((uint8_t)(UnicodeValue     ) & 0x3f ) | 0x80;
        *pNumBytesWritten = 2;
        jlStatus = JL_STATUS_SUCCESS;
    }
    else if( UnicodeValue <=0xffff)
    {
        OutputUtf8[0] = ((uint8_t)(UnicodeValue >> 12) & 0x0f ) | 0xe0;
        OutputUtf8[1] = ((uint8_t)(UnicodeValue >> 6 ) & 0x3f ) | 0x80;
        OutputUtf8[2] = ((uint8_t)(UnicodeValue      ) & 0x3f ) | 0x80;
        *pNumBytesWritten = 3;
        jlStatus = JL_STATUS_SUCCESS;
    }
    else if( UnicodeValue <=0x10ffff)
    {
        OutputUtf8[0] = ((uint8_t)(UnicodeValue >> 18) & 0x07 ) | 0xf0;
        OutputUtf8[1] = ((uint8_t)(UnicodeValue >> 12) & 0x3f ) | 0x80;
        OutputUtf8[2] = ((uint8_t)(UnicodeValue >> 6 ) & 0x3f ) | 0x80;
        OutputUtf8[3] = ((uint8_t)(UnicodeValue      ) & 0x3f ) | 0x80;
        *pNumBytesWritten = 4;
        jlStatus = JL_STATUS_SUCCESS;
    }
    else
    {
        // Invalid Unicode value. (Unicode is defined from 0 - 0x10ffff)
        jlStatus = JL_STATUS_INVALID_DATA;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlUnicodeCharFromUtf8
//
//  Returns the unicode value of the next utf8 character in a string.
//  *pNumBytesUsed will be set to the number of bytes read (1-4)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlUnicodeCharFromUtf8
    (
        char const*     Utf8String,
        size_t          Utf8StringLength,
        uint32_t*       pUnicodeValue,
        size_t*         pNumBytesUsed
    )
{
    // Determine number of bytes needed.
    // We assume Utf8StringLength is at least 1 byte long.
    size_t numBytesNeeded = 0;
    uint8_t topBits = 0;
    JL_STATUS jlStatus;

    if( 0xf0 == (Utf8String[0] & 0xf8) )
    {
        numBytesNeeded = 4;
        topBits = Utf8String[0] & 0x07;
        jlStatus = JL_STATUS_SUCCESS;
    }
    else if( 0xe0 == (Utf8String[0] & 0xf0) )
    {
        numBytesNeeded = 3;
        topBits = Utf8String[0] & 0x0f;
        jlStatus = JL_STATUS_SUCCESS;
    }
    else if( 0xc0 == (Utf8String[0] & 0xe0) )
    {
        numBytesNeeded = 2;
        topBits = Utf8String[0] & 0x1f;
        jlStatus = JL_STATUS_SUCCESS;
    }
    else if( 0x00 == (Utf8String[0] & 0x80) )
    {
        numBytesNeeded = 1;
        topBits = Utf8String[0] & 0x7f;
        jlStatus = JL_STATUS_SUCCESS;
    }
    else
    {
        // Is either a continuation character without a start, or an invalid start such as
        // a 5 or 6 byte character which is not valid in UTF8
        jlStatus = JL_STATUS_INVALID_DATA;
    }

    if( JL_STATUS_SUCCESS == jlStatus )
    {
        if( numBytesNeeded <= Utf8StringLength )
        {
            // Start with the top bits
            uint32_t unicode = topBits;

            /// Now slide in the next bits in sets of 6 for each continuation character
            for( uint32_t i=1; i<numBytesNeeded; i++ )
            {
                // Verify that this is a continuation utf8 char
                if( 0x80 == (Utf8String[i] & 0xc0) )
                {
                    unicode <<= 6;
                    unicode |= (Utf8String[i] & 0x3f );
                }
                else
                {
                    // Not a continuation character. This is invalid UTF8
                    jlStatus = JL_STATUS_INVALID_DATA;
                    break;
                }
            }

            if( JL_STATUS_SUCCESS == jlStatus )
            {
                if(     unicode <= 0x10ffff
                    &&  (unicode < 0xd800 || unicode > 0xdfff) )
                {
                    // Valid unicode range (We don't allow UTF16 surrogate pairs to appear in the utf8)
                    *pUnicodeValue = unicode;
                    *pNumBytesUsed = numBytesNeeded;
                }
                else
                {
                    jlStatus = JL_STATUS_INVALID_DATA;
                }
            }
        }
        else
        {
            // Not enough bytes to complete utf8 character
            jlStatus = JL_STATUS_INVALID_DATA;
        }
    }

    return jlStatus;
}
