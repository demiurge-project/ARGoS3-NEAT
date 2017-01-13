/* Loop-function */
#include "../neat_loop_function.h"

#include <argos3/plugins/simulator/entities/light_entity.h>


/****************************************/
/****************************************/

using namespace argos;

class CGoToLightLoopFunctions : public CNeatLoopFunctions {

public:

   CGoToLightLoopFunctions();
   virtual ~CGoToLightLoopFunctions();

   virtual void Init(TConfigurationNode& t_node);
   virtual void Reset();
   virtual void PostStep();

private:

   CLightEntity* m_pcLight;
};
