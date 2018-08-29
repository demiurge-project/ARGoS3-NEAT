/**
  * @file <loop-functions/PwStopLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "PwLoopFunc.h"

/****************************************/
/****************************************/

PwLoopFunction::PwLoopFunction() {
    m_cCoordSpotB = CVector2(0.0,0.62);
    m_cCoordSpotW = CVector2(0.0,-0.62);
    m_fRadiusSpot = 0.30;
    m_fObjectiveFunction = 0;
}

/****************************************/
/****************************************/

PwLoopFunction::PwLoopFunction(const PwLoopFunction& orig) {
}

/****************************************/
/****************************************/

PwLoopFunction::~PwLoopFunction() {}

/****************************************/
/****************************************/

void PwLoopFunction::Destroy() {
}

/****************************************/
/****************************************/

argos::CColor PwLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

    Real db = (m_cCoordSpotB - c_position_on_plane).Length();

    if (db <= m_fRadiusSpot)
        return CColor::BLACK;
    else{
        Real dw = (m_cCoordSpotW - c_position_on_plane).Length();
        if (dw <= m_fRadiusSpot)
            return CColor::WHITE;
    }

    return CColor::GRAY50;
}

/****************************************/
/****************************************/

void PwLoopFunction::Init(TConfigurationNode& t_tree) {
    AutoMoDeLoopFunctions::Init(t_tree);
    GetRobotPositions(true);
}

/****************************************/
/****************************************/

void PwLoopFunction::Reset() {
    AutoMoDeLoopFunctions::Reset();
    m_fObjectiveFunction = 0;
}

/****************************************/
/****************************************/

void PwLoopFunction::PostStep() {
    UInt32 unClock = GetSpace().GetSimulationClock();

    ArenaControl(unClock);
    GetRobotPositions(false);
    m_fObjectiveFunction += GetMissionScore(unClock);
    m_tMemPositions = m_tPositions;
}

/****************************************/
/****************************************/

void PwLoopFunction::PostExperiment() {

  LOG << m_fObjectiveFunction << std::endl;
}

/****************************************/
/****************************************/

Real PwLoopFunction::GetObjectiveFunction() {

  return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void PwLoopFunction::ArenaControl(UInt32 unClock) {

    if (unClock == 1 && m_unPwConfig == 0)
        m_pcArena->SetArenaColor(CColor::GREEN);

    if (unClock == m_unPwTime && m_unPwConfig == 0)
        m_pcArena->SetArenaColor(CColor::RED);

    if (unClock == 1 && m_unPwConfig == 1)
        m_pcArena->SetArenaColor(CColor::RED);

    if (unClock == m_unPwTime && m_unPwConfig == 1)
        m_pcArena->SetArenaColor(CColor::GREEN);


    return;
}

/****************************************/
/****************************************/

Real PwLoopFunction::GetMissionScore(UInt32 unClock){

    Real unScore = 0;

    if (m_unPwExp == 0 && m_unPwConfig == 0){
        unScore += PwFunctionMove(unClock,0,m_unPwTime,false);
        unScore += PwFunctionStop(unClock,m_unPwTime,(2*m_unPwTime));
        return unScore;
    }

    if (m_unPwExp == 0 && m_unPwConfig == 1){
        unScore += PwFunctionStop(unClock,0,m_unPwTime);
        unScore += PwFunctionMove(unClock,m_unPwTime,(2*m_unPwTime),false);
        return unScore;
    }

    if (m_unPwExp == 1 && m_unPwConfig == 0){
        unScore += PwFunctionAgg(unClock,0,m_unPwTime,false);
        unScore += PwFunctionAgg(unClock,m_unPwTime,(2*m_unPwTime),true);
        return unScore;
    }

    if (m_unPwExp == 1 && m_unPwConfig == 1){
        unScore += PwFunctionAgg(unClock,0,m_unPwTime,true);
        unScore += PwFunctionAgg(unClock,m_unPwTime,(2*m_unPwTime),false);
        return unScore;
    }

    if (m_unPwExp == 2 && m_unPwConfig == 0){
        unScore += PwFunctionMove(unClock,0,m_unPwTime,true);
        unScore += PwFunctionAgg(unClock,m_unPwTime,(2*m_unPwTime),false);
        return unScore;
    }

    if (m_unPwExp == 2 && m_unPwConfig == 1){
        unScore += PwFunctionAgg(unClock,0,m_unPwTime,false);
        unScore += PwFunctionMove(unClock,m_unPwTime,(2*m_unPwTime),true);
        return unScore;
    }

    if (m_unPwExp == 3 && m_unPwConfig == 0){
        unScore += PwFunctionAgg(unClock,0,m_unPwTime,false);
        unScore += PwFunctionFlee(unClock,m_unPwTime,(2*m_unPwTime));
        return unScore;
    }

    if (m_unPwExp == 3 && m_unPwConfig == 1){
        unScore += PwFunctionFlee(unClock,0,m_unPwTime);
        unScore += PwFunctionAgg(unClock,m_unPwTime,(2*m_unPwTime),false);
        return unScore;
    }

    if (m_unPwExp == 4){
        unScore += PwFunctionMove(unClock,0,m_unPwTime,false);
        return unScore;
    }

    if (m_unPwExp == 5){
        unScore += PwFunctionStop(unClock,0,m_unPwTime);
        return unScore;
    }

    if (m_unPwExp == 6){
        unScore += PwFunctionAgg(unClock,0,m_unPwTime,false);
        return unScore;
    }

    if (m_unPwExp == 7){
        unScore += PwFunctionAgg(unClock,0,m_unPwTime,true);
        return unScore;
    }

    if (m_unPwExp == 8){
        unScore += PwFunctionMove(unClock,0,m_unPwTime,true);
        return unScore;
    }

    if (m_unPwExp == 9){
        unScore += PwFunctionFlee(unClock,0,m_unPwTime);
        return unScore;
    }

    return 0;
}

/****************************************/
/****************************************/

void PwLoopFunction::GetRobotPositions(bool bSavePositions) {
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

Real PwLoopFunction::PwFunctionStop(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime) {

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

Real PwLoopFunction::PwFunctionMove(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime, bool bCheckColor) {

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

Real PwLoopFunction::PwFunctionAgg(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime, bool bWhiteColor) {

    if (unClock > unInitTime && unClock <= unEndTime){
        Real unScore = 0;
        TPosMap::iterator it;
        for (it = m_tPositions.begin(); it != m_tPositions.end(); ++it) {

            if (bWhiteColor){
                if(GetFloorColor(it->second) != CColor::WHITE)
                    unScore+=1;
            }
            else {
                if(GetFloorColor(it->second) != CColor::BLACK)
                    unScore+=1;
            }
        }
        return unScore;
    }

    return 0;
}

/****************************************/
/****************************************/

Real PwLoopFunction::PwFunctionFlee(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime) {

    if (unClock > unInitTime && unClock <= unEndTime){
        Real unScore = 0;
        Real d = 0;
        TPosMap::iterator it;
        for (it = m_tPositions.begin(); it != m_tPositions.end(); ++it) {
            d = ((it->second - m_cCoordSpotB).Length())-m_fRadiusSpot;
                        if(d < 0)
                            unScore+=1;
                        else{
                            unScore+=(1-(d/1.42));
                        }
            //Real d = (it->second - m_cCoordSpotB).Length();
            //if (d < 1.62)
            //    unScore+=1;
        }
        return unScore;
    }

    return 0;
}

/****************************************/
/****************************************/

CVector3 PwLoopFunction::GetRandomPosition() {
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

REGISTER_LOOP_FUNCTIONS(PwLoopFunction, "pw_loop_function");
