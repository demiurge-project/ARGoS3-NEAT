#include "TMaze.h"
#include <argos3/core/utility/logging/argos_log.h>

/****************************************/
/****************************************/

static CRange<Real> NN_OUTPUT_RANGE(0.0f, 1.0f);
static CRange<Real> WHEEL_ACTUATION_RANGE(-16.0f, 16.0f);

/****************************************/
/************* CONSTRUCTOR **************/
/****************************************/

TMazeController::TMazeController() {
    m_pcWheels = NULL;
    m_pcProximity = NULL;
    m_pcLight = NULL;
    m_net = NULL;
    m_nId = -1;
    m_fMaxVelocity = 16.0;
    m_bTrial = false;
    time_t t;
    /* Intializes random number generator */
    srand((unsigned) time(&t));
    argos::CRandom::CreateCategory("epuckNNController", rand());
    m_pcRNG = argos::CRandom::CreateRNG("epuckNNController");
}

/****************************************/
/************** DESTRUCTOR **************/
/****************************************/

TMazeController::~TMazeController() {
   if(m_bTrial) {
     delete m_net;
   }
}

/****************************************/
/**************** INIT ******************/
/****************************************/

void TMazeController::Init(TConfigurationNode& t_node) {

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

   // Load the parameters for the neural network.
   GetNodeAttributeOrDefault(t_node, "genome_file", m_strFile, m_strFile);
   if(m_strFile != "") {
      try{
         LoadNetwork(m_strFile);
      } catch(CARGoSException& ex) {
         THROW_ARGOSEXCEPTION_NESTED("cannot load parameters from the genome file.", ex);
      }
   }

   WHEEL_ACTUATION_RANGE.Set(-m_fMaxVelocity, m_fMaxVelocity);
}

/****************************************/
/*************** CONTROL ****************/
/****************************************/

void TMazeController::ControlStep() {
   // Get Proximity sensory data from sensors 0, 1, and 2 (left hand side of epuck) and 5, 6, and 7 (right hand side of epuck).
   if(m_pcProximity != NULL) {
      const CCI_EPuckProximitySensor::TReadings& tProx = m_pcProximity->GetReadings();
      // Sensors 0, 1, 2.
      for(size_t i=0; i<3; i++) {
         m_inputs[i] = tProx[i].Value;
      }
      // Sensors 5, 6, 7.
      for(size_t i=5; i<8; i++) {
         m_inputs[i-2] = tProx[i].Value;
      }
   } else {
      for(size_t i=0; i<6; i++) {
         m_inputs[i] = 0;
      }
   }

   // Get Light sensory data from sensors 2 (left hand side of epuck) and 5 (right hand side of epuck)
   if(m_pcLight != NULL) {
      const CCI_EPuckLightSensor::TReadings& tLight = m_pcLight->GetReadings();
      m_inputs[6] = tLight[2].Value;
      m_inputs[7] = tLight[5].Value;
   } else {
     m_inputs[6] = 0;
     m_inputs[7] = 0;
   }

   // Feed the network with those inputs
   m_net->load_sensors((double*)m_inputs);
   // Transmit the signals to the next layer.
   m_net->activate();

   // Apply NN outputs to actuation. The NN outputs are in the range [0,1], we remap this range into [-5:5] linearly.
   NN_OUTPUT_RANGE.MapValueIntoRange(m_fLeftSpeed, (m_net->outputs[0])->activation, WHEEL_ACTUATION_RANGE);
   NN_OUTPUT_RANGE.MapValueIntoRange(m_fRightSpeed, (m_net->outputs[1])->activation, WHEEL_ACTUATION_RANGE);

   if(m_pcWheels != NULL) {
      m_pcWheels->SetLinearVelocity(m_fLeftSpeed, m_fRightSpeed);
   }
}

/****************************************/
/****************************************/

void TMazeController::Reset() {
}

/****************************************/
/****************************************/

void TMazeController::Destroy() {
  if(CRandom::ExistsCategory("epuckNNController")) {
     CRandom::RemoveCategory("epuckNNController");
  }
}

REGISTER_CONTROLLER(TMazeController, "tmaze_controller")
