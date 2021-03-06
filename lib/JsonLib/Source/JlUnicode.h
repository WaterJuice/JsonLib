////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//  This module handles internal unicode conversions
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <string.h>
#include "JlStatus.h"

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
    );

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
    );
