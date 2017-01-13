/* 
 * File:   CEpuckLCNLoopFunction.cpp
 * Author: Andreagiovanni Reina
 *
 * Created on 12th February 2014
 */

#include "CEpuckLCNLoopFunction.h"
#include <set>
#include <limits>

CEpuckLCNLoopFunction::CEpuckLCNLoopFunction(){
}

CEpuckLCNLoopFunction::CEpuckLCNLoopFunction(const CEpuckLCNLoopFunction& orig) {
}

CEpuckLCNLoopFunction::~CEpuckLCNLoopFunction() {
}

void CEpuckLCNLoopFunction::Init(TConfigurationNode& t_tree) {

   /* Initialize the objective function */
   //setIsTheObjectiveFunctionAlreadyMinimizing(false);

   GetNodeAttribute(t_tree, "arena_center", arenaCenter);
   GetNodeAttribute(t_tree, "arena_radius", arenaRadius);
   GetNodeAttribute(t_tree, "max_communication_distance", maxCommunicationDistance);
   GetNodeAttribute(t_tree, "sensing_range", sensingRange);
   GetNodeAttribute(t_tree, "samplings_per_meter2", samplingsPerMeter2);
    
   CNeatLoopFunctions::Init(t_tree);
}

void CEpuckLCNLoopFunction::Reset() {
    CNeatLoopFunctions::Reset();
}

void CEpuckLCNLoopFunction::AddNeighs(std::list<CNetAgent> &agents, std::list<CNetAgent>::iterator ag) {
	for (std::list<CNetAgent>::iterator neigh = agents.begin(); neigh != agents.end(); ++neigh) {
		if (neigh->clusterID != 0) 
         continue;
		if ( Distance(ag->pos, neigh->pos) < maxCommunicationDistance ) {
			neigh->clusterID = ag->clusterID;
			AddNeighs(agents, neigh);
		}
	}
}

/*void CEpuckLCNLoopFunction::Destroy() {
}

void CEpuckLCNLoopFunction::PreStep() {
    CNeatLoopFunctions::PreStep();
}

void CEpuckLCNLoopFunction::PostStep() {
    CNeatLoopFunctions::PostStep();
}

bool CEpuckLCNLoopFunction::IsExperimentFinished() {
    return CNeatLoopFunctions::IsExperimentFinished();
}*/

void CEpuckLCNLoopFunction::PostExperiment() {
   m_dPerformance = CalculateObjectiveFunction();
   CNeatLoopFunctions::PostExperiment();
}

CColor CEpuckLCNLoopFunction::GetFloorColor(const CVector2& c_position_on_plane) {
   return CColor::GRAY50;    
}

/* If you want to compute the loop function at the end of the experiment put it in ComputePerformanceInExperiment */
Real CEpuckLCNLoopFunction::CalculateObjectiveFunction() {

   std::list<CNetAgent> agents;
   CSpace::TMapPerType m_tEPuckEntityMap = GetSpace().GetEntitiesByType("epuck");

   /* create a list with the agents positions (using the objects CNetAgents) */
   for (CSpace::TMapPerType::iterator itEPuckEntity = m_tEPuckEntityMap.begin(); itEPuckEntity != m_tEPuckEntityMap.end(); itEPuckEntity++) {
      //CEPuckEntity* cEPuckEntity = any_cast<CEPuckEntity*> ((*itEPuckEntity).second);
      CEPuckEntity* cEPuckEntity = any_cast<CEPuckEntity*> (itEPuckEntity->second);
      CVector2 pos;
      pos = cEPuckEntity->GetEmbodiedEntity().GetPosition().ProjectOntoXY(pos);
      agents.push_back(CNetAgent(pos));
   }

   /* Cluster the agents in groups with maximum distance MAX_COMM_DIST */
   UInt32 maxUsedID = 0;
   for (std::list<CNetAgent>::iterator ag = agents.begin(); ag != agents.end(); ++ag){
      if (ag->clusterID != 0) 
         continue;
      ag->clusterID = ++maxUsedID;
      AddNeighs(agents, ag);
   }

   /* Determine the biggest group */
   size_t maxGroupSize = 0;
   UInt32 biggestGroupID = 0;
   for (UInt32 i = 1; i <= maxUsedID; i++){
      size_t size = 0;
      for (std::list<CNetAgent>::iterator ag = agents.begin(); ag != agents.end(); ++ag){
         if (ag->clusterID == i) {
            size++;
         }
      }
      
      // TODO
      // what in case there are 2 groups of the same size?
      if (maxGroupSize < size){
         maxGroupSize = size;
         biggestGroupID = i;
      }
   }

   /* create a list of positions of only the elements in the biggest group */
   /* and at the same time, calculate the extreme coordinates of the group */
   std::list<CVector2> biggestGroup;
   Real min_x = std::numeric_limits<double>::max();
   Real min_y = std::numeric_limits<double>::max();
   Real max_x = 0;
   Real max_y = 0;
   for (std::list<CNetAgent>::iterator ag = agents.begin(); ag != agents.end(); ++ag){
      if (ag->clusterID == biggestGroupID) {
         min_x = (ag->pos.GetX() < min_x)? ag->pos.GetX() : min_x;
         min_y = (ag->pos.GetY() < min_y)? ag->pos.GetY() : min_y;
         max_x = (ag->pos.GetX() > max_x)? ag->pos.GetX() : max_x;
         max_y = (ag->pos.GetY() > max_y)? ag->pos.GetY() : max_y;
         biggestGroup.push_back(ag->pos);
      }
   }

   /* calculate the bounding box sizes (ranges) */
   //Real width  = max_x - min_x + 2*SENSING_DIST;
   //Real height = max_y - min_y + 2*SENSING_DIST;
   CRange<Real> width( min_x - sensingRange, max_x + sensingRange);
   CRange<Real> height(min_y - sensingRange, max_y + sensingRange);

   /* Monte-Carlo sampling to estimate the ratio of the bounding box that is covered by the sensing range */
   Real avg = 0;
   for (size_t i = 0; i < (UInt32)Ceil((width.GetSpan()*height.GetSpan())*samplingsPerMeter2); i++) {
      Real rx = m_pcRNG->Uniform(width);
      Real ry = m_pcRNG->Uniform(height);
      CVector2 rndPoint(rx, ry);

      for (std::list<CVector2>::iterator pos = biggestGroup.begin(); pos != biggestGroup.end(); ++pos) {
         if (Distance(rndPoint, *pos) <= sensingRange && Distance(rndPoint, arenaCenter) < arenaRadius) {
            avg++;
            break;
         }
      }
   }

   /* Compute the coverage-ratio */
   avg = avg / Ceil((width.GetSpan()*height.GetSpan())*samplingsPerMeter2);
   /* Multiply the area size (bounding box) by the coverage-ratio */
   Real performance = width.GetSpan() * height.GetSpan() * avg;
    
   return performance;
}



REGISTER_LOOP_FUNCTIONS(CEpuckLCNLoopFunction, "lcn_loop_functions");
