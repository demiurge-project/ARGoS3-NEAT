add_library(stop_mtc_loopfunc SHARED StopMtcLoopFunc.h StopMtcLoopFunc.cpp)
target_link_libraries(stop_mtc_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(syn_mtc_loopfunc SHARED SynMtcLoopFunc.h SynMtcLoopFunc.cpp)
target_link_libraries(syn_mtc_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(forag_mtc_loopfunc SHARED ForagMtcLoopFunc.h ForagMtcLoopFunc.cpp)
target_link_libraries(forag_mtc_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)


add_library(stop_trn_loopfunc SHARED StopTrnLoopFunc.h StopTrnLoopFunc.cpp)
target_link_libraries(stop_trn_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(syn_trn_loopfunc SHARED SynTrnLoopFunc.h SynTrnLoopFunc.cpp)
target_link_libraries(syn_trn_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(forag_trn_loopfunc SHARED ForagTrnLoopFunc.h ForagTrnLoopFunc.cpp)
target_link_libraries(forag_trn_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)



