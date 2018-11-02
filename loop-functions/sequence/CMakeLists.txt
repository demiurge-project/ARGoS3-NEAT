add_library(seq_loopfunc SHARED SeqLoopFunc.h SeqLoopFunc.cpp)
target_link_libraries(seq_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)



