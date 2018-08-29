/**
  * @file <loop-functions/ColorsLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "ColorsTestLoopFunc.h"

/****************************************/
/****************************************/

ColorsTestLoopFunction::ColorsTestLoopFunction() {
  m_fRadius = 0.40;
  m_cCoordSpot1 = CVector2(0,1.15);
  m_cCoordSpot2 = CVector2(0,-1.15);
  m_fObjectiveFunction = 0;
  m_bInitializationStep = true;
}

/****************************************/
/****************************************/

ColorsTestLoopFunction::ColorsTestLoopFunction(const ColorsTestLoopFunction& orig) {
}

/****************************************/
/****************************************/

ColorsTestLoopFunction::~ColorsTestLoopFunction() {}

/****************************************/
/****************************************/

void ColorsTestLoopFunction::Destroy() {
}

/****************************************/
/****************************************/

argos::CColor ColorsTestLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {
  CVector2 vCurrentPoint(c_position_on_plane.GetX(), c_position_on_plane.GetY());

  if (vCurrentPoint.GetY() > 0.60)
      return CColor::BLACK;
  else
      return CColor::GRAY50;
}


/****************************************/
/****************************************/

void ColorsTestLoopFunction::Reset() {
  m_fObjectiveFunction = 0;
  for (UInt32 i = 0; i <= m_unNumberRobots; i++) {
    m_punFoodData[i] = 0;
  }
  AutoMoDeLoopFunctions::Reset();
}

/****************************************/
/****************************************/

void ColorsTestLoopFunction::PostStep() {
    ArenaControl();

}

/****************************************/
/****************************************/

Real ColorsTestLoopFunction::GetObjectiveFunction() {
  return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void ColorsTestLoopFunction::ArenaControl() {
    UInt32 unClock = GetSpace().GetSimulationClock();
    switch (unClock){

    case 1200:
        m_pcArena->SetArenaColor(CColor::CYAN);
        break;

    default:
        break;
    }
    return;
}

/****************************************/
/****************************************/

CVector3 ColorsTestLoopFunction::GetRandomPosition() {
  Real temp;
  Real a = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
  Real b = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
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

REGISTER_LOOP_FUNCTIONS(ColorsTestLoopFunction, "colors_test_loop_function");
