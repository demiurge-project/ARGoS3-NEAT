/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "ForagMtcLoopFunc.h"

/****************************************/
/****************************************/

ForagMtcLoopFunction::ForagMtcLoopFunction() {
    m_fObjectiveFunction = 0;
    m_fObjectiveFunctionT1 = 0;
    m_fObjectiveFunctionT2 = 0;
    m_fObjectiveFunctionT3 = 0;
    m_fObjectiveFunctionR = 0;
    m_fObjectiveFunctionG = 0;
    m_fObjectiveFunctionB = 0;
    m_fRandomIndex = 0;
    m_cCoordSpot1 = CVector2(0.54, 0.54);
    m_cCoordSpot2 = CVector2(0.765, 0.00);
    m_cCoordSpot3 = CVector2(0.54,-0.54);
    m_fSafeDist = 0.16;
    m_fRadiusSpot = 0.125;
    m_cForagColor = CColor::BLACK;
    m_bEvaluate = false;
}

/****************************************/
/****************************************/

ForagMtcLoopFunction::ForagMtcLoopFunction(const ForagMtcLoopFunction& orig) {
}

/****************************************/
/****************************************/

ForagMtcLoopFunction::~ForagMtcLoopFunction() {}

/****************************************/
/****************************************/

void ForagMtcLoopFunction::Destroy() {
}

/****************************************/
/****************************************/

argos::CColor ForagMtcLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

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

void ForagMtcLoopFunction::Init(TConfigurationNode& t_tree) {
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

void ForagMtcLoopFunction::Reset() {
    CNeatLoopFunctions::Reset();
    m_fObjectiveFunction = 0;
    m_fObjectiveFunctionT1 = 0;
    m_fObjectiveFunctionT2 = 0;
    m_fObjectiveFunctionT3 = 0;
    m_fObjectiveFunctionR = 0;
    m_fObjectiveFunctionG = 0;
    m_fObjectiveFunctionB = 0;
    if (m_unPwConfig  == 0)
        m_fRandomIndex = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
    else{
        m_fRandomIndex = (m_unPwConfig * 0.167) - (0.167/2) ;
    }
    m_cForagColor = CColor::BLACK;

    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
        m_tMemObjects[pcEpuck] = false;
        m_tMemColors[pcEpuck] = CColor::BLACK;
    }
}

/****************************************/
/****************************************/

void ForagMtcLoopFunction::PostStep() {
    UInt32 unClock = GetSpace().GetSimulationClock();
    m_fObjectiveFunction += GetStepScore(unClock);
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

void ForagMtcLoopFunction::PostExperiment() {
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

Real ForagMtcLoopFunction::GetObjectiveFunction() {
  return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void ForagMtcLoopFunction::ArenaControl(UInt32 unClock) {

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

void ForagMtcLoopFunction::ArenaControlSelector(UInt32 unClock) {

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

void ForagMtcLoopFunction::ArenaConfigOne () {
    m_pcArena->SetArenaColor(CColor::BLACK);
    m_pcArena->SetBoxColor(2,1,CColor::GREEN);
    m_pcArena->SetBoxColor(2,7,CColor::GREEN);
    m_pcArena->SetBoxColor(2,8,CColor::GREEN);
    m_cForagColor = CColor::YELLOW;

    return;
}

/****************************************/
/****************************************/

void ForagMtcLoopFunction::ArenaConfigTwo () {
    m_pcArena->SetArenaColor(CColor::BLACK);
    m_pcArena->SetBoxColor(2,1,CColor::BLUE);
    m_pcArena->SetBoxColor(2,7,CColor::BLUE);
    m_pcArena->SetBoxColor(2,8,CColor::BLUE);
    m_cForagColor = CColor::CYAN;

    return;
}

/****************************************/
/****************************************/

void ForagMtcLoopFunction::ArenaConfigThree () {
    m_pcArena->SetArenaColor(CColor::BLACK);
    m_pcArena->SetBoxColor(2,1,CColor::RED);
    m_pcArena->SetBoxColor(2,7,CColor::RED);
    m_pcArena->SetBoxColor(2,8,CColor::RED);
    m_cForagColor = CColor::MAGENTA;

    return;
}

/****************************************/
/****************************************/

Real ForagMtcLoopFunction::GetStepScore(UInt32 unClock) {

    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    CColor cEpuckColor = CColor::BLACK;
    Real fScore = 0;
    Real fDa, fDb, fDc;

    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {

        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);

        cEpuckColor = pcEpuck->GetLEDEquippedEntity().GetLED(10).GetColor();

        cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                           pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());


        if (cEpuckColor == m_cForagColor) {

            fDa = (cEpuckPosition - m_cCoordSpot1).Length();
            fDb = (cEpuckPosition - m_cCoordSpot2).Length();
            fDc = (cEpuckPosition - m_cCoordSpot3).Length();

            if (fDa <= m_fSafeDist || fDb <= m_fSafeDist || fDc <= m_fSafeDist){
                m_tMemObjects[pcEpuck] = true;
                m_tMemColors[pcEpuck] = cEpuckColor;
            }
        }

        if (m_tMemObjects[pcEpuck] == true && cEpuckPosition.GetX() <= -0.34) {
            if (m_bEvaluate){
                if (m_tMemColors[pcEpuck] == CColor::MAGENTA)
                     m_fObjectiveFunctionR-=1;
                else if (m_tMemColors[pcEpuck] == CColor::YELLOW)
                     m_fObjectiveFunctionG-=1;
                else if (m_tMemColors[pcEpuck] == CColor::CYAN)
                     m_fObjectiveFunctionB-=1;
                 m_tMemColors[pcEpuck] = CColor::BLACK;
            }
            m_tMemObjects[pcEpuck] = false;
            fScore-=1;
        }
    }

    return fScore;
}

/****************************************/
/****************************************/

Real ForagMtcLoopFunction::AdditionalMetrics(){
    Real fNewMetric = 999999;
    if (m_unPwExp == 1)
        fNewMetric = m_fObjectiveFunctionT1;
    else if (m_unPwExp == 2)
        fNewMetric = m_fObjectiveFunctionT2;
    else if (m_unPwExp == 3)
        fNewMetric = m_fObjectiveFunctionT3;
    else if (m_unPwExp == 4)
        fNewMetric = m_fObjectiveFunctionR;
    else if (m_unPwExp == 5)
        fNewMetric = m_fObjectiveFunctionG;
    else if (m_unPwExp == 6)
        fNewMetric = m_fObjectiveFunctionB;

    return fNewMetric;
}

/****************************************/
/****************************************/

CVector3 ForagMtcLoopFunction::GetRandomPosition() {
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

REGISTER_LOOP_FUNCTIONS(ForagMtcLoopFunction, "forag_mtc_loop_function");
