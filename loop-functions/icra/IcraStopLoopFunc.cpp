/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "IcraStopLoopFunc.h"

/****************************************/
/****************************************/

IcraStopLoopFunction::IcraStopLoopFunction() {
    m_fObjectiveFunction = 0;
    m_fRandomIndex = 0;
}

/****************************************/
/****************************************/

IcraStopLoopFunction::IcraStopLoopFunction(const IcraStopLoopFunction& orig) {
}

/****************************************/
/****************************************/

IcraStopLoopFunction::~IcraStopLoopFunction() {}

/****************************************/
/****************************************/

void IcraStopLoopFunction::Destroy() {
}

/****************************************/
/****************************************/

argos::CColor IcraStopLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

        if (c_position_on_plane.GetY() <= -0.655 || c_position_on_plane.GetX() >= 0.655)
            return CColor::WHITE;
        else if(c_position_on_plane.GetX() <= -0.655 || c_position_on_plane.GetY() >= 0.655)
            return CColor::BLACK;

    return CColor::GRAY50;
}

/****************************************/
/****************************************/

void IcraStopLoopFunction::Init(TConfigurationNode& t_tree) {
    AutoMoDeLoopFunctions::Init(t_tree);
    GetRobotPositions(true);

    m_fRandomIndex = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
}

/****************************************/
/****************************************/

void IcraStopLoopFunction::Reset() {
    AutoMoDeLoopFunctions::Reset();
    m_fObjectiveFunction = 0;
    m_fRandomIndex = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
}

/****************************************/
/****************************************/

void IcraStopLoopFunction::PostStep() {
    UInt32 unClock = GetSpace().GetSimulationClock();

    ArenaControl(unClock);
    GetRobotPositions(false);
    m_fObjectiveFunction += GetMissionScore(unClock);
    m_tMemPositions = m_tPositions;
}

/****************************************/
/****************************************/

void IcraStopLoopFunction::PostExperiment() {

  LOG << m_fObjectiveFunction << std::endl;
}

/****************************************/
/****************************************/

Real IcraStopLoopFunction::GetObjectiveFunction() {

  return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void IcraStopLoopFunction::ArenaControl(UInt32 unClock) {

    if (unClock == 1 && m_unPwConfig == 0)
        m_pcArena->SetArenaColor(CColor::BLACK);

    if (unClock == m_unPwTime && m_unPwConfig == 0)
        m_pcArena->SetArenaColor(CColor::RED);

    if (unClock == 1 && m_unPwConfig == 1 && m_fRandomIndex <= 0.5)
        m_pcArena->SetArenaColor(CColor::BLACK);

    if (unClock == 1 && m_unPwConfig == 1 && m_fRandomIndex > 0.5)
        m_pcArena->SetArenaColor(CColor::RED);

    if (unClock == m_unPwTime && m_unPwConfig == 1 && m_fRandomIndex <= 0.5)
        m_pcArena->SetArenaColor(CColor::RED);

    if (unClock == m_unPwTime && m_unPwConfig == 1 && m_fRandomIndex > 0.5)
        m_pcArena->SetArenaColor(CColor::BLACK);

    return;
}

/****************************************/
/****************************************/

Real IcraStopLoopFunction::GetMissionScore(UInt32 unClock){

    Real unScore = 0;

    if (m_unPwConfig == 0){
        unScore += PwFunctionMove(unClock,0,m_unPwTime,false);
        unScore += PwFunctionStop(unClock,m_unPwTime,(2*m_unPwTime));
        return unScore;
    }

    if (m_unPwConfig == 1){
        if (m_fRandomIndex <= 0.5){
            unScore += PwFunctionMove(unClock,0,m_unPwTime,false);
            unScore += PwFunctionStop(unClock,m_unPwTime,(2*m_unPwTime));
            return unScore;
        }
        else{
            unScore += PwFunctionStop(unClock,0,m_unPwTime);
            unScore += PwFunctionMove(unClock,m_unPwTime,(2*m_unPwTime),false);
            return unScore;
        }
    }

    return 0;
}

/****************************************/
/****************************************/

void IcraStopLoopFunction::GetRobotPositions(bool bSavePositions) {
    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
        cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                           pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

        if (bSavePositions)
            m_tMemPositions[pcEpuck] = cEpuckPosition;
        else
            m_tPositions[pcEpuck] = cEpuckPosition;
    }
}

/****************************************/
/****************************************/

Real IcraStopLoopFunction::PwFunctionStop(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime) {

    if (unClock > unInitTime && unClock <= unEndTime){
        Real unScore = 0;
        TPosMap::iterator it, jt;
        for (it = m_tPositions.begin(), jt = m_tMemPositions.begin(); it != m_tPositions.end(); ++it, ++jt) {
            Real d = (it->second - jt->second).Length();
            if (d > 0.0005)
                unScore+=1;
        }
        return unScore;
    }

    return 0;
}

/****************************************/
/****************************************/

Real IcraStopLoopFunction::PwFunctionMove(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime, bool bCheckColor) {

    if (unClock > unInitTime && unClock < unEndTime){
        Real unScore = 0;
        TPosMap::iterator it, jt;
        for (it = m_tPositions.begin(), jt = m_tMemPositions.begin(); it != m_tPositions.end(); ++it, ++jt) {
            Real d = (it->second - jt->second).Length();
            if (d > 0.0005){
                if (bCheckColor){
                    if(GetFloorColor(it->second) != CColor::GRAY50)
                        unScore+=1;
                }
            }
            else
                unScore+=1;
        }
        return unScore;
    }

    return 0;
}

/****************************************/
/****************************************/

CVector3 IcraStopLoopFunction::GetRandomPosition() {
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

REGISTER_LOOP_FUNCTIONS(IcraStopLoopFunction, "icra_stop_loop_function");
