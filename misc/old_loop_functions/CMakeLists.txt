# Add the header files to the search path
include_directories(${CMAKE_SOURCE_DIR})

# Set libraries search path (needed for the target_link_libraries)
link_directories(
	${CMAKE_BINARY_DIR}/NEAT
	${CMAKE_BINARY_DIR}/controllers/epuck_nn
)

# Add the shared library neat_loop_function
# Note: shared libraries are linked dynamically and loaded at runtime
add_library(neat_loop_function SHARED neat_loop_function.cpp)

# Link the library neat_loop_function with the necessary libraries
target_link_libraries(neat_loop_function
  neat
  epuck_nn
  argos3core_simulator
  argos3plugin_simulator_dynamics2d
  argos3plugin_simulator_entities
  argos3plugin_simulator_epuck
  argos3plugin_simulator_genericrobot
  argos3plugin_simulator_media)



####################################
## Loop Functions for Experiments ##
####################################

# For the aggregation with ambient cues:
add_subdirectory(aac)

# For the largest covering network
# add_subdirectory(lcn)

# For the shelter with constrained access
# add_subdirectory(sca)

# For the the coverage with forbidden areas
# add_subdirectory(cfa)

# For the surface and perimeter coverage
# add_subdirectory(spc)



###########################
## Other Loop Functions  ##
###########################

add_subdirectory(other)
