////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//  Defines the status codes used within JsonLib.
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum
{
    JL_STATUS_SUCCESS = 0,
    JL_STATUS_INVALID_PARAMETER = -1,
    JL_STATUS_OUT_OF_MEMORY = -2,
    JL_STATUS_INVALID_TYPE = -3,
    JL_STATUS_WRONG_TYPE = -4,
    JL_STATUS_DICTIONARY_ITEM_REPEATED = -5,
    JL_STATUS_END_OF_DATA = -6,
    JL_STATUS_INVALID_DATA = -7,
    JL_STATUS_NOT_FOUND = -8,
    JL_STATUS_BUFFER_TOO_SMALL = -9,
    JL_STATUS_TOO_MANY_ITEMS = -10,
    JL_STATUS_INVALID_SPECIFICATION = -11,
    JL_STATUS_VALUE_OUT_OF_RANGE = -12,
    JL_STATUS_CORRUPT_MEMORY = -13,
    JL_STATUS_INTERNAL_ERROR = -14,
    JL_STATUS_STRING_NOT_TERMINATED = -15,
    JL_STATUS_JSON_NESTING_TOO_DEEP = -16,
    JL_STATUS_COUNT_FIELD_TOO_SMALL = -17,
} JL_STATUS;
