/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "StopTrnLoopFunc.h"

/****************************************/
/****************************************/

StopTrnLoopFunction::StopTrnLoopFunction() {
    m_fObjectiveFunction = 0;
    m_fObjectiveFunctionT1 = 0;
    m_fObjectiveFunctionT2 = 0;
    m_fRandomIndex = 0;
    m_cCoordSpot1 = CVector2(0.54, 0.54);
    m_cCoordSpot2 = CVector2(0.765, 0.00);
    m_cCoordSpot3 = CVector2(0.54,-0.54);
    m_fRadiusSpot = 0.125;
    m_bEvaluate = false;
}

/****************************************/
/****************************************/

StopTrnLoopFunction::StopTrnLoopFunction(const StopTrnLoopFunction& orig) {
}

/****************************************/
/****************************************/

StopTrnLoopFunction::~StopTrnLoopFunction() {}

/****************************************/
/****************************************/

void StopTrnLoopFunction::Destroy() {
}

/****************************************/
/****************************************/

argos::CColor StopTrnLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

    if (c_position_on_plane.GetX() <= -0.375)
        return CColor::WHITE;
    else {
        Real d = (m_cCoordSpot1 - c_position_on_plane).Length();
        if ( d <= m_fRadiusSpot)
            return CColor::WHITE;
        else{
            d = (m_cCoordSpot2 - c_position_on_plane).Length();
            if (d <= m_fRadiusSpot)
                return CColor::WHITE;
            else {
                d = (m_cCoordSpot3 - c_position_on_plane).Length();
                if (d <= m_fRadiusSpot)
                    return CColor::WHITE;
            }
        }
    }

    return CColor::GRAY50;
}

/****************************************/
/****************************************/

void StopTrnLoopFunction::Init(TConfigurationNode& t_tree) {
    CNeatLoopFunctions::Init(t_tree);
    GetRobotPositions(true);

    if (m_unPwConfig  == 0)
        m_fRandomIndex = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
    else{
        m_fRandomIndex = (m_unPwConfig * 0.5) - (0.5/2) ;
    }
    if (m_unPwExp != 0)
        m_bEvaluate = true;
}

/****************************************/
/****************************************/

void StopTrnLoopFunction::Reset() {
    CNeatLoopFunctions::Reset();
    m_fObjectiveFunction = 0;
    m_fObjectiveFunctionT1 = 0;
    m_fObjectiveFunctionT2 = 0;
    if (m_unPwConfig  == 0)
        m_fRandomIndex = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
    else{
        m_fRandomIndex = (m_unPwConfig * 0.5) - (0.5/2) ;
    }
}

/****************************************/
/****************************************/

void StopTrnLoopFunction::PostStep() {
    UInt32 unClock = GetSpace().GetSimulationClock();

    ArenaControl(unClock);
    GetRobotPositions(false);
    m_fObjectiveFunction += GetMissionScore(unClock);
    m_tMemPositions = m_tPositions;

    if (m_bEvaluate){
        if (unClock == 600)
            m_fObjectiveFunctionT1 = m_fObjectiveFunction;
        if (unClock == 1200)
            m_fObjectiveFunctionT2 = m_fObjectiveFunction - m_fObjectiveFunctionT1;
    }
}

/****************************************/
/****************************************/

void StopTrnLoopFunction::PostExperiment() {

    if (m_bEvaluate){
        Real fNewMetric = AdditionalMetrics();
        LOG << fNewMetric << std::endl;
        m_fObjectiveFunction = fNewMetric;
    }
    else
        LOG << m_fObjectiveFunction << std::endl;
}

/****************************************/
/****************************************/

Real StopTrnLoopFunction::GetObjectiveFunction() {

  return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void StopTrnLoopFunction::ArenaControl(UInt32 unClock) {

    if (unClock == 1 && m_fRandomIndex <= 0.5)
        m_pcArena->SetArenaColor(CColor::BLACK);

    if (unClock == 1 && m_fRandomIndex > 0.5){
        m_pcArena->SetArenaColor(CColor::BLACK);
        m_pcArena->SetWallColor(4,CColor::RED);
        m_pcArena->SetWallColor(3,CColor::RED);
        m_pcArena->SetWallColor(5,CColor::RED);
    }

    if (unClock == m_unPwTime && m_fRandomIndex <= 0.5) {
        m_pcArena->SetArenaColor(CColor::BLACK);
        m_pcArena->SetWallColor(4,CColor::RED);
        m_pcArena->SetWallColor(3,CColor::RED);
        m_pcArena->SetWallColor(5,CColor::RED);
    }

    if (unClock == m_unPwTime && m_fRandomIndex > 0.5)
        m_pcArena->SetArenaColor(CColor::BLACK);

    return;
}

/****************************************/
/****************************************/

Real StopTrnLoopFunction::GetMissionScore(UInt32 unClock){

    Real unScore = 0;

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

    return 0;
}

/****************************************/
/****************************************/

void StopTrnLoopFunction::GetRobotPositions(bool bSavePositions) {
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

Real StopTrnLoopFunction::PwFunctionStop(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime) {

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

Real StopTrnLoopFunction::PwFunctionMove(UInt32 unClock, UInt32 unInitTime, UInt32 unEndTime, bool bCheckColor) {

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

Real StopTrnLoopFunction::AdditionalMetrics(){
    Real fNewMetric = 999999;
    if (m_unPwExp == 1)
        fNewMetric = m_fObjectiveFunctionT1;
    else if (m_unPwExp == 2)
        fNewMetric = m_fObjectiveFunctionT2;

    return fNewMetric;
}

/****************************************/
/****************************************/

CVector3 StopTrnLoopFunction::GetRandomPosition() {
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

  return CVector3((fPosX * 0.8) -0.60 , fPosY*2, 0);
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(StopTrnLoopFunction, "stop_trn_loop_function");
