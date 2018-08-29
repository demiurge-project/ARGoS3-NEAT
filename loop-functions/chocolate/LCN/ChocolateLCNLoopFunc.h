/*
 * Largest Covering Network
 *
 * @file <loop-functions/example/ChocolateLCNLoopFunc.cpp>
 *
 * @author Antoine Ligot - <aligot@ulb.ac.be>
 *
 * @package ARGoS3-AutoMoDe
 *
 * @license MIT License
 */

#ifndef CHOCOLATE_LCN_LOOP_FUNC_H
#define CHOCOLATE_LCN_LOOP_FUNC_H

#include "../../../src/core/AutoMoDeLoopFunctions.h"
#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>
#include <limits>

using namespace argos;

class ChocolateLCNLoopFunction : public AutoMoDeLoopFunctions {

   public:
      ChocolateLCNLoopFunction();
      ChocolateLCNLoopFunction(const ChocolateLCNLoopFunction& orig);
      virtual ~ChocolateLCNLoopFunction();

      virtual void Destroy();
      virtual void Reset();
      virtual void PostExperiment();

      Real GetObjectiveFunction();

      virtual CColor GetFloorColor(const CVector2& c_position_on_plane);

      virtual CVector3 GetRandomPosition();

    private:

      struct CNetAgent {
        CVector2 cPosition;
        UInt32 unClusterID;

        CNetAgent(CVector2 c_position) {
          cPosition = c_position;
          unClusterID = 0;
        }
      };

      Real ComputeObjectiveFunction();

      void AddNeighs(std::vector<CNetAgent> &agents, std::vector<CNetAgent>::iterator ag);

      Real m_fRadiusRobot;

      Real m_fObjectiveFunction;

      Real m_fSensingRange;

      Real m_fCommunicationDistance;

      UInt32 m_unNumberPoints;

      CVector2 m_cArenaCenter;
};

#endif
