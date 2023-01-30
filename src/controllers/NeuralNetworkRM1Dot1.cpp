#include "NeuralNetworkRM1Dot1.h"
#include <argos3/core/utility/logging/argos_log.h>

/****************************************/
/************* CONSTRUCTOR **************/
/****************************************/

NeuralNetworkRM1Dot1::NeuralNetworkRM1Dot1() {
    m_pcWheels = NULL;
    m_pcProximity = NULL;
    m_pcLight = NULL;
    m_pcGroundColor = NULL;
    m_pcLidar = NULL;
    m_pcOmnidirectionalCamera = NULL;
    m_net = NULL;
    m_nId = -1;
    m_unTimeStep = 0;
    m_pcRNG = argos::CRandom::CreateRNG("argos");
}

/****************************************/
/************** DESTRUCTOR **************/
/****************************************/

NeuralNetworkRM1Dot1::~NeuralNetworkRM1Dot1() {
   delete m_pcRobotState;
}

/****************************************/
/**************** INIT ******************/
/****************************************/

void NeuralNetworkRM1Dot1::Init(TConfigurationNode& t_node) {
  CRVRNEATController::Init(t_node);

  /* Reference model */
  m_pcRobotState = new ReferenceModel1Dot1();
  m_pcRobotState->SetRobotIdentifier(getRobotId());
  m_cWheelActuationRange.Set(-m_pcRobotState->GetMaxVelocity(), m_pcRobotState->GetMaxVelocity());

  m_cNeuralNetworkOutputRange.Set(0.0f, 1.0f);

  // Activate the RAB actuator (send the robot's id)
  if(m_pcRABAct != NULL) {
    UInt8 data[2];
    data[0] = getRobotId();
    data[1] = 0;
    m_pcRABAct->SetData(data);
  }
}

/****************************************/
/*************** CONTROL ****************/
/****************************************/

void NeuralNetworkRM1Dot1::ControlStep() {
   // Get Proximity sensory data.
   if(m_pcProximity != NULL) {
      const CCI_RVRProximitySensor::TReadings& cProxiReadings = m_pcProximity->GetReadings();
      // Feed readings to RVRDAO which will process them as needed
      m_pcRobotState->SetProximityInput(cProxiReadings);
      // Collecting processed readings
      CCI_RVRProximitySensor::TReadings cProcessedProxiReadings = m_pcRobotState->GetProximityInput();
      // Injecting processed readings as input of the NN
      for(size_t i=0; i<8; i++) {
         m_inputs[i] = cProcessedProxiReadings[i].Value;
      }
   } else {
      for(size_t i=0; i<8; i++) {
         m_inputs[i] = 0;
      }
   }

   // Get Light sensory data.
   if(m_pcLight != NULL) {
      const CCI_RVRLightSensor::TReadings& cLightReadings = m_pcLight->GetReadings();
      m_pcRobotState->SetLightInput(cLightReadings);
      CCI_RVRLightSensor::TReadings cProcessedLightReadings = m_pcRobotState->GetLightInput();
      for(size_t i=8; i<16; i++) {
         m_inputs[i] = cProcessedLightReadings[i-8].Value;
      }
   } else {
      for(size_t i=8; i<16; i++) {
         m_inputs[i] = 0;
      }
   }

   // Get Ground sensory data.
   if(m_pcGround != NULL) {
      const CCI_RVRGroundSensor::SReadings& cGroundReadings = m_pcGround->GetReadings();
      m_pcRobotState->SetGroundInput(cGroundReadings);
      CCI_RVRGroundSensor::SReadings cProcessedGroundReadings = m_pcRobotState->GetGroundInput();
      for(size_t i=16; i<19; i++) {
         m_inputs[i] = cProcessedGroundReadings[i-16];
         if(cProcessedGroundReadings[i-16] <= 0.1) { //black
            m_inputs[i] = 0;
         } else if(cProcessedGroundReadings[i-16] >= 0.95){ //white
            m_inputs[i] = 1;
         } else { //gray
            UInt32 index = m_pcRNG->Uniform(CRange<UInt32>(0, 4204));
            if(i == 16)
               m_inputs[i] = m_GraySamplesLeft[index];
            else if(i == 17)
               m_inputs[i] = m_GraySamplesCenter[index];
            else
            m_inputs[i] = m_GraySamplesRight[index];
         }
      }
   } else {
      for(size_t i=16; i<19; i++) {
         m_inputs[i] = 0;
      }
   }

   // Get RAB sensory data.
   if(m_pcRAB != NULL) {
      const CCI_RVRRangeAndBearingSensor::TPackets& cRABReadings = m_pcRAB->GetPackets();
      m_pcRobotState->SetRangeAndBearingMessages(cRABReadings);

      CCI_RVRRangeAndBearingSensor::TPackets sLastPackets = m_pcRobotState->GetRangeAndBearingMessages();
      CCI_RVRRangeAndBearingSensor::SReceivedPacket cProcessedRabReading = m_pcRobotState->GetAttractionVectorToNeighbors(1.0);

      CVector2 cRabReading = CVector2(cProcessedRabReading.Range, cProcessedRabReading.Bearing);
      Real len = cRabReading.Length();
      if(len != 0) {
         cRabReading.Normalize(); // now, sRabVectorSum.Length = 1
         cRabReading *= (2/(1+exp(-len)) - 1);
      }

      UInt8 unNumberNeighbors = m_pcRobotState->GetNumberNeighbors();

      // Set the RAB input of the NN
      m_inputs[19] = 1 - (2 / (1 + exp(unNumberNeighbors))); // Saturate at 5, and is in [0,1]
      for(int i = 20; i < 24; i++) {
         CRadians cDirection = CRadians::PI*(2*(i-20) + 1)/4;
         Real value = cRabReading.DotProduct(CVector2(1.0, cDirection));
         m_inputs[i] = (value > 0 ? value : 0); // only 2 inputs (rarely 3) will be different from 0.
      }
   } else {
      for(size_t i = 19; i<24; i++) {
         m_inputs[i] = 0;
      }
   }

   // Bias Unit
   m_inputs[24] = 1;

   // Feed the network with those inputs
   m_net->load_sensors((double*)m_inputs);
   // Transmit the signals to the next layer.
   m_net->activate();

   // Apply NN outputs to actuation. The NN outputs are in the range [0,1], we remap this range into [-5:5] linearly.
   m_cNeuralNetworkOutputRange.MapValueIntoRange(m_fLeftSpeed, (m_net->outputs[0])->activation, m_cWheelActuationRange);
   m_cNeuralNetworkOutputRange.MapValueIntoRange(m_fRightSpeed, (m_net->outputs[1])->activation, m_cWheelActuationRange);

   if(m_pcWheels != NULL) {
      m_pcWheels->SetLinearVelocity(m_fLeftSpeed, m_fRightSpeed);
   }

   m_unTimeStep++;
   //Display(1);
}

/****************************************/
/****************************************/

void NeuralNetworkRM1Dot1::Reset() {
  // Reinit the network: Puts the network back into an inactive state
  if (m_net != NULL) {
    m_net->flush();
  }
  m_unTimeStep = 0;
  m_pcRobotState->Reset();
  // Activate the RAB actuator (send the robot's id)
  if(m_pcRABAct != NULL) {
    UInt8 data[2];
    data[0] = getRobotId();
    data[1] = 0;
    m_pcRABAct->SetData(data);
  }
}

/****************************************/
/****************************************/

void NeuralNetworkRM1Dot1::Destroy() {}

REGISTER_CONTROLLER(NeuralNetworkRM1Dot1, "nn_rm_1dot1_controller")
