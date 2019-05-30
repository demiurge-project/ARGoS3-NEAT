/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "TuttiFrLoopFunc.h"

/****************************************/
/****************************************/

TuttiFrLoopFunction::TuttiFrLoopFunction() {
    m_unClock = 0;
    m_fObjectiveFunction = 0;
}

/****************************************/
/****************************************/

TuttiFrLoopFunction::TuttiFrLoopFunction(const TuttiFrLoopFunction& orig) {
}

/****************************************/
/****************************************/

TuttiFrLoopFunction::~TuttiFrLoopFunction() {}

/****************************************/
/****************************************/

void TuttiFrLoopFunction::Destroy() {

    m_tRobotStates.clear();
}

/****************************************/
/****************************************/

void TuttiFrLoopFunction::Init(TConfigurationNode& t_tree) {

    CNeatLoopFunctions::Init(t_tree);
    InitRobotStates();

}

/****************************************/
/****************************************/

void TuttiFrLoopFunction::Reset() {
    CNeatLoopFunctions::Reset();

    m_unClock = 0;
    m_fObjectiveFunction = 0;

    m_tRobotStates.clear();

    InitRobotStates();
}

/****************************************/
/****************************************/

void TuttiFrLoopFunction::PostStep() {

    m_unClock = GetSpace().GetSimulationClock();

    ScoreControl();
    ArenaControl();

}

/****************************************/
/****************************************/

void TuttiFrLoopFunction::PostExperiment() {
    LOG << -m_fObjectiveFunction << std::endl;
}

/****************************************/
/****************************************/

Real TuttiFrLoopFunction::GetObjectiveFunction() {
    return -m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void TuttiFrLoopFunction::ArenaControl() {

    if (m_unClock == 1) {
        m_pcArena->SetBoxColor(1,4,CColor::GREEN);
        m_pcArena->SetBoxColor(2,4,CColor::GREEN);
        m_pcArena->SetBoxColor(3,4,CColor::GREEN);
        m_pcArena->SetBoxColor(4,4,CColor::RED);
        m_pcArena->SetBoxColor(5,4,CColor::RED);
        m_pcArena->SetBoxColor(6,4,CColor::RED);
        m_pcArena->SetWallColor(2,CColor::BLUE);
    }


    return;
}

/****************************************/
/****************************************/

void TuttiFrLoopFunction::ScoreControl(){

    m_fObjectiveFunction += -GetForageScore();
}

/****************************************/
/****************************************/

Real TuttiFrLoopFunction::GetForageScore() {

    UpdateRobotPositions();

    bool bInNest;
    UInt32 unInSource;
    Real unScore = 0;
    TRobotStateMap::iterator it;

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {

        if (it->second.unItem != 0){

            unInSource = IsRobotInSourceID(it->second.cPosition);
            if (unInSource == 1) {
                it->second.unItem = 1;
            }
            else if (unInSource == 2) {
                it->second.unItem = 2;
            }
            else {
                bInNest = IsRobotInNest(it->second.cPosition);
                if (bInNest) {
                    if (it->second.unItem == 1) {
                        unScore-=1;
                    }
                    else if (it->second.unItem == 2) {
                        unScore+=1;
                    }
                    it->second.unItem = 0;
                }
            }
        }
        else {
            unInSource = IsRobotInSourceID(it->second.cPosition);
            if (unInSource == 1) {
                it->second.unItem = 1;
            }
            else if (unInSource == 2) {
                it->second.unItem = 2;
            }
        }
    }

    return unScore;
}

/****************************************/
/****************************************/

argos::CColor TuttiFrLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

    if (c_position_on_plane.GetX() <= -0.60){
        return CColor::WHITE;
    }
    else if (c_position_on_plane.GetX() >= 0.60){
        return CColor::BLACK;
    }

    return CColor::GRAY50;
}

/****************************************/
/****************************************/

bool TuttiFrLoopFunction::IsRobotInNest (CVector2 tRobotPosition) {

    if (tRobotPosition.GetX() <= -0.565)
        return true;

    return false;
}

/****************************************/
/****************************************/

UInt32 TuttiFrLoopFunction::IsRobotInSourceID (CVector2 tRobotPosition){

    UInt32 unSourceId = 0;

    if (tRobotPosition.GetX() >= 0.565) {

        if (tRobotPosition.GetY() <= -0.006)
            unSourceId = 1;
        else if (tRobotPosition.GetY() >= 0.006)
            unSourceId = 2;
    }

    return unSourceId;
}

/****************************************/
/****************************************/

void TuttiFrLoopFunction::UpdateRobotPositions() {
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

void TuttiFrLoopFunction::InitRobotStates() {

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

CVector3 TuttiFrLoopFunction::GetRandomPosition() {
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
  Real fPosX = (c * m_fDistributionRadius / 2) + m_fDistributionRadius * cos(2 * CRadians::PI.GetValue() * (a/b));
  Real fPosY = (d * m_fDistributionRadius / 2) + m_fDistributionRadius * sin(2 * CRadians::PI.GetValue() * (a/b));

  return CVector3(fPosX, fPosY, 0);
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(TuttiFrLoopFunction, "tutti_fr_loop_function");
