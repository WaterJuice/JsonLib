////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WjTestLib
//
//  A simple unit test framework.
//
//  This is free and unencumbered software released into the public domain - August 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WjTestLib.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct WjtlGroupList WjtlGroupList;
typedef struct WjtlTestList WjtlTestList;

struct WjtlTestList
{
    WjtlTestList*               Next;

    char const*                 TestName;
    WJTL_UNIT_TEST_FUNCTION     TestFunction;

    uint64_t                    NumAllocations;
    uint64_t                    NumDeallocations;
    uint64_t                    NumAsserts;
    uint64_t                    NumFailedAsserts;
    WJTL_STATUS                 TestStatus;
};

struct WjtlGroupList
{
    WjtlGroupList*              Next;

    char const*                 GroupName;

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
            ||  stricmp( group->GroupName, Group )==0 )
        {
            printf( ":::::::: Test Group : %s ::::::::\n" );
            foundGroup = true;
            for( WjtlTestList* test=group->TestListHead; test!=NULL; test=test->Next )
            {
                if(     NULL == Test
                    ||  stricmp( test->TestName, Test )==0 )
                {
                    printf( ":::: Test : %s ::::\n" );
                    foundTest = true;

                    wjtlStatus = RunTest( test );
                    if( WJTL_STATUS_SUCCESS != testFailed )
                    {
                        testFailed = true;
                    }
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

}
