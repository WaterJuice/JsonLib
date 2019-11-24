////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//  This module contains the code for parsing JSON text into a JlDataObject tree
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "JlStatus.h"
#include "JlMemory.h"
#include "JlBuffer.h"
#include "JlDataModel.h"
#include "JlUnicode.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum
{
    DICT_SCAN_STATE_NONE = 0,
    DICT_SCAN_STATE_NEED_KEY = 1,
    DICT_SCAN_STATE_NEED_COLON = 2,
    DICT_SCAN_STATE_NEED_VALUE = 3,
    DICT_SCAN_STATE_NEED_COMMA_OR_END = 4,
} DICT_SCAN_STATE;

typedef struct
{
    JL_DATA_TYPE    Type;
    JlDataObject*   Object;
    bool            FinishedProcessing;
    bool            ProcessedFirstItem;
    bool            ScanForSubObjects;
    bool            PrevScanComma;
    DICT_SCAN_STATE DictionaryScanState;
    char*           CurrentKeyName;
    size_t          CurrentKeyStringIndex;
} ParseStack;

typedef struct
{
    ParseStack*     Stack;
    int32_t         StackIndex;
    char const*     JsonString;
    size_t          JsonStringLength;
    size_t          StringIndex;
    bool            IsJson5;

} ParseParameters;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
    #define strncasecmp strnicmp
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParseForNewType
//
//  Parses the json string from current position until it reaches a new type.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    DetectNewType
    (
        char const*     JsonString,
        size_t          JsonStringLength,
        size_t          StringIndex,
        bool            IsJson5,
        size_t*         pNewStringIndex,
        JL_DATA_TYPE*   pNewType,
        size_t*         pErrorAtPos
    )
{
    JL_STATUS jlStatus;
    bool inSingleLineComment = false;
    bool inMultiLineComment = false;

    *pNewStringIndex = StringIndex;
    *pNewType = JL_DATA_TYPE_NONE;

    jlStatus = JL_STATUS_END_OF_DATA;
    for( size_t i=StringIndex; i<JsonStringLength; i++ )
    {
        JL_DATA_TYPE newType = JL_DATA_TYPE_NONE;
        char currentChar = JsonString[i];
        char nextChar = JsonString[i+1];
        char nextNextChar = 0 != nextChar ? JsonString[i+2] : 0;

        // See if currently in a comment and if this is the end of comment.
        if( inSingleLineComment && '\n' == currentChar )
        {
            inSingleLineComment = false;
            continue;
        }
        else if( inMultiLineComment && '*' == currentChar && '/' == nextChar )
        {
            inMultiLineComment = false;
            i += 1; // Skip the second character in end of comment marker
            continue;
        }
        else if( inSingleLineComment || inMultiLineComment )
        {
            continue;
        }

        if(     ' '  == currentChar
            ||  '\n' == currentChar
            ||  '\r' == currentChar
            ||  '\t' == currentChar
            ||  ( '\f' == currentChar && IsJson5 )
            ||  ( '\v' == currentChar && IsJson5 ) )
        {
            // Whitespace
            continue;
        }
        else if( IsJson5 && '\xc2' == currentChar && '\xa0' == nextChar )
        {
            // Non-breaking space allowed by json5
            i += 1;
            continue;
        }
        else if(    IsJson5
                 && (   ( '\xe2' == currentChar && '\x80' == nextChar && '\xa8' == nextNextChar )        // Line separator
                     || ( '\xe2' == currentChar && '\x80' == nextChar && '\xa9' == nextNextChar )        // Paragraph separator
                     || ( '\xef' == currentChar && '\xbb' == nextChar && '\xbf' == nextNextChar ) ) )    // Byte order mark
        {
            // Extra white space allowed by json5
            i += 2;
            continue;
        }

        if( '{' == currentChar )
        {
            newType = JL_DATA_TYPE_DICTIONARY;
        }
        else if( '[' == currentChar )
        {
            newType = JL_DATA_TYPE_LIST;
        }
        else if( '\"' == currentChar )
        {
            newType = JL_DATA_TYPE_STRING;
        }
        else if( IsJson5 && '\'' == currentChar )
        {
            newType = JL_DATA_TYPE_STRING;
        }
        else if(    'f' == currentChar
                 || 't' == currentChar )
        {
            newType = JL_DATA_TYPE_BOOL;
        }
        else if(    ( currentChar >= '0' && currentChar <= '9' )
                 || '-' == currentChar
                 || ( '.' == currentChar && IsJson5 )
                 || ( '+' == currentChar && IsJson5 )
                 )
        {
            newType = JL_DATA_TYPE_NUMBER;
        }
        else if(    IsJson5
                 && (
                        ( ( 'i' == currentChar || 'I' == currentChar ) && ( 'n' == nextChar || 'N' == nextChar ) )
                     || ( ( 'n' == currentChar || 'N' == currentChar ) && ( 'a' == nextChar || 'A' == nextChar ) ) ) )
        {
            // Allow for "inf" or "nan" for json5
            newType = JL_DATA_TYPE_NUMBER;
        }
        else if( 'n' == currentChar )
        {
            // If NULL type then we will just treat as a string as we have NULL strings. tyle
            newType = JL_DATA_TYPE_STRING;
        }
        else if( IsJson5 && '/' == currentChar && '/' == nextChar )
        {
            // Single line comment
            i += 1; // Skip the second character in comment marker
            inSingleLineComment = true;
            continue;
        }
        else if( IsJson5 && '/' == currentChar && '*' == nextChar )
        {
            // Multi line comment
            i += 1; // Skip the second character in comment marker
            inMultiLineComment = true;
            continue;
        }
        else
        {
            // Unexpected character at this time.
            jlStatus = JL_STATUS_INVALID_DATA;
            if( NULL != pErrorAtPos )
            {
                *pErrorAtPos = i;
            }
        }

        if( JL_DATA_TYPE_NONE != newType )
        {
            *pNewStringIndex = i;
            *pNewType = newType;
            jlStatus = JL_STATUS_SUCCESS;
        }

        break;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParseForNewType
//
//  Parses the json string from current position until it reaches a new type, and then adds the new type to the
//  stack. Returns JL_STATUS_END_OF_DATA if the end of the string is reached.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    ParseForNewType
    (
        ParseParameters*    Params,
        bool                IsJson5,
        size_t*             pErrorAtPos
    )
{
    JL_STATUS jlStatus;
    JL_DATA_TYPE newType = JL_DATA_TYPE_NONE;
    size_t newStringIndex;

    jlStatus = DetectNewType(
        Params->JsonString,
        Params->JsonStringLength,
        Params->StringIndex,
        IsJson5,
        &newStringIndex,
        &newType,
        pErrorAtPos );

    if( JL_STATUS_SUCCESS == jlStatus  &&  JL_DATA_TYPE_NONE != newType )
    {
        if( Params->StackIndex <= MAX_JSON_DEPTH )
        {
            Params->StringIndex = newStringIndex;
            Params->StackIndex += 1;
            ParseStack* stackItem = &Params->Stack[Params->StackIndex];
            memset( stackItem, 0, sizeof(*stackItem) );

            stackItem->Type = newType;
            jlStatus = JlCreateObjectWithTag( newType, Params->StringIndex, &stackItem->Object );
        }
        else
        {
            // JSON nesting too deep.
            jlStatus = JL_STATUS_JSON_NESTING_TOO_DEEP;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyOnlyTrailingWhiteSpace
//
//  Verify there is only white space left in this string
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    VerifyOnlyTrailingWhiteSpace
    (
        char const*         String,
        bool                IsJson5
    )
{
    JL_STATUS jlStatus;
    size_t len = strlen( String );
    size_t newIndex = 0;
    JL_DATA_TYPE newType = JL_DATA_TYPE_NONE;
    size_t errorAtPos = 0;

    // We should not be able to detect a new type, this should just scan through white space and comments that
    // may be trailing.
    jlStatus = DetectNewType( String, len, 0, IsJson5, &newIndex, &newType, &errorAtPos );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        // This is an error as we should not have found a type
        jlStatus = JL_STATUS_INVALID_DATA;
    }
    else
    {
        // This is good, we should not have found anything
        jlStatus = JL_STATUS_SUCCESS;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyHexString
//
//  Verifies that the string with specified length contains only hex digits 0-9,a-f,A-F
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    VerifyHexString
    (
        char const*     String,
        size_t          StringLength
    )
{
    JL_STATUS jlStatus = JL_STATUS_SUCCESS;

    for( size_t i=0; i<StringLength; i++ )
    {
        if(     ( String[i] >= '0' && String[i] <= '9' )
            ||  ( String[i] >= 'A' && String[i] <= 'F' )
            ||  ( String[i] >= 'a' && String[i] <= 'f' ) )
        {
            // A valid hex char
        }
        else
        {
            jlStatus = JL_STATUS_INVALID_DATA;
            break;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ConvertEscapedUnicode
//
//  This takes a string starting AFTER the \u and processes the next 4 characters as a UTF-16 code. If it is part of
//  a surrogate pair then this will attempt to process the following 6 characters after that as \uxxxx for the second
//  part of pair.
//  *pNumExtraCharsRead will be set to number of characters read from String. This will be either 4 or 10.
//  *pProcessedStringPos will be incremented by the number of UTF-8 bytes that are added to ProcessedString. There is
//  no size parameter as we know that ProcessedString will always be large enough.
//  PRocessedString was originally allocated same length as original String, and a Unicode escape will be either 6 or
//  12 bytes in length which is more than any utf8 sequence.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    ConvertEscapedUnicode
    (
        char const*         String,
        size_t              StringLength,
        char*               ProcessedString,
        size_t*             pProcessedStringPos,
        size_t*             pNumExtraCharsRead
    )
{
    JL_STATUS jlStatus;

    if( StringLength >= 4 )
    {
        jlStatus = VerifyHexString( String, 4 );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            // Convert hex to number
            uint32_t number = 0;
            sscanf( String, "%4x", &number );

            if( number >= 0xD800 && number <= 0xDBFF )
            {
                // This is the high surrogate of a pair, so we need to be able to get the low surrogate next
                uint32_t highSurrogate = number;
                if( StringLength >= 10 )
                {
                    if( '\\' == String[4] && 'u' == String[5] )
                    {
                        jlStatus = VerifyHexString( String+6, 4 );
                        if( JL_STATUS_SUCCESS == jlStatus )
                        {
                            uint32_t lowSurrogate = 0;
                            sscanf( String+6, "%4x", &lowSurrogate );

                            if( lowSurrogate >= 0xDC00 && number <=0xDFFF )
                            {
                                //  Now assemble the surrogate pairs into a single Unicode point
                                number = ( ( ( highSurrogate & 0x03ff ) << 10 )
                                          | ( lowSurrogate & 0x03ff ) )
                                        + 0x10000;

                                *pNumExtraCharsRead += 10;
                            }
                            else
                            {
                                // Second surrogate was also high. This is invalid
                                jlStatus = JL_STATUS_INVALID_DATA;
                            }
                        }
                    }
                    else
                    {
                        // Not a Unicode escape
                        jlStatus = JL_STATUS_INVALID_DATA;
                    }
                }
                else
                {
                    // Not enough room in string for the second half of surrogate pair.
                    jlStatus = JL_STATUS_INVALID_DATA;
                }
            }
            else if( number >= 0xDC00 && number <=0xDFFF )
            {
                // This is the low surrogate of a pair, this can not come first, so this is an invalid string
                jlStatus = JL_STATUS_INVALID_DATA;
            }
            else
            {
                // Regular UTF16 character in BMP plane.
                *pNumExtraCharsRead += 4;
                jlStatus = JL_STATUS_SUCCESS;
            }

            if( JL_STATUS_SUCCESS == jlStatus )
            {
                // number contains the unicode value, now encode it into utf8
                size_t numUtf8Bytes = 0;
                jlStatus = JlUnicodeValueToUtf8( number, ProcessedString+(*pProcessedStringPos), &numUtf8Bytes );
                if( JL_STATUS_SUCCESS == jlStatus )
                {
                    *pProcessedStringPos += numUtf8Bytes;
                }
            }
        }
    }
    else
    {
        // String is too short and does not contain 4 characters after the initial \u
        jlStatus = JL_STATUS_INVALID_DATA;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParseString
//
//  Parses the String as a string type in json. Returns a JlDataObject containing the string. *pAmountProcessed will
//  be set with the amount of the string consumed by the parse function (including end quote).
//  If IsBareWord then this is treated as a Json5 bare key name.
//  If AllowNewLines is true then New line characters are allowed directly within the (non bareword) string (Json5)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    ParseString
    (
        char const*         String,
        size_t              StringLength,
        bool                IsBareWord,
        bool                AllowNewLines,
        size_t*             pAmountProcessed,
        char**              pProcessedString
    )
{
    JL_STATUS jlStatus;
    size_t strLength = 0;
    size_t strEndPos = 0;
    char* processedString = NULL;
    bool singleQuoted = '\'' == String[0] ? true : false;

    // Find end of string
    jlStatus = JL_STATUS_INVALID_DATA;
    if( IsBareWord )
    {
        // Find length of bareword. Simply scan through until we find a non valid bareword char
        for( size_t i=0; i<StringLength; i++ )
        {
            strEndPos = i;
            if( '\\' == String[i] )
            {
                // Skip next character
                i += 1;
                strLength += 1;
            }
            else if ( '_' == String[i] || '$' == String[i]
                      || ( String[i] >= 'a' && String[i] <= 'z' )
                      || ( String[i] >= 'A' && String[i] <= 'Z' )
                      || ( String[i] >= '0' && String[i] <= '9' ) )
            {
                jlStatus = JL_STATUS_SUCCESS;   // At least one character
            }
            else
            {
                // End of bareword
                break;
            }
            strLength += 1;
        }
    }
    else
    {
        // Quoted string. Find the length
        for( size_t i=1; i<StringLength; i++ )      // Start at 1 to skip the first double quote
        {
            strEndPos = i;
            if( '\\' == String[i] )
            {
                // Skip next character
                i += 1;
                strLength += 1;
                if( AllowNewLines )
                {
                    // Check if we are escaping a crlf sequence
                    if( '\r' == String[i] && '\n' == String[i+1] )
                    {
                        i += 1;
                        strLength += 1;
                    }
                }
            }
            else if(    ( !singleQuoted && '\"' == String[i] )
                     || (  singleQuoted && '\'' == String[i] ) )
            {
                jlStatus = JL_STATUS_SUCCESS;
                break;
            }
            else if( String[i] < 32 )
            {
                // Control chars not allowed.
                jlStatus = JL_STATUS_INVALID_DATA;
                *pAmountProcessed = i;
                break;
            }
            strLength += 1;
        }
    }

    if( JL_STATUS_SUCCESS == jlStatus )
    {
        // Allocate space for processed string
        size_t strPos = 0;
        processedString = JlAlloc( strLength + 1 );
        if( NULL != processedString )
        {
            // Process string
            for( size_t i=IsBareWord?0:1; i<strEndPos; i++ )     // Skip first char which is double quote unless IsBareWord true
            {
                char currentChar = String[i];

                if( '\\' == currentChar )
                {
                    // Escaped chars. (There will definitely be at least one character after this
                    // due to the way we parsed in previous loop)
                    char nextChar = String[i+1];
                    char nextNextChar = String[i+2];
                    if( 'n' == nextChar )
                    {
                        processedString[strPos] = '\n';
                        strPos += 1;
                    }
                    else if( 'r' == nextChar )
                    {
                        processedString[strPos] = '\r';
                        strPos += 1;
                    }
                    else if( 'f' == nextChar )
                    {
                        processedString[strPos] = '\f';
                        strPos += 1;
                    }
                    else if( 'b' == nextChar )
                    {
                        processedString[strPos] = '\b';
                        strPos += 1;
                    }
                    else if( 't' == nextChar )
                    {
                        processedString[strPos] = '\t';
                        strPos += 1;
                    }
                    else if( '\\' == nextChar )
                    {
                        processedString[strPos] = '\\';
                        strPos += 1;
                    }
                    else if( '/' == nextChar )
                    {
                        processedString[strPos] = '/';
                        strPos += 1;
                    }
                    else if( '\"' == nextChar )
                    {
                        processedString[strPos] = '\"';
                        strPos += 1;
                    }
                    else if( '\'' == nextChar )
                    {
                        processedString[strPos] = '\'';
                        strPos += 1;
                    }
                    else if( 'u' == nextChar )
                    {
                        size_t numExtraCharsRead = 0;
                        // attempt to convert the escaped unicode value.
                        jlStatus = ConvertEscapedUnicode( String+i+2, strLength-i-1, processedString, &strPos, &numExtraCharsRead );
                        if( JL_STATUS_SUCCESS == jlStatus )
                        {
                            // skip the processed chars (note this is after the \u, we will also skip the \u part later)
                            // numCharsRead will be either 4 or 10 depending whether surrogate pairs were used/
                            i += numExtraCharsRead;
                        }
                        else
                        {
                            *pAmountProcessed = i;
                            break;
                        }
                    }
                    else if( AllowNewLines && ( '\r' == nextChar || '\n' == nextChar ) )
                    {
                        // Escaped new lines are allowed, they get discarded
                        if( '\r' == nextChar && '\n' == nextNextChar )
                        {
                            // the Windows CR/LF was escaped without a separate escape for the \n
                            i += 1;  // skip the extra one
                        }
                    }
                    else
                    {
                        // Invalid escape char
                        jlStatus = JL_STATUS_INVALID_DATA;
                        *pAmountProcessed = i;
                        break;
                    }
                    i += 1;  // Skip the char after the initial backslash as we have processed it
                }
                else
                {
                    processedString[strPos] = currentChar;
                    strPos += 1;
                }
            }

            if( JL_STATUS_SUCCESS == jlStatus )
            {
                *pProcessedString = processedString;
                *pAmountProcessed = strEndPos + (IsBareWord ? 0 : 1);     // If not bareword then also skip the ending quote
            }
            else
            {
                JlFree( processedString );
            }
        }
        else
        {
            jlStatus = JL_STATUS_OUT_OF_MEMORY;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParseForString
//
//  Parses the Json string at current position as a string (after the first double quotes)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    ParseForString
    (
        ParseParameters*    Params,
        size_t*             pErrorAtPos
    )
{
    JL_STATUS jlStatus;
    char* processedString = NULL;
    char const* stringStart = Params->JsonString + Params->StringIndex;
    size_t maxStringLen = Params->JsonStringLength - Params->StringIndex;
    size_t actualStringLen = 0;

    // Check if this is a json null
    jlStatus = JL_STATUS_NOT_FOUND;
    if( maxStringLen >= 4 )
    {
        if( memcmp( stringStart, "null", 4 ) == 0 )
        {
            // We already have a string object created, so just leave it and it will be left as NULL.
            Params->Stack[Params->StackIndex].FinishedProcessing = true;
            Params->StringIndex += 4;
            jlStatus = JL_STATUS_SUCCESS;
        }
    }

    if( JL_STATUS_NOT_FOUND == jlStatus )
    {
        // Now treat as a normal string
        jlStatus = ParseString( stringStart, maxStringLen, false, Params->IsJson5, &actualStringLen, &processedString );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            Params->StringIndex += actualStringLen;

            // Set string in object (was already setup as a string object)
            Params->Stack[Params->StackIndex].FinishedProcessing = true;
            jlStatus = JlSetObjectString( Params->Stack[Params->StackIndex].Object, processedString );

            JlFree( processedString );
        }
        else
        {
            if( actualStringLen > 0 )
            {
                // If an error occured in the middle of the string, this will be set to where the error happened.
                if( NULL != pErrorAtPos )
                {
                    *pErrorAtPos = Params->StringIndex + actualStringLen;
                }
            }
        }
    }

    if( JL_STATUS_SUCCESS != jlStatus )
    {
        (void) JlFreeObjectTree( &Params->Stack[Params->StackIndex].Object );
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParseOutNumberString
//
//  Parses the String as a number type in json. This will return a string containing just the number.
//  *pAmountProcessed will be set with the amount of the string consumed by the parse function (including end quote).
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    ParseOutNumberString
    (
        char const*         String,
        size_t              StringLength,
        bool                AllowJson5,
        size_t*             pAmountProcessed,
        char**              pProcessedString
    )
{
    JL_STATUS jlStatus;
    size_t strLength = 0;
    size_t strEndPos = 0;
    size_t numSkippedChars = 0;
    char* processedString = NULL;
    bool hexMode = false;
    bool isSpecial = false;

    jlStatus = JL_STATUS_SUCCESS;

    if( AllowJson5 )
    {
        // Check if inf or nan
        if( StringLength >= 4 )
        {
            if(     strncasecmp( String, "+inf", 4 ) == 0
                ||  strncasecmp( String, "-inf", 4 ) == 0 )
            {
                strLength = 4;
                isSpecial = true;
            }
        }
        else if( StringLength >= 3 )
        {
            if(     strncasecmp( String, "inf", 3 ) == 0
                ||  strncasecmp( String, "nan", 3 ) == 0 )
            {
                strLength = 3;
                isSpecial = true;
            }
        }
    }

    // Find end of number
    if( !isSpecial )
    {
        jlStatus = JL_STATUS_SUCCESS;
        for( size_t i=0; i<StringLength; i++ )
        {
            if(     ( String[i] >= '0' && String[i] <= '9' )
                ||  '-' == String[i]
                ||  '.' == String[i]
                ||  'E' == String[i]
                ||  'e' == String[i]
                ||  '+' == String[i] )
            {
                strEndPos = i;
            }
            else if(    ' ' == String[i]
                     || '\r' == String[i]
                     || '\n' == String[i]
                     || '\t' == String[i]
                     || ( '\v' == String[i] && AllowJson5 )
                     || ( '\f' == String[i] && AllowJson5 )
                     || ',' == String[i]
                     || '}' == String[i]
                     || ']' == String[i]
                     || ( AllowJson5 && '\xc2' == String[i] && '\xa0' == String[i+1] )
                     || ( AllowJson5 && '\xe2' == String[i] && '\x80' == String[i+1] && '\xa8' == String[i+2] )
                     || ( AllowJson5 && '\xe2' == String[i] && '\x80' == String[i+1] && '\xa9' == String[i+2] )
                     || ( AllowJson5 && '\xef' == String[i] && '\xbb' == String[i+1] && '\xbf' == String[i+2] ) )
            {
                // Valid character that can follow a number element.
                break;
            }
            else if(    AllowJson5
                     && '/' == String[i] )
            {
                // Start of comment
                break;
            }
            else if(    AllowJson5
                     && 1 == i
                     && '0' == String[0]
                     && ( 'x' == String[1] || 'X' == String[1] ) )
            {
                hexMode = true;
                strEndPos = i;
            }
            else if(    hexMode
                     && (   ( String[i] >= 'a' && String[i] <= 'f' )
                         || ( String[i] >= 'A' && String[i] <= 'F' ) ) )
            {
                strEndPos = i;
            }
            else
            {
                // Invalid character
                jlStatus = JL_STATUS_INVALID_DATA;
                break;
            }
        }
    }

    if( hexMode && 1 == strEndPos )
    {
        // Its an invalid string to only be 0x with nothing following.
        jlStatus = JL_STATUS_INVALID_DATA;
    }
    else if( hexMode && strEndPos > 17 )
    {
        // Its too long for a 64 bit hex number which is the largest that can be handled.
        jlStatus = JL_STATUS_INVALID_DATA;
    }

    if( JL_STATUS_SUCCESS == jlStatus && !AllowJson5 )
    {
        // Don't allow trailing decimal point (unless Json5)
        if( '.' == String[strEndPos] )
        {
            jlStatus = JL_STATUS_INVALID_DATA;
        }
    }

    if( JL_STATUS_SUCCESS == jlStatus && !isSpecial )
    {
        // Now verify that it is a valid number
        bool minusAllowed = true;
        bool plusAllowed = AllowJson5 ? true : false;      // Json5 allows a leading point
        bool eAllowed = false;
        bool pointAllowed = AllowJson5 ? true : false;      // Json5 allows a leading point

        bool hadPoint = false;
        bool hadE = false;

        strLength = strEndPos + 1;
        jlStatus = JL_STATUS_SUCCESS;

        for( size_t i=0; i<strLength; i++ )
        {
            if( hexMode )
            {
                if(     i < 2
                    ||  ( String[i] >= '0' && String[i] <= '9' )
                    ||  ( String[i] >= 'a' && String[i] <= 'f' )
                    ||  ( String[i] >= 'A' && String[i] <= 'F' ) )
                {
                    strEndPos = i;
                }
                else
                {
                    jlStatus = JL_STATUS_INVALID_DATA;
                    break;
                }
            }
            else if(    ( String[i] >= '0' && String[i] <= '9' )
                    ||  ( '-' == String[i] && minusAllowed )
                    ||  ( '.' == String[i] && pointAllowed )
                    ||  ( 'E' == String[i] && eAllowed )
                    ||  ( 'e' == String[i] && eAllowed )
                    ||  ( '+' == String[i] && plusAllowed ) )
            {
                strEndPos = i;

                if( !hadPoint && !hadE )
                {
                    pointAllowed = true;
                }
                if( !hadE )
                {
                    eAllowed = true;
                }

                if( 'e' == String[i] || 'E' == String[i] )
                {
                    minusAllowed = true;
                    plusAllowed = true;
                    pointAllowed = false;
                    eAllowed = false;
                    hadE = true;
                }
                else if( '.' == String[i] )
                {
                    hadPoint = true;
                    pointAllowed = false;
                    eAllowed = false;       // can't have E straight after decimal point
                }
                else
                {
                    minusAllowed = false;
                    plusAllowed = false;
                }
            }
            else
            {
                jlStatus = JL_STATUS_INVALID_DATA;
                break;
            }
        }
    }

    if( JL_STATUS_SUCCESS == jlStatus )
    {
        char const* stringPtr = String;

        if( AllowJson5 && '+' == stringPtr[0] )
        {
            // Skip leading '+' allowed by Json5
            stringPtr += 1;
            strLength -= 1;
            numSkippedChars += 1;
        }

        // Copy out number string into newly allocated string buffer
        processedString = JlAlloc( strLength + 1 );
        if( NULL != processedString )
        {
            memcpy( processedString, stringPtr, strLength );
            *pProcessedString = processedString;
            *pAmountProcessed = strLength + numSkippedChars;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            jlStatus = JL_STATUS_OUT_OF_MEMORY;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParseForNumber
//
//  Parses the Json string at current position as a number
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    ParseForNumber
    (
        ParseParameters*    Params
    )
{
    JL_STATUS jlStatus;
    char* processedString = NULL;
    char const* stringStart = Params->JsonString + Params->StringIndex;
    size_t maxStringLen = Params->JsonStringLength - Params->StringIndex;
    size_t actualStringLen = 0;

    jlStatus = ParseOutNumberString( stringStart, maxStringLen, Params->IsJson5, &actualStringLen, &processedString );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        Params->StringIndex += actualStringLen;

        // Set number in object (was already setup as a number object)
        Params->Stack[Params->StackIndex].FinishedProcessing = true;
        jlStatus = JlSetObjectNumberString( Params->Stack[Params->StackIndex].Object, processedString );

        JlFree( processedString );
    }

    if( JL_STATUS_SUCCESS != jlStatus )
    {
        (void) JlFreeObjectTree( &Params->Stack[Params->StackIndex].Object );
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParseForBool
//
//  Parses the Json string at current position as a number
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    ParseForBool
    (
        ParseParameters*    Params
    )
{
    JL_STATUS jlStatus;
    char const* stringStart = Params->JsonString + Params->StringIndex;
    size_t maxStringLen = Params->JsonStringLength - Params->StringIndex;
    size_t actualStringLen = 0;
    bool boolValue = false;

    // Check for "true"
    if(     maxStringLen >= 4
        &&  0 == memcmp( stringStart, "true", 4 ) )
    {
        jlStatus = JL_STATUS_SUCCESS;
        actualStringLen = 4;
        boolValue = true;
    }
    else if(    maxStringLen >= 5
            &&  0 == memcmp( stringStart, "false", 5 ) )
    {
        jlStatus = JL_STATUS_SUCCESS;
        actualStringLen = 5;
        boolValue = false;
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_DATA;
    }

    if( JL_STATUS_SUCCESS == jlStatus )
    {
        Params->StringIndex += actualStringLen;

        // Set bool in object (was already setup as a bool object)
        Params->Stack[Params->StackIndex].FinishedProcessing = true;
        jlStatus = JlSetObjectBool( Params->Stack[Params->StackIndex].Object, boolValue );
    }

    if( JL_STATUS_SUCCESS != jlStatus )
    {
        (void) JlFreeObjectTree( &Params->Stack[Params->StackIndex].Object );
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  SkipWhiteSpace
//
//  Fast forwards the StringIndex in Params until reaches the next none whitespace character. Returns character.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    SkipWhiteSpace
    (
        ParseParameters*    Params,
        char*               pNextChar
    )
{
    JL_STATUS jlStatus;
    bool inSingleLineComment = false;
    bool inMultiLineComment = false;

    jlStatus = JL_STATUS_END_OF_DATA;
    while( Params->StringIndex < Params->JsonStringLength )
    {
        char currentChar = Params->JsonString[Params->StringIndex];
        char nextChar = Params->JsonString[Params->StringIndex+1];
        char nextNextChar = 0 != nextChar ? Params->JsonString[Params->StringIndex+2] : 0;

        // See if currently in a comment and if this is the end of comment.
        if( inSingleLineComment && '\n' == currentChar )
        {
            inSingleLineComment = false;
        }
        else if( inMultiLineComment && '*' == currentChar && '/' == nextChar )
        {
            inMultiLineComment = false;
            Params->StringIndex  += 1;      // Skip the extra char (the '/')
        }
        else if( inSingleLineComment || inMultiLineComment )
        {
            // In comment, so ignore
        }
        else if(     ' '  == currentChar
            ||  '\n' == currentChar
            ||  '\r' == currentChar
            ||  '\t' == currentChar
            ||  ( '\v' == currentChar && Params->IsJson5 )
            ||  ( '\f' == currentChar && Params->IsJson5 ) )
        {
            // White space, so ignore
        }
        else if( Params->IsJson5 && '\xc2' == currentChar && '\xa0' == nextChar )
        {
            // Non-breaking space allowed by json5
            Params->StringIndex += 1;
        }
        else if(    Params->IsJson5
                 && (   ( '\xe2' == currentChar && '\x80' == nextChar && '\xa8' == nextNextChar )        // Line separator
                     || ( '\xe2' == currentChar && '\x80' == nextChar && '\xa9' == nextNextChar )        // Paragraph separator
                     || ( '\xef' == currentChar && '\xbb' == nextChar && '\xbf' == nextNextChar ) ) )    // Byte order mark
        {
            // Extra white space allowed by json5
            Params->StringIndex += 2;
        }
        else if( Params->IsJson5 && '/' == currentChar && '/' == nextChar )
        {
            inSingleLineComment = true;
            Params->StringIndex += 1;
        }
        else if( Params->IsJson5 && '/' == currentChar && '*' == nextChar )
        {
            inMultiLineComment = true;
            Params->StringIndex += 1;
        }
        else
        {
            // First non whitespace/comment
            *pNextChar = currentChar;
            jlStatus = JL_STATUS_SUCCESS;
            break;
        }

        Params->StringIndex += 1;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParseForList
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    ParseForList
    (
        ParseParameters*    Params,
        size_t*             pErrorAtPos     // OPTIONAL
    )
{
    JL_STATUS jlStatus;
    ParseStack* currentStack = &Params->Stack[Params->StackIndex];
    char nextChar;

    if( !currentStack->ProcessedFirstItem )
    {
        // Skip past opening square bracket
        Params->StringIndex += 1;
    }

    jlStatus = SkipWhiteSpace( Params, &nextChar );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        if( !currentStack->PrevScanComma )
        {
            if( ']' == nextChar )
            {
                // End of array
                currentStack->FinishedProcessing = true;
                Params->StringIndex += 1;
            }
            else if( ',' == nextChar )
            {
                if( ! currentStack->ProcessedFirstItem )
                {
                    // Should not be a comma before first item
                    jlStatus = JL_STATUS_INVALID_DATA;
                }
                else
                {
                    Params->StringIndex += 1;
                    currentStack->PrevScanComma = true;
                }
            }
            else
            {
                if( currentStack->ProcessedFirstItem )
                {
                    // After first item we need to have a comma or a end bracket after
                    // the data.
                    jlStatus = JL_STATUS_INVALID_DATA;
                }
                else
                {
                    currentStack->ScanForSubObjects = true;
                }
            }
        }
        else
        {
            // If in last scan we had a comma then we must not have a comma again or an end bracket unless
            // its Json5
            if(     ',' == nextChar
                ||  ( ']' == nextChar && !Params->IsJson5 ) )
            {
                jlStatus = JL_STATUS_INVALID_DATA;
            }
            else if( ']' == nextChar && Params->IsJson5 )
            {
                // Allowed an end bracket after a trailing comma in Json5
                currentStack->FinishedProcessing = true;
                Params->StringIndex += 1;
            }
            else
            {
                currentStack->ScanForSubObjects = true;
                currentStack->PrevScanComma = false;
            }
        }

        currentStack->ProcessedFirstItem = true;
    }

    if(     JL_STATUS_SUCCESS != jlStatus
        &&  NULL != pErrorAtPos
        &&  0 == *pErrorAtPos )
    {
        *pErrorAtPos = Params->StringIndex;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParseKeyName
//
//  Parses the KeyName from the json string.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    ParseKeyName
    (
        ParseParameters*    Params,
        bool                IsBareWord
    )
{
    JL_STATUS jlStatus;
    size_t amountProcessed = 0;

    jlStatus = ParseString(
        Params->JsonString + Params->StringIndex,
        Params->JsonStringLength - Params->StringIndex,
        IsBareWord,
        false,
        &amountProcessed,
        &Params->Stack[Params->StackIndex].CurrentKeyName );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        Params->Stack[Params->StackIndex].CurrentKeyStringIndex = Params->StringIndex;
        Params->StringIndex += amountProcessed;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParseForDictionary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    ParseForDictionary
    (
        ParseParameters*    Params,
        size_t*             pErrorAtPos     // OPTIONAL
    )
{
    JL_STATUS jlStatus;
    ParseStack* currentStack = &Params->Stack[Params->StackIndex];
    char nextChar;

    if( !currentStack->ProcessedFirstItem )
    {
        // Skip past opening curly bracket
        Params->StringIndex += 1;
        currentStack->DictionaryScanState = DICT_SCAN_STATE_NEED_KEY;
    }

    jlStatus = SkipWhiteSpace( Params, &nextChar );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        if( DICT_SCAN_STATE_NEED_KEY == currentStack->DictionaryScanState )
        {
            if(     '\"' == nextChar
                ||  ( Params->IsJson5 && '\'' == nextChar ) )
            {
                // Read key
                jlStatus = ParseKeyName( Params, false );
                currentStack->DictionaryScanState = DICT_SCAN_STATE_NEED_COLON;
            }
            else if( Params->IsJson5 &&
                     (      '_' == nextChar
                       ||   '$' == nextChar
                       ||   ( nextChar >= 'a' && nextChar <= 'z' )
                       ||   ( nextChar >= 'A' && nextChar <= 'Z' )
                       ) )
            {
                // Json5 bareword key
                jlStatus = ParseKeyName( Params, true );
                currentStack->DictionaryScanState = DICT_SCAN_STATE_NEED_COLON;
            }
            else if( '}' == nextChar && ( !currentStack->ProcessedFirstItem || Params->IsJson5 ) )
            {
                // This means an empty dictionary or, in Json5, there was a trailing comma
                currentStack->FinishedProcessing = true;
                currentStack->DictionaryScanState = DICT_SCAN_STATE_NONE;
                Params->StringIndex += 1;   // Move past end curly bracket
            }
            else
            {
                jlStatus = JL_STATUS_INVALID_DATA;
            }
        }
        else if( DICT_SCAN_STATE_NEED_COLON == currentStack->DictionaryScanState )
        {
            if( ':' == nextChar )
            {
                currentStack->DictionaryScanState = DICT_SCAN_STATE_NEED_VALUE;
                currentStack->ScanForSubObjects = true;
                Params->StringIndex += 1;   // Move past colon
            }
            else
            {
                jlStatus = JL_STATUS_INVALID_DATA;
            }
        }
        else if( DICT_SCAN_STATE_NEED_VALUE == currentStack->DictionaryScanState )
        {
            // We will have added object already because ScanForSubObjects was set and
            // value processed in main loop. So we just need to skip on to the next state
            currentStack->DictionaryScanState = DICT_SCAN_STATE_NEED_COMMA_OR_END;
            currentStack->ScanForSubObjects = false;
        }
        else if( DICT_SCAN_STATE_NEED_COMMA_OR_END == currentStack->DictionaryScanState )
        {
            if( ',' == nextChar )
            {
                // Set back to scanning for a new keyname
                currentStack->DictionaryScanState = DICT_SCAN_STATE_NEED_KEY;
                Params->StringIndex += 1;   // Move past comma
            }
            else if( '}' == nextChar )
            {
                // End dictionary
                currentStack->FinishedProcessing = true;
                currentStack->DictionaryScanState = DICT_SCAN_STATE_NONE;
                Params->StringIndex += 1;   // Move past end curly bracket
            }
            else
            {
                jlStatus = JL_STATUS_INVALID_DATA;
            }
        }

        if(     JL_STATUS_SUCCESS != jlStatus
            &&  NULL != currentStack->CurrentKeyName )
        {
            JlFree( currentStack->CurrentKeyName );
            currentStack->CurrentKeyName = NULL;
        }

        currentStack->ProcessedFirstItem = true;

        if(     JL_STATUS_SUCCESS != jlStatus
            &&  NULL != pErrorAtPos
            &&  0 == *pErrorAtPos )
        {
            *pErrorAtPos = Params->StringIndex;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  AttachStackObjectToPreviousObject
//
//  Attaches the latest object in the stack to the previous object if the previous one is a list or dictionary.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    AttachStackObjectToPreviousObject
    (
        ParseStack*     Stack,
        int32_t         StackIndex,
        size_t*         pErrorAtPos
    )
{
    JL_STATUS jlStatus;

    if( StackIndex > 0 )
    {
        ParseStack* currentStackItem = &Stack[StackIndex];
        ParseStack* prevStackItem = &Stack[StackIndex-1];

        if( JL_DATA_TYPE_LIST == prevStackItem->Type )
        {
            jlStatus = JlAttachObjectToListObject( prevStackItem->Object, currentStackItem->Object );
        }
        else if( JL_DATA_TYPE_DICTIONARY == prevStackItem->Type )
        {
            jlStatus = JlAttachObjectToDictionaryObject( prevStackItem->Object, prevStackItem->CurrentKeyName, currentStackItem->Object );
            if(     JL_STATUS_SUCCESS != jlStatus
                &&  NULL != pErrorAtPos )
            {
                *pErrorAtPos = prevStackItem->CurrentKeyStringIndex;
            }
            JlFree( prevStackItem->CurrentKeyName );
            prevStackItem->CurrentKeyName = NULL;
        }
        else
        {
            // nothing to attach it to. This occurs when the root object in the json is not a dictionary or list
            jlStatus = JL_STATUS_SUCCESS;
        }
    }
    else
    {
        // nothing to attach it to. This occurs when the root object in the json is not a dictionary or list
        jlStatus = JL_STATUS_SUCCESS;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlParseJson
//
//  Parses JSON 5 in a string and returns a JlDataObject representing it.
//  If an error occurs (other than JL_STATUS_INVALID_PARAMETER) then *pErrorAtPos will be set with the position
//  within JsonString where the error occurred. pErrorAtPos is an OPTIONAL parameter
//  If string JSON 1 parsing is required use JlParseJsonEx
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlParseJson
    (
        char const*     JsonString,
        JlDataObject**  pRootObject,
        size_t*         pErrorAtPos
    )
{
    return JlParseJsonEx( JsonString, true, pRootObject, pErrorAtPos );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlParseJsonEx
//
//  Parses JSON in a string and returns a JlDataObject representing it.
//  If an error occurs (other than JL_STATUS_INVALID_PARAMETER) then *pErrorAtPos will be set with the position
//  within JsonString where the error occurred. pErrorAtPos is an OPTIONAL parameter.
//  If IsJson5 is true then JSON 5 parsing will be done, otherwise strict JSON 1 parsing.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlParseJsonEx
    (
        char const*     JsonString,
        bool            IsJson5,
        JlDataObject**  pRootObject,
        size_t*         pErrorAtPos
    )
{
    JL_STATUS jlStatus;

    if(     NULL != JsonString
        &&  NULL != pRootObject )
    {
        size_t prevStringIndex = 0;
        ParseParameters params = { 0 };
        params.JsonString = JsonString;
        params.JsonStringLength = strlen( JsonString );
        params.StringIndex = 0;
        params.IsJson5 = IsJson5;
        // Stack is allocated 2 more because we don't use the first element and we need an extra one to contain the last list
        // or dictionary nest levels items.
        params.Stack = JlAlloc( sizeof(ParseStack) * (MAX_JSON_DEPTH+2) );

        if( NULL != pErrorAtPos )
        {
            *pErrorAtPos = 0;
        }

        if( NULL != params.Stack )
        {
            // Setup first item in stack as the "none" type (for whitespace). When this is popped off we
            // will have finished.
            params.StackIndex = 0;
            params.Stack[0].Type = JL_DATA_TYPE_NONE;
            params.Stack[0].ScanForSubObjects = true;

            jlStatus = JL_STATUS_SUCCESS;
            while( params.StackIndex >= 0  &&  JL_STATUS_SUCCESS == jlStatus )
            {
                ParseStack* stackItem = &params.Stack[params.StackIndex];

                if( ! stackItem->FinishedProcessing )
                {
                    prevStringIndex = params.StringIndex;
                    if( params.Stack[params.StackIndex].ScanForSubObjects )
                    {
                        if( JL_DATA_TYPE_NONE == params.Stack[params.StackIndex].Type )
                        {
                            // Note we must mark it finished first because ParseForNewType will adjust StackIndex
                            stackItem->FinishedProcessing = true;
                            stackItem->ScanForSubObjects = false;
                        }
                        // Turn off scanning for sub objects as we will need to go back to list or dictionary
                        // processing between objects
                        stackItem->ScanForSubObjects = false;

                        jlStatus = ParseForNewType( &params, IsJson5, pErrorAtPos );
                    }
                    else if( JL_DATA_TYPE_STRING == stackItem->Type )
                    {
                        jlStatus = ParseForString( &params, pErrorAtPos );
                    }
                    else if( JL_DATA_TYPE_NUMBER == stackItem->Type )
                    {
                        jlStatus = ParseForNumber( &params );
                    }
                    else if( JL_DATA_TYPE_BOOL == stackItem->Type )
                    {
                        jlStatus = ParseForBool( &params );
                    }
                    else if( JL_DATA_TYPE_LIST == stackItem->Type )
                    {
                        jlStatus = ParseForList( &params, pErrorAtPos );
                    }
                    else if( JL_DATA_TYPE_DICTIONARY == stackItem->Type )
                    {
                        jlStatus = ParseForDictionary( &params, pErrorAtPos );
                    }

                }
                else
                {
                    // Attempt to attach this object to previous level (if its a list or dictionary)
                    jlStatus = AttachStackObjectToPreviousObject( params.Stack, params.StackIndex, pErrorAtPos );
                    // Finished with this level. So pop off the stack
                    params.StackIndex -= 1;
                }
            }

            if( JL_STATUS_SUCCESS == jlStatus )
            {
                // Verify that there is nothing but white space after the final object.
                jlStatus = VerifyOnlyTrailingWhiteSpace( JsonString + params.StringIndex, IsJson5 );
                if( JL_STATUS_SUCCESS != jlStatus )
                {
                    // At this point we have a full object tree created, but the params are now
                    // reset. So modify index so it will remove the full tree (at index 1)
                    params.StackIndex = 1;

                    if( NULL != pErrorAtPos )
                    {
                        *pErrorAtPos = params.StringIndex;
                    }
                }
            }

            if( JL_STATUS_SUCCESS == jlStatus )
            {
                // Get root object (in index position 1)
                *pRootObject = params.Stack[1].Object;
            }
            else
            {
                // Free object tree created so far
                for( int32_t i=params.StackIndex; i>0; i-- )
                {
                    (void) JlFreeObjectTree( &params.Stack[i].Object );
                    if( NULL != params.Stack[i].CurrentKeyName )
                    {
                        JlFree( params.Stack[i].CurrentKeyName );
                        params.Stack[i].CurrentKeyName = NULL;
                    }
                }
            }

            JlFree( params.Stack );
        }
        else
        {
            jlStatus = JL_STATUS_OUT_OF_MEMORY;
        }

        if( JL_STATUS_SUCCESS != jlStatus )
        {
            // If optional parameter was provided, then set the position of the error in the string.
            if(     NULL != pErrorAtPos
                &&  0 == *pErrorAtPos )
            {
                *pErrorAtPos = prevStringIndex;
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}
