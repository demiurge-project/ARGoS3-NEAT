/**
  * @file <loop-functions/example/ColorsLoopFunc.h>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#ifndef COLORS_STOP_LOOP_FUNC
#define COLORS_STOP_LOOP_FUNC

#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

#include "../../src/core/AutoMoDeLoopFunctions.h"

using namespace argos;

class ColorsStopLoopFunction: public AutoMoDeLoopFunctions {
  public:
    ColorsStopLoopFunction();
    ColorsStopLoopFunction(const ColorsStopLoopFunction& orig);
    virtual ~ColorsStopLoopFunction();

    virtual void Destroy();

    virtual argos::CColor GetFloorColor(const argos::CVector2& c_position_on_plane);
    virtual void PostExperiment();
    virtual void PostStep();
    virtual void Init(TConfigurationNode& t_tree);
    virtual void Reset();
    virtual CVector3 GetRandomPosition();

    Real GetObjectiveFunction();

  private:
    Real m_fRadius;
    CVector2 m_cCoordSpot1;
    //CVector2 m_CCoordRect1;
    //CVector2 m_CCoordRect2;
    CVector2 m_CCoordRect2Pos;
    CVector2 m_CCoordRect1Pos;

    typedef std::map<CEPuckEntity*, CVector2 > TOldPosMap;
    TOldPosMap m_tOldPosPoints;

    UInt8 m_unState;
    UInt32 m_unCostI;
    UInt32 m_unCostO;
    UInt32 m_unTbar;
    Real m_fObjectiveFunction;
};

#endif
