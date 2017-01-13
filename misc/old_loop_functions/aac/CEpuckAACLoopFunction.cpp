/*
 * Aggregation with Ambient Clues
 * Author: Touraj Soleymani
 */

#include "CEpuckAACLoopFunction.h"
#include <set>

CEpuckAACLoopFunction::CEpuckAACLoopFunction() {
}

CEpuckAACLoopFunction::~CEpuckAACLoopFunction() {
}

void CEpuckAACLoopFunction::Init(TConfigurationNode& t_tree) {

   /* Initialize the objective function */
   //setIsTheObjectiveFunctionAlreadyMinimizing(false);

   // Light
   /*m_pcLight = new CLightEntity("light0", CVector3(m_pcRNG->Uniform(m_cArenaSideX), m_pcRNG->Uniform(m_cArenaSideY), 1), CColor::RED, 3.0);
   AddEntity(*m_pcLight);*/

   CNeatLoopFunctions::Init(t_tree);
}

void CEpuckAACLoopFunction::Reset() {
   // Light
   /*if(m_pcLight!=NULL && m_nTrial>0) {
      m_pcLight->SetPosition(CVector3(m_pcRNG->Uniform(m_cArenaSideX), m_pcRNG->Uniform(m_cArenaSideY), 1));
   }*/

   CNeatLoopFunctions::Reset();
}

/*void CEpuckAACLoopFunction::Destroy() {
}

void CEpuckAACLoopFunction::PreStep() {
}*/

void CEpuckAACLoopFunction::PostStep() {

   CNeatLoopFunctions::PostStep();

   /* map of all epucks */
   CSpace::TMapPerType& m_tEPuckEntityMap = GetSpace().GetEntitiesByType("epuck");

   CVector2 cEpuckPosition(0,0);

  /* for each footbot */
   for(CSpace::TMapPerType::iterator it = m_tEPuckEntityMap.begin(); it != m_tEPuckEntityMap.end(); ++it) {

      /* Get epuck entity */
      CEPuckEntity& pcEpuck = *any_cast<CEPuckEntity*>(it->second);

      /* Get the position of the epuck */
      cEpuckPosition.Set(pcEpuck.GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                         pcEpuck.GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

      /* Calculate Objective function */
      if(sqrt(pow(cEpuckPosition.GetX(),2) + pow(cEpuckPosition.GetY() - 0.6,2)) < 0.40f) {
         m_dPerformance += 1;
      }
   }
}

/*bool CEpuckAACLoopFunction::IsExperimentFinished() {
    return CNeatLoopFunctions::IsExperimentFinished();
}*/

CColor CEpuckAACLoopFunction::GetFloorColor(const CVector2& c_position_on_plane) {

   if(sqrt(pow(c_position_on_plane.GetX(),2) + pow(c_position_on_plane.GetY() - 0.6,2)) < 0.30f) {
	  //return CColor::WHITE;
    return CColor::BLACK;
	}

	if(sqrt(pow(c_position_on_plane.GetX(),2) + pow(c_position_on_plane.GetY() + 0.6,2)) < 0.30f) {
	  //return CColor::BLACK;
    return CColor::WHITE;
	}

  return CColor::GRAY50;
}


REGISTER_LOOP_FUNCTIONS(CEpuckAACLoopFunction, "aac_loop_functions");
