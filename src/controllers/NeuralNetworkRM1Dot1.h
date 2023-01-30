#ifndef EVOSTICK_CONTROLLER
#define EVOSTICK_CONTROLLER

#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/vector2.h>
#include <argos3/core/utility/math/rng.h>

#include <argos3/plugins/robots/rvr/control_interface/ci_rvr_wheels_actuator.h>
#include <argos3/plugins/robots/rvr/control_interface/ci_rvr_proximity_sensor.h>
#include <argos3/plugins/robots/rvr/control_interface/ci_rvr_light_sensor.h>
#include <argos3/plugins/robots/rvr/control_interface/ci_rvr_ground_color_sensor.h>
#include <argos3/plugins/robots/rvr/control_interface/ci_rvr_lidar_sensor.h>
#include <argos3/plugins/robots/rvr/control_interface/ci_rvr_colored_blob_omnidirectional_camera_sensor.h>

// Reference Model RM1.1
#include <argos3/demiurge/rvr-dao/ReferenceModel1Dot1.h>

#include "../NEATController.h"

#include <map>

using namespace argos;

class NeuralNetworkRM1Dot1 : public CRVRNEATController {

public:

   NeuralNetworkRM1Dot1();
   virtual ~NeuralNetworkRM1Dot1();

   virtual void Init(TConfigurationNode& t_node);
   virtual void ControlStep();
   virtual void Reset();
   virtual void Destroy();

private:

   Real m_inputs[25];

};

#endif
