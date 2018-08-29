add_library(pw_loopfunc SHARED PwLoopFunc.h PwLoopFunc.cpp)
target_link_libraries(pw_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

