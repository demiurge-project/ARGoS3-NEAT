/* Loop-function */
#include "neat_loop_function.h"

/****************************************/
/****************************************/

using namespace argos;

class CObstacleAvoidanceLoopFunctions : public CNeatLoopFunctions {

public:

   CObstacleAvoidanceLoopFunctions();
   virtual ~CObstacleAvoidanceLoopFunctions();

   virtual void PostStep();

};

