#include "NeuralNetworkRM1Dot1.h"
#include <argos3/core/utility/logging/argos_log.h>

/****************************************/
/************* CONSTRUCTOR **************/
/****************************************/

NeuralNetworkRM1Dot1::NeuralNetworkRM1Dot1() {
    m_pcWheels = NULL;
    m_pcRABAct = NULL;
    m_pcProximity = NULL;
    m_pcLight = NULL;
    m_pcGround = NULL;
    m_pcRAB = NULL;
    m_net = NULL;
    m_nId = -1;
    m_unTimeStep = 0;
    m_unRABTime = 10;
    m_bTrial = false;
    m_mapMessages.clear();
    m_pcRNG = argos::CRandom::CreateRNG("argos");
}

/****************************************/
/************** DESTRUCTOR **************/
/****************************************/

NeuralNetworkRM1Dot1::~NeuralNetworkRM1Dot1() {
   if(m_bTrial) {
     delete m_net;
   }
}

/****************************************/
/**************** INIT ******************/
/****************************************/

void NeuralNetworkRM1Dot1::Init(TConfigurationNode& t_node) {
  /* Reference model */
  m_pcRobotState = new ReferenceModel1Dot1();

  /* Get sensor/actuator handles */
  try {
    m_pcWheels = GetActuator<CCI_EPuckWheelsActuator>("epuck_wheels");
  } catch(CARGoSException& ex) {}

  try {
    m_pcRABAct = GetActuator<CCI_EPuckRangeAndBearingActuator>("epuck_range_and_bearing");
  } catch(CARGoSException& ex) {}

  try {
    m_pcProximity = GetSensor<CCI_EPuckProximitySensor>("epuck_proximity");
  } catch(CARGoSException& ex) {}

  try {
    m_pcLight = GetSensor<CCI_EPuckLightSensor>("epuck_light");
  } catch(CARGoSException& ex) {}

  try {
    m_pcGround = GetSensor<CCI_EPuckGroundSensor>("epuck_ground");
  } catch(CARGoSException& ex) {}

  try {
    m_pcRAB = GetSensor<CCI_EPuckRangeAndBearingSensor>("epuck_range_and_bearing");
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

  GetNodeAttributeOrDefault(t_node, "rab_time", m_unRABTime, m_unRABTime);

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
      const CCI_EPuckProximitySensor::TReadings& cProxiReadings = m_pcProximity->GetReadings();
      // Feed readings to EpuckDAO which will process them as needed
      m_pcRobotState->SetProximityInput(cProxiReadings);
      // Collecting processed readings
      CCI_EPuckProximitySensor::TReadings cProcessedProxiReadings = m_pcRobotState->GetProximityInput();
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
      const CCI_EPuckLightSensor::TReadings& tLight = m_pcLight->GetReadings();
      for(size_t i=8; i<16; i++) {
         m_inputs[i] = tLight[i-8].Value;
      }
   } else {
      for(size_t i=8; i<16; i++) {
         m_inputs[i] = 0;
      }
   }

   // Get Ground sensory data.
   if(m_pcGround != NULL) {
      const CCI_EPuckGroundSensor::SReadings& tGround = m_pcGround->GetReadings();
      for(size_t i=16; i<19; i++) {
         m_inputs[i] = tGround[i-16];
         if(tGround[i-16] <= 0.1) { //black
            m_inputs[i] = 0;
         } else if(tGround[i-16] >= 0.95){ //white
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
      const CCI_EPuckRangeAndBearingSensor::TPackets& tRAB = m_pcRAB->GetPackets();

      // Update
      size_t size = tRAB.size();
      for (size_t i=0; i < size; i++) {
         const CCI_EPuckRangeAndBearingSensor::SReceivedPacket& cPacket = *tRAB[i];
         if (cPacket.Data[0] != m_nId) {
            m_mapMessages[cPacket.Data[0]] = RBPacket(cPacket.Data[0], m_unTimeStep, cPacket);
         }
      }
      m_pcRAB->ClearPackets();

      // Get valid messages (takes into consideration the messages that we received during the last 10 time steps)
      std::vector<RBPacket> vecMessages;
      UInt32 unNumMsg = 0;
      for (std::map<UInt32,RBPacket>::iterator itPackets = m_mapMessages.begin(); itPackets != m_mapMessages.end(); itPackets++) {
         RBPacket& cPacket = itPackets->second;
         if (m_unTimeStep - cPacket.unTimestamp < m_unRABTime) {
            unNumMsg++;
            vecMessages.push_back(cPacket); //unNumMsg = vecMessage.size()...
         }
      }
      // TODO: do we need to take into account our own message? (unNumMsg++)??

      // Update the vector
      CVector2 sum(0.0, 0.0);
      size = vecMessages.size();
      for (size_t i = 0; i < size; i++) {
         CCI_EPuckRangeAndBearingSensor::SReceivedPacket& cPacket = vecMessages[i].cMessage;
         sum += CVector2((1 / (Real) (1 + cPacket.Range)), cPacket.Bearing.SignedNormalize());
      }
      // Normalize it
      Real len = sum.Length();
      if(len != 0) {
         sum.Normalize(); // now, sum.Length = 1
         sum *= (2/(1+exp(-len)) - 1);
      }

      // Set the RAB input of the NN
      m_inputs[19] = 1 - (2 / (1 + exp(unNumMsg))); // Saturate at 5, and is in [0,1]
      for(int i = 20; i < 24; i++) {
         CRadians cDirection = CRadians::PI*(2*(i-20) + 1)/4;
         Real value = sum.DotProduct(CVector2(1.0, cDirection));
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
   // LOG << "RESET" << std::endl;
   /*if(m_net != NULL) {
      m_net->flush();
   }*/
   m_unTimeStep = 0;
   m_mapMessages.clear();
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

void NeuralNetworkRM1Dot1::Destroy() {
}

REGISTER_CONTROLLER(NeuralNetworkRM1Dot1, "nn_rm_1dot1_controller")
