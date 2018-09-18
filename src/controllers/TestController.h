#ifndef TEST_CONTROLLER
#define TEST_CONTROLLER

#include <argos3/core/utility/math/rng.h>
#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/vector2.h>
#include <argos3/core/control_interface/ci_controller.h>


#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_wheels_actuator.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_range_and_bearing_actuator.h>

#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_proximity_sensor.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_light_sensor.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_ground_sensor.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_range_and_bearing_sensor.h>


#include <map>

using namespace argos;

class TestController: public CCI_Controller {

public:

   TestController();
   virtual ~TestController();

   virtual void Init(TConfigurationNode& t_node);
   virtual void ControlStep();
   virtual void Reset();
   virtual void Destroy();

private:
  CCI_EPuckProximitySensor* m_pcProximity;
  CCI_EPuckLightSensor* m_pcLight;

  CCI_EPuckWheelsActuator* m_pcWheels;

  argos::CRandom::CRNG* m_pcRNG;

  Real m_fLeftSpeed;
  Real m_fRightSpeed;


  UInt8 m_unState;
  UInt8 m_unTurningSteps; 
};

#endif
