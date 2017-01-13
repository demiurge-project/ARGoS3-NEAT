# For the goToLight loop function.
add_library(goToLight_loop_function SHARED goToLight_loop_function.cpp)
target_link_libraries(goToLight_loop_function neat_loop_function)

# For the aggregation loop function.
#add_library(aggregation_loop_function SHARED aggregation_loop_function.cpp)
#target_link_libraries(aggregation_loop_function neat_loop_function)

# For the obstacleAvoidance loop function.
#add_library(obstacleAvoidance_loop_function SHARED obstacleAvoidance_loop_function.cpp)
#target_link_libraries(obstacleAvoidance_loop_function neat_loop_function)
	



