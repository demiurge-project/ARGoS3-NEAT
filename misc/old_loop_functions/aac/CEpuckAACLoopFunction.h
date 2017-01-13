/* 
 * Aggregation with Ambient Clues
 * Author: Touraj Soleymani
 */

#ifndef CEPUCKAACLOOPFUNCTIONS_H
#define	CEPUCKAACLOOPFUNCTIONS_H

#include "../neat_loop_function.h"
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

using namespace argos;

class CEpuckAACLoopFunction : public CNeatLoopFunctions {

   public:
      CEpuckAACLoopFunction();
      virtual ~CEpuckAACLoopFunction();

      virtual void Init(TConfigurationNode& t_tree);
      virtual void Reset();
      /*virtual void Destroy();
      virtual void PreStep();*/
      virtual void PostStep();
      //virtual bool IsExperimentFinished();

      virtual CColor GetFloorColor(const CVector2& c_position_on_plane);
};

#endif	
/* CEpuckAACLoopFunction_H */