/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "StopMtcLoopFunc.h"

/****************************************/
/****************************************/

StopMtcLoopFunction::StopMtcLoopFunction() {
    m_fObjectiveFunction = 0;
    m_fRandomIndex = 0;
    m_cCoordSpot1 = CVector2(0.54, 0.54);
    m_cCoordSpot2 = CVector2(0.765, 0.00);
    m_cCoordSpot3 = CVector2(0.54,-0.54);
    m_cStopCoord = CVector2(0,0);
    m_fSafeDist = 0.16;
    m_fRadiusSpot = 0.125;
    m_bStopSignal = false;
    m_cStopColor = CColor::BLACK;
    m_cTriggerColor = CColor::BLACK;
    m_bInit = false;
    m_bEvaluate = false;
}

/****************************************/
/****************************************/

StopMtcLoopFunction::StopMtcLoopFunction(const StopMtcLoopFunction& orig) {
}

/****************************************/
/****************************************/

StopMtcLoopFunction::~StopMtcLoopFunction() {}

/****************************************/
/****************************************/

void StopMtcLoopFunction::Destroy() {
}

/****************************************/
/****************************************/

argos::CColor StopMtcLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

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

void StopMtcLoopFunction::Init(TConfigurationNode& t_tree) {
    CNeatLoopFunctions::Init(t_tree);
    if (m_unPwConfig  == 0)
        m_fRandomIndex = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
    else{
        m_fRandomIndex = (m_unPwConfig * 0.167) - (0.167/2) ;
    }
    if (m_unPwExp != 0)
        m_bEvaluate = true;
}

/****************************************/
/****************************************/

void StopMtcLoopFunction::Reset() {
    CNeatLoopFunctions::Reset();
    m_fObjectiveFunction = 0;
    m_bInit = false;
    m_bStopSignal = false;
    m_cStopColor = CColor::BLACK;
    m_cTriggerColor = CColor::BLACK;
    if (m_unPwConfig  == 0)
        m_fRandomIndex = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
    else{
        m_fRandomIndex = (m_unPwConfig * 0.167) - (0.167/2) ;
    }
}

/****************************************/
/****************************************/

void StopMtcLoopFunction::PostStep() {
    UInt32 unClock = GetSpace().GetSimulationClock();
    ArenaControl(unClock);
    m_fObjectiveFunction += GetStepScore(unClock);
}

/****************************************/
/****************************************/

void StopMtcLoopFunction::PostExperiment() {
    if (!m_bStopSignal)
        m_unStopTime = 1200;

    Real fTimeScore = (Real)m_unStopTime * (Real)m_unNumberRobots;
    m_fObjectiveFunction += fTimeScore;

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

Real StopMtcLoopFunction::GetObjectiveFunction() {
  return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void StopMtcLoopFunction::ArenaControl(UInt32 unClock) {

    if (unClock == 1)
        ArenaControlSelector(unClock);

    if (unClock == 400)
        ArenaControlSelector(unClock);

    if (unClock == 800)
        ArenaControlSelector(unClock);

    return;
}

/****************************************/
/****************************************/

void StopMtcLoopFunction::ArenaControlSelector(UInt32 unClock) {

    Real fSelector = 0.167;

    if (m_fRandomIndex < (fSelector)) {
        if (unClock == 1)
            ArenaConfigOne();
        if (unClock == 400)
            ArenaConfigTwo();
        if (unClock == 800)
            ArenaConfigThree();
        return;
    }

    if (m_fRandomIndex < (2*fSelector)) {
        if (unClock == 1)
            ArenaConfigOne();
        if (unClock == 400)
            ArenaConfigThree();
        if (unClock == 800)
            ArenaConfigTwo();
        return;
    }

    if (m_fRandomIndex < (3*fSelector)) {
        if (unClock == 1)
            ArenaConfigTwo();
        if (unClock == 400)
            ArenaConfigOne();
        if (unClock == 800)
            ArenaConfigThree();
        return;
    }

    if (m_fRandomIndex < (4*fSelector)) {
        if (unClock == 1)
            ArenaConfigTwo();
        if (unClock == 400)
            ArenaConfigThree();
        if (unClock == 800)
            ArenaConfigOne();
        return;
    }

    if (m_fRandomIndex < (5*fSelector)) {
        if (unClock == 1)
            ArenaConfigThree();
        if (unClock == 400)
            ArenaConfigOne();
        if (unClock == 800)
            ArenaConfigTwo();
        return;
    }

    if (m_fRandomIndex < (6*fSelector)) {
        if (unClock == 1)
            ArenaConfigThree();
        if (unClock == 400)
            ArenaConfigTwo();
        if (unClock == 800)
            ArenaConfigOne();
        return;
    }

    return;
}

/****************************************/
/****************************************/

void StopMtcLoopFunction::ArenaConfigOne () {
    m_pcArena->SetArenaColor(CColor::BLACK);
    m_pcArena->SetBoxColor(2,1,CColor::GREEN);
    m_cStopColor = CColor::YELLOW;
    m_cStopCoord = m_cCoordSpot1;

    return;
}

/****************************************/
/****************************************/

void StopMtcLoopFunction::ArenaConfigTwo () {
    m_pcArena->SetArenaColor(CColor::BLACK);
    m_pcArena->SetBoxColor(2,8,CColor::BLUE);
    m_cStopColor = CColor::CYAN;
    m_cStopCoord = m_cCoordSpot2;


    return;
}

/****************************************/
/****************************************/

void StopMtcLoopFunction::ArenaConfigThree () {
    m_pcArena->SetArenaColor(CColor::BLACK);
    m_pcArena->SetBoxColor(2,7,CColor::RED);
    m_cStopColor = CColor::MAGENTA;
    m_cStopCoord = m_cCoordSpot3;

    return;
}

/****************************************/
/****************************************/

Real StopMtcLoopFunction::GetStepScore(UInt32 unClock) {

    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    CColor cEpuckColor = CColor::BLACK;
    Real fScore = 0;
    Real dt, dm;

    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {

        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);

        cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                           pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

        if (m_bInit) {

            dm = (cEpuckPosition - m_tMemPositions[pcEpuck]).Length();

            if (!m_bStopSignal) {

                cEpuckColor = pcEpuck->GetLEDEquippedEntity().GetLED(10).GetColor();
                dt = (cEpuckPosition - m_cStopCoord).Length();

                if (dm <= 0.0005)
                    fScore+=1;

                if (dt <= m_fSafeDist)
                    if (cEpuckColor == m_cStopColor) {
                        m_cTriggerColor = m_cStopColor;
                        m_bStopSignal = true;
                        m_unStopTime = unClock;
                    }
            }
            else
                if (dm > 0.0005)
                    fScore+=1;
        }
        else
            m_bInit = true;

        m_tMemPositions[pcEpuck] = cEpuckPosition;
    }

    return fScore;
}

/****************************************/
/****************************************/

Real StopMtcLoopFunction::AdditionalMetrics(){
    Real fNewMetric = 999999;
    if (m_unPwExp == 1){
        fNewMetric = (Real)m_unStopTime;
    }
    else if (m_unPwExp == 2){
        if (m_cTriggerColor == CColor::BLACK)
            fNewMetric = 0;
        else if (m_cTriggerColor == CColor::MAGENTA)
            fNewMetric = 1;
        else if (m_cTriggerColor == CColor::YELLOW)
            fNewMetric = 2;
        else if (m_cTriggerColor == CColor::CYAN)
            fNewMetric = 3;
    }

    return fNewMetric;
}

/****************************************/
/****************************************/

CVector3 StopMtcLoopFunction::GetRandomPosition() {
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

REGISTER_LOOP_FUNCTIONS(StopMtcLoopFunction, "stop_mtc_loop_function");
