/**
  * @file <loop-functions/example/ChocolateSPCLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#include "ChocolateSPCLoopFunc.h"

/****************************************/
/****************************************/

ChocolateSPCLoopFunction::ChocolateSPCLoopFunction() {
  m_fSideSquare = 0.6;
  m_fRadiusCircle = 0.3;
  m_fRadiusRobot = 0.04;

  m_cCoordCircleSpot = CVector2(0.6, 0);
  m_cCoordSquareSpot = CVector2(-0.6, 0);

  m_unNumberPoints = 1000;

  m_fObjectiveFunction = 0;
  m_fDoptA = 0.08;
  m_fDoptP = 0.06;
}

/****************************************/
/****************************************/

ChocolateSPCLoopFunction::ChocolateSPCLoopFunction(const ChocolateSPCLoopFunction& orig) {}

/****************************************/
/****************************************/

ChocolateSPCLoopFunction::~ChocolateSPCLoopFunction() {}

/****************************************/
/****************************************/

void ChocolateSPCLoopFunction::Destroy() {}

/****************************************/
/****************************************/

void ChocolateSPCLoopFunction::Reset() {
  AutoMoDeLoopFunctions::Reset();
}

/****************************************/
/****************************************/

argos::CColor ChocolateSPCLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {
  CVector2 cCurrentPoint(c_position_on_plane.GetX(), c_position_on_plane.GetY());

  if (IsOnSquareArea(cCurrentPoint)){
      return CColor::WHITE;
  } else if ((cCurrentPoint - m_cCoordCircleSpot).Length() < m_fRadiusCircle) {
      return CColor::BLACK;
  } else{
      return CColor::GRAY50;
  }
}

/****************************************/
/****************************************/

void ChocolateSPCLoopFunction::PostExperiment() {
  m_fObjectiveFunction = ComputeObjectiveFunction();
}

/****************************************/
/****************************************/

Real ChocolateSPCLoopFunction::GetObjectiveFunction() {
  return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

Real ChocolateSPCLoopFunction::ComputeObjectiveFunction() {
    CVector2 cRandomPoint;
    Real dA=0, dP=0;
    CSpace::TMapPerType mEpucks = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    Real fDistanceToRandomPoint = 0;

    // White square area
    for(UInt32 i = 0; i < m_unNumberPoints; i++){

        Real fMinDistanceOnSquare = 0.85;  // Correspond to the diagonal of the square area

        cRandomPoint = RandomPointOnSquareArea();

        for (CSpace::TMapPerType::iterator it = mEpucks.begin(); it != mEpucks.end(); ++it) {
            CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*> ((*it).second);
            cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                               pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());
            if(IsOnSquareArea(cEpuckPosition)){
                fDistanceToRandomPoint = (cRandomPoint - cEpuckPosition).Length();
                if(fDistanceToRandomPoint < fMinDistanceOnSquare){
                    fMinDistanceOnSquare = fDistanceToRandomPoint;
                }
            }
        }

        dA += fMinDistanceOnSquare;
    }
    dA /= m_unNumberPoints;

    // Black circle area
    for(UInt32 i = 0; i < m_unNumberPoints; ++i){

        Real fMinDistanceOnCircle = 0.6; // Correspond to the diameter of the circular spot
        cRandomPoint = RandomPointOnCirclePerimeter();

        for (CSpace::TMapPerType::iterator it = mEpucks.begin(); it != mEpucks.end(); ++it) {
            CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*> ((*it).second);
            cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                               pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());
            if(IsOnCirclePerimeter(cEpuckPosition)){
                fDistanceToRandomPoint = (cRandomPoint - cEpuckPosition).Length();
                if(fDistanceToRandomPoint < fMinDistanceOnCircle){
                    fMinDistanceOnCircle = fDistanceToRandomPoint;
                }
            }
        }

        dP += fMinDistanceOnCircle;
    }

    dP /= m_unNumberPoints;
    Real performance = (dA/m_fDoptA) + (dP/m_fDoptP);

    return performance;
}

/****************************************/
/****************************************/

CVector2 ChocolateSPCLoopFunction::RandomPointOnSquareArea(){
    return CVector2(m_pcRng->Uniform(CRange<Real>(m_cCoordSquareSpot.GetX() - m_fSideSquare/2.0f, m_cCoordSquareSpot.GetX() + m_fSideSquare/2.0f)),
                    m_pcRng->Uniform(CRange<Real>(m_cCoordSquareSpot.GetY() - m_fSideSquare/2.0f, m_cCoordSquareSpot.GetY() + m_fSideSquare/2.0f)));
}

/****************************************/
/****************************************/

CVector2 ChocolateSPCLoopFunction::RandomPointOnCirclePerimeter(){
    CRadians cAngle = m_pcRng->Uniform(CRange<CRadians>(CRadians::ZERO,CRadians::TWO_PI));
    return CVector2(m_cCoordCircleSpot.GetX() + Cos(cAngle) * m_fRadiusCircle, m_cCoordCircleSpot.GetY() + Sin(cAngle) * m_fRadiusCircle);
}

/****************************************/
/****************************************/

bool ChocolateSPCLoopFunction::IsOnSquareArea(CVector2 c_point){
    CRange<Real> cRangeSquareX(m_cCoordSquareSpot.GetX() - m_fSideSquare/2.0f, m_cCoordSquareSpot.GetX() + m_fSideSquare/2.0f);
    CRange<Real> cRangeSquareY(m_cCoordSquareSpot.GetY() - m_fSideSquare/2.0f, m_cCoordSquareSpot.GetY() + m_fSideSquare/2.0f);

    if (cRangeSquareX.WithinMinBoundIncludedMaxBoundIncluded(c_point.GetX()) &&
            cRangeSquareY.WithinMinBoundIncludedMaxBoundIncluded(c_point.GetY())) {
        return true;
    }
    return false;
}

/****************************************/
/****************************************/

bool ChocolateSPCLoopFunction::IsOnCirclePerimeter(CVector2 c_point) {
    CRange<Real> cAcceptanceRange(m_fRadiusCircle - m_fRadiusRobot, m_fRadiusCircle + m_fRadiusRobot);
    Real fDistanceFromCenter = (c_point - m_cCoordCircleSpot).Length();
    if(cAcceptanceRange.WithinMinBoundIncludedMaxBoundIncluded(fDistanceFromCenter)){
        return true;
    }
    return false;
}

/****************************************/
/****************************************/

CVector3 ChocolateSPCLoopFunction::GetRandomPosition() {
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


REGISTER_LOOP_FUNCTIONS(ChocolateSPCLoopFunction, "chocolate_spc_loop_functions");
