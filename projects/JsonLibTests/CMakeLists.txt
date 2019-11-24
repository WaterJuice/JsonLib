# Unit Tests for JsonLib
add_executable( JsonLibTests
    Source/JsonLibTests.c
    Source/JsonLibTests_Parsing.c
    Source/JsonLibTests_Output.c
    Source/JsonLibTests_Unmarshall.c
    Source/JsonLibTests_Marshall.c
    Source/JsonLibTests_RoundTrip.c
    Source/JsonLibTests_Base64.c
    Source/JsonLibTests.h
    ../../JsonLibConfig.h )
target_link_libraries( JsonLibTests
    JsonLib
    WjTestLib)

install(TARGETS JsonLibTests DESTINATION .)
