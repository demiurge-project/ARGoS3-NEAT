/**
  * @file <loop-functions/example/PwLoopFunc.h>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#ifndef FORAG_MTC_LOOP_FUNC
#define FORAG_MTC_LOOP_FUNC

#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

#include "../../src/loop-functions/neat_loop_function.h"

using namespace argos;

class ForagMtcLoopFunction: public CNeatLoopFunctions {
    public:
      ForagMtcLoopFunction();
      ForagMtcLoopFunction(const ForagMtcLoopFunction& orig);
      virtual ~ForagMtcLoopFunction();

      virtual void Destroy();

      virtual argos::CColor GetFloorColor(const argos::CVector2& c_position_on_plane);
      virtual void PostExperiment();
      virtual void PostStep();
      virtual void Reset();
      virtual void Init(TConfigurationNode& t_tree);

      Real GetObjectiveFunction();

      CVector3 GetRandomPosition();

      Real GetStepScore(UInt32 unClock);
      Real AdditionalMetrics();

      void ArenaControl(UInt32 unClock);
      void ArenaControlSelector(UInt32 unClock);
      void ArenaConfigOne();
      void ArenaConfigTwo();
      void ArenaConfigThree();


    private:
      CVector2 m_cCoordSpot1;
      CVector2 m_cCoordSpot2;
      CVector2 m_cCoordSpot3;
      CColor m_cForagColor;
      Real m_fRadiusSpot;
      Real m_fSafeDist;
      Real m_fObjectiveFunction;
      Real m_fObjectiveFunctionT1;
      Real m_fObjectiveFunctionT2;
      Real m_fObjectiveFunctionT3;
      Real m_fObjectiveFunctionR;
      Real m_fObjectiveFunctionG;
      Real m_fObjectiveFunctionB;
      Real m_fRandomIndex;
      bool m_bEvaluate;

      typedef std::map<CEPuckEntity*, bool > TObjectMap;
      typedef std::map<CEPuckEntity*, CColor > TColorMap;

      TObjectMap m_tMemObjects;
      TColorMap m_tMemColors;

};

#endif
