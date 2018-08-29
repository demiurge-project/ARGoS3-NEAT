/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "IcraDesLoopFunc.h"

/****************************************/
/****************************************/

IcraDesLoopFunction::IcraDesLoopFunction() {
    m_fObjectiveFunction = 0;
    m_fRandomIndex = 0;
}

/****************************************/
/****************************************/

IcraDesLoopFunction::IcraDesLoopFunction(const IcraDesLoopFunction& orig) {
}

/****************************************/
/****************************************/

IcraDesLoopFunction::~IcraDesLoopFunction() {}

/****************************************/
/****************************************/

void IcraDesLoopFunction::Destroy() {
}

/****************************************/
/****************************************/

argos::CColor IcraDesLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

        if (c_position_on_plane.GetY() <= -0.655 || c_position_on_plane.GetX() >= 0.655)
            return CColor::WHITE;
        else if(c_position_on_plane.GetX() <= -0.655 || c_position_on_plane.GetY() >= 0.655)
            return CColor::BLACK;

    return CColor::GRAY50;
}

/****************************************/
/****************************************/

void IcraDesLoopFunction::Init(TConfigurationNode& t_tree) {
    AutoMoDeLoopFunctions::Init(t_tree);
    GetRobotPositions(true);

    m_fRandomIndex = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
}

/****************************************/
/****************************************/

void IcraDesLoopFunction::Reset() {
    AutoMoDeLoopFunctions::Reset();
    m_fObjectiveFunction = 0;
    m_fRandomIndex = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
}

/****************************************/
/****************************************/

void IcraDesLoopFunction::PostStep() {
    UInt32 unClock = GetSpace().GetSimulationClock();

    ArenaControl(unClock);
    GetRobotPositions(false);
    m_fObjectiveFunction += GetMissionScore(unClock);
    m_tMemPositions = m_tPositions;
}

/****************************************/
/****************************************/

void IcraDesLoopFunction::PostExperiment() {

  LOG << m_fObjectiveFunction << std::endl;
}

/****************************************/
/****************************************/

Real IcraDesLoopFunction::GetObjectiveFunction() {

  return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void IcraDesLoopFunction::ArenaControl(UInt32 unClock) {

    if (unClock == 1 && m_unPwConfig == 0)
        m_pcArena->SetArenaColor(CColor::BLACK);

    if (unClock == m_unPwTime && m_unPwConfig == 0 && m_fRandomIndex <= 0.5)
        m_pcArena->SetArenaColor(CColor::BLUE);

    if (unClock == m_unPwTime && m_unPwConfig == 0 && m_fRandomIndex > 0.5)
        m_pcArena->SetArenaColor(CColor::GREEN);

    return;
}

/****************************************/
/****************************************/

Real IcraDesLoopFunction::GetMissionScore(UInt32 unClock){

    Real unScore = 0;

    if (m_unPwConfig == 0){
        unScore += PwFunctionMove(unClock,0,m_unPwTime,false);
        if (m_fRandomIndex <= 0.5){
            unScore += PwFunctionAgg(unClock,m_unPwTime,(2*m_unPwTime),false);
            return unScore;
        }
        else{
            unScore += PwFunctionAgg(unClock,m_unPwTime,(2*m_unPwTime),true);
            return unScore;
        }
        return unScore;
    }

    return 0;
}

/****************************************/
/****************************************/

void IcraDesLoopFunction::GetRobotPositions(bool bSavePositions) {
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

Real IcraDesLoopFunction::PwFunctionAgg(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime, bool bWhiteColor) {

    if (unClock > unInitTime && unClock <= unEndTime){
        Real unScore = 0;
        TPosMap::iterator it, jt;
        for (it = m_tPositions.begin(), jt = m_tMemPositions.begin(); it != m_tPositions.end(); ++it, ++jt) {
            Real d = (it->second - jt->second).Length();
            if (d > 0.0005)
                unScore+=1;
            else {
                if (bWhiteColor){
                    if(GetFloorColor(it->second) != CColor::WHITE)
                        unScore+=1;
                }
                else {
                    if(GetFloorColor(it->second) != CColor::BLACK)
                        unScore+=1;
                }
            }
        }
        return unScore;
    }

    return 0;
}

/****************************************/
/****************************************/

Real IcraDesLoopFunction::PwFunctionStop(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime) {

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

Real IcraDesLoopFunction::PwFunctionMove(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime, bool bCheckColor) {

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

CVector3 IcraDesLoopFunction::GetRandomPosition() {
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

REGISTER_LOOP_FUNCTIONS(IcraDesLoopFunction, "icra_des_loop_function");
