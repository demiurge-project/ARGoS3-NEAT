if(ARGOS_BUILD_FOR_SIMULATOR)

    ############
    # Evostick #
    ############

    # Add shared library rvr_nn
    # shared libraries are linked dynamically and loaded at runtime
    add_library(nn_rm_1dot1 SHARED
      NeuralNetworkRM1Dot1.cpp
      NeuralNetworkRM1Dot1.h
    )

    # Link the library rvr_nn with the necessary libraries
    target_link_libraries(nn_rm_1dot1
      neat_controller
      argos3core_${ARGOS_BUILD_FOR}
      argos3plugin_${ARGOS_BUILD_FOR}_rvr
      argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)


      add_library(nn_rm_1dot1_bin SHARED
        NeuralNetworkRM1Dot1Binary.cpp
        NeuralNetworkRM1Dot1Binary.h
      )

      # Link the library rvr_nn with the necessary libraries
      target_link_libraries(nn_rm_1dot1_bin
        neat_controller
        argos3core_${ARGOS_BUILD_FOR}
        argos3plugin_${ARGOS_BUILD_FOR}_rvr
        argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)


    # Add shared library rvr_nn
    # shared libraries are linked dynamically and loaded at runtime
    add_library(nn_rm_1dot2 SHARED
      NeuralNetworkRM1Dot2.cpp
      NeuralNetworkRM1Dot2.h
    )

    # Link the library rvr_nn with the necessary libraries
    target_link_libraries(nn_rm_1dot2
      neat_controller
      argos3core_${ARGOS_BUILD_FOR}
      argos3plugin_${ARGOS_BUILD_FOR}_rvr
      argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)

      # Add shared library rvr_nn
      # shared libraries are linked dynamically and loaded at runtime
      add_library(nn_rm_1dot2_bin SHARED
        NeuralNetworkRM1Dot2Binary.cpp
        NeuralNetworkRM1Dot2Binary.h
      )

      # Link the library rvr_nn with the necessary libraries
      target_link_libraries(nn_rm_1dot2_bin
        neat_controller
        argos3core_${ARGOS_BUILD_FOR}
        argos3plugin_${ARGOS_BUILD_FOR}_rvr
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
    #  argos3plugin_${ARGOS_BUILD_FOR}_rvr
    #  argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)


    ######################
    # Vectorial Evostick #
    ######################

    # Add shared library rvr_nn
    # shared libraries are linked dynamically and loaded at runtime
    #add_library(vectorial_evostick_controller SHARED
    #  VectorialEvostick.cpp
    #  VectorialEvostick.h
    #)

    # Link the library rvr_nn with the necessary libraries
    #target_link_libraries(vectorial_evostick_controller
    #  neat_controller
    #  argos3core_${ARGOS_BUILD_FOR}
    #  argos3plugin_${ARGOS_BUILD_FOR}_rvr
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
    #  argos3plugin_${ARGOS_BUILD_FOR}_rvr
    #  argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)


else()


endif()
