/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "ForagTrnLoopFunc.h"

/****************************************/
/****************************************/

ForagTrnLoopFunction::ForagTrnLoopFunction() {
    m_fObjectiveFunction = 0;
    m_fRandomIndex = 0;
    m_cCoordSpot1 = CVector2(0.54, 0.54);
    m_cCoordSpot2 = CVector2(0.765, 0.00);
    m_cCoordSpot3 = CVector2(0.54,-0.54);
    m_fSafeDist = 0.16;
    m_fRadiusSpot = 0.125;
    m_fTotalObjects = 0;
    m_fTotalRobots = 0;
    m_bEvaluate = false;
}

/****************************************/
/****************************************/

ForagTrnLoopFunction::ForagTrnLoopFunction(const ForagTrnLoopFunction& orig) {
}

/****************************************/
/****************************************/

ForagTrnLoopFunction::~ForagTrnLoopFunction() {}

/****************************************/
/****************************************/

void ForagTrnLoopFunction::Destroy() {
}

/****************************************/
/****************************************/

argos::CColor ForagTrnLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

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

void ForagTrnLoopFunction::Init(TConfigurationNode& t_tree) {
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

void ForagTrnLoopFunction::Reset() {
    CNeatLoopFunctions::Reset();
    m_fObjectiveFunction = 0;
    m_fTotalObjects = 0;
    m_fTotalRobots = 0;
    if (m_unPwConfig  == 0)
        m_fRandomIndex = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
    else{
        m_fRandomIndex = (m_unPwConfig * 0.5) - (0.5/2) ;
    }

    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
        m_tMemObjects[pcEpuck] = false;
    }
}

/****************************************/
/****************************************/

void ForagTrnLoopFunction::PostStep() {
    UInt32 unClock = GetSpace().GetSimulationClock();

    if (unClock <= 600 && m_fRandomIndex <= 0.5)
        GetStepScore(false);

    if (unClock == 600 && m_fRandomIndex > 0.5)
        GetStepScore(true);

    if (unClock == 1200 && m_fRandomIndex <= 0.5)
        GetStepScore(true);

    if (unClock >= 600 && m_fRandomIndex > 0.5)
        GetStepScore(false);

    ArenaControl(unClock);
}

/****************************************/
/****************************************/

void ForagTrnLoopFunction::PostExperiment() {

    if (m_fTotalRobots >= 12) {
        m_fObjectiveFunction = m_fTotalObjects;
    }

    else {
         m_fObjectiveFunction = 0;
    }

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

Real ForagTrnLoopFunction::GetObjectiveFunction() {
    m_fObjectiveFunction = -m_fObjectiveFunction;
    return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void ForagTrnLoopFunction::ArenaControl(UInt32 unClock) {

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

void ForagTrnLoopFunction::GetStepScore(bool bAggregate) {

    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    Real fDa, fDb, fDc;

    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {

        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);

        cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                           pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

        if (!bAggregate) {

            if (m_tMemObjects[pcEpuck] == true && cEpuckPosition.GetX() <= -0.34) {
                m_tMemObjects[pcEpuck] = false;
                m_fTotalObjects-=1;
            }
            else {
                fDa = (cEpuckPosition - m_cCoordSpot1).Length();
                fDb = (cEpuckPosition - m_cCoordSpot2).Length();
                fDc = (cEpuckPosition - m_cCoordSpot3).Length();

                if (fDa <= m_fSafeDist || fDb <= m_fSafeDist || fDc <= m_fSafeDist)
                    m_tMemObjects[pcEpuck] = true;
            }
        }
        else {
            if (cEpuckPosition.GetX() <= -0.34)
                m_fTotalRobots+=1;
        }
    }

    return;
}

/****************************************/
/****************************************/

Real ForagTrnLoopFunction::AdditionalMetrics(){
    Real fNewMetric = 999999;
    if (m_unPwExp == 1)
        fNewMetric = m_fTotalObjects;
    else if (m_unPwExp == 2)
        fNewMetric = m_fTotalRobots;

    return fNewMetric;
}

/****************************************/
/****************************************/

CVector3 ForagTrnLoopFunction::GetRandomPosition() {
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

REGISTER_LOOP_FUNCTIONS(ForagTrnLoopFunction, "forag_trn_loop_function");
