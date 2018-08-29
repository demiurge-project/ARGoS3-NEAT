/**
  * @file <loop-functions/example/PwLoopFunc.h>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#ifndef PW_LOOP_FUNC
#define PW_LOOP_FUNC

#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

#include "../../src/core/AutoMoDeLoopFunctions.h"

using namespace argos;

class PwLoopFunction: public AutoMoDeLoopFunctions {
  public:
    PwLoopFunction();
    PwLoopFunction(const PwLoopFunction& orig);
    virtual ~PwLoopFunction();

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

    Real PwFunctionAgg(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime, bool bWhiteColor);

    Real PwFunctionFlee(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime);

  private:
    CVector2 m_cCoordSpotB;
    CVector2 m_cCoordSpotW;
    Real m_fRadiusSpot;
    Real m_fObjectiveFunction;

    typedef std::map<CEPuckEntity*, CVector2 > TPosMap;

    TPosMap m_tMemPositions;
    TPosMap m_tPositions;

};

#endif
