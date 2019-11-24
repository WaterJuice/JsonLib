////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLibSample
//
//  Sample program using JsonLib
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "JsonLib.h"
#include "../Source/JlMemory.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ReadFileIntoMemory
//
//  Reads an entire text file into memory. Allocates the space for it. This will be read as a string and zero
//  terminated.
//  Returns true if success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
bool
    ReadFileIntoMemory
    (
        char const*     Filename,                   // [in]
        char**          pStringData                 // [out]
    )
{
    bool success;

    *pStringData = NULL;

    FILE* file = fopen( Filename, "rt" );
    if( NULL != file )
    {
        fseek( file, 0, SEEK_END );
        int fileLen = ftell( file );
        fseek( file, 0, SEEK_SET );

        if( fileLen > 0 )
        {
            // allocate buffer
            char* buffer = malloc( fileLen + 1 );
            if( NULL != buffer )
            {
                int amount;
                amount = (int)fread( buffer, 1, fileLen, file );

                if( amount <= fileLen && amount > 0 )
                {
                    buffer[amount] = 0;     // zero terminate
                    *pStringData = buffer;
                    success = true;
                }
                else
                {
                    // Failed to read file
                    success = false;
                }
            }
            else
            {
                // Out of memory
                success = false;
            }
        }
        else
        {
            // Failed to get file length
            success = false;
        }

        fclose( file );
    }
    else
    {
        success = false;
    }

    return success;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  SaveTextFile
//
//  Writes a zero terminated string out as a text file.
//  Returns true if success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
bool
    SaveTextFile
    (
        char const*     Filename,                   // [in]
        char const*     StringData                  // [in]
    )
{
    bool success;

    FILE* file = fopen( Filename, "w+t" );
    if( NULL != file )
    {
        int amount;
        amount = (int)fprintf( file, "%s", StringData );

        if( amount >= 0 )
        {
            success = true;
        }
        else
        {
            // Failed to read file
            success = false;
        }

        fclose( file );
    }
    else
    {
        success = false;
    }

    return success;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  main
//
//  Entry point.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int
main
    (
        int     ArgC,
        char**  ArgV
    )
{
    int result = 0;

    // The structure that represents the JSON file
    typedef struct
    {
        char* Name;
        uint32_t NumRuns;
    } SampleConfigStruct;

    // Mapping of JSON elements to structure elements
    JlMarshallElement marshalSampleConfigStruct[] =
    {
        JlMarshallString( SampleConfigStruct, Name, "Name" ),
        JlMarshallUnsigned( SampleConfigStruct, NumRuns, "NumRuns" ),
    };
    size_t marshalSampleConfigStructCount = sizeof(marshalSampleConfigStruct)/sizeof(marshalSampleConfigStruct[0]);

    SampleConfigStruct myConfig = {0};

    if( 2 != ArgC )
    {
        printf(
            "Syntax:\n"
            "  JsonLibSample <SampleJsonFile>\n"
            "\n" );
        result = 1;
    }
    else
    {
        char const* inputFilename = ArgV[1];
        bool success;
        char* inputJsonString = NULL;
        JL_STATUS jlStatus;
        size_t errorAtPos = 0;

        success = ReadFileIntoMemory( inputFilename, &inputJsonString );
        if( success )
        {

            // Unmarshall the JSON into the structure.
            jlStatus = JlJsonToStruct( inputJsonString, marshalSampleConfigStruct, marshalSampleConfigStructCount, &myConfig, &errorAtPos );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                // Increment the run count
                myConfig.NumRuns += 1;
            }
            else
            {
                printf( "Failed to parse json in file: %s Position: %u\n", inputFilename, (uint32_t)errorAtPos );
                result = 3;
                success = false;
            }
        }
        else
        {
            // No Json file, so create new struct
            myConfig.Name = JlAlloc( 32 );
            strcpy( myConfig.Name, "Example Json File" );
            myConfig.NumRuns = 1;
            success = true;
        }

        if( success )
        {
            // Print info
            printf( "Name: %s\nNumRuns: %u\n", myConfig.Name, myConfig.NumRuns );

            // Now generate new Json
            char* outputJsonString = NULL;
            jlStatus = JlStructToJson( &myConfig, marshalSampleConfigStruct, marshalSampleConfigStructCount, true, &outputJsonString );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                // Save json file back out
                success = SaveTextFile( inputFilename, outputJsonString );
                if( success )
                {
                    // Success
                }
                else
                {
                    printf( "Failed to write Json back out to file: %s\n", inputFilename );
                    result = 5;
                }

                JlFreeJsonStringBuffer( &outputJsonString );
            }
            else
            {
                printf( "Failed to generate Json\n" );
                result = 4;
            }

            JlUnmarshallFreeStructAllocs( marshalSampleConfigStruct, marshalSampleConfigStructCount, &myConfig );
        }
    }

    return result;
}
