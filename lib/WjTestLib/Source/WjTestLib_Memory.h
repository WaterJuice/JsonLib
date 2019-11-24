////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WjTestLib
//
//  A simple unit test framework.
//  This module handles memory tracking. It contains alternatives to malloc, calloc, and free that can be used which
//  will have tracking. The test library automatically will check for leaks if these functions are used.
//  Note: The functions WjTestLib_Calloc, WjTestLib_Malloc and WjTestLib_Free are declared in WjTestLib.h
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WjTestLib_Memory_GetStats
//
//  Gets the total number of allocations and frees that have happened so far
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
    WjTestLib_Memory_GetStats
    (
        uint64_t*           pTotalAllocs,           // OPTIONAL
        uint64_t*           pTotalFrees,            // OPTIONAL
        uint64_t*           pTotalOutstanding       // OPTIONAL
    );