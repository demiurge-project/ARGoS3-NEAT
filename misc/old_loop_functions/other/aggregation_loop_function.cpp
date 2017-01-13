#include "aggregation_loop_function.h"

/****************************************/
/****************************************/

CAggregationLoopFunctions::CAggregationLoopFunctions() :
   CNeatLoopFunctions(),
   m_unRadius(0.5) {}

/****************************************/
/****************************************/

CAggregationLoopFunctions::~CAggregationLoopFunctions() {
}

/****************************************/
/****************************************/

void CAggregationLoopFunctions::Init(TConfigurationNode& t_node) {
   TConfigurationNode& t_node1 = GetNode(t_node, "parameters");

   // Get the radius of the black spot on the floor from the configuration file.
   GetNodeAttributeOrDefault(t_node1, "radius", m_unRadius, m_unRadius);

   // Call the CNeatLoopFunctions init method.
   CNeatLoopFunctions::Init(t_node);
}

/****************************************/
/****************************************/

/*void CAggregationLoopFunctions::PostStep() {
   int nNbEpucksLeft = 0, nNbEpucksRight = 0;
   double nLength = 0.0;
   CVector3 cvecCenterLeft(0,1,0);
   CVector3 cvecCenterRight(0,-1,0);

   for(UInt32 i=0; i <  m_pvecEpucks.size(); i++) {
      nLength = (m_pvecEpucks[i]->GetEmbodiedEntity().GetPosition() - cvecCenterLeft).Length();
      if(nLength < m_unRadius) {
	 nNbEpucksLeft++;
      } else {
	 nLength = (m_pvecEpucks[i]->GetEmbodiedEntity().GetPosition() - cvecCenterRight).Length();
	 if(nLength < m_unRadius) {
	    nNbEpucksRight++;
	 }
      }
   }

   if(nNbEpucksLeft > nNbEpucksRight) {
      m_performance += nNbEpucksLeft;
   } else {
      m_performance += nNbEpucksRight;
   }
}*/

/****************************************/
/****************************************/

void CAggregationLoopFunctions::PostExperiment() {
   int nNbEpucksLeft = 0, nNbEpucksRight = 0;
   double nLength = 0.0;
   CVector3 cvecCenterLeft(0,1,0);
   CVector3 cvecCenterRight(0,-1,0);

   for(UInt32 i=0; i <  m_pvecEpucks.size(); i++) {
      nLength = (m_pvecEpucks[i]->GetEmbodiedEntity().GetPosition() - cvecCenterLeft).Length();
      if(nLength < m_unRadius) {
	 nNbEpucksLeft++;
      } else {
	 nLength = (m_pvecEpucks[i]->GetEmbodiedEntity().GetPosition() - cvecCenterRight).Length();
	 if(nLength < m_unRadius) {
	    nNbEpucksRight++;
	 }
      }
   }

   if(nNbEpucksLeft > nNbEpucksRight) {
      m_performance = nNbEpucksLeft;
   } else {
      m_performance = nNbEpucksRight;
   }

   LOG << "Left: " << nNbEpucksLeft << std::endl;
   LOG << "Right: " << nNbEpucksRight << std::endl;
   LOG << "Fitness = " << m_performance << std::endl;
}

/****************************************/
/****************************************/

CColor CAggregationLoopFunctions::GetFloorColor(const CVector2& c_pos_on_floor) {

   if( (sqrt(pow(c_pos_on_floor.GetX(), 2.0) + pow(c_pos_on_floor.GetY()-1, 2.0)) < m_unRadius) || 
	 (sqrt(pow(c_pos_on_floor.GetX(), 2.0) + pow(c_pos_on_floor.GetY()+1, 2.0)) < m_unRadius) ) {
      return CColor::BLACK;
   } else {
      return CColor::WHITE;
   }

}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CAggregationLoopFunctions, "aggregation_loop_functions")
