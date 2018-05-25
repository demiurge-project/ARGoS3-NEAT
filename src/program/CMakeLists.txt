# Add the header files to the search path
include_directories(${CMAKE_SOURCE_DIR})

# Set libraries search path (needed for the target_link_libraries)
link_directories(
	${CMAKE_BINARY_DIR}/src/NEAT
	${CMAKE_BINARY_DIR}/src
)

# Find the package MPI
# To force to use mpich, uncomment the following lines and modify them with the correct path.
#SET(MPI_C_COMPILER "/opt/mpich2/gnu/bin/mpicc")
#SET(MPI_CXX_COMPILER "/opt/mpich2/gnu/bin/mpic++")
# To force to use openmpi, uncomment the following lines and modify them with the correct path.
#SET(MPI_C_COMPILER "/opt/openmpi/bin/mpicc)
#SET(MPI_CXX_COMPILER "opt/openmpi/bin/mpic++)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(NEAT-launch launch.cpp)
target_link_libraries(NEAT-launch neat neat_controller
					argos3core_${ARGOS_BUILD_FOR}
					argos3plugin_${ARGOS_BUILD_FOR}_epuck
					argos3_demiurge_loop_functions
					argos3_demiurge_epuck_dao)

find_package(MPI)
if(MPI_FOUND)

	# Message to print in the terminal
	message(STATUS "MPI package found!")

	# Add the header files to the search path
	include_directories(${MPI_INCLUDE_PATH})

	# Add the executable NEAT-evolution
	add_executable(NEAT-evolution evolution.cpp)
	# Link the executable evolution with the necessary libraries
	target_link_libraries(NEAT-evolution neat neat_controller ${MPI_LIBRARIES}
						argos3core_${ARGOS_BUILD_FOR}
						argos3plugin_${ARGOS_BUILD_FOR}_epuck
						argos3_demiurge_loop_functions
						argos3_demiurge_epuck_dao)

	# Add the executable scheduler
	add_executable(scheduler scheduler.cpp)
	target_link_libraries(scheduler neat neat_controller ${MPI_LIBRARIES}
						argos3core_${ARGOS_BUILD_FOR}
						argos3_demiurge_loop_functions
						argos3_demiurge_epuck_dao)

else(MPI_FOUND)

	# Message to print in the terminal
	message(STATUS "MPI package NOT found!")

	# Add the executable evolution
	add_executable(NEAT-evolution-seq evolution-sequential.cpp
						argos3_demiurge_loop_functions
						argos3_demiurge_epuck_dao)

	# Link the executable evolution with the necessary libraries
	target_link_libraries(NEAT-evolution-seq neat neat_controller
						argos3core_${ARGOS_BUILD_FOR}
						argos3plugin_${ARGOS_BUILD_FOR}_epuck
						argos3_demiurge_loop_functions
						argos3_demiurge_epuck_dao)

endif(MPI_FOUND)
