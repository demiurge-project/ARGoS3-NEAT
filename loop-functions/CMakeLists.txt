# Set up ARGoS compilation information
# Add the header files to the search path
include_directories(${CMAKE_SOURCE_DIR})

# Set libraries search path (needed for the target_link_libraries)
link_directories(
	${CMAKE_BINARY_DIR}/src/loop-functions
)

add_subdirectory(example)
add_subdirectory(chocolate)
