#include "goToLight_loop_function.h"

/****************************************/
/****************************************/

CGoToLightLoopFunctions::CGoToLightLoopFunctions() :
   CNeatLoopFunctions(),
   m_pcLight(NULL) {
}

/****************************************/
/****************************************/

CGoToLightLoopFunctions::~CGoToLightLoopFunctions() {
}

/****************************************/
/****************************************/

void CGoToLightLoopFunctions::Init(TConfigurationNode& t_node) {

   CNeatLoopFunctions::Init(t_node);

   m_pcLight = new CLightEntity("light0", CVector3(m_pcRNG->Uniform(m_cArenaSideX), m_pcRNG->Uniform(m_cArenaSideY), 1), CColor::YELLOW, 3.0);
   AddEntity(*m_pcLight);
}

/****************************************/
/****************************************/

void CGoToLightLoopFunctions::Reset() {
   CNeatLoopFunctions::Reset();
   if(m_pcLight!=NULL && m_nTrial>0) {
      m_pcLight->SetPosition(CVector3(m_pcRNG->Uniform(m_cArenaSideX), m_pcRNG->Uniform(m_cArenaSideY), 1));
   }
}

/****************************************/
/****************************************/

void CGoToLightLoopFunctions::PostStep() {

   for(UInt32 i=0; i <  m_pvecEpucks.size(); i++) {
      m_dPerformance += (10 - (m_pcLight->GetPosition() - m_pvecEpucks[i]->GetEmbodiedEntity().GetOriginAnchor().Position).Length()) / 1000;
   }
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CGoToLightLoopFunctions, "goToLight_loop_functions")
