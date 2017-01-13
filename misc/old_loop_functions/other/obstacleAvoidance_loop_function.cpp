#include "obstacleAvoidance_loop_function.h"

/****************************************/
/****************************************/

CObstacleAvoidanceLoopFunctions::CObstacleAvoidanceLoopFunctions() :
   CNeatLoopFunctions() {}

/****************************************/
/****************************************/

CObstacleAvoidanceLoopFunctions::~CObstacleAvoidanceLoopFunctions() {
}

/****************************************/
/****************************************/

void CObstacleAvoidanceLoopFunctions::PostStep() {
   
   m_performance += 1;
   size_t size = m_pvecEpucks.size();
   double penalty = 1/size;

   for(UInt32 i=0; i < size; i++) {
      if(m_pvecEpucks[i]->GetEmbodiedEntity().isCollidingWithSomething()) {
	 m_performance -= penalty;
      }
      //m_pvecControllers[i]->GetProximitySensors(); //need to create the method inside the controller...
      //m_pvecEpucks[i]->GetComponent("");
      //m_performance += (10-(m_pvecEpucks[i]->GetEmbodiedEntity().GetPosition().Length()))/1000;
   }
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CObstacleAvoidanceLoopFunctions, "obstacleAvoidance_loop_functions")
