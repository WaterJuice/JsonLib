set( PUBLIC_FILES
    Include/JsonLib.h
    Include/JlMarshallTypes.h
    Include/JlDataModel.h
    Include/JlDataModelHelpers.h
    Include/JlStatus.h )
set( PRIVATE_FILES
    Source/JsonLib.c
    Source/JlDataModel.c
    Source/JlDataModelHelpers.c
    Source/JlMemory.c
    Source/JlMemory.h
    Source/JlOutputJson.c
    Source/JlBuffer.c
    Source/JlBuffer.h
    Source/JlParseJson.c
    Source/JlUnmarshall.c
    Source/JlMarshall.c
    Source/JlUnicode.c
    Source/JlUnicode.h
    Source/JlLinkedLists.h
    Source/JlBase64.c
    Source/JlBase64.h)

set( INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Include )

add_library( JsonLib STATIC ${PUBLIC_FILES} ${PRIVATE_FILES} )
target_include_directories( JsonLib PUBLIC ${INC_DIR} )
set_target_properties ( JsonLib PROPERTIES FOLDER lib )
source_group( Include FILES ${PUBLIC_FILES} )
source_group( Source FILES ${PRIVATE_FILES} )
