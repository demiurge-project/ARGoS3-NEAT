/**
  * @file <loop-functions/example/ExampleAggregationLoopFunc.h>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#ifndef EXAMPLE_AGGREGATION_LOOP_FUNC
#define EXAMPLE_AGGREGATION_LOOP_FUNC

#include "../../src/core/AutoMoDeLoopFunctions.h"
#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>


using namespace argos;

class ExampleAggregationLoopFunction: public AutoMoDeLoopFunctions {
  public:
    ExampleAggregationLoopFunction();
    ExampleAggregationLoopFunction(const ExampleAggregationLoopFunction& orig);
    virtual ~ExampleAggregationLoopFunction();

    virtual void Destroy();
    virtual void Reset();

    virtual argos::CColor GetFloorColor(const argos::CVector2& c_position_on_plane);
    virtual void PostExperiment();
    virtual void PostStep();

    Real GetObjectiveFunction();

    /*
     * Returns a vector containing a random position inside a circle of radius
     * m_fDistributionRadius and centered in (0,0).
     */
    virtual CVector3 GetRandomPosition();

  private:
    Real m_fRadius;
    CVector2 m_cCoordBlackSpot;

    UInt32 m_unScoreSpot;
    Real m_fObjectiveFunction;
};

#endif
