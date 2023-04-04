if(ARGOS_BUILD_FOR_SIMULATOR)

    ############
    # Evostick #
    ############

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


else()


endif()
