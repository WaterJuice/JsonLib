add_executable( JsonLibSample Source/JsonLibSample.c)
target_link_libraries( JsonLibSample JsonLib WjTestLib)

install(TARGETS JsonLibSample DESTINATION .)
