set( PUBLIC_FILES
    Include/WjTestLib.h )
set( PRIVATE_FILES
    Source/WjTestLib.c
    Source/WjTestLib_Asserts.c
    Source/WjTestLib_Asserts.h
    Source/WjTestLib_Memory.c
    Source/WjTestLib_Memory.h )

set( INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Include )

add_library( WjTestLib STATIC ${PUBLIC_FILES} ${PRIVATE_FILES} )
target_include_directories( WjTestLib PUBLIC ${INC_DIR} )
set_target_properties ( WjTestLib PROPERTIES FOLDER lib )
source_group( Include FILES ${PUBLIC_FILES} )
source_group( Source FILES ${PRIVATE_FILES} )
