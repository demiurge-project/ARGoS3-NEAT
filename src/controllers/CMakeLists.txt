if(ARGOS_BUILD_FOR_SIMULATOR)

    ############
    # Evostick #
    ############

    # Add shared library epuck_nn
    # shared libraries are linked dynamically and loaded at runtime
    add_library(nn_rm_1dot1 SHARED
      NeuralNetworkRM1Dot1.cpp
      NeuralNetworkRM1Dot1.h
    )

    # Link the library epuck_nn with the necessary libraries
    target_link_libraries(nn_rm_1dot1
      neat_controller
      argos3core_${ARGOS_BUILD_FOR}
      argos3plugin_${ARGOS_BUILD_FOR}_epuck
      argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)

    # Add shared library epuck_nn
    # shared libraries are linked dynamically and loaded at runtime
    add_library(nn_rm_1dot2 SHARED
      NeuralNetworkRM1Dot2.cpp
      NeuralNetworkRM1Dot2.h
    )

    # Link the library epuck_nn with the necessary libraries
    target_link_libraries(nn_rm_1dot2
      neat_controller
      argos3core_${ARGOS_BUILD_FOR}
      argos3plugin_${ARGOS_BUILD_FOR}_epuck
      argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)

    # Add shared library epuck_nn
    # shared libraries are linked dynamically and loaded at runtime
    #add_library(nn_rm_1dot3 SHARED
    #  NeuralNetworkRM1Dot3.cpp
    #  NeuralNetworkRM1Dot3.h
    #)

    # Link the library epuck_nn with the necessary libraries
    #target_link_libraries(nn_rm_1dot3
    #  neat_controller
    #  argos3core_${ARGOS_BUILD_FOR}
    #  argos3plugin_${ARGOS_BUILD_FOR}_epuck
    #  argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)
    
    # Add shared library epuck_nn
    # shared libraries are linked dynamically and loaded at runtime
    add_library(nn_rm_3dot0 SHARED
      NeuralNetworkRM3Dot0.cpp
      NeuralNetworkRM3Dot0.h
    )

    # Link the library epuck_nn with the necessary libraries
    target_link_libraries(nn_rm_3dot0
      neat_controller
      argos3core_${ARGOS_BUILD_FOR}
      argos3plugin_${ARGOS_BUILD_FOR}_epuck
      argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)


    #########
    # TMaze #
    #########

    #add_library(tmaze_controller SHARED
    #  TMaze.cpp
    #  TMaze.h
    #)

    #target_link_libraries(tmaze_controller
    #  neat_controller
    #  argos3core_${ARGOS_BUILD_FOR}
    #  argos3plugin_${ARGOS_BUILD_FOR}_epuck
    #  argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)


    ######################
    # Vectorial Evostick #
    ######################

    # Add shared library epuck_nn
    # shared libraries are linked dynamically and loaded at runtime
    #add_library(vectorial_evostick_controller SHARED
    #  VectorialEvostick.cpp
    #  VectorialEvostick.h
    #)

    # Link the library epuck_nn with the necessary libraries
    #target_link_libraries(vectorial_evostick_controller
    #  neat_controller
    #  argos3core_${ARGOS_BUILD_FOR}
    #  argos3plugin_${ARGOS_BUILD_FOR}_epuck
    #  argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)


    ###################
    # Test Controller #
    ###################

    #add_library(test_controller SHARED
    #  TestController.cpp
    #  TestController.h
    #)

    #target_link_libraries(test_controller
    #  argos3core_${ARGOS_BUILD_FOR}
    #  argos3plugin_${ARGOS_BUILD_FOR}_epuck
    #  argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)


else()

    add_executable(epuck_nn
        #/home/arena/dgarzon/iridia-tracking-system/src/plugins/robots/e-puck/real_robot/real_epuck_its.h
        #/home/arena/dgarzon/iridia-tracking-system/src/plugins/robots/e-puck/real_robot/real_epuck_its_main.cpp
        NeuralNetworkRM3Dot0.cpp
        NeuralNetworkRM3Dot0.h
    )

    # Link with the necessary libraries
    target_link_libraries(epuck_nn
      neat_controller
      argos3plugin_${ARGOS_BUILD_FOR}_epuck
      argos3_demiurge_epuck_dao
      argos3plugin_${ARGOS_BUILD_FOR}_genericvirtualsensorsandactuators)
      
    # Add exec for running on epuck
    #add_executable(nn_rm_1dot3
    #  NeuralNetworkRM1Dot3.cpp
    #  NeuralNetworkRM1Dot3.h
    #)

    # Link with the necessary libraries
    #target_link_libraries(nn_rm_1dot3
    #  neat_controller
    #  argos3plugin_${ARGOS_BUILD_FOR}_epuck
    #  argos3_demiurge_epuck_dao)

endif()
