#include "NEATController.h"
#include <argos3/core/utility/logging/argos_log.h>



void CEPuckNEATController::Init(TConfigurationNode& t_node) {
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
     m_pcOmniCam = GetSensor<CCI_EPuckOmnidirectionalCameraSensor>("epuck_omnidirectional_camera");
  } catch(CARGoSException& ex) {}

  if(m_pcOmniCam != NULL){
     m_pcOmniCam->Enable();
  }

  // Load the parameters for the neural network.
  GetNodeAttributeOrDefault(t_node, "genome_file", m_strFile, m_strFile);
  if(m_strFile != "") {
    try{
       LoadNetwork(m_strFile);
    } catch(CARGoSException& ex) {
       THROW_ARGOSEXCEPTION_NESTED("cannot load parameters from the genome file.", ex);
    }
  } else {
    LOGERR << "Warning: no genome file specified in .argos" << std::endl;
  }

}

/****************************************/
/****************************************/

CEPuckNEATController::~CEPuckNEATController() {
  for (UInt8 i = 0; i < m_cNetworkVector.size(); i++) {
    delete m_cNetworkVector.at(i);
  }
}

/****************************************/
/****************************************/

void CEPuckNEATController::LoadNetwork(const std::string& filename) {
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
      m_cNetworkVector.push_back(m_net);
      delete g;
   } else {
      THROW_ARGOSEXCEPTION("Cannot open genome file '" << filename << "' for reading");
   }

   iFile.close();

   //DisplayNetwork();
}

/****************************************/
/****************************************/

void CEPuckNEATController::Display(int i) {
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

   LOG << "wheels: (" << m_fLeftSpeedO1 - m_fLeftSpeedO2 << ", " << m_fRightSpeedO1 - m_fRightSpeedO2 << ")." << std::endl;
}

/****************************************/
/****************************************/

void CEPuckNEATController::DisplayNetwork() {
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

UInt32 CEPuckNEATController::getRobotId() {
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
