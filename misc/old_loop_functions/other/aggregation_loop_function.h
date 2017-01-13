/* Loop-function */
#include "neat_loop_function.h"

/****************************************/
/****************************************/

using namespace argos;

class CAggregationLoopFunctions : public CNeatLoopFunctions {

public:

   CAggregationLoopFunctions();
   virtual ~CAggregationLoopFunctions();

   virtual void Init(TConfigurationNode& t_node);
   //virtual void PostStep();
   virtual void PostExperiment();
   virtual CColor GetFloorColor(const CVector2& c_pos_on_floor);

private:

   double m_unRadius;

};

