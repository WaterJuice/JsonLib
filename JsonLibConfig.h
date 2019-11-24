////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibConfig.h
//
//  This file will be include in JsonLib.h and is used to redirect the allocation functions to the WjTestLib alloc
//  functions for memory tracking.
//  This file is included by by having JL_INCLUDE_H set to this file (This is done in cmake file)
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

void*
    WjTestLib_Calloc
    (
        size_t      NumOfElements,
        size_t      SizeOfElements
    );

void
    WjTestLib_Free
    (
        void*       Memory
    );

#ifndef NDEBUG
    // Don't include the memory tracking in release mode
    #define JlAlloc( AllocSize )    WjTestLib_Calloc ( (AllocSize), 1 )
    #define JlFree( Allocation )    WjTestLib_Free( Allocation )
#endif