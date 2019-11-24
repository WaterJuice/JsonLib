////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
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
//  TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct JlBuffer;
typedef struct JlBuffer JlBuffer;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlBufferCreate
//
//  Creates a new empty JlBuffer.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JlBuffer*
    JlBufferCreate
    (
        void
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlBufferFree
//
//  Deallocates a JlBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlBufferFree
    (
        JlBuffer**      pBufferContext
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlBufferAdd
//
//  Add data to the end of the buffer, extending it if required
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlBufferAdd
    (
        JlBuffer*       BufferContext,
        void const*     Data,
        size_t          DataSize
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlBufferGetDataBuffer
//
//  Gets the pointer to the internal data buffer in JlBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void*
    JlBufferGetDataBuffer
    (
        JlBuffer*       BufferContext
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlBufferGetDataSize
//
//  Gets the size of the data used in the JlBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t
    JlBufferGetDataSize
    (
        JlBuffer*       BufferContext
    );