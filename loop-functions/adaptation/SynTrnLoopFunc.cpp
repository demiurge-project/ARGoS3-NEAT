/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "SynTrnLoopFunc.h"

/****************************************/
/****************************************/

SynTrnLoopFunction::SynTrnLoopFunction() {
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

SynTrnLoopFunction::SynTrnLoopFunction(const SynTrnLoopFunction& orig) {
}

/****************************************/
/****************************************/

SynTrnLoopFunction::~SynTrnLoopFunction() {}

/****************************************/
/****************************************/

void SynTrnLoopFunction::Destroy() {
}

/****************************************/
/****************************************/

argos::CColor SynTrnLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

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

void SynTrnLoopFunction::Init(TConfigurationNode& t_tree) {
    CNeatLoopFunctions::Init(t_tree);
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

void SynTrnLoopFunction::Reset() {
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

void SynTrnLoopFunction::PostStep() {
    UInt32 unClock = GetSpace().GetSimulationClock();

    if (unClock == 600 && m_fRandomIndex <= 0.5)
        m_fObjectiveFunction += GetStepScore(false);

    if (unClock == 600 && m_fRandomIndex > 0.5)
        m_fObjectiveFunction += GetStepScore(true);

    if (unClock == 1200 && m_fRandomIndex <= 0.5)
        m_fObjectiveFunction += GetStepScore(true);

    if (unClock == 1200 && m_fRandomIndex > 0.5)
        m_fObjectiveFunction += GetStepScore(false);

    ArenaControl(unClock);

    if (m_bEvaluate){
        if (unClock == 600)
            m_fObjectiveFunctionT1 = m_fObjectiveFunction;
        if (unClock == 1200)
            m_fObjectiveFunctionT2 = m_fObjectiveFunction - m_fObjectiveFunctionT1;
    }

}

/****************************************/
/****************************************/

void SynTrnLoopFunction::PostExperiment() {
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

Real SynTrnLoopFunction::GetObjectiveFunction() {
    m_fObjectiveFunction = -m_fObjectiveFunction;
    return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void SynTrnLoopFunction::ArenaControl(UInt32 unClock) {

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

Real SynTrnLoopFunction::GetStepScore(bool bConsensus) {

    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CColor cEpuckColor = CColor::BLACK;
    Real fBlack = 0;
    Real fMagenta = 0;
    Real fYellow = 0;
    Real fCyan = 0;
    Real fVariance = 0;
    Real fScore = 0;

    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {

        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);

        cEpuckColor = pcEpuck->GetLEDEquippedEntity().GetLED(10).GetColor();

        if (cEpuckColor == CColor::BLACK)
            fBlack+=1;

        if (cEpuckColor == CColor::MAGENTA)
            fMagenta+=1;

        if (cEpuckColor == CColor::YELLOW)
            fYellow+=1;

        if (cEpuckColor == CColor::CYAN)
            fCyan+=1;
    }

    fVariance = (1.0/4.0) * (pow((fBlack-5.0),2.0) + pow((fMagenta-5.0),2.0) + pow((fYellow-5.0),2.0) + pow((fCyan-5.0),2.0));

    if (bConsensus)
        fScore = 75.0 - fVariance;
    else
        fScore = fVariance;

    return fScore;
}

/****************************************/
/****************************************/

Real SynTrnLoopFunction::AdditionalMetrics(){
    Real fNewMetric = 999999;
    if (m_unPwExp == 1)
        fNewMetric = m_fObjectiveFunctionT1;
    else if (m_unPwExp == 2)
        fNewMetric = m_fObjectiveFunctionT2;

    return fNewMetric;
}

/****************************************/
/****************************************/

CVector3 SynTrnLoopFunction::GetRandomPosition() {
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

REGISTER_LOOP_FUNCTIONS(SynTrnLoopFunction, "syn_trn_loop_function");
