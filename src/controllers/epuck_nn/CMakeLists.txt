if(ARGOS_BUILD_FOR_SIMULATOR)
    # Add shared library epuck_nn
    # shared libraries are linked dynamically and loaded at runtime
    add_library(epuck_nn SHARED
      epuck_nn_controller.cpp
      epuck_nn_controller.h
    )

    # Link the library epuck_nn with the necessary libraries
    target_link_libraries(epuck_nn
      neat
      argos3core_${ARGOS_BUILD_FOR}
      argos3plugin_${ARGOS_BUILD_FOR}_epuck
      argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)

    ####################################
    ##### Epuck: NN with 23 inputs #####
    ####################################

    # Add shared library epuck_nn_23
    add_library(epuck_nn_23 SHARED
      epuck_nn_controller23.cpp
      epuck_nn_controller23.h
    )

    # Link the library epuck_nn_23 with the necessary libraries
    target_link_libraries(epuck_nn_23
      neat
      argos3core_simulator
      argos3plugin_${ARGOS_BUILD_FOR}_epuck
      argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)

else()
    # Add exec for running on epuck
    add_executable(epuck_nn
        epuck_nn_controller.cpp
        epuck_nn_controller.h
    )

    # Link with the necessary libraries
    target_link_libraries(epuck_nn
      neat
      argos3plugin_${ARGOS_BUILD_FOR}_epuck)

    ####################################
    ##### Epuck: NN with 23 inputs #####
    ####################################

    # Add shared library epuck_nn_23
    add_executable(epuck_nn_23
      epuck_nn_controller23.cpp
      epuck_nn_controller23.h
    )

    # Link with the necessary libraries
    target_link_libraries(epuck_nn_23
      neat
      argos3plugin_${ARGOS_BUILD_FOR}_epuck)

endif()
