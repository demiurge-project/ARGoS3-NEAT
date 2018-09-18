#include "VectorialEvostick.h"
#include <argos3/core/utility/logging/argos_log.h>

/****************************************/
/****************************************/

static CRange<Real> NN_OUTPUT_RANGE(0.0f, 1.0f);
static CRange<Real> WHEEL_ACTUATION_RANGE(-16.0f, 16.0f);

/****************************************/
/************* CONSTRUCTOR **************/
/****************************************/

VectorialEvostickController::VectorialEvostickController() {
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
  m_fMaxVelocity = 16.0;
  m_bTrial = false;
  m_mapMessages.clear();
  time_t t;
  /* Intializes random number generator */
  srand((unsigned) time(&t));
  argos::CRandom::CreateCategory("epuckNNController", rand());
  m_pcRNG = argos::CRandom::CreateRNG("epuckNNController");
}

/****************************************/
/************** DESTRUCTOR **************/
/****************************************/

VectorialEvostickController::~VectorialEvostickController() {}

/****************************************/
/**************** INIT ******************/
/****************************************/

void VectorialEvostickController::Init(TConfigurationNode& t_node) {

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

void VectorialEvostickController::ControlStep() {
  // Get Proximity sensory data.
  if(m_pcProximity != NULL) {
     const CCI_EPuckProximitySensor::TReadings& tProx = m_pcProximity->GetReadings();
     CVector2 sProxVectorSum(0.0, 0.0);
     for(size_t i=0; i<8; i++) {
        sProxVectorSum += CVector2(tProx[i].Value, tProx[i].Angle);
     }
     // Normalize vector
     Real fLenProx = sProxVectorSum.Length();
     if(fLenProx != 0) {
        sProxVectorSum.Normalize(); // now, sum.Length = 1
        sProxVectorSum *= (2/(1+exp(-fLenProx)) - 1);
     }
     for(int i = 0; i < 4; i++) {
        CRadians cDirection = CRadians::PI*(2*(i-0) + 1)/4;
        Real value = sProxVectorSum.DotProduct(CVector2(1.0, cDirection));
        m_inputs[i] = (value > 0 ? value : 0); // only 2 inputs (rarely 3) will be different from 0.
     }
  } else {
    for(int i = 0; i < 4; i++) {
      m_inputs[i] = 0;
    }
  }

  // Get Light sensory data.
  if(m_pcLight != NULL) {
     const CCI_EPuckLightSensor::TReadings& tLight = m_pcLight->GetReadings();
     CVector2 sLightVectorSum(0.0, 0.0);
     for(size_t i=0; i<8; i++) {
        sLightVectorSum += CVector2(tLight[i].Value, tLight[i].Angle);
     }
     // Normalize vector
     Real fLenLight = sLightVectorSum.Length();
     if(fLenLight != 0) {
        sLightVectorSum.Normalize(); // now, sum.Length = 1
        sLightVectorSum *= (2/(1+exp(-fLenLight)) - 1);
     }
     for(int i = 4; i < 8; i++) {
        CRadians cDirection = CRadians::PI*(2*(i-4) + 1)/4;
        Real value = sLightVectorSum.DotProduct(CVector2(1.0, cDirection));
        m_inputs[i] = (value > 0 ? value : 0); // only 2 inputs (rarely 3) will be different from 0.
     }
  } else {
    for(int i = 0; i < 4; i++) {
      m_inputs[i] = 0;
    }
  }

  // Get Ground sensory data.
  if(m_pcGround != NULL) {
     const CCI_EPuckGroundSensor::SReadings& tGround = m_pcGround->GetReadings();
     for(size_t i=8; i<11; i++) {
        m_inputs[i] = tGround[i-8];
        if(tGround[i-8] <= 0.1) { //black
           m_inputs[i] = 0;
        } else if(tGround[i-8] >= 0.95){ //white
           m_inputs[i] = 1;
        } else { //gray
           UInt32 index = m_pcRNG->Uniform(CRange<UInt32>(0, 4204));
           if(i == 8)
              m_inputs[i] = m_GraySamplesLeft[index];
           else if(i == 9)
              m_inputs[i] = m_GraySamplesCenter[index];
           else
              m_inputs[i] = m_GraySamplesRight[index];
        }
     }
  } else {
     for(size_t i=8; i<11; i++) {
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
        //sum += CVector2((1 / (Real) (1 + 0.0f)), CRadians(0).SignedNormalize());        /* Test with null messages. */
     }
     // Normalize it
     Real len = sum.Length();
     if(len != 0) {
        sum.Normalize(); // now, sum.Length = 1
        sum *= (2/(1+exp(-len)) - 1);
     }

     // Set the RAB input of the NN
     m_inputs[11] = 1 - (2 / (1 + exp(unNumMsg))); // Saturate at 5, and is in [0,1]
     //LOG << unNumMsg << std::endl;
     for(int i = 12; i < 16; i++) {
        CRadians cDirection = CRadians::PI*(2*(i-8) + 1)/4;
        Real value = sum.DotProduct(CVector2(1.0, cDirection));
        m_inputs[i] = (value > 0 ? value : 0); // only 2 inputs (rarely 3) will be different from 0.
     }

  } else {
     for(size_t i = 11; i<16; i++) {
        m_inputs[i] = 0;
     }
  }

  // Bias Unit
  m_inputs[16] = 1;

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

  m_unTimeStep++;
}

/****************************************/
/****************************************/

void VectorialEvostickController::Reset() {
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

void VectorialEvostickController::Destroy() {
  if(CRandom::ExistsCategory("epuckNNController")) {
     CRandom::RemoveCategory("epuckNNController");
  }
}

REGISTER_CONTROLLER(VectorialEvostickController, "vectorial_evostick_controller")
