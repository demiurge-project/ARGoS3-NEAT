/**
  * @file <loop-functions/example/ExampleAggregationLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#include "ExampleAggregationLoopFunc.h"

/****************************************/
/****************************************/

ExampleAggregationLoopFunction::ExampleAggregationLoopFunction() {
  m_fRadius = 0.3;
  m_cCoordBlackSpot = CVector2(0,0);
  m_unScoreSpot = 0;
  m_fObjectiveFunction = 0;
}

/****************************************/
/****************************************/

ExampleAggregationLoopFunction::ExampleAggregationLoopFunction(const ExampleAggregationLoopFunction& orig) {}

/****************************************/
/****************************************/

ExampleAggregationLoopFunction::~ExampleAggregationLoopFunction() {}

/****************************************/
/****************************************/

void ExampleAggregationLoopFunction::Destroy() {}

/****************************************/
/****************************************/

void ExampleAggregationLoopFunction::Reset() {
  m_fObjectiveFunction = 0;
  m_unScoreSpot = 0;
  AutoMoDeLoopFunctions::Reset();
}


/****************************************/
/****************************************/

argos::CColor ExampleAggregationLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {
  CVector2 vCurrentPoint(c_position_on_plane.GetX(), c_position_on_plane.GetY());
  Real d = (m_cCoordBlackSpot - vCurrentPoint).Length();
  if (d <= m_fRadius) {
    return CColor::BLACK;
  }

  return CColor::GRAY50;
}

/****************************************/
/****************************************/

void ExampleAggregationLoopFunction::PostStep() {
    CBoxEntity* pcBox;
    CSpace::TMapPerType& tBoxMap = GetSpace().GetEntitiesByType("box");
    for (CSpace::TMapPerType::iterator it = tBoxMap.begin(); it != tBoxMap.end(); ++it) {
      pcBox = any_cast<CBoxEntity*>(it->second);
      CLEDEquippedEntity& pcLED = pcBox->GetLEDEquippedEntity();
      pcLED.SetAllLEDsColors(CColor::RED);
    }
}

/****************************************/
/****************************************/

void ExampleAggregationLoopFunction::PostExperiment() {
  CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
  CVector2 cEpuckPosition(0,0);
  for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
    CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
    cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                       pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

    Real fDistanceSpot = (m_cCoordBlackSpot - cEpuckPosition).Length();
    if (fDistanceSpot <= m_fRadius) {
      m_unScoreSpot += 1;
    }
  }

  m_fObjectiveFunction = m_unScoreSpot / (Real) m_unNumberRobots;

  LOG << m_fObjectiveFunction << std::endl;
}


/****************************************/
/****************************************/

Real ExampleAggregationLoopFunction::GetObjectiveFunction() {
  return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

CVector3 ExampleAggregationLoopFunction::GetRandomPosition() {
  Real a;
  Real b;
  Real temp;

  a = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
  b = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
  // If b < a, swap them
  if (b < a) {
    temp = a;
    a = b;
    b = temp;
  }
  Real fPosX = b * m_fDistributionRadius * cos(2 * CRadians::PI.GetValue() * (a/b));
  Real fPosY = b * m_fDistributionRadius * sin(2 * CRadians::PI.GetValue() * (a/b));
  return CVector3(fPosX, fPosY, 0);
}

REGISTER_LOOP_FUNCTIONS(ExampleAggregationLoopFunction, "example_aggregation_loop_functions");
