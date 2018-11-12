/**
  * @file <loop-functions/example/PwLoopFunc.h>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#ifndef SEQ_LOOP_FUNC
#define SEQ_LOOP_FUNC

#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

#include "../../src/loop-functions/neat_loop_function.h"

using namespace argos;

class SeqLoopFunction: public CNeatLoopFunctions {
  public:
    SeqLoopFunction();
    SeqLoopFunction(const SeqLoopFunction& orig);
    virtual ~SeqLoopFunction();

    virtual void Destroy();

    virtual argos::CColor GetFloorColor(const argos::CVector2& c_position_on_plane);
    virtual void PostExperiment();
    virtual void PostStep();
    virtual void Reset();
    virtual void Init(TConfigurationNode& t_tree);

    Real GetObjectiveFunction();

    CVector3 GetRandomPosition();

    void ArenaControl();
    bool SelectColorOrder(UInt32 un_ColorOrderParam);
    void AsignArenaColors(UInt32 un_NumberColorsParam);

    void InitRobotStates();
    void InitSourceItems();
    void UpdateRobotPositions();
    void UpdateRobotColors();

    bool IsRobotInNest (CVector2 tRobotPosition);
    bool IsRobotInSource (CVector2 tRobotPosition);
    UInt32 IsRobotInSourceID (CVector2 tRobotPosition);

    void ScoreControl();
    Real GetScore(UInt32 unTask);
    Real GetNormalizedScore(Real fScore, UInt32 unTask);
    Real GetStopScore();
    Real GetAllBlackScore();
    Real GetForageScore();
    Real GetMimicryScore();
    Real GetDistributeScore();
    Real GetAggregationScore();

  private:

    UInt32 m_unClock;
    Real m_fObjectiveFunction;
    Real m_fObjectiveFunctionBlue;
    Real m_fObjectiveFunctionRed;
    Real m_bBlueFirst;
    CColor m_cArenaColor;
    CColor m_cTaskAsignedColor;
    CColor m_cTaskEvalColor;

    struct RobotStateStruct {
        CVector2 cLastPosition;
        CVector2 cPosition;
        CColor cColor;
        bool bItem;
    };

    typedef std::map<CEPuckEntity*, RobotStateStruct> TRobotStateMap;
    typedef std::map<UInt32, UInt32> TSourceItemsMap;

    TRobotStateMap m_tRobotStates;
    TSourceItemsMap m_tSourceItems;

};

#endif
