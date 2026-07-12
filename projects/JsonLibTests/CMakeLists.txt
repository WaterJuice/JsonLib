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

# Register with ctest. The test binary now returns a non-zero exit code if any test fails.
add_test( NAME JsonLibTests COMMAND JsonLibTests )

install(TARGETS JsonLibTests DESTINATION .)
