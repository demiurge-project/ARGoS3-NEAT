/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "IcraDesInbLoopFunc.h"

/****************************************/
/****************************************/

IcraDesInbLoopFunction::IcraDesInbLoopFunction() {
    m_fObjectiveFunction = 0;
}

/****************************************/
/****************************************/

IcraDesInbLoopFunction::IcraDesInbLoopFunction(const IcraDesInbLoopFunction& orig) {
}

/****************************************/
/****************************************/

IcraDesInbLoopFunction::~IcraDesInbLoopFunction() {}

/****************************************/
/****************************************/

void IcraDesInbLoopFunction::Destroy() {
}

/****************************************/
/****************************************/

argos::CColor IcraDesInbLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

        if (c_position_on_plane.GetY() <= -0.655 || c_position_on_plane.GetX() >= 0.655)
            return CColor::WHITE;
        else if(c_position_on_plane.GetX() <= -0.655 || c_position_on_plane.GetY() >= 0.655)
            return CColor::BLACK;

    return CColor::GRAY50;
}

/****************************************/
/****************************************/

void IcraDesInbLoopFunction::Init(TConfigurationNode& t_tree) {
    AutoMoDeLoopFunctions::Init(t_tree);
    GetRobotPositions(true);
}

/****************************************/
/****************************************/

void IcraDesInbLoopFunction::Reset() {
    AutoMoDeLoopFunctions::Reset();
    m_fObjectiveFunction = 0;
}

/****************************************/
/****************************************/

void IcraDesInbLoopFunction::PostStep() {
    UInt32 unClock = GetSpace().GetSimulationClock();

    ArenaControl(unClock);
    GetRobotPositions(false);
    m_fObjectiveFunction += GetMissionScore(unClock);
    m_tMemPositions = m_tPositions;
}

/****************************************/
/****************************************/

void IcraDesInbLoopFunction::PostExperiment() {

  LOG << m_fObjectiveFunction << std::endl;
}

/****************************************/
/****************************************/

Real IcraDesInbLoopFunction::GetObjectiveFunction() {

  return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void IcraDesInbLoopFunction::ArenaControl(UInt32 unClock) {

    if (unClock == 1)
        m_pcArena->SetArenaColor(CColor::BLACK);

    if (unClock == m_unPwTime)
        m_pcArena->SetArenaColor(CColor::GREEN);

    return;
}

/****************************************/
/****************************************/

Real IcraDesInbLoopFunction::GetMissionScore(UInt32 unClock){

    Real unScore = 0;

    unScore += PwFunctionMove(unClock,0,m_unPwTime,false);
    unScore += PwFunctionAgg(unClock,m_unPwTime,(2*m_unPwTime),true);

    return unScore;

}

/****************************************/
/****************************************/

void IcraDesInbLoopFunction::GetRobotPositions(bool bSavePositions) {
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

Real IcraDesInbLoopFunction::PwFunctionAgg(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime, bool bWhiteColor) {

    if (unClock == unEndTime){
        Real unScore = 0;
        TPosMap::iterator it;
        for (it = m_tPositions.begin(); it != m_tPositions.end(); ++it) {

                if (bWhiteColor){
                    if(it->second.GetY() > -0.62  && it->second.GetX() < 0.62)
                        unScore+=900;
                }
                else {
                    if(it->second.GetX() > -0.62 && it->second.GetY() < 0.62)
                        unScore+=900;
                }

        }
        return unScore;
    }

    return 0;
}

/****************************************/
/****************************************/

Real IcraDesInbLoopFunction::PwFunctionStop(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime) {

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

Real IcraDesInbLoopFunction::PwFunctionMove(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime, bool bCheckColor) {

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

CVector3 IcraDesInbLoopFunction::GetRandomPosition() {
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

REGISTER_LOOP_FUNCTIONS(IcraDesInbLoopFunction, "icra_des_inb_loop_function");
