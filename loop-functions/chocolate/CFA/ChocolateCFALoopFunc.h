/*
 * Coverage with Forbidden Areas
 *
 * @author Antoine Ligot - <aligot@ulb.ac.be>
 */

#ifndef CHOCOLATE_CFA_LOOP_FUNC_H
#define CHOCOLATE_CFA_LOOP_FUNC_H

#include "../../../src/core/AutoMoDeLoopFunctions.h"
#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

using namespace argos;

class ChocolateCFALoopFunction : public AutoMoDeLoopFunctions {

   public:
      ChocolateCFALoopFunction();
      ChocolateCFALoopFunction(const ChocolateCFALoopFunction& orig);
      virtual ~ChocolateCFALoopFunction();

      virtual void Destroy();
      virtual void Reset();
      virtual void PostExperiment();

      Real GetObjectiveFunction();

      virtual CColor GetFloorColor(const CVector2& c_position_on_plane);

      virtual CVector3 GetRandomPosition();
    private:

      Real GetExpectedDistance();
      CVector2 GetRandomPointInArena();
      Real GetClosestEpuckDistanceFromPoint(CVector2& c_random_point);
      bool IsOnBlackArea(CVector2& c_epuck_pos);

      Real m_fSpotRadius;
      CVector2 m_cCoordSpot1;
      CVector2 m_cCoordSpot2;
      CVector2 m_cCoordSpot3;

      Real m_fObjectiveFunction;
};

#endif
