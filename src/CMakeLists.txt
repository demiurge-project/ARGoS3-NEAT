#

# Set up ARGoS compilation information
include_directories(${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS})
link_directories(${ARGOS_LIBRARY_DIRS})

# Headers
set(CONTROLLER_HEADERS
      NEATController.h)

# Sources
set(CONTROLLER_SOURCES
      NEATController.cpp)

add_library(neat_controller SHARED ${CONTROLLER_HEADERS} ${CONTROLLER_SOURCES} )
target_link_libraries(neat_controller neat argos3plugin_${ARGOS_BUILD_FOR}_epuck)

if(ARGOS_BUILD_FOR_SIMULATOR)
  add_subdirectory(controllers)
  add_subdirectory(NEAT)
  add_subdirectory(program)
  add_subdirectory(genome_parser)
else()                                  #else build for robots
  add_subdirectory(controllers)
  add_subdirectory(NEAT)
endif()
