cmake_minimum_required(VERSION 3.6.0)

project( WaterJuice )

# Set JsonLib to use WjTestLib memory allocation for tracking. This is only required for the unit tesst
add_definitions( "-DJL_INCLUDE_H=\"${CMAKE_CURRENT_SOURCE_DIR}/JsonLibConfig.h\"")

# Add the project directories
add_subdirectory( lib/JsonLib )
add_subdirectory( lib/WjTestLib )
add_subdirectory( projects/JsonLibTests )
add_subdirectory( projects/JsonRewrite )
add_subdirectory( projects/JsonLibSample )
