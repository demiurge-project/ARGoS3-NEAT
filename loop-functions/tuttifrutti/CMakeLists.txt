add_library(tutti_st_loopfunc SHARED TuttiStLoopFunc.h TuttiStLoopFunc.cpp)
target_link_libraries(tutti_st_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(tutti_ag_loopfunc SHARED TuttiAgLoopFunc.h TuttiAgLoopFunc.cpp)
target_link_libraries(tutti_ag_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(tutti_fr_loopfunc SHARED TuttiFrLoopFunc.h TuttiFrLoopFunc.cpp)
target_link_libraries(tutti_fr_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)



