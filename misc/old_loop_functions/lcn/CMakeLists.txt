set(epuck_lcn_loop_functions_HEADERS
  CEpuckLCNLoopFunction.h
)

set(epuck_lcn_loop_functions_IMPLEMENTATIONS
  CEpuckLCNLoopFunction.cpp
)

#if(ARGOS_COMPILE_REAL_EPUCK)
 
#else(ARGOS_COMPILE_REAL_EPUCK)

	add_library(lcn_loop_functions SHARED ${epuck_lcn_loop_functions_IMPLEMENTATIONS})
	target_link_libraries(lcn_loop_functions neat_loop_function)

#endif(ARGOS_COMPILE_REAL_EPUCK)
	



