set(bl_epuck_loop_functions_HEADERS
  CEpuckAACLoopFunction.h
)

set(bl_epuck_loop_functions_IMPLEMENTATIONS
  CEpuckAACLoopFunction.cpp
)

#if(ARGOS_COMPILE_REAL_EPUCK)
 
#else(ARGOS_COMPILE_REAL_EPUCK)

	add_library(aac_loop_functions SHARED ${bl_epuck_loop_functions_IMPLEMENTATIONS})
	target_link_libraries(aac_loop_functions neat_loop_function)

#endif(ARGOS_COMPILE_REAL_EPUCK)
	



