/**
  * @file <loop-functions/example/ColorsLoopFunc.h>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#ifndef COLORS_TEST_LOOP_FUNC
#define COLORS_TEST_LOOP_FUNC

#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_omnidirectional_camera_sensor.h>

#include "../../src/core/AutoMoDeLoopFunctions.h"

using namespace argos;

class ColorsTestLoopFunction: public AutoMoDeLoopFunctions {
  public:
    ColorsTestLoopFunction();
    ColorsTestLoopFunction(const ColorsTestLoopFunction& orig);
    virtual ~ColorsTestLoopFunction();

    virtual void Destroy();

    virtual argos::CColor GetFloorColor(const argos::CVector2& c_position_on_plane);
    virtual void PostStep();
    virtual void Reset();

    Real GetObjectiveFunction();

    CVector3 GetRandomPosition();

    void ArenaControl();

  private:
    Real m_fRadius;
    CVector2 m_cCoordSpot1;
    CVector2 m_cCoordSpot2;

    bool m_bInitializationStep;

    Real m_fObjectiveFunction;

    UInt32 * m_punFoodData;
};

#endif
