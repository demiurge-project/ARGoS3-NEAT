/* Loop-function */
#include "neat_loop_function.h"

/****************************************/
/****************************************/

using namespace argos;

class CChessLoopFunctions : public CNeatLoopFunctions {

public:

   CChessLoopFunctions();
   virtual ~CChessLoopFunctions();

   virtual CColor GetFloorColor(const CVector2& c_pos_on_floor);

};

