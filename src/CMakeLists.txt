#
if(ARGOS_BUILD_FOR_SIMULATOR)

add_subdirectory(controllers)
add_subdirectory(loop-functions)
add_subdirectory(NEAT)
add_subdirectory(program)
add_subdirectory(genome_parser)

#else build for robots
else()
add_subdirectory(controllers)
add_subdirectory(NEAT)
endif()
