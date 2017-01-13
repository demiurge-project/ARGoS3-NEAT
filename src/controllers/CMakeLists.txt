# Add the header files to the search path
include_directories(${CMAKE_SOURCE_DIR})

# Set libraries search path (needed for the target_link_libraries)
link_directories(
	${CMAKE_BINARY_DIR}/src/NEAT
	${ARGOS_LIBRARY_DIRS}
)

# Descend into the epuck_nn directory
add_subdirectory(epuck_nn)
