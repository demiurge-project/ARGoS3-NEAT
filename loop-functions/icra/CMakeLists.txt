add_library(icra_stop_loopfunc SHARED IcraStopLoopFunc.h IcraStopLoopFunc.cpp)
target_link_libraries(icra_stop_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)




add_library(icra_agg_loopfunc SHARED IcraAggLoopFunc.h IcraAggLoopFunc.cpp)
target_link_libraries(icra_agg_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(icra_agg_clas_loopfunc SHARED IcraAggClasLoopFunc.h IcraAggClasLoopFunc.cpp)
target_link_libraries(icra_agg_clas_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(icra_agg_ina_loopfunc SHARED IcraAggInaLoopFunc.h IcraAggInaLoopFunc.cpp)
target_link_libraries(icra_agg_ina_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(icra_agg_inb_loopfunc SHARED IcraAggInbLoopFunc.h IcraAggInbLoopFunc.cpp)
target_link_libraries(icra_agg_inb_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)




add_library(icra_des_loopfunc SHARED IcraDesLoopFunc.h IcraDesLoopFunc.cpp)
target_link_libraries(icra_des_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(icra_des_clas_loopfunc SHARED IcraDesClasLoopFunc.h IcraDesClasLoopFunc.cpp)
target_link_libraries(icra_des_clas_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(icra_des_ina_loopfunc SHARED IcraDesInaLoopFunc.h IcraDesInaLoopFunc.cpp)
target_link_libraries(icra_des_ina_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(icra_des_inb_loopfunc SHARED IcraDesInbLoopFunc.h IcraDesInbLoopFunc.cpp)
target_link_libraries(icra_des_inb_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)


