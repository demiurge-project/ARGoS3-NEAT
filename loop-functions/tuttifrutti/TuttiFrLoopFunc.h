/**
  * @file <loop-functions/example/PwLoopFunc.h>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#ifndef TUTTI_FR_LOOP_FUNC
#define TUTTI_FR_LOOP_FUNC

#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

#include "../../src/loop-functions/neat_loop_function.h"

using namespace argos;

class TuttiFrLoopFunction: public CNeatLoopFunctions {
  public:
    TuttiFrLoopFunction();
    TuttiFrLoopFunction(const TuttiFrLoopFunction& orig);
    virtual ~TuttiFrLoopFunction();

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
    void UpdateRobotPositions();

    bool IsRobotInNest (CVector2 tRobotPosition);
    UInt32 IsRobotInSourceID (CVector2 tRobotPosition);

    void ScoreControl();
    Real GetForageScore();

  private:

    UInt32 m_unClock;
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
