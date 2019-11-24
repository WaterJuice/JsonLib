////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WjTestLib
//
//  A simple unit test framework.
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WjTestLib.h"
#include "WjTestLib_Memory.h"
#include "WjTestLib_Asserts.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifndef _MSC_VER
    #include <inttypes.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  DEFINES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
    #define PRIu64 "llu"
    #define PRId64 "lld"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
    #define strcasecmp stricmp
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct WjtlGroupList WjtlGroupList;
typedef struct WjtlTestList WjtlTestList;

struct WjtlTestList
{
    WjtlTestList*               Next;

    char*                       TestName;
    WJTL_UNIT_TEST_FUNCTION     TestFunction;

    uint64_t                    NumAllocations;
    uint64_t                    NumDeallocations;
    uint64_t                    NumAsserts;
    uint64_t                    NumFailedAsserts;
    WJTL_STATUS                 TestStatus;
    int64_t                     NumMemLeaks;
    bool                        TestRun;
};

struct WjtlGroupList
{
    WjtlGroupList*              Next;

    char*                       GroupName;
    bool                        GroupRun;

    WjtlTestList*               TestListHead;
    WjtlTestList*               TestListTail;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  GLOBALS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static WjtlGroupList*           gGroupListHead  = NULL;
static WjtlGroupList*           gGroupListTail  = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  InternalError
//
//  Displays error message to stderr and quits program
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
void
    InternalError
    (
        char const*     ErrorMessage
    )
{
    fprintf(
        stderr,
        "********************************************************************\n"
        "** INTERNAL ERROR (WjTestLib)\n"
        "** %s\n"
        "********************************************************************\n",
        ErrorMessage );
    exit( 1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  RunTest
//
//  Runs an individual test.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    RunTest
    (
        WjtlTestList*       Test
    )
{
    WJTL_STATUS wjtlStatus;

    if( NULL != Test )
    {
        uint64_t startMemAllocs = 0;
        uint64_t startMemFrees = 0;
        uint64_t startMemOutstanding = 0;
        uint64_t endMemAllocs = 0;
        uint64_t endMemFrees = 0;
        uint64_t endMemOutstanding = 0;
        uint64_t numAsserts = 0;
        uint64_t numAssertFails = 0;

        WjTestLib_Memory_GetStats( &startMemAllocs, &startMemFrees, &startMemOutstanding );
        WjTestLib_Asserts_Reset( );

        wjtlStatus = Test->TestFunction( );

        WjTestLib_Memory_GetStats( &endMemAllocs, &endMemFrees, &endMemOutstanding );
        WjTestLib_Asserts_GetStats( &numAsserts, &numAssertFails );

        Test->TestStatus = wjtlStatus;
        Test->NumAsserts = numAsserts;
        Test->NumFailedAsserts = numAssertFails;

        if( numAssertFails > 0 )
        {
            printf( "#### %"PRIu64" Assert fails in test: %s\n", numAssertFails, Test->TestName );
        }

        Test->NumAllocations = endMemAllocs - startMemAllocs;
        Test->NumMemLeaks = endMemOutstanding - startMemOutstanding;
        if( 0 != Test->NumMemLeaks )
        {
            printf( "#### %"PRIu64" Memory leaks in test: %s\n", Test->NumMemLeaks, Test->TestName );
        }

        if( WJTL_STATUS_SUCCESS == wjtlStatus )
        {
            if( numAssertFails > 0 )
            {
                // If any assert fail then the test automatically fails
                wjtlStatus = WJTL_STATUS_FAILED;
            }
        }
    }
    else
    {
        wjtlStatus = WJTL_STATUS_INVALID_PARAMETER;
    }

    return wjtlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  RunTests
//
//  Runs all the tests, or just tests in one specified group, or just one test within one specified group
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
WJTL_STATUS
    RunTests
    (
        char const*     Group,      // OPTIONAL
        char const*     Test        // OPTIONAL
    )
{
    WJTL_STATUS wjtlStatus;
    bool foundGroup = false;
    bool foundTest = false;
    bool testFailed = false;

    for( WjtlGroupList* group=gGroupListHead; group!=NULL; group=group->Next )
    {
        if(     NULL == Group
            ||  strcasecmp( group->GroupName, Group )==0 )
        {
            printf( ":::::::: Test Group : %s ::::::::\n", group->GroupName );
            foundGroup = true;
            for( WjtlTestList* test=group->TestListHead; test!=NULL; test=test->Next )
            {
                if(     NULL == Test
                    ||  strcasecmp( test->TestName, Test )==0 )
                {
                    printf( ":::: Test : %s ::::\n", test->TestName );
                    foundTest = true;

                    wjtlStatus = RunTest( test );
                    if( WJTL_STATUS_SUCCESS != testFailed )
                    {
                        testFailed = true;
                    }

                    group->GroupRun = true;
                    test->TestRun = true;
                }
            }
        }
    }

    if( !foundGroup )
    {
        printf( "No group named: %s\n", Group );
        wjtlStatus = WJTL_STATUS_GROUP_NOT_FOUND;
    }
    else if( !foundTest )
    {
        printf( "No test named: %s\n", Test );
        wjtlStatus = WJTL_STATUS_TEST_NOT_FOUND;
    }
    else if( testFailed )
    {
        wjtlStatus = WJTL_STATUS_FAILED;
    }
    else
    {
        wjtlStatus = WJTL_STATUS_SUCCESS;
    }

    return wjtlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WjTestLib_NewGroup
//
//  Starts a new group of unit tests. Tests registered with WjTestLib_AddTest will be linked with this group.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
WjTestLib_NewGroup
    (
        char const*     GroupName
    )
{
    WjtlGroupList* newGroup = calloc( 1, sizeof(WjtlGroupList) );
    if( NULL != newGroup )
    {
        newGroup->GroupName = strdup( GroupName );
        if( NULL != newGroup->GroupName )
        {
            newGroup->Next = NULL;
            newGroup->TestListHead = NULL;
            newGroup->TestListTail = NULL;

            if( NULL != gGroupListTail )
            {
                gGroupListTail->Next = newGroup;
            }
            else
            {
                gGroupListHead = newGroup;
            }
            gGroupListTail = newGroup;
        }
        else
        {
            InternalError( "Memory Fail" );
        }
    }
    else
    {
        InternalError( "Memory Fail" );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WjTestLib_AddTest
//
//  Adds a unit test function. This will be attached to the last group set by WjTestLib_NewGroup (or default if none
//  has been created)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
    WjTestLib_AddTest
    (
        WJTL_UNIT_TEST_FUNCTION     TestFunction,
        char const*                 TestName
    )
{
    if(     NULL != TestFunction
        &&  NULL != TestName )
    {
        if( NULL == gGroupListTail )
        {
            // No group created yet, so make a default one
            WjTestLib_NewGroup( "Default" );
        }

        if( NULL != gGroupListTail )
        {
            WjtlTestList* newTest = calloc( 1, sizeof(WjtlTestList) );
            if( NULL != newTest )
            {
                newTest->TestName = strdup( TestName );
                if( NULL != newTest->TestName )
                {
                    newTest->NumAllocations = 0;
                    newTest->NumDeallocations = 0;
                    newTest->NumAsserts = 0;
                    newTest->NumFailedAsserts = 0;
                    newTest->TestFunction = TestFunction;
                    newTest->Next = NULL;

                    if( NULL == gGroupListTail->TestListTail )
                    {
                        gGroupListTail->TestListHead = newTest;
                    }
                    else
                    {
                        gGroupListTail->TestListTail->Next = newTest;
                    }
                    gGroupListTail->TestListTail = newTest;
                }
                else
                {
                    InternalError( "Memory fail" );
                }
            }
            else
            {
                InternalError( "Memory fail" );
            }
        }
        else
        {
            InternalError( "Failed to create default group" );
        }
    }
    else
    {
        InternalError( "Invalid parameters" );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WjTestLib_Run
//
//  Runs the tests that have been registered with WjTestLib_NewGroup and WjTestLib_AddTest. Pass the ArgC and ArgV
//  that came from command line.
//  This should be run from main() after the tests have been registered. This provides an interface complete with
//  help (-h or --help). By default all tests will be run. Command line options allow for testing single groups.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int
    WjTestLib_Run
    (
        int         ArgC,
        char**      ArgV
    )
{
    WJTL_STATUS wjtlStatus;
    int retValue;

    wjtlStatus = RunTests( NULL, NULL );

    if( WJTL_STATUS_SUCCESS == wjtlStatus )
    {
        // Success
        retValue = 0;
    }
    else
    {
        retValue = 1;
    }

    return retValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WjTestLib_DisplayResults
//
//  Displays the results of the tests to stdout
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
    WjTestLib_DisplayResults
    (
        void
    )
{
    uint64_t TotalAsserts = 0;
    uint64_t TotalAssertFails = 0;
    uint64_t TotalMemAllocs = 0;
    int64_t  TotalMemLeaks = 0;
    uint32_t NumTests = 0;
    uint32_t NumTestFails = 0;

    printf( "\n\nResults\n-------\n" );

    for( WjtlGroupList* group=gGroupListHead; group!=NULL; group=group->Next )
    {
        if( group->GroupRun )
        {
            printf( "Group: %s\n", group->GroupName );
            printf( "-----------------------------------------------------------------------------------------------\n" );
            printf( "                                           Asserts (FAIL)  Allocs (LEAKS)  Tests (FAIL)\n" );
            uint64_t groupTotalAsserts = 0;
            uint64_t groupTotalAssertFails = 0;
            uint64_t groupTotalMemAllocs = 0;
            int64_t  groupTotalMemLeaks = 0;
            uint32_t groupNumTests = 0;
            uint32_t groupNumTestFails = 0;

            for( WjtlTestList* test=group->TestListHead; test!=NULL; test=test->Next )
            {
                if( test->TestRun )
                {
                    bool ok = true;
                    if(     WJTL_STATUS_SUCCESS != test->TestStatus
                        ||  test->NumFailedAsserts > 0
                        ||  test->NumMemLeaks != 0 )
                    {
                        ok = false;
                    }
                    printf(
                        "Test: %-30s        %6"PRIu64" %6"PRIu64"  %6"PRIu64" %6"PRIu64"  %6u %6u  - %s\n",
                        test->TestName,
                        test->NumAsserts,
                        test->NumFailedAsserts,
                        test->NumAllocations,
                        test->NumMemLeaks,
                        1,
                        ok ? 0 : 1,
                        ok ? "Pass" : "FAIL" );

                    groupTotalAsserts += test->NumAsserts;
                    groupTotalAssertFails += test->NumFailedAsserts;
                    groupTotalMemAllocs += test->NumAllocations;
                    groupTotalMemLeaks += test->NumMemLeaks;
                    groupNumTests += 1;
                    groupNumTestFails += ok ? 0 : 1;
                }
            }

            printf( "-----------------------------------------------------------------------------------------------\n" );
            printf(
                "Group Totals                                %6"PRIu64" %6"PRIu64"  %"PRId64" %6"PRId64"  %6u %6u  - %s\n",
                groupTotalAsserts,
                groupTotalAssertFails,
                groupTotalMemAllocs,
                groupTotalMemLeaks,
                groupNumTests,
                groupNumTestFails,
                0 == groupNumTestFails ? "Pass" : "FAIL" );

            TotalAsserts += groupTotalAsserts;
            TotalAssertFails += groupTotalAssertFails;
            TotalMemAllocs += groupTotalMemAllocs;
            TotalMemLeaks += groupTotalMemLeaks;
            NumTests += groupNumTests;
            NumTestFails += groupNumTestFails;

            printf( "\n" );
        }
    }

    printf( "===============================================================================================\n" );
    printf( "                                           Asserts (FAIL)  Allocs (LEAKS)  Tests (FAIL)\n" );
    printf(
        "TOTALS                                      %6"PRIu64" %6"PRIu64"  %6"PRId64" %6"PRId64"  %6u %6u  - %s\n",
        TotalAsserts,
        TotalAssertFails,
        TotalMemAllocs,
        TotalMemLeaks,
        NumTests,
        NumTestFails,
        0 == NumTestFails ? "Pass" : "FAIL" );

    printf( "\n" );
    if( 0 == NumTestFails )
    {
        printf( "***** SUCCESS - ALL TESTS ******\n" );
    }
    else
    {
        printf( "FAIL: Not all tests passed\n" );
    }

    printf( "\n" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WjTestLib_FreeTests
//
//  Frees all resources associated with tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
    WjTestLib_FreeTests
    (
        void
    )
{
    WjtlGroupList* nextGroup = NULL;
    for( WjtlGroupList* group=gGroupListHead; group!=NULL; group=nextGroup )
    {
        nextGroup = group->Next;

        WjtlTestList* nextTest = NULL;
        for( WjtlTestList* test=group->TestListHead; test!=NULL; test=nextTest )
        {
            nextTest = test->Next;

            free( test->TestName );
            test->TestName = NULL;
            free( test );
            test = NULL;
        }

        group->TestListHead = NULL;
        group->TestListTail = NULL;

        free( group->GroupName );
        group->GroupName = NULL;
        free( group );
        group = NULL;
    }

    gGroupListHead = NULL;
    gGroupListTail = NULL;
}
