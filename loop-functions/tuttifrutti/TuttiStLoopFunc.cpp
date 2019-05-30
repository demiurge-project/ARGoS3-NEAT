/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "TuttiStLoopFunc.h"

/****************************************/
/****************************************/

TuttiStLoopFunction::TuttiStLoopFunction() {
    m_unClock = 0;
    m_unStopTime = 0;
    m_unStopEdge = 2;
    m_unStopBox = 2;
    m_fObjectiveFunction = 0;
}

/****************************************/
/****************************************/

TuttiStLoopFunction::TuttiStLoopFunction(const TuttiStLoopFunction& orig) {
}

/****************************************/
/****************************************/

TuttiStLoopFunction::~TuttiStLoopFunction() {}

/****************************************/
/****************************************/

void TuttiStLoopFunction::Destroy() {

    m_tRobotStates.clear();
}

/****************************************/
/****************************************/

void TuttiStLoopFunction::Init(TConfigurationNode& t_tree) {

    CNeatLoopFunctions::Init(t_tree);
    InitRobotStates();
    m_pcArena->SetArenaColor(CColor::BLACK);

}

/****************************************/
/****************************************/

void TuttiStLoopFunction::Reset() {
    CNeatLoopFunctions::Reset();

    m_pcArena->SetArenaColor(CColor::BLACK);
    m_unClock = 0;
    m_unStopEdge = 2;
    m_unStopBox = 2;
    m_unStopTime = 0;
    m_fObjectiveFunction = 0;

    m_tRobotStates.clear();

    InitRobotStates();
}

/****************************************/
/****************************************/

void TuttiStLoopFunction::PostStep() {

    m_unClock = GetSpace().GetSimulationClock();

    ScoreControl();
    ArenaControl();
}

/****************************************/
/****************************************/

void TuttiStLoopFunction::PostExperiment() {
    LOG << -m_fObjectiveFunction << std::endl;
}

/****************************************/
/****************************************/

Real TuttiStLoopFunction::GetObjectiveFunction() {
    return -m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void TuttiStLoopFunction::ArenaControl() {

    if (m_unClock == m_unStopTime)
        m_pcArena->SetBoxColor(m_unStopBox,m_unStopEdge,CColor::RED);

    return;
}

/****************************************/
/****************************************/

void TuttiStLoopFunction::ScoreControl(){

    if (m_unClock == 1) {
        m_unStopTime = GetRandomTime(400, 601);
        m_unStopEdge = GetRandomTime(1, 9);
        m_unStopBox = GetRandomTime(1, 4);
    }

    if (m_unClock <= m_unStopTime)
        m_fObjectiveFunction += GetMoveScore();
    else
        m_fObjectiveFunction += GetStopScore();
}

/****************************************/
/****************************************/

Real TuttiStLoopFunction::GetStopScore() {

    UpdateRobotPositions();

    Real unScore = 0;
    TRobotStateMap::iterator it;
    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {
        Real d = (it->second.cPosition - it->second.cLastPosition).Length();
        if (d > 0.0005)
            unScore+=1;
    }

    return unScore;
}

/****************************************/
/****************************************/

Real TuttiStLoopFunction::GetMoveScore() {

    UpdateRobotPositions();

    Real unScore = 0;
    TRobotStateMap::iterator it;
    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {
        Real d = (it->second.cPosition - it->second.cLastPosition).Length();
        if (d <= 0.0005)
            unScore+=1;
    }

    return unScore;
}

/****************************************/
/****************************************/

argos::CColor TuttiStLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

    return CColor::GRAY50;
}

/****************************************/
/****************************************/

void TuttiStLoopFunction::UpdateRobotPositions() {
    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
        cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                           pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

        m_tRobotStates[pcEpuck].cLastPosition = m_tRobotStates[pcEpuck].cPosition;
        m_tRobotStates[pcEpuck].cPosition = cEpuckPosition;
    }
}

/****************************************/
/****************************************/

void TuttiStLoopFunction::InitRobotStates() {

    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
        cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                           pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

        m_tRobotStates[pcEpuck].cLastPosition = cEpuckPosition;
        m_tRobotStates[pcEpuck].cPosition = cEpuckPosition;
        m_tRobotStates[pcEpuck].unItem = 0;
    }
}

/****************************************/
/****************************************/

CVector3 TuttiStLoopFunction::GetRandomPosition() {
  Real temp;
  Real a = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
  Real b = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
  Real c = m_pcRng->Uniform(CRange<Real>(-1.0f, 1.0f));
  Real d = m_pcRng->Uniform(CRange<Real>(-1.0f, 1.0f));
  // If b < a, swap them
  if (b < a) {
    temp = a;
    a = b;
    b = temp;
  }
  m_fDistributionRadius = 0.4;
  Real fPosX = (c * m_fDistributionRadius / 2) + m_fDistributionRadius * cos(2 * -CRadians::PI_OVER_TWO .GetValue() * (a/b));
  Real fPosY = -0.20 + (d * m_fDistributionRadius / 2) + m_fDistributionRadius * sin(2 * -CRadians::PI_OVER_TWO.GetValue() * (a/b));

  return CVector3(fPosX, fPosY, 0);
}

/****************************************/
/****************************************/

UInt32 TuttiStLoopFunction::GetRandomTime(UInt32 unMin, UInt32 unMax) {
  UInt32 unStopAt = m_pcRng->Uniform(CRange<UInt32>(unMin, unMax));
  return unStopAt;

}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(TuttiStLoopFunction, "tutti_st_loop_function");
