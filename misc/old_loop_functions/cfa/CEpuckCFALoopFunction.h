/* 
 * File: CEpuckCFALoopFunction.cpp
 * Author: Gianpiero Francesca
 *
 * Created on 16 septembre 2011, 15.47
 */

#ifndef CEpuckCFALoopFunctionS_H
#define	CEpuckCFALoopFunctionS_H

#include "../neat_loop_function.h"
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

using namespace argos;

class CEpuckCFALoopFunction : public CNeatLoopFunctions {

   public:
      CEpuckCFALoopFunction();
      virtual ~CEpuckCFALoopFunction();

      virtual void Init(TConfigurationNode& t_tree);
      virtual void Reset();
      /*virtual void Destroy();
      virtual void PreStep();
      virtual void PostStep();
      virtual bool IsExperimentFinished();*/
      virtual void PostExperiment();

      virtual CColor GetFloorColor(const CVector2& c_position_on_plane);

      Real GetExpectedDistance();
      CVector2 GetRandomPointInArena();
      Real GetClosestEpuckDistanceFromPoint(CVector2 &vRandomPoint);
      bool IsOnBlackArea(CVector2 &vEpuckPos);

   private:
      CVector2 m_vC1, m_vC2, m_vC3;
      Real m_fRadius;
};

#endif	
/* CEpuckCFALoopFunction_H */