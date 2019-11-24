add_executable( JsonRewrite Source/JsonRewrite.c)
target_link_libraries( JsonRewrite JsonLib WjTestLib)

install(TARGETS JsonRewrite DESTINATION .)
