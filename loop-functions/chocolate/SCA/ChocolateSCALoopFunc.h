/*
 * Shelter with Constrained Access
 *
 * @author Antoine Ligot - <aligot@ulb.ac.be>
 */

#ifndef CHOCOLATE_SCA_LOOP_FUNC_H
#define CHOCOLATE_SCA_LOOP_FUNC_H

#include "../../../src/core/AutoMoDeLoopFunctions.h"
#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/simulator/entities/box_entity.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

using namespace argos;

class ChocolateSCALoopFunction : public AutoMoDeLoopFunctions {

   public:
      ChocolateSCALoopFunction();
      ChocolateSCALoopFunction(const ChocolateSCALoopFunction& orig);
      virtual ~ChocolateSCALoopFunction();

      virtual void Destroy();
      virtual void Init(TConfigurationNode& t_tree);
      virtual void Reset();
      virtual void PostStep();

      Real GetObjectiveFunction();

      virtual CColor GetFloorColor(const CVector2& c_position_on_plane);

      virtual CVector3 GetRandomPosition();

    private:
      bool IsInShelter(CVector2& c_position);

      Real m_fSpotRadius;
      Real m_fWidthShelter;
      Real m_fHeightShelter;
      CVector2 m_cPositionShelter;
      CVector2 m_cCoordBlackSpot;
      CVector2 m_cCoordWhiteSpot;

      Real m_fObjectiveFunction;
      CBoxEntity *m_pcBoxLeft, *m_pcBoxRight, *m_pcBoxCenter;
};

#endif
