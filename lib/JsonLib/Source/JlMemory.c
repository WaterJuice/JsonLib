////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//  This module provides the core memory allocation functions. They rely on macros JlAlloc and JlFree. These are
//  by default defined in JsonLib.h as calloc and free. However they can be defined as anything else with the same
//  prototype by defining the macros JlAlloc and JlFree in a header file and then setting a global project define of
//  JL_INCLUDE_H to point to that file. This will allow the use of a different memory allocator, as long as the
//  prototype matches calloc and free.
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JlMemory.h"
#include "JsonLib.h"
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  GLOBALS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlStrDup
//
//  Duplicates a string like strdup (But uses JlAlloc function)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char*
    JlStrDup
    (
        char const*     SourceString
    )
{
    char* newString = NULL;

    if( NULL != SourceString )
    {
        size_t stringSize = strlen( SourceString ) + 1;
        newString = JlAlloc( stringSize );
        if( NULL != newString )
        {
            memcpy( newString, SourceString, stringSize );
        }
    }

    return newString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlRealloc
//
//  Allocates a new buffer of a different size from the original buffer and copies the contents across, then deallocates
//  the original buffer.
//  If an error occurs NULL is returned. Otherwise a valid pointer is returned and OriginalBuffer is NO LONGER VALID
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void*
    JlRealloc
    (
        void*       OriginalBuffer,
        size_t      OriginalSize,
        size_t      NewSize
    )
{
    void* retPtr;

    if(     NULL != OriginalBuffer
        &&  0 != OriginalSize
        &&  0 != NewSize )
    {
        void* newBuffer = JlAlloc( NewSize );
        if( NULL != newBuffer )
        {
            if( NewSize >= OriginalSize )
            {
                memcpy( newBuffer, OriginalBuffer, OriginalSize );
            }
            else
            {
                // Shrunk
                memcpy( newBuffer, OriginalBuffer, NewSize );
            }

            // Deallocate original
            JlFree( OriginalBuffer );
            retPtr = newBuffer;
        }
        else
        {
            // Failed to allocate memory
            retPtr = NULL;
        }
    }
    else
    {
        // Invalid Parameters;
        retPtr = NULL;
    }

    return retPtr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMemoryWriteCountValue
//
//  Writes the count value to the location pointed to by CountFieldPtr and CountFieldSize. This will write
//  an 8bit, 16bit, 32bit, or 64bit size value to the struct.
//  This will return JL_STATUS_COUNT_FIELD_TOO_SMALL if the count field is too small for the number being written.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlMemoryWriteCountValue
    (
        void*           CountFieldPtr,
        size_t          CountFieldSize,
        size_t          CountValue
    )
{
    JL_STATUS jlStatus;

    if( sizeof(uint8_t) == CountFieldSize )
    {
        uint8_t* countElementInStructU8 = (uint8_t*)CountFieldPtr;
        if( CountValue <= UINT8_MAX )
        {
            *countElementInStructU8 = (uint8_t)CountValue;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            jlStatus = JL_STATUS_COUNT_FIELD_TOO_SMALL;
        }
    }
    else if( sizeof(uint16_t) == CountFieldSize )
    {
        uint16_t* countElementInStructU16 = (uint16_t*)CountFieldPtr;
        if( CountValue <= UINT16_MAX )
        {
            *countElementInStructU16 = (uint16_t)CountValue;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            jlStatus = JL_STATUS_COUNT_FIELD_TOO_SMALL;
        }
    }
    else if( sizeof(uint32_t) == CountFieldSize )
    {
        uint32_t* countElementInStructU32 = (uint32_t*)CountFieldPtr;
        if( CountValue <= UINT32_MAX )
        {
            *countElementInStructU32 = (uint32_t)CountValue;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            jlStatus = JL_STATUS_COUNT_FIELD_TOO_SMALL;
        }
    }
    else if( sizeof(uint64_t) == CountFieldSize )
    {
        uint64_t* countElementInStructU64 = (uint64_t*)CountFieldPtr;
        *countElementInStructU64 = CountValue;
        jlStatus = JL_STATUS_SUCCESS;
    }
    else
    {
        // Invalid size.
        jlStatus = JL_STATUS_INTERNAL_ERROR;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMemoryReadCountValue
//
//  Reads the count value from the structure using the pointer CountFieldPtr and CountFieldSize values. This will read
//  an 8bit, 16bit, 32bit, or 64bit size value from the struct and always return a size_t value.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t
    JlMemoryReadCountValue
    (
        void const*     CountFieldPtr,
        size_t          CountFieldSize
    )
{
    size_t countValue = 0;

    if( sizeof(uint8_t) == CountFieldSize )
    {
        uint8_t const* countElementInStructU8 = (uint8_t*)CountFieldPtr;
        countValue = *countElementInStructU8;
    }
    else if( sizeof(uint16_t) == CountFieldSize )
    {
        uint16_t const* countElementInStructU16 = (uint16_t*)CountFieldPtr;
        countValue = *countElementInStructU16;
    }
    else if( sizeof(uint32_t) == CountFieldSize )
    {
        uint32_t const* countElementInStructU32 = (uint32_t*)CountFieldPtr;
        countValue = *countElementInStructU32;
    }
    else if( sizeof(uint64_t) == CountFieldSize )
    {
        uint64_t const* countElementInStructU64 = (uint64_t*)CountFieldPtr;
        countValue = *countElementInStructU64;
    }
    // else:
    // Invalid size for count field. This should not be possible with the macros, so we don't return an error.
    // Just return a size of 0. If this happens there is some other internal error going on.

    return countValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMemoryVerifyCountFieldLargeEnoughForValue
//
//  Verifies that the size_t value will fit in an unsigned int of specified size (in bytes)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlMemoryVerifyCountFieldLargeEnoughForValue
    (
        size_t          CountFieldSize,
        size_t          Value
    )
{
    JL_STATUS jlStatus;

    jlStatus = JL_STATUS_COUNT_FIELD_TOO_SMALL;
    if( sizeof(uint8_t) == CountFieldSize )
    {
        if( Value <= 0xff )
        {
            jlStatus = JL_STATUS_SUCCESS;
        }
    }
    else if( sizeof(uint16_t) == CountFieldSize )
    {
        if( Value <= 0xffff )
        {
            jlStatus = JL_STATUS_SUCCESS;
        }
    }
    else if( sizeof(uint32_t) == CountFieldSize )
    {
        if( Value <= 0xffffffff )
        {
            jlStatus = JL_STATUS_SUCCESS;
        }
    }
    else if( sizeof(uint64_t) == CountFieldSize )
    {
        jlStatus = JL_STATUS_SUCCESS;
    }
    else
    {
        jlStatus = JL_STATUS_INTERNAL_ERROR;
    }

    return jlStatus;
}
