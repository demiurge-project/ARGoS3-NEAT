/**
  * @file <loop-functions/example/PwLoopFunc.h>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#ifndef FORAG_TRN_LOOP_FUNC
#define FORAG_TRN_LOOP_FUNC

#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

#include "../../src/loop-functions/neat_loop_function.h"

using namespace argos;

class ForagTrnLoopFunction: public CNeatLoopFunctions {
    public:
      ForagTrnLoopFunction();
      ForagTrnLoopFunction(const ForagTrnLoopFunction& orig);
      virtual ~ForagTrnLoopFunction();

      virtual void Destroy();

      virtual argos::CColor GetFloorColor(const argos::CVector2& c_position_on_plane);
      virtual void PostExperiment();
      virtual void PostStep();
      virtual void Reset();
      virtual void Init(TConfigurationNode& t_tree);

      Real GetObjectiveFunction();

      CVector3 GetRandomPosition();

      void GetStepScore(bool bAggregate);
      Real AdditionalMetrics();

      void ArenaControl(UInt32 unClock);

    private:
      CVector2 m_cCoordSpot1;
      CVector2 m_cCoordSpot2;
      CVector2 m_cCoordSpot3;
      Real m_fRadiusSpot;
      Real m_fSafeDist;
      Real m_fObjectiveFunction;
      Real m_fRandomIndex;
      Real m_fTotalObjects;
      Real m_fTotalRobots;
      bool m_bEvaluate;

      typedef std::map<CEPuckEntity*, bool > TObjectMap;

      TObjectMap m_tMemObjects;

};

#endif
