/**
  * @file <loop-functions/example/PwLoopFunc.h>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#ifndef STOP_TRN_LOOP_FUNC
#define STOP_TRN_LOOP_FUNC

#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

#include "../../src/loop-functions/neat_loop_function.h"

using namespace argos;

class StopTrnLoopFunction: public CNeatLoopFunctions {
  public:
    StopTrnLoopFunction();
    StopTrnLoopFunction(const StopTrnLoopFunction& orig);
    virtual ~StopTrnLoopFunction();

    virtual void Destroy();

    virtual argos::CColor GetFloorColor(const argos::CVector2& c_position_on_plane);
    virtual void PostExperiment();
    virtual void PostStep();
    virtual void Reset();
    virtual void Init(TConfigurationNode& t_tree);

    Real GetObjectiveFunction();

    CVector3 GetRandomPosition();

    void ArenaControl(UInt32 unClock);

    void GetRobotPositions(bool bSavePositions);

    Real GetMissionScore(UInt32 unClock);

    Real PwFunctionStop(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime);

    Real PwFunctionMove(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime, bool bCheckColor);

  private:
    CVector2 m_cCoordSpot1;
    CVector2 m_cCoordSpot2;
    CVector2 m_cCoordSpot3;
    CVector2 m_cStopCoord;
    Real m_fRadiusSpot;
    Real m_fObjectiveFunction;
    Real m_fRandomIndex;

    typedef std::map<CEPuckEntity*, CVector2 > TPosMap;

    TPosMap m_tMemPositions;
    TPosMap m_tPositions;

};

#endif
