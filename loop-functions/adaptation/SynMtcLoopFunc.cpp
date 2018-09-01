/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "SynMtcLoopFunc.h"

/****************************************/
/****************************************/

SynMtcLoopFunction::SynMtcLoopFunction() {
    m_fObjectiveFunction = 0;
    m_fObjectiveFunctionT1 = 0;
    m_fObjectiveFunctionT2 = 0;
    m_fObjectiveFunctionT3 = 0;
    m_fRandomIndex = 0;
    m_cCoordSpot1 = CVector2(0.54, 0.54);
    m_cCoordSpot2 = CVector2(0.765, 0.00);
    m_cCoordSpot3 = CVector2(0.54,-0.54);
    m_fRadiusSpot = 0.125;
    m_fSafeDist = 0.16;
    m_bEvaluate = false;
}

/****************************************/
/****************************************/

SynMtcLoopFunction::SynMtcLoopFunction(const SynMtcLoopFunction& orig) {
}

/****************************************/
/****************************************/

SynMtcLoopFunction::~SynMtcLoopFunction() {}

/****************************************/
/****************************************/

void SynMtcLoopFunction::Destroy() {
}

/****************************************/
/****************************************/

argos::CColor SynMtcLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

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

void SynMtcLoopFunction::Init(TConfigurationNode& t_tree) {
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

void SynMtcLoopFunction::Reset() {
    CNeatLoopFunctions::Reset();
    m_fObjectiveFunction = 0;
    m_fObjectiveFunctionT1 = 0;
    m_fObjectiveFunctionT2 = 0;
    m_fObjectiveFunctionT3 = 0;
    if (m_unPwConfig  == 0)
        m_fRandomIndex = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
    else{
        m_fRandomIndex = (m_unPwConfig * 0.167) - (0.167/2) ;
    }
}

/****************************************/
/****************************************/

void SynMtcLoopFunction::PostStep() {
    UInt32 unClock = GetSpace().GetSimulationClock();
    m_fObjectiveFunction += GetStepScore();
    ArenaControl(unClock);

    if (m_bEvaluate){
        if (unClock == 400)
            m_fObjectiveFunctionT1 = m_fObjectiveFunction;
        if (unClock == 800)
            m_fObjectiveFunctionT2 = m_fObjectiveFunction - m_fObjectiveFunctionT1;
        if (unClock == 1200)
            m_fObjectiveFunctionT3 = m_fObjectiveFunction - m_fObjectiveFunctionT2 - m_fObjectiveFunctionT1;
    }
}

/****************************************/
/****************************************/

void SynMtcLoopFunction::PostExperiment() {
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

Real SynMtcLoopFunction::GetObjectiveFunction() {
    m_fObjectiveFunction = -m_fObjectiveFunction;
    return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void SynMtcLoopFunction::ArenaControl(UInt32 unClock) {

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

void SynMtcLoopFunction::ArenaControlSelector(UInt32 unClock) {

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

void SynMtcLoopFunction::ArenaConfigOne () {
    m_pcArena->SetArenaColor(CColor::BLACK);
    m_pcArena->SetBoxColor(2,1,CColor::RED);
    m_pcArena->SetBoxColor(2,8,CColor::GREEN);
    m_pcArena->SetBoxColor(2,7,CColor::BLUE);
    m_cSynColor1 = CColor::MAGENTA;
    m_cSynColor2 = CColor::YELLOW;
    m_cSynColor3 = CColor::CYAN;


    return;
}

/****************************************/
/****************************************/

void SynMtcLoopFunction::ArenaConfigTwo () {
    m_pcArena->SetArenaColor(CColor::BLACK);
    m_pcArena->SetBoxColor(2,1,CColor::BLUE);
    m_pcArena->SetBoxColor(2,8,CColor::RED);
    m_pcArena->SetBoxColor(2,7,CColor::GREEN);
    m_cSynColor1 = CColor::CYAN;
    m_cSynColor2 = CColor::MAGENTA;
    m_cSynColor3 = CColor::YELLOW;

    return;
}

/****************************************/
/****************************************/

void SynMtcLoopFunction::ArenaConfigThree () {
    m_pcArena->SetArenaColor(CColor::BLACK);
    m_pcArena->SetBoxColor(2,1,CColor::GREEN);
    m_pcArena->SetBoxColor(2,8,CColor::BLUE);
    m_pcArena->SetBoxColor(2,7,CColor::RED);
    m_cSynColor1 = CColor::YELLOW;
    m_cSynColor2 = CColor::CYAN;
    m_cSynColor3 = CColor::MAGENTA;

    return;
}

/****************************************/
/****************************************/

Real SynMtcLoopFunction::GetStepScore() {

    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    CColor cEpuckColor = CColor::BLACK;
    Real fScore = 0;
    Real fDist;
    bool bScoreC1 = false;
    bool bScoreC2 = false;
    bool bScoreC3 = false;

    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {

        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);

        cEpuckColor = pcEpuck->GetLEDEquippedEntity().GetLED(10).GetColor();

        cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                           pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

        if (cEpuckColor == m_cSynColor1){
            fDist = (cEpuckPosition - m_cCoordSpot1).Length();
            if (fDist <= m_fSafeDist)
                bScoreC1 = true;
        }
        else if (cEpuckColor == m_cSynColor2){
            fDist = (cEpuckPosition - m_cCoordSpot2).Length();
            if (fDist <= m_fSafeDist)
                bScoreC2 = true;
        }
        else if (cEpuckColor == m_cSynColor3){
            fDist = (cEpuckPosition - m_cCoordSpot3).Length();
            if (fDist <= m_fSafeDist)
                bScoreC3 = true;
        }
    }

    if (bScoreC1 && bScoreC2 && bScoreC3)
        fScore = -1;

    return fScore;
}

/****************************************/
/****************************************/

Real SynMtcLoopFunction::AdditionalMetrics(){
    Real fNewMetric = 999999;
    if (m_unPwExp == 1)
        fNewMetric = m_fObjectiveFunctionT1;
    else if (m_unPwExp == 2)
        fNewMetric = m_fObjectiveFunctionT2;
    else if (m_unPwExp == 3)
        fNewMetric = m_fObjectiveFunctionT3;

    return fNewMetric;
}

/****************************************/
/****************************************/

CVector3 SynMtcLoopFunction::GetRandomPosition() {
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


REGISTER_LOOP_FUNCTIONS(SynMtcLoopFunction, "syn_mtc_loop_function");
