////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonRewrite
//
//  Simple executable to read a json file, parse it, and then output a new json file with specified parameters.
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "JsonLib.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
    #define strcasecmp stricmp
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParseCommandLineBoolArg
//
//  Scans the ArgV array looking for specified match. If it exists this function will remove it from the list and will
//  return true. If it is not in the array it will return false.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
bool
    ParseCommandLineBoolArg
    (
        int*        pArgC,      // [in,out]
        char***     pArgV,      // [in,out]
        char const* String1,    // [in]
        char const* String2     // [in]
    )
{
    bool itemExists = false;

    for( int i=0; i<*pArgC; i++ )
    {
        if(     strcasecmp( String1, (*pArgV)[i] ) == 0
            ||  strcasecmp( String2, (*pArgV)[i] ) == 0 )
        {
            itemExists = true;
            // ripple down remaining items
            for( int x=i; x<(*pArgC)-1; x++ )
            {
                (*pArgV)[x] = (*pArgV)[x+1];
            }
            *pArgC -= 1;
        }
    }

    return itemExists;
}

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

    bool outputAscii = ParseCommandLineBoolArg( &ArgC, &ArgV, "-a", "--ascii" );
    bool outputIndent = ParseCommandLineBoolArg( &ArgC, &ArgV, "-i", "--indent" );
    bool outputHex = ParseCommandLineBoolArg( &ArgC, &ArgV, "-x", "--hex" );
    bool outputComma = ParseCommandLineBoolArg( &ArgC, &ArgV, "-c", "--comma" );
    bool outputBare = ParseCommandLineBoolArg( &ArgC, &ArgV, "-b", "--bare" );
    bool outputSingleQuote = ParseCommandLineBoolArg( &ArgC, &ArgV, "-s", "--singlequote" );
    bool outputJson5 = ParseCommandLineBoolArg( &ArgC, &ArgV, "-5", "--json5" );
    bool help = ParseCommandLineBoolArg( &ArgC, &ArgV, "-h", "--help" );
    help |= ParseCommandLineBoolArg( &ArgC, &ArgV, "/h", "/?" );

    if( 2 != ArgC || help )
    {
        printf(
            "Syntax:\n"
            "  JsonRewrite [options] <JsonFile>\n"
            "   options:\n"
            "       -a, --ascii       - Escape all non ascii in output\n"
            "       -i, --indent      - Apply indent formatting to output\n"
            "       -x, --hex         - Allow hex in output (Json5)\n"
            "       -b, --bare        - Allow bare keywords in dictionaries (Json5)\n"
            "       -s, --singlequote - Use single quotes instead od doubles for strings (Json5)\n"
            "       -c, --comma       - Put in trailing commas (Json5)\n"
            "       -5, --json5       - Combines --hex --bare --singlequote\n"
            "\n" );
        result = 1;
    }
    else
    {
        char const* inputFilename = ArgV[1];
        bool success;
        char* inputJsonString = NULL;

        // Create output option flags.
        JL_OUTPUT_FLAGS outputFlags =
                ( outputAscii ? JL_OUTPUT_FLAGS_ASCII : 0 )
            |   ( outputIndent ? JL_OUTPUT_FLAGS_INDENT : 0 )
            |   ( outputHex ? JL_OUTPUT_FLAGS_J5_ALLOW_HEX : 0 )
            |   ( outputComma ? JL_OUTPUT_FLAGS_J5_TRAILING_COMMAS : 0 )
            |   ( outputBare ? JL_OUTPUT_FLAGS_J5_USE_BARE_KEYWORDS : 0 )
            |   ( outputSingleQuote ? JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES : 0 )
            |   ( outputJson5 ? JL_OUTPUT_FLAGS_JSON5 : 0 );

        success = ReadFileIntoMemory( inputFilename, &inputJsonString );
        if( success )
        {
            JL_STATUS jlStatus;
            size_t errorAtPos = 0;
            JlDataObject* jsonObject = NULL;

            jlStatus = JlParseJson( inputJsonString, &jsonObject, &errorAtPos );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                char* outputJsonString = NULL;
                jlStatus = JlOutputJsonEx( jsonObject, outputFlags, &outputJsonString );
                if( JL_STATUS_SUCCESS == jlStatus )
                {
                    // Print out the new formatted Json.
                    printf( "%s", outputJsonString );

                    JlFreeJsonStringBuffer( &outputJsonString );
                }
                else
                {
                    printf( "Failed to output json\n" );
                    result = 4;
                }

                JlFreeObjectTree( &jsonObject );
            }
            else
            {
                printf( "Failed to parse json in file: %s Position: %u\n", inputFilename, (uint32_t)errorAtPos );
                result = 3;
            }

            free( inputJsonString );
        }
        else
        {
            printf( "Failed to read file: %s\n", inputFilename );
            result = 2;
        }

    }

    return result;
}
