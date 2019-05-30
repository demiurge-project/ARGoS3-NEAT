/**
  * @file <loop-functions/example/PwLoopFunc.h>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#ifndef TUTTI_AG_LOOP_FUNC
#define TUTTI_AG_LOOP_FUNC

#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

#include "../../src/core/AutoMoDeLoopFunctions.h"

using namespace argos;

class TuttiAgLoopFunction: public AutoMoDeLoopFunctions {
  public:
    TuttiAgLoopFunction();
    TuttiAgLoopFunction(const TuttiAgLoopFunction& orig);
    virtual ~TuttiAgLoopFunction();

    virtual void Destroy();

    virtual argos::CColor GetFloorColor(const argos::CVector2& c_position_on_plane);
    virtual void PostExperiment();
    virtual void PostStep();
    virtual void Reset();
    virtual void Init(TConfigurationNode& t_tree);

    Real GetObjectiveFunction();

    CVector3 GetRandomPosition();
    UInt32 GetRandomTime(UInt32 unMin, UInt32 unMax);

    void ArenaControl();

    void InitRobotStates();
    void GetArenaPoints(UInt32 unNumberPoints);
    void UpdateRobotPositions();

    bool IsRobotInNest (CVector2 tRobotPosition);
    bool IsRobotInAgg (CVector2 tRobotPosition);
    UInt32 IsRobotInSourceID (CVector2 tRobotPosition);

    void ScoreControl();
    Real GetScore(UInt32 unTask);
    Real GetStopScore();  
    Real GetAggregationScore();
    Real GetForageScore();

  private:

    UInt32 m_unClock;
    UInt32 m_unStopTime;
    Real m_fObjectiveFunction;

    struct RobotStateStruct {
        CVector2 cLastPosition;
        CVector2 cPosition;
        UInt32 unItem;
    };

    typedef std::map<CEPuckEntity*, RobotStateStruct> TRobotStateMap;

    TRobotStateMap m_tRobotStates;

};

#endif
