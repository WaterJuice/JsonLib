////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//  This module encodes and decodes base 64. This uses modified base 64 that does not require padding characters.
//  It will not emit padding characters, but it will safely ignore any on input.
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JlBase64.h"
#include "JlMemory.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CONSTANTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static char const* BASE64CHARSET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
#define __ 0
static uint8_t const BASE64INVERT[] = {
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,    // 0-31
    __,__,__,__,__,__,__,__,__,__,__,62,__,__,__,63,52,53,54,55,56,57,58,59,60,61,__,__,__,__,__,__,    // 32-63
    __, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,__,__,__,__,__,    // 64-95
    __,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,__,__,__,__,__ };  // 96-127
#undef __

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Encode3BytesToBase64
//
//  Encode a 3 byte block to 4 Base 64 characters
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
    Encode3BytesToBase64
    (
        uint8_t const*      ThreeByteBlock,         // [in]
        char*               FourCharBlock           // [out]
    )
{
    // Take 6 bits at a time from the three byte block and put them into the 4 character block
    FourCharBlock[0] = ThreeByteBlock[0] >> 2;
    FourCharBlock[1] = ((ThreeByteBlock[0] & 0x03) << 4) | (ThreeByteBlock[1] >> 4);
    FourCharBlock[2] = ((ThreeByteBlock[1] & 0x0f) << 2) | (ThreeByteBlock[2] >> 6);
    FourCharBlock[3] = (ThreeByteBlock[2] & 0x3f);

    // Now use the 6 bit values as indices into the base64 character set
    FourCharBlock[0] = BASE64CHARSET[ (int)FourCharBlock[0] ];
    FourCharBlock[1] = BASE64CHARSET[ (int)FourCharBlock[1] ];
    FourCharBlock[2] = BASE64CHARSET[ (int)FourCharBlock[2] ];
    FourCharBlock[3] = BASE64CHARSET[ (int)FourCharBlock[3] ];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Decode4Base64ToBytes
//
//  Decode a 4 character base64 block to 3 bytes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
    Decode4Base64ToBytes
    (
        char const*         FourCharBlock,          // [in]
        uint8_t*            ThreeByteBlock          // [out]
    )
{
    // Convert the characters into 6 bit values
    uint8_t sixBitValues [4] = {0};

    sixBitValues[0] = BASE64INVERT[ FourCharBlock[0] & 0x7f ];
    sixBitValues[1] = BASE64INVERT[ FourCharBlock[1] & 0x7f ];
    sixBitValues[2] = BASE64INVERT[ FourCharBlock[2] & 0x7f ];
    sixBitValues[3] = BASE64INVERT[ FourCharBlock[3] & 0x7f ];

    // Assemble the 6 bit values into three bytes.
    ThreeByteBlock[0] = ( sixBitValues[0] << 2 ) | ( sixBitValues[1] >> 4 );
    ThreeByteBlock[1] = ( sixBitValues[1] << 4 ) | ( sixBitValues[2] >> 2 );
    ThreeByteBlock[2] = ( sixBitValues[2] << 6 ) | ( sixBitValues[3] );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlBase64Encode
//
//  Encodes binary into modified Base 64 (does not include padding character). Uses standard Base64 for MIME character
//  set. This function allocates *pBase64String as a zero terminated string. Use JlFree to deallocate the string.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlBase64Encode
    (
        void const*         Data,
        size_t              DataSize,
        char**              pBase64String
    )
{
    JL_STATUS jlStatus;

    if(     NULL != Data
        &&  0 != DataSize
        &&  NULL != pBase64String )
    {
        uint8_t const* dataBytes = Data;
        *pBase64String = NULL;

        size_t numCompleteBlocks = DataSize / 3;
        size_t numTrailingBytes = DataSize % 3;
        size_t numExtraChars = 0;

        // Calculate size of string needed
        size_t stringLength = numCompleteBlocks * 4;
        if( numTrailingBytes > 0 )
        {
            numExtraChars = numTrailingBytes + 1;
            stringLength += numExtraChars;
        }

        // Allocate string (one larger for the zero terminator)
        char* string = JlAlloc( stringLength + 1 );
        if( NULL != string )
        {
            // Process in blocks of 3 input bytes at a time
            size_t outIndex = 0;
            for( size_t blockIndex=0; blockIndex<numCompleteBlocks; blockIndex+=1 )
            {
                Encode3BytesToBase64( dataBytes + (blockIndex*3), string + outIndex );
                outIndex += 4;
            }

            if( numTrailingBytes > 0 )
            {
                // Process the final partial block
                uint8_t byteBlock[3] = {0};
                char charBlock[4] = {0};
                for( size_t i=0; i<numTrailingBytes; i++ )
                {
                    byteBlock[i] = dataBytes[ (numCompleteBlocks*3) + i ];
                }
                Encode3BytesToBase64( byteBlock, charBlock );

                // Now copy out the required number of characters
                for( size_t i=0; i<numExtraChars; i++ )
                {
                    string[outIndex] = charBlock[i];
                    outIndex += 1;
                }
            }

            *pBase64String = string;

            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            jlStatus = JL_STATUS_OUT_OF_MEMORY;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlBase64Decode
//
//  Decodes base64 into binary. This takes a string and will stop when it hits the end of string OR a padding
//  character. Any characters following the padding character will be ignored. Non base64 characters in the input
//  will return an error.
//  Use JlFree to deallocate *pData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlBase64Decode
    (
        char const*         Base64String,
        void**              pData,
        size_t*             pDataSize
    )
{
    JL_STATUS jlStatus;
    size_t base64Length = 0;

    if(     NULL != Base64String
        &&  NULL != pData
        &&  NULL != pDataSize )
    {
        *pData = NULL;
        *pDataSize = 0;

        // Get length of string by looking for a zero terminator OR a padding character.
        // Also validate every character is base64
        jlStatus = JL_STATUS_SUCCESS;
        while( 0 != Base64String[base64Length] && '=' != Base64String[base64Length] )
        {
            const char ch = Base64String[base64Length];
            if(     ( ch >= 'A' && ch <= 'Z' )
                ||  ( ch >= 'a' && ch <= 'z' )
                ||  ( ch >= '0' && ch <= '9' )
                ||  ( '+' == ch )
                ||  ( '/' == ch ) )
            {
                base64Length += 1;
            }
            else
            {
                jlStatus = JL_STATUS_INVALID_DATA;
                break;
            }
        }

        if( JL_STATUS_SUCCESS == jlStatus )
        {
            size_t numCompleteBlocks = base64Length / 4;
            size_t numTrailingChars = base64Length % 4;
            size_t numExtraBytes = 0;

            // Calculate size of output
            size_t dataSize = numCompleteBlocks * 3;
            if( numTrailingChars > 0 )
            {
                numExtraBytes = numTrailingChars - 1;
                dataSize += numExtraBytes;
            }

            uint8_t* data = JlAlloc( dataSize );
            if( NULL != data )
            {
                // Process in blocks of 4 characters forming 3 bytes
                size_t outIndex = 0;
                for( size_t blockIndex=0; blockIndex<numCompleteBlocks; blockIndex+=1 )
                {
                    Decode4Base64ToBytes( Base64String + (blockIndex*4), data + outIndex );
                    outIndex += 3;
                }

                if( numExtraBytes > 0 )
                {
                    // Process the final partial block
                    char charBlock[5] = "AAAA";
                    uint8_t byteBlock[3] = {0};
                    for( size_t i=0; i<numTrailingChars; i++ )
                    {
                        charBlock[i] = Base64String[ (numCompleteBlocks*4) + i ];
                    }
                    Decode4Base64ToBytes( charBlock, byteBlock );

                    // Now copy out the required number of bytes
                    for( size_t i=0; i<numExtraBytes; i++ )
                    {
                        data[outIndex] = byteBlock[i];
                        outIndex += 1;
                    }
                }
                else if( numTrailingChars > 0 && 0 == numExtraBytes )
                {
                    // This occurs if there is 1 character in the trailing block. This only represents 6 bits
                    // and therefore not a complete byte, so the data is truncated. So we report this as corrupt
                    jlStatus = JL_STATUS_INVALID_DATA;
                }

                if( JL_STATUS_SUCCESS == jlStatus )
                {
                    *pData = data;
                    *pDataSize = dataSize;
                }
                else
                {
                    JlFree( data );
                }
            }
            else
            {
                jlStatus = JL_STATUS_OUT_OF_MEMORY;
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}
