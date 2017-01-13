set(bl_epuck_loop_functions_HEADERS
  CEpuckSPCLoopFunction.h
)

set(bl_epuck_loop_functions_IMPLEMENTATIONS
  CEpuckSPCLoopFunction.cpp
)

#if(ARGOS_COMPILE_REAL_EPUCK)
 
#else(ARGOS_COMPILE_REAL_EPUCK)

	add_library(spc_loop_functions SHARED ${bl_epuck_loop_functions_IMPLEMENTATIONS})
	target_link_libraries(spc_loop_functions neat_loop_function)

#endif(ARGOS_COMPILE_REAL_EPUCK)
	



