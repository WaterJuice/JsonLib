# Examples

These are complete, self-contained programs. Only `JsonLib.h` needs to be included. Link against the
`JsonLib` static library.

## Round trip: JSON to struct and back

Parse a JSON document into a struct, modify it, and write it back out as indented JSON.

```c
#include <stdio.h>
#include "JsonLib.h"

typedef struct
{
    char*       Name;
    uint32_t    Age;
    bool        Active;
} Person;

static JlMarshallElement PersonMap[] =
{
    JlMarshallString(   Person, Name,   "name" ),
    JlMarshallUnsigned( Person, Age,    "age" ),
    JlMarshallBool(     Person, Active, "active" ),
};
static size_t const PersonMapCount = sizeof(PersonMap) / sizeof(PersonMap[0]);

int main( void )
{
    char const* json = "{ \"name\": \"Ada\", \"age\": 36, \"active\": true }";

    // Parse the JSON into the struct.
    Person person = {0};
    size_t errorPos = 0;
    JL_STATUS status = JlJsonToStruct( json, PersonMap, PersonMapCount, &person, &errorPos );
    if( JL_STATUS_SUCCESS != status )
    {
        printf( "Parse failed at position %zu (status %d)\n", errorPos, (int)status );
        return 1;
    }

    printf( "Name=%s Age=%u Active=%d\n", person.Name, person.Age, (int)person.Active );

    // Modify a field and write the struct back out as indented JSON.
    person.Age += 1;

    char* output = NULL;
    status = JlStructToJson( &person, PersonMap, PersonMapCount, true, &output );
    if( JL_STATUS_SUCCESS == status )
    {
        printf( "%s\n", output );
        JlFreeJsonStringBuffer( &output );
    }

    // Free everything the parse allocated inside the struct (here, person.Name).
    JlUnmarshallFreeStructAllocs( PersonMap, PersonMapCount, &person );
    return 0;
}
```

This prints:

```json
{
    "name": "Ada",
    "age": 37,
    "active": true
}
```

## Nested structs and arrays

A struct can contain another struct, allocated arrays of strings or numbers, and more. The input below
uses JSON5 (bare keywords and single quotes), which is accepted by default.

```c
#include <stdio.h>
#include "JsonLib.h"

typedef struct
{
    char*   Street;
    char*   City;
} Address;

typedef struct
{
    char*       Name;
    Address     Home;           // nested struct
    char**      Tags;           // allocated array of strings
    size_t      TagsCount;
    uint32_t*   Scores;         // allocated array of numbers
    size_t      ScoresCount;
} Profile;

static JlMarshallElement AddressMap[] =
{
    JlMarshallString( Address, Street, "street" ),
    JlMarshallString( Address, City,   "city" ),
};

static JlMarshallElement ProfileMap[] =
{
    JlMarshallString(        Profile, Name,   "name" ),
    JlMarshallStruct(        Profile, Home,   "home", AddressMap, sizeof(AddressMap) / sizeof(AddressMap[0]) ),
    JlMarshallStringArray(   Profile, Tags,   TagsCount,   "tags" ),
    JlMarshallUnsignedArray( Profile, Scores, ScoresCount, "scores" ),
};
static size_t const ProfileMapCount = sizeof(ProfileMap) / sizeof(ProfileMap[0]);

int main( void )
{
    char const* json =
        "{"
        "  name: 'Grace',"
        "  home: { street: '1 Navy Way', city: 'Arlington' },"
        "  tags: [ 'admin', 'ops' ],"
        "  scores: [ 10, 20, 30 ]"
        "}";

    Profile profile = {0};
    size_t errorPos = 0;
    JL_STATUS status = JlJsonToStruct( json, ProfileMap, ProfileMapCount, &profile, &errorPos );
    if( JL_STATUS_SUCCESS != status )
    {
        printf( "Parse failed at position %zu\n", errorPos );
        return 1;
    }

    printf( "%s lives in %s. %zu tags, %zu scores.\n",
            profile.Name, profile.Home.City, profile.TagsCount, profile.ScoresCount );

    JlUnmarshallFreeStructAllocs( ProfileMap, ProfileMapCount, &profile );
    return 0;
}
```

## Controlling the output format

`JlStructToJsonEx` takes a set of flags. Combine them to choose indentation, JSON5 features, and more.
For example, to produce compact JSON5 with bare keywords and single quotes:

```c
char* output = NULL;
JL_STATUS status = JlStructToJsonEx(
    &person, PersonMap, PersonMapCount,
    JL_OUTPUT_FLAGS_J5_USE_BARE_KEYWORDS | JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES,
    &output );
if( JL_STATUS_SUCCESS == status )
{
    printf( "%s\n", output );        // {name:'Ada',age:37,active:true}
    JlFreeJsonStringBuffer( &output );
}
```

See the [Functions](functions.md) page for the full list of output flags.
