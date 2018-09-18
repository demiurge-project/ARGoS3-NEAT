#include "TestController.h"
#include <argos3/core/utility/logging/argos_log.h>

/****************************************/
/****************************************/

static CRange<Real> NN_OUTPUT_RANGE(0.0f, 1.0f);
static CRange<Real> WHEEL_ACTUATION_RANGE(-16.0f, 16.0f);

/****************************************/
/************* CONSTRUCTOR **************/
/****************************************/

TestController::TestController() {
    m_pcWheels = NULL;
    m_pcProximity = NULL;
    m_pcLight = NULL;
    m_unTurningSteps = 0;
    m_unState = 0; // Straight
    time_t t;
    /* Intializes random number generator */
    srand((unsigned) time(&t));
    argos::CRandom::CreateCategory("epuckNNController", rand());
    m_pcRNG = argos::CRandom::CreateRNG("epuckNNController");
}

/****************************************/
/************** DESTRUCTOR **************/
/****************************************/

TestController::~TestController() {
}

/****************************************/
/**************** INIT ******************/
/****************************************/

void TestController::Init(TConfigurationNode& t_node) {

   /* Get sensor/actuator handles */
   try {
      m_pcWheels = GetActuator<CCI_EPuckWheelsActuator>("epuck_wheels");
   } catch(CARGoSException& ex) {}

   try {
      m_pcProximity = GetSensor<CCI_EPuckProximitySensor>("epuck_proximity");
   } catch(CARGoSException& ex) {}

   try {
      m_pcLight = GetSensor<CCI_EPuckLightSensor>("epuck_light");
   } catch(CARGoSException& ex) {}
}

/****************************************/
/*************** CONTROL ****************/
/****************************************/

void TestController::ControlStep() {
   // Get Proximity sensory data from sensors 0, 1, and 2 (left hand side of epuck) and 5, 6, and 7 (right hand side of epuck).
   if(m_pcProximity != NULL) {
     const CCI_EPuckProximitySensor::TReadings& tProx = m_pcProximity->GetReadings();
     //LOG << "Proxi " << tProx[0].Value << " " << tProx[7].Value << std::endl;
     if ((tProx[0].Value > 0.1 || tProx[7].Value > 0.1) && (m_unState != 1)) {
       m_unState = 1; //Turn
       m_unTurningSteps = 2;
     }
  }

   // Get Light sensory data from sensors 2 (left hand side of epuck) and 5 (right hand side of epuck)
   if(m_pcLight != NULL) {
     const CCI_EPuckLightSensor::TReadings& tLight = m_pcLight->GetReadings();
     LOG << "Light " << tLight[2].Value << " " << tLight[5].Value << std::endl;
   }



   // Go straight
   if (m_unState == 0) {
     m_fLeftSpeed = 16.0;
     m_fRightSpeed = 16.0;
   }

   // Turn
   if ((m_unState == 1) && (m_unTurningSteps > 0)) {
     m_fLeftSpeed = 14.0;
     m_fRightSpeed = -14.0;
     m_unTurningSteps -= 1;
   } else {
     m_unState = 0;
   }

   if(m_pcWheels != NULL) {
      m_pcWheels->SetLinearVelocity(m_fLeftSpeed, m_fRightSpeed);
   }
}

/****************************************/
/****************************************/

void TestController::Reset() {
}

/****************************************/
/****************************************/

void TestController::Destroy() {
}

REGISTER_CONTROLLER(TestController, "test_controller")
