#ifndef EVOSTICK_CONTROLLER_RM_1DO1_BINARY
#define EVOSTICK_CONTROLLER_RM_1DO1_BINARY

#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/vector2.h>
#include <argos3/core/utility/math/rng.h>

#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_wheels_actuator.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_range_and_bearing_actuator.h>

#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_proximity_sensor.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_light_sensor.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_ground_sensor.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_range_and_bearing_sensor.h>

// Reference Model RM1.1
#include <argos3/demiurge/epuck-dao/ReferenceModel1Dot1.h>

#include "../NEATController.h"

#include <map>

using namespace argos;

class NeuralNetworkRM1Dot1Binary : public CEPuckNEATController {

public:

   NeuralNetworkRM1Dot1Binary();
   virtual ~NeuralNetworkRM1Dot1Binary();

   virtual void Init(TConfigurationNode& t_node);
   virtual void ControlStep();
   virtual void Reset();
   virtual void Destroy();

   std::string GetState();

private:

   Real m_inputs[25];
   void UpdateState();
   std::string m_strStatus;

};

#endif
