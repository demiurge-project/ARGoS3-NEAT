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
    CVector2 GetRandomArenaPoint();
    UInt32 GetRandomTime(UInt32 unMin, UInt32 unMax);

    void ArenaControl();
    bool SelectColorOrder(UInt32 un_ColorOrderParam);
    void AsignArenaColors(UInt32 un_NumberColorsParam);

    void InitRobotStates();
    void InitSources();
    void GetArenaPoints(UInt32 unNumberPoints);
    void UpdateRobotPositions();
    void UpdateRobotColors();

    bool IsRobotInDock (CVector2 tRobotPosition);
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

    Real GetTransportScore();
    Real GetRechargeScore();
    Real GetManufactureScore();
    Real GetSurveillanceScore();
    Real GetRestoreScore();
    Real GetRefillScore();
    Real GetPickUpScore();
    Real GetDispersionScore();
    Real GetCleaningScore();
    void ExecuteRestore();

    Real GetColorStopScore();
    Real GetColorAllBlackScore();
    Real GetColorForageScore();
    Real GetColorAggregationScore();

  private:

    UInt32 m_unClock;
    Real m_fObjectiveFunction;
    Real m_fObjectiveFunctionBlue;
    Real m_fObjectiveFunctionRed;
    Real m_bBlueFirst;
    CColor m_cArenaColor;
    CColor m_cTaskAsignedColorBlue;
    CColor m_cTaskEvalColorBlue;
    CColor m_cTaskAsignedColorRed;
    CColor m_cTaskEvalColorRed;

    struct RobotStateStruct {
        CVector2 cLastPosition;
        CVector2 cPosition;
        CColor cColor;
        bool bItem;
        bool bMaterial;
        bool bMoving;
    };

    typedef std::map<CEPuckEntity*, RobotStateStruct> TRobotStateMap;
    typedef std::map<UInt32, UInt32> TSourceItemsMap;
    typedef std::map<UInt32, UInt32> TSourceOperationMap;
    typedef std::map<UInt32, UInt32> TSourceRestoringMap;
    typedef std::vector<CVector2> TArenaPoints;

    TRobotStateMap m_tRobotStates;
    TSourceItemsMap m_tSourceItems;
    TSourceOperationMap m_tSourceOperation;
    TSourceOperationMap m_tSourceReparation;
    TSourceRestoringMap m_tSourceRestoring;
    TArenaPoints m_tArenaPoints;

};

#endif
