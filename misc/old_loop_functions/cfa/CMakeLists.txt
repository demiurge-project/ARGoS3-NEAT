set(epuck_cfa_loop_functions_HEADERS
  CEpuckCFALoopFunction.h
)

set(epuck_cfa_loop_functions_loop_functions_IMPLEMENTATIONS
  CEpuckCFALoopFunction.cpp
)

#if(ARGOS_COMPILE_REAL_EPUCK)
 
#else(ARGOS_COMPILE_REAL_EPUCK)

	add_library(cfa_loop_functions SHARED ${epuck_cfa_loop_functions_loop_functions_IMPLEMENTATIONS})
	target_link_libraries(cfa_loop_functions neat_loop_function)

#endif(ARGOS_COMPILE_REAL_EPUCK)
	



