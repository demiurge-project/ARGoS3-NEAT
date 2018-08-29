/**
  * @file <loop-functions/example/ChocolateCFALoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#include "ChocolateCFALoopFunc.h"

/****************************************/
/****************************************/

ChocolateCFALoopFunction::ChocolateCFALoopFunction() {
  m_fSpotRadius = 0.3;
  m_cCoordSpot1 = CVector2(0.5,0.5);
  m_cCoordSpot2 = CVector2(0.5,-0.5);
  m_cCoordSpot3 = CVector2(-0.5,0.0);
  m_fObjectiveFunction = 0;
}

/****************************************/
/****************************************/

ChocolateCFALoopFunction::ChocolateCFALoopFunction(const ChocolateCFALoopFunction& orig) {}

/****************************************/
/****************************************/

ChocolateCFALoopFunction::~ChocolateCFALoopFunction() {}

/****************************************/
/****************************************/

void ChocolateCFALoopFunction::Destroy() {}

/****************************************/
/****************************************/

void ChocolateCFALoopFunction::Reset() {
  AutoMoDeLoopFunctions::Reset();
}

/****************************************/
/****************************************/

argos::CColor ChocolateCFALoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {
  CVector2 vCurrentPoint(c_position_on_plane.GetX(), c_position_on_plane.GetY());
  Real d = (m_cCoordSpot1 - vCurrentPoint).Length();
  if (d <= m_fSpotRadius) {
    return CColor::BLACK;
  }

  d = (m_cCoordSpot2 - vCurrentPoint).Length();
  if (d <= m_fSpotRadius) {
    return CColor::BLACK;
  }

  d = (m_cCoordSpot3 - vCurrentPoint).Length();
  if (d <= m_fSpotRadius) {
    return CColor::BLACK;
  }



  return CColor::GRAY50;
}

/****************************************/
/****************************************/

void ChocolateCFALoopFunction::PostExperiment() {
  m_fObjectiveFunction = ((2*1.25) - GetExpectedDistance()) * 100;
  if (m_fObjectiveFunction < 0)
    m_fObjectiveFunction = 0;
  LOG << m_fObjectiveFunction << std::endl;
}

/****************************************/
/****************************************/

Real ChocolateCFALoopFunction::GetObjectiveFunction() {
  return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

CVector3 ChocolateCFALoopFunction::GetRandomPosition() {
  Real temp;
  Real a = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
  Real  b = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
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

/****************************************/
/****************************************/

CVector2 ChocolateCFALoopFunction::GetRandomPointInArena() {
  return CVector2(m_pcRng->Uniform(CRange<Real>(0.0f, m_fDistributionRadius)), m_pcRng->Uniform(CRange<CRadians>(CRadians::ZERO, CRadians::TWO_PI)));
}

/****************************************/
/****************************************/

bool ChocolateCFALoopFunction::IsOnBlackArea(CVector2& c_epuck_pos) {
  Real fDistSpot1 = (m_cCoordSpot1 - c_epuck_pos).Length();
  Real fDistSpot2 = (m_cCoordSpot2 - c_epuck_pos).Length();
  Real fDistSpot3 = (m_cCoordSpot3 - c_epuck_pos).Length();

  if (fDistSpot1 <= m_fSpotRadius || fDistSpot2 <= m_fSpotRadius || fDistSpot3 <= m_fSpotRadius) {
    return true;
  }

  return false;
}

/****************************************/
/****************************************/

Real ChocolateCFALoopFunction::GetExpectedDistance() {
  // The total distance between each randomly selected point in the arena and the closest robot.
  Real fTotalDistance = 0;

  // The expected distance between a randomly selected point and any robot
  Real fExpectedDistance = 0;

  UInt32 unNumberTrials = 10000;

  for (UInt32 i = 0; i < unNumberTrials; ++i) {
    CVector2 cRandomPoint = GetRandomPointInArena();
    fTotalDistance = GetClosestEpuckDistanceFromPoint(cRandomPoint);
  }

  fExpectedDistance = fTotalDistance / unNumberTrials;

  return fExpectedDistance;
}

/****************************************/
/****************************************/

Real ChocolateCFALoopFunction::GetClosestEpuckDistanceFromPoint(CVector2& c_random_point) {
  CSpace::TMapPerType& mapEpucks = GetSpace().GetEntitiesByType("epuck");
  CVector2 cEpuckPosition;

  // Maximum distance = diameter of arena
  Real fMinDistance = 2.5;
  for (CSpace::TMapPerType::iterator it = mapEpucks.begin(); it != mapEpucks.end(); ++it) {
    CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
    cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                       pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());
    // Only consider epucks not on black spots
    if (!IsOnBlackArea(cEpuckPosition)) {
      Real fDistance = (c_random_point - cEpuckPosition).Length();
      if (fDistance <= fMinDistance) {
        fMinDistance = fDistance;
      }
    }
  }
  return fMinDistance;
}

REGISTER_LOOP_FUNCTIONS(ChocolateCFALoopFunction, "chocolate_cfa_loop_functions");
