#include "NeuralNetworkRM3Dot0.h"
#include <argos3/core/utility/logging/argos_log.h>

/****************************************/
/************* CONSTRUCTOR **************/
/****************************************/

NeuralNetworkRM3Dot0::NeuralNetworkRM3Dot0() {
    m_pcWheels = NULL;
    m_pcRABAct = NULL;
    m_pcLEDsActuator = NULL;
    m_pcProximity = NULL;
    m_pcLight = NULL;
    m_pcGround = NULL;
    m_pcRAB = NULL;
    m_pcOmniCam = NULL;
    m_net = NULL;
    m_nId = -1;
    m_unTimeStep = 0;
    m_mapMessages.clear();
    m_pcRNG = argos::CRandom::CreateRNG("argos");
}

/****************************************/
/************** DESTRUCTOR **************/
/****************************************/

NeuralNetworkRM3Dot0::~NeuralNetworkRM3Dot0() {
   delete m_pcRobotState;
}

/****************************************/
/**************** INIT ******************/
/****************************************/

void NeuralNetworkRM3Dot0::Init(TConfigurationNode& t_node) {
  CEPuckNEATController::Init(t_node);

  /* Reference model */
  m_pcRobotState = new ReferenceModel3Dot0();
  m_pcRobotState->SetRobotIdentifier(getRobotId());
  m_cWheelActuationRange.Set(0.0f, m_pcRobotState->GetMaxVelocity());

  if(m_pcLEDsActuator != NULL){
     m_pcLEDsActuator->SetColors(CColor::BLACK);
  }

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

void NeuralNetworkRM3Dot0::ControlStep() {

   // Get Proximity sensory data.
   if(m_pcProximity != NULL) {
      const CCI_EPuckProximitySensor::TReadings& cProxiReadings = m_pcProximity->GetReadings();
      // Feed readings to EpuckDAO which will process them as needed
      m_pcRobotState->SetProximityInput(cProxiReadings);
      // Collecting processed readings
      //CCI_EPuckProximitySensor::TReadings cProcessedProxiReadings = m_pcRobotState->GetProximityInput();
      // Injecting processed readings as input of the NN
      for(size_t i=0; i<8; i++) {
         m_inputs[i] = cProxiReadings[i].Value;
      }
   } else {
      for(size_t i=0; i<8; i++) {
         m_inputs[i] = 0;
      }
   }

   // Get Light sensory data.
   /*
   if(m_pcLight != NULL) {
      const CCI_EPuckLightSensor::TReadings& cLightReadings = m_pcLight->GetReadings();
      m_pcRobotState->SetLightInput(cLightReadings);
      CCI_EPuckLightSensor::TReadings cProcessedLightReadings = m_pcRobotState->GetLightInput();
      for(size_t i=8; i<16; i++) {
         m_inputs[i] = cProcessedLightReadings[i-8].Value;
      }
   } else {
      for(size_t i=8; i<16; i++) {
         m_inputs[i] = 0;
      }
   }
   */

   // Get Ground sensory data.
   if(m_pcGround != NULL) {
      const CCI_EPuckGroundSensor::SReadings& cGroundReadings = m_pcGround->GetReadings();
      m_pcRobotState->SetGroundInput(cGroundReadings);
      Real fProcessedGroundReadings = m_pcRobotState->GetGroundReading();
      for(size_t i=8; i<11; i++) {
         m_inputs[i] = fProcessedGroundReadings;
         if(fProcessedGroundReadings <= 0.1) { //black
            m_inputs[i] = 0;
         } else if(fProcessedGroundReadings >= 0.95){ //white
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
      const CCI_EPuckRangeAndBearingSensor::TPackets& cRABReadings = m_pcRAB->GetPackets();
      m_pcRobotState->SetRangeAndBearingMessages(cRABReadings);

      CCI_EPuckRangeAndBearingSensor::SReceivedPacket cProcessedRabReading = m_pcRobotState->GetAttractionVectorToNeighbors(1.0);   // alpha = 1 (artbitrary value)
      CVector2 cRabReading = CVector2(cProcessedRabReading.Range, cProcessedRabReading.Bearing);
      UInt8 unNumberNeighbors = m_pcRobotState->GetNumberNeighbors();

      // Set the RAB input of the NN
      m_inputs[11] = 1 - (2 / (1 + exp(unNumberNeighbors))); // Saturate at 5, and is in [0,1]
      for(int i = 12; i < 16; i++) {
         CRadians cDirection = CRadians::PI*(2*(i-12) + 1)/4;
         Real value = cRabReading.DotProduct(CVector2(1.0, cDirection));
         m_inputs[i] = (value > 0 ? value : 0); // only 2 inputs (rarely 3) will be different from 0.
      }
   } else {
      for(size_t i = 11; i<16; i++) {
         m_inputs[i] = 0;
      }
   }

   // Get Omnidirectional camera sensory data.
   if(m_pcOmniCam != NULL) {

      const CCI_EPuckOmnidirectionalCameraSensor::SReadings& sOmniCam = m_pcOmniCam->GetReadings();
      CCI_EPuckOmnidirectionalCameraSensor::TBlobList::const_iterator it;
      CVector2 sSumVectorR(0,CRadians::ZERO);
      CVector2 sSumVectorG(0,CRadians::ZERO);
      CVector2 sSumVectorB(0,CRadians::ZERO);
      CVector2 sSumVectorY(0,CRadians::ZERO);
      CVector2 sSumVectorM(0,CRadians::ZERO);
      CVector2 sSumVectorC(0,CRadians::ZERO);

      // Compute color vectors

      for (it = sOmniCam.BlobList.begin(); it != sOmniCam.BlobList.end(); it++) {

          if ((*it)->Color == CColor::RED && (*it)->Distance >= 6.0) {
              sSumVectorR += CVector2(1 / (((*it)->Distance)+1),(*it)->Angle.SignedNormalize());
          }
          if ((*it)->Color == CColor::GREEN && (*it)->Distance >= 6.0) {
              sSumVectorG += CVector2(1 / (((*it)->Distance)+1),(*it)->Angle.SignedNormalize());
          }
          if ((*it)->Color == CColor::BLUE && (*it)->Distance >= 6.0) {
              sSumVectorB += CVector2(1 / (((*it)->Distance)+1),(*it)->Angle.SignedNormalize());
          }
          if ((*it)->Color == CColor::YELLOW && (*it)->Distance >= 6.0) {
              sSumVectorY += CVector2(1 / (((*it)->Distance)+1),(*it)->Angle.SignedNormalize());
          }
          if ((*it)->Color == CColor::MAGENTA && (*it)->Distance >= 6.0) {
              sSumVectorM += CVector2(1 / (((*it)->Distance)+1),(*it)->Angle.SignedNormalize());
          }
          if ((*it)->Color == CColor::CYAN && (*it)->Distance >= 6.0) {
              sSumVectorC += CVector2(1 / (((*it)->Distance)+1),(*it)->Angle.SignedNormalize());
          }
      }

      // Normalize color vectors

      Real lenR = sSumVectorR.Length();
      if(lenR != 0) {
         sSumVectorR.Normalize(); // now, sum.Length = 1
         sSumVectorR *= (2/(1+exp(-lenR)) - 1);
      }

      Real lenG = sSumVectorG.Length();
      if(lenG != 0) {
         sSumVectorG.Normalize(); // now, sum.Length = 1
         sSumVectorG *= (2/(1+exp(-lenG)) - 1);
      }

      Real lenB = sSumVectorB.Length();
      if(lenB != 0) {
         sSumVectorB.Normalize(); // now, sum.Length = 1
         sSumVectorB *= (2/(1+exp(-lenB)) - 1);
      }

      Real lenY = sSumVectorY.Length();
      if(lenY != 0) {
         sSumVectorY.Normalize(); // now, sum.Length = 1
         sSumVectorY *= (2/(1+exp(-lenY)) - 1);
      }

      Real lenM = sSumVectorM.Length();
      if(lenM != 0) {
         sSumVectorM.Normalize(); // now, sum.Length = 1
         sSumVectorM *= (2/(1+exp(-lenM)) - 1);
      }

      Real lenC = sSumVectorC.Length();
      if(lenC != 0) {
         sSumVectorC.Normalize(); // now, sum.Length = 1
         sSumVectorC *= (2/(1+exp(-lenC)) - 1);
      }

      // Set the OmniCam input of the NN

      for(int i = 16; i < 20; i++) {
         CRadians cDirectionG = CRadians::PI*(2*(i-16) + 1)/4;
         Real valueG = sSumVectorG.DotProduct(CVector2(1.0, cDirectionG));
         m_inputs[i] = (valueG > 0 ? valueG : 0); // only 2 inputs (rarely 3) will be different from 0.
      }

      for(int i = 20; i < 24; i++) {
         CRadians cDirectionB = CRadians::PI*(2*(i-20) + 1)/4;
         Real valueB = sSumVectorB.DotProduct(CVector2(1.0, cDirectionB));
         m_inputs[i] = (valueB > 0 ? valueB : 0); // only 2 inputs (rarely 3) will be different from 0.
      }

      for(int i = 24; i < 28; i++) {
         CRadians cDirectionR = CRadians::PI*(2*(i-24) + 1)/4;
         Real valueR = sSumVectorR.DotProduct(CVector2(1.0, cDirectionR));
         m_inputs[i] = (valueR > 0 ? valueR : 0); // only 2 inputs (rarely 3) will be different from 0.
      }

      for(int i = 28; i < 32; i++) {
         CRadians cDirectionY = CRadians::PI*(2*(i-28) + 1)/4;
         Real valueY = sSumVectorY.DotProduct(CVector2(1.0, cDirectionY));
         m_inputs[i] = (valueY > 0 ? valueY : 0); // only 2 inputs (rarely 3) will be different from 0.
      }

      for(int i = 32; i < 36; i++) {
         CRadians cDirectionM = CRadians::PI*(2*(i-32) + 1)/4;
         Real valueM = sSumVectorM.DotProduct(CVector2(1.0, cDirectionM));
         m_inputs[i] = (valueM > 0 ? valueM : 0); // only 2 inputs (rarely 3) will be different from 0.
      }

      for(int i = 36; i < 40; i++) {
         CRadians cDirectionC = CRadians::PI*(2*(i-36) + 1)/4;
         Real valueC = sSumVectorC.DotProduct(CVector2(1.0, cDirectionC));
         m_inputs[i] = (valueC > 0 ? valueC : 0); // only 2 inputs (rarely 3) will be different from 0.
      }

   } else {
      for(size_t i=16; i<40; i++) {
         m_inputs[i] = 0;
      }
   }

   // Bias Unit
   m_inputs[40] = 1;

   // Feed the network with those inputs
   m_net->load_sensors((double*)m_inputs);
   // Transmit the signals to the next layer.
   m_net->activate();

   // Apply NN outputs to actuation. The NN outputs are in the range [0,1], we remap this range into [-5:5] linearly.
   m_cNeuralNetworkOutputRange.MapValueIntoRange(m_fLeftSpeedO1, (m_net->outputs[0])->activation, m_cWheelActuationRange);
   m_cNeuralNetworkOutputRange.MapValueIntoRange(m_fLeftSpeedO2, (m_net->outputs[1])->activation, m_cWheelActuationRange);
   m_cNeuralNetworkOutputRange.MapValueIntoRange(m_fRightSpeedO1, (m_net->outputs[2])->activation, m_cWheelActuationRange);
   m_cNeuralNetworkOutputRange.MapValueIntoRange(m_fRightSpeedO2, (m_net->outputs[3])->activation, m_cWheelActuationRange);

   if(m_pcWheels != NULL) {
      m_pcWheels->SetLinearVelocity((m_fLeftSpeedO1-m_fLeftSpeedO2), (m_fRightSpeedO1-m_fRightSpeedO2));
   }

   m_fMaxColorOutput = Max((m_net->outputs[4])->activation,(m_net->outputs[5])->activation);
   m_fMaxColorOutput = Max(m_fMaxColorOutput,(m_net->outputs[6])->activation);
   m_fMaxColorOutput = Max(m_fMaxColorOutput,(m_net->outputs[7])->activation);

   if (m_pcLEDsActuator != NULL) {

       if (m_fMaxColorOutput == (m_net->outputs[4])->activation){
           //m_pcLEDsActuator->SetColors(CColor::BLACK);
           m_pcLEDsActuator->SetColor(2,CColor::BLACK);
           //m_pcLEDsActuator->SetColors(CColor::BLACK); // For Real Robots
       }

       else if (m_fMaxColorOutput == (m_net->outputs[5])->activation){
           //m_pcLEDsActuator->SetColors(CColor::YELLOW);
           m_pcLEDsActuator->SetColor(2,CColor::YELLOW);
           //m_pcLEDsActuator->SetColors(CColor(250,80,0)); // For Real Robots
       }

       else if (m_fMaxColorOutput == (m_net->outputs[6])->activation){
           //m_pcLEDsActuator->SetColors(CColor::MAGENTA);
           m_pcLEDsActuator->SetColor(2,CColor::MAGENTA);
           //m_pcLEDsActuator->SetColors(CColor(150,0,60)); // For Real Robots
       }

       else if (m_fMaxColorOutput == (m_net->outputs[7])->activation){
           //m_pcLEDsActuator->SetColors(CColor::CYAN);
           m_pcLEDsActuator->SetColor(2,CColor::CYAN);
           //m_pcLEDsActuator->SetColors(CColor(0,200,50)); // For Real Robots
       }
   }

   m_unTimeStep++;
   //Display(1);
}

/****************************************/
/****************************************/

void NeuralNetworkRM3Dot0::Reset() {
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
  m_pcLEDsActuator->SetColors(CColor::BLACK);
}

/****************************************/
/****************************************/

void NeuralNetworkRM3Dot0::Destroy() {}

REGISTER_CONTROLLER(NeuralNetworkRM3Dot0, "nn_rm_3dot0_controller")
