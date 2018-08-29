#include "epuck_nn_controller.h"
#include <argos3/core/utility/logging/argos_log.h>

/****************************************/
/****************************************/

static CRange<Real> NN_OUTPUT_RANGE(0.0f, 1.0f);
static CRange<Real> WHEEL_ACTUATION_RANGE(-5.0f, 5.0f);

/****************************************/
/************* CONSTRUCTOR **************/
/****************************************/

CEPuckNNController::CEPuckNNController() :
   m_pcWheels(NULL),
   m_pcRABAct(NULL),
   m_pcProximity(NULL),
   m_pcLight(NULL),
   m_pcGround(NULL),
   m_pcRAB(NULL),
   m_pcOmniCam(NULL),
   m_net(NULL),
   m_nId(-1),
   m_unTimeStep(0),
   m_unRABTime(10),
   m_bTrial(false) {
      m_mapMessages.clear();
      time_t t;
      /* Intializes random number generator */
      m_pcRNG = argos::CRandom::CreateRNG("argos");
}

/****************************************/
/************** DESTRUCTOR **************/
/****************************************/

CEPuckNNController::~CEPuckNNController() { //TODO : check for mem leak??
   if(m_bTrial) {
     delete m_net;
   } //else, it's done by neat
}

/****************************************/
/**************** INIT ******************/
/****************************************/

void CEPuckNNController::Init(TConfigurationNode& t_node) {

   /* Get sensor/actuator handles */
   try {
      m_pcWheels = GetActuator<CCI_EPuckWheelsActuator>("epuck_wheels");
   } catch(CARGoSException& ex) {}

   try {
      m_pcRABAct = GetActuator<CCI_EPuckRangeAndBearingActuator>("epuck_range_and_bearing");
   } catch(CARGoSException& ex) {}

   try {
       m_pcLEDsActuator = GetActuator<CCI_EPuckRGBLEDsActuator>("epuck_rgb_leds");
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

   try {
      m_pcOmniCam = GetSensor<CCI_EPuckOmnidirectionalCameraSensor>("epuck_omni_cam");
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

void CEPuckNNController::ControlStep() {
   // Get Proximity sensory data.
   if(m_pcProximity != NULL) {
      const CCI_EPuckProximitySensor::TReadings& tProx = m_pcProximity->GetReadings();
      for(size_t i=0; i<8; i++) {
         m_inputs[i] = tProx[i].Value;
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

   // Get Ground sensory data.
   if(m_pcOmniCam != NULL) {

      const CCI_EPuckOmnidirectionalCameraSensor::SReadings& sOmniCam = m_pcOmniCam->GetReadings();

      CCI_EPuckOmnidirectionalCameraSensor::TBlobList::const_iterator it;

      SInt32 sR15BlobCounter = 0;
      SInt32 sG15BlobCounter = 0;
      SInt32 sB15BlobCounter = 0;
      SInt32 sY15BlobCounter = 0;
      SInt32 sM15BlobCounter = 0;
      SInt32 sC15BlobCounter = 0;

      SInt32 sR30BlobCounter = 0;
      SInt32 sG30BlobCounter = 0;
      SInt32 sB30BlobCounter = 0;
      SInt32 sY30BlobCounter = 0;
      SInt32 sM30BlobCounter = 0;
      SInt32 sC30BlobCounter = 0;

      for (it = sOmniCam.BlobList.begin(); it != sOmniCam.BlobList.end(); it++) {

          if ((*it)->Color == CColor::RED) {
              if ((*it)->Distance <= 30){
                  sR30BlobCounter++;
                  if ((*it)->Distance <= 15){
                      sR15BlobCounter++;
                  }
              }
          }

          if ((*it)->Color == CColor::GREEN) {
              if ((*it)->Distance <= 30){
                  sG30BlobCounter++;
                  if ((*it)->Distance <= 15){
                      sG15BlobCounter++;
                  }
              }
          }

          if ((*it)->Color == CColor::BLUE) {
              if ((*it)->Distance <= 30){
                  sB30BlobCounter++;
                  if ((*it)->Distance <= 15){
                      sB15BlobCounter++;
                  }
              }
          }

          if ((*it)->Color == CColor::YELLOW) {
              if ((*it)->Distance <= 30){
                  sY30BlobCounter++;
                  if ((*it)->Distance <= 15){
                      sY15BlobCounter++;
                  }
              }
          }

          if ((*it)->Color == CColor::MAGENTA) {
              if ((*it)->Distance <= 30){
                  sM30BlobCounter++;
                  if ((*it)->Distance <= 15){
                      sM15BlobCounter++;
                  }
              }
          }

          if ((*it)->Color == CColor::CYAN) {
              if ((*it)->Distance <= 30){
                  sC30BlobCounter++;
                  if ((*it)->Distance <= 15){
                      sC15BlobCounter++;
                  }
              }
          }
      }

      m_inputs[24] = (1/(1 + exp(5 * (3 - sR15BlobCounter))));
      m_inputs[25] = (1/(1 + exp(5 * (3 - sR30BlobCounter))));

      m_inputs[26] = (1/(1 + exp(5 * (3 - sG15BlobCounter))));
      m_inputs[27] = (1/(1 + exp(5 * (3 - sG30BlobCounter))));

      m_inputs[28] = (1/(1 + exp(5 * (3 - sB15BlobCounter))));
      m_inputs[29] = (1/(1 + exp(5 * (3 - sB30BlobCounter))));

      m_inputs[30] = (1/(1 + exp(5 * (3 - sY15BlobCounter))));
      m_inputs[31] = (1/(1 + exp(5 * (3 - sY30BlobCounter))));

      m_inputs[32] = (1/(1 + exp(5 * (3 - sM15BlobCounter))));
      m_inputs[33] = (1/(1 + exp(5 * (3 - sM30BlobCounter))));

      m_inputs[34] = (1/(1 + exp(5 * (3 - sC15BlobCounter))));
      m_inputs[35] = (1/(1 + exp(5 * (3 - sC30BlobCounter))));

      //

   } else {
      for(size_t i=24; i<36; i++) {
         m_inputs[i] = 0;
      }
   }

   //TODOOOO DAGR the number of inputs

   // Bias Unit
   m_inputs[36] = 1;

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

    // Apply NN outputs to actuation. The NN outputs are in the range [0,1], we remap this range into [-5:5] linearly.
   if (m_pcLEDsActuator != NULL) {

       m_pcLEDsActuator->SetColors();
   }

   m_unTimeStep++;
   //Display(1);
}

/****************************************/
/************ LOAD NETWORK **************/
/****************************************/

void CEPuckNNController::LoadNetwork(const std::string& filename) {
   std::ifstream iFile(filename.c_str(),std::ios::in);

   if(iFile) {
      char curword[20];
      int id = 0;

      do {
         iFile >> curword;
         // skip comments in the genome file
         if(strcmp(curword, "/*") == 0) {
            do {
               iFile >> curword;
            } while(strcmp(curword, "*/") != 0);
         }
      } while(strcmp(curword, "genomestart") != 0);

      iFile >> id;
      NEAT::Genome* g = new NEAT::Genome(id,iFile);
      m_net = g->genesis(g->genome_id);
      delete g;
      m_bTrial = true;
   } else {
      THROW_ARGOSEXCEPTION("Cannot open genome file '" << filename << "' for reading");
   }

   iFile.close();

   //DisplayNetwork();
}

/****************************************/
/************** DISPLAY *****************/
/****************************************/

void CEPuckNNController::Display(int i) {
   LOG << "SEQ: " << i << std::endl;
   /*LOG << "INPUTS" << std::endl;
   LOG << "Proximity: " << std::endl;
   for(int i=0; i<7; i++) {
      LOG << m_inputs[i] << ", ";
   }
   LOG << m_inputs[7] << "." << std::endl;
   LOG << "Light: " << std::endl;
   for(int i=8; i<15; i++) {
      LOG << m_inputs[i] << ", ";
   }
   LOG << m_inputs[15] << "." << std::endl;
   LOG << "Ground: " << std::endl;
   for(int i=16; i<18; i++) {
      LOG << m_inputs[i] << ", ";
   }
   LOG << m_inputs[18] << "." << std::endl;*/

   /*LOG << "OUTPUTS" << std::endl;
   LOG << "outputs: (" << (m_net->outputs[0])->activation << ", " << (m_net->outputs[1])->activation << ")." << std::endl; */

   DisplayNetwork();

   LOG << "wheels: (" << m_fLeftSpeed << ", " << m_fRightSpeed << ")." << std::endl;
}

void CEPuckNNController::DisplayNetwork() {
   // Some variables
   int count1=1, count2=1;
   std::vector<NEAT::NNode*>::iterator curnode;
   std::vector<NEAT::Link*>::iterator curlink;

   // Display useful informations
   LOG << "NETWORK: " << std::endl;
   LOG << "depth: " << m_net->max_depth() << std::endl;
   /*LOG << "nb of nodes: " << m_net->numnodes << std::endl;
   LOG << "nb of links: " << m_net->numlinks << std::endl;
   LOG << "Inputs: " << std::endl;
   for(curnode = (m_net->inputs).begin(), count1=1; curnode != (m_net->inputs).end(); ++curnode, count1++) {
      LOG << "[Input #" << count1 << ": " << (*curnode)->activation << "] " << std::endl;
   }*/
   LOG << "Outputs: " << std::endl;
   LOG << "active? " << !m_net->outputsoff() << std::endl;
   for(curnode = (m_net->outputs).begin(), count1=1; curnode != (m_net->outputs).end(); ++curnode, count1++) {
      LOG << "[Output #" << count1 << ": sum=" << (*curnode)->activesum << ", y=" << (*curnode)->activation << "]" << std::endl;
      for(curlink = ((*curnode)->incoming).begin(), count2=1; curlink != ((*curnode)->incoming).end(); ++curlink, count2++) {
	 LOG << "[Link #" << count2 << ": w=" << (*curlink)->weight << " | i=" << ((*curlink)->in_node)->get_active_out() << std::endl;
      }
   }
}


/****************************************/
/****************************************/

void CEPuckNNController::Reset() {
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

void CEPuckNNController::Destroy() {
  if(CRandom::ExistsCategory("epuckNNController")) {
     CRandom::RemoveCategory("epuckNNController");
  }
}

/****************************************/
/****************************************/

UInt32 CEPuckNNController::getRobotId() {
   if (m_nId < 0) {
      std::string strId = GetId();
      std::string::size_type pos = strId.find_first_of("0123456789");
      std::string numero = strId.substr(pos, strId.size());
      if (!(std::istringstream(numero) >> m_nId)) {
         m_nId = 0;
      }
   }
   return m_nId;
}

REGISTER_CONTROLLER(CEPuckNNController, "epuck_nn_controller")
