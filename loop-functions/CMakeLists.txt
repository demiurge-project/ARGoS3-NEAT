# Set up ARGoS compilation information
include_directories(${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS})
link_directories(${ARGOS_LIBRARY_DIRS})

# add_subdirectory(example)
# add_subdirectory(chocolate)
# add_subdirectory(foraging)
# add_subdirectory(colors)
# add_subdirectory(piecewise)
#add_subdirectory(icra)
add_subdirectory(adaptation)
