/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "SeqLoopFunc.h"

/****************************************/
/****************************************/

SeqLoopFunction::SeqLoopFunction() {
    m_unClock = 0;
    m_fObjectiveFunction = 0;
    m_fObjectiveFunctionBlue = 0;
    m_fObjectiveFunctionRed = 0;
    m_bBlueFirst = true;
    m_cArenaColor = CColor::BLACK;
    m_cTaskAsignedColor = CColor::BLACK;
    m_cTaskEvalColor = CColor::BLACK;
}

/****************************************/
/****************************************/

SeqLoopFunction::SeqLoopFunction(const SeqLoopFunction& orig) {
}

/****************************************/
/****************************************/

SeqLoopFunction::~SeqLoopFunction() {}

/****************************************/
/****************************************/

void SeqLoopFunction::Destroy() {
    m_tRobotStates.clear();
    m_tSourceItems.clear();
}

/****************************************/
/****************************************/

argos::CColor SeqLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

    if (c_position_on_plane.GetX() <= -0.375)
        return CColor::WHITE;

    else if (c_position_on_plane.GetX() >= -0.125 && c_position_on_plane.GetX() <= 0.125){
        if (c_position_on_plane.GetY() >= 0.65033 || c_position_on_plane.GetY() <= -0.65033)
            return CColor::BLACK;
    }

    else if (c_position_on_plane.GetX() >= 0.65033 && c_position_on_plane.GetY() >= -0.125 && c_position_on_plane.GetY() <= 0.125)
        return CColor::BLACK;

    else if (c_position_on_plane.GetY() >= -c_position_on_plane.GetX() + 0.9267766094 &&
             c_position_on_plane.GetY() <=  c_position_on_plane.GetX() + 0.1767766953 &&
             c_position_on_plane.GetY() >=  c_position_on_plane.GetX() - 0.1767766953)
        return CColor::BLACK;

    else if (c_position_on_plane.GetY() <=  c_position_on_plane.GetX() - 0.9267766094 &&
             c_position_on_plane.GetY() <= -c_position_on_plane.GetX() + 0.1767766953 &&
             c_position_on_plane.GetY() >= -c_position_on_plane.GetX() - 0.1767766953)
        return CColor::BLACK;

    return CColor::GRAY50;
}

/****************************************/
/****************************************/

void SeqLoopFunction::Init(TConfigurationNode& t_tree) {
    CNeatLoopFunctions::Init(t_tree);

    m_bBlueFirst = SelectColorOrder(m_unColorOrder);

    AsignArenaColors(m_unNumerColors);

    InitRobotStates();
    InitSourceItems();

}

/****************************************/
/****************************************/

void SeqLoopFunction::Reset() {
    CNeatLoopFunctions::Reset();

    m_unClock = 0;
    m_fObjectiveFunction = 0;
    m_fObjectiveFunctionBlue = 0;
    m_fObjectiveFunctionRed = 0;

    m_bBlueFirst = SelectColorOrder(m_unColorOrder);

    m_tRobotStates.clear();
    m_tSourceItems.clear();

    InitRobotStates();
    InitSourceItems();
}

/****************************************/
/****************************************/

void SeqLoopFunction::PostStep() {

    m_unClock = GetSpace().GetSimulationClock();

    ScoreControl();
    ArenaControl();

}

/****************************************/
/****************************************/

void SeqLoopFunction::PostExperiment() {

    if (m_unEvalTask == 0){
        m_fObjectiveFunction = 0.5*GetNormalizedScore(m_fObjectiveFunctionBlue, m_unBlueTask)
                              +0.5*GetNormalizedScore(m_fObjectiveFunctionRed, m_unRedTask);
        LOG << (-m_fObjectiveFunction) << std::endl;
    }
    else if (m_unEvalTask == 1)
        LOG << (-m_fObjectiveFunctionBlue) << std::endl;
    else if (m_unEvalTask == 2)
        LOG << (-m_fObjectiveFunctionRed) << std::endl;
    else
        LOG << 888888 << std::endl;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetObjectiveFunction() {

    if (m_unEvalTask == 0)
        return (-m_fObjectiveFunction);
    else if (m_unEvalTask == 1)
        return (-m_fObjectiveFunctionBlue);
    else if (m_unEvalTask == 2)
        return (-m_fObjectiveFunctionRed);
    else
        return 888888;

}

/****************************************/
/****************************************/

void SeqLoopFunction::ArenaControl() {

    if (m_unClock == 1) {
        if (m_bBlueFirst)
            m_cArenaColor = m_cTaskAsignedColor;
        else
            m_cArenaColor = CColor::RED;
    }

    if (m_unClock == m_unTrnTime) {
        if (m_bBlueFirst)
            m_cArenaColor = CColor::RED;
        else
            m_cArenaColor = m_cTaskAsignedColor;
    }

    if (m_unClock == 1 || m_unClock == m_unTrnTime) {
        m_pcArena->SetArenaColor(m_cArenaColor);
        m_pcArena->SetBoxColor(2,1,CColor::GREEN);
        m_pcArena->SetBoxColor(2,2,CColor::GREEN);
        m_pcArena->SetBoxColor(2,6,CColor::GREEN);
        m_pcArena->SetBoxColor(2,7,CColor::GREEN);
        m_pcArena->SetBoxColor(2,8,CColor::GREEN);
    }

    return;
}

/****************************************/
/****************************************/

void SeqLoopFunction::ScoreControl(){

    if (m_unEvalTask == 0) {
        if (m_bBlueFirst){
            if (m_unClock <= m_unTrnTime){
                m_fObjectiveFunctionBlue += GetScore(m_unBlueTask);
            }
            else {
                m_fObjectiveFunctionRed += GetScore(m_unRedTask);
            }
        }
        else{
            if (m_unClock <= m_unTrnTime){
                m_fObjectiveFunctionRed += GetScore(m_unRedTask);
            }
            else {
                m_fObjectiveFunctionBlue += GetScore(m_unBlueTask);
            }
        }
    }

    else if (m_unEvalTask == 1) {
        if (m_bBlueFirst){
            if (m_unClock <= m_unTrnTime){
                m_fObjectiveFunctionBlue += GetScore(m_unBlueTask);
                //LOG << "Blue in Blue first" << std::endl;
            }
        }
        else{
            if (m_unClock > m_unTrnTime){
                m_fObjectiveFunctionBlue += GetScore(m_unBlueTask);
                //LOG << "Blue in Blue second" << std::endl;
            }
        }
    }

    else if (m_unEvalTask == 2) {
        if (!m_bBlueFirst){
            if (m_unClock <= m_unTrnTime){
                m_fObjectiveFunctionRed += GetScore(m_unRedTask);
                //LOG << "Red in Red first" << std::endl;
            }
        }
        else{
            if (m_unClock > m_unTrnTime){
                m_fObjectiveFunctionRed += GetScore(m_unRedTask);
                //LOG << "Red in Red second" << std::endl;
            }
        }
    }

    else {
        m_fObjectiveFunction = 888888;
        m_fObjectiveFunctionBlue = 888888;
        m_fObjectiveFunctionRed = 888888;
    }
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetScore(UInt32 unTask) {

    Real unScore = 0;

    switch (unTask){
    case 0:
        unScore = GetStopScore();
        break;
    case 1:
        unScore = GetAllBlackScore();
        break;
    case 2:
        unScore = -GetForageScore();
        break;
    case 3:
        unScore = GetMimicryScore();
        break;
    case 4:
        unScore = -GetDistributeScore();
        break;
    case 5:
        unScore = GetAggregationScore();
        break;
    default:
        unScore = 999999;
        break;
    }

    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetNormalizedScore(Real fScore, UInt32 unTask) {

    Real fNormalizedScore = 0;

    switch (unTask){
    case 0:
        fNormalizedScore = fScore/12000; // 12000 =  (Mission time / 2) * Number of robots
        break;
    case 1:
        fNormalizedScore = fScore/12000;
        break;
    case 2:
        fNormalizedScore = (180+fScore)/180; // 180 = (MaxItemsPerRobot * 20)
        break;
    case 3:
        fNormalizedScore = fScore/12000;
        break;
    case 4:
        fNormalizedScore = (12+fScore)/12; // 12 = (MaxItemsFiveRobots * 4)
        break;
    case 5:
        fNormalizedScore = (fScore-104)/(610); // 104 = Min score obtained with aggregation, 714 = Max score obtained with aggregation, 610 = Max-Min
        break;
    default:
        fNormalizedScore = 999999;
        break;
    }

    return fNormalizedScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetStopScore() {

    UpdateRobotPositions();

    Real unScore = 0;
    TRobotStateMap::iterator it;
    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {
        Real d = (it->second.cPosition - it->second.cLastPosition).Length();
        if (d > 0.0005)
            unScore+=1;
    }

    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetAllBlackScore() {

    UpdateRobotPositions();

    bool bInSource;
    Real unScore = 0;
    TRobotStateMap::iterator it;
    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {
        bInSource = IsRobotInSource(it->second.cPosition);
        if (!bInSource)
            unScore+=1;
    }

    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetForageScore() {

    UpdateRobotPositions();

    bool bInNest;
    bool bInSource;
    Real unScore = 0;
    TRobotStateMap::iterator it;

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {

        if (it->second.bItem == true){
            bInNest = IsRobotInNest(it->second.cPosition);
            if (bInNest) {
                unScore+=1;
                it->second.bItem = false;
            }
        }
        else {
            bInSource = IsRobotInSource(it->second.cPosition);
            if (bInSource) {
                it->second.bItem = true;
            }
        }
    }

    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetMimicryScore() {

    UpdateRobotColors();

    Real unScore = 0;
    TRobotStateMap::iterator it;

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {

        if (m_cArenaColor == m_cTaskAsignedColor &&
            it->second.cColor != m_cTaskEvalColor)
            unScore+=1;

        else if (m_cArenaColor == CColor::RED &&
                 it->second.cColor != CColor::MAGENTA)
            unScore+=1;
    }

    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetDistributeScore() {

    UpdateRobotPositions();

    bool bInNest;
    UInt32 bInSourceID = 0;

    TRobotStateMap::iterator it;

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {

        if (it->second.bItem == true){
            bInSourceID = IsRobotInSourceID(it->second.cPosition);
            if (bInSourceID != 0) {
                m_tSourceItems[bInSourceID] += 1;
                it->second.bItem = false;
            }
        }
        else {
            bInNest = IsRobotInNest(it->second.cPosition);
            if (bInNest) {
                it->second.bItem = true;
            }
        }
    }

    if (m_unClock == m_unTrnTime || m_unClock == 2*m_unTrnTime){
        TSourceItemsMap::iterator it;
        UInt32 unMinInSource = 999999;
        for (it = m_tSourceItems.begin(); it != m_tSourceItems.end(); ++it) {
            unMinInSource = Min(unMinInSource,(it->second));
        }
        return unMinInSource;
    }
    else
        return 0;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetAggregationScore() {

    UpdateRobotPositions();

    Real unScore = 0;
    Real d = 0;
    Real fCounter = 0;
    TRobotStateMap::iterator it, jt;

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {
        for (jt = m_tRobotStates.begin(); jt != it; ++jt) {
            d = (it->second.cPosition - jt->second.cPosition).Length();
            unScore+=d;
            fCounter+=1;
        }
    }

    unScore = unScore / fCounter;

    return unScore;
}

/****************************************/
/****************************************/

bool SeqLoopFunction::IsRobotInNest (CVector2 tRobotPosition) {
    if (tRobotPosition.GetX() <= -0.34)
        return true;

    return false;
}

/****************************************/
/****************************************/

bool SeqLoopFunction::IsRobotInSource (CVector2 tRobotPosition){

    if (tRobotPosition.GetX() >= -0.16) {
        if (tRobotPosition.GetX() >= -0.16 && tRobotPosition.GetX() <= 0.16){
            if (tRobotPosition.GetY() >= 0.61533 || tRobotPosition.GetY() <= -0.61533)
                return true;
        }

        else if (tRobotPosition.GetX() >= 0.61533 && tRobotPosition.GetY() >= -0.16 && tRobotPosition.GetY() <= 0.16)
            return true;

        else if (tRobotPosition.GetY() >= -tRobotPosition.GetX() + 0.87727913472 &&
                 tRobotPosition.GetY() <=  tRobotPosition.GetX() + 0.22450640303 &&
                 tRobotPosition.GetY() >=  tRobotPosition.GetX() - 0.22450640303)
            return true;

        else if (tRobotPosition.GetY() <=  tRobotPosition.GetX() - 0.87727913472 &&
                 tRobotPosition.GetY() <= -tRobotPosition.GetX() + 0.22450640303 &&
                 tRobotPosition.GetY() >= -tRobotPosition.GetX() - 0.22450640303)
            return true;
    }

    return false;
}

/****************************************/
/****************************************/

UInt32 SeqLoopFunction::IsRobotInSourceID (CVector2 tRobotPosition){

    UInt32 unSourceId = 0;

    if (tRobotPosition.GetX() >= -0.16) {
        if (tRobotPosition.GetX() >= -0.16 && tRobotPosition.GetX() <= 0.16){
            if (tRobotPosition.GetY() >= 0.61533)
                unSourceId = 1;
            else if (tRobotPosition.GetY() <= -0.61533)
                unSourceId = 5;
        }

        else if (tRobotPosition.GetX() >= 0.61533 && tRobotPosition.GetY() >= -0.16 && tRobotPosition.GetY() <= 0.16)
            unSourceId = 3;

        else if (tRobotPosition.GetY() >= -tRobotPosition.GetX() + 0.87727913472 &&
                 tRobotPosition.GetY() <=  tRobotPosition.GetX() + 0.22450640303 &&
                 tRobotPosition.GetY() >=  tRobotPosition.GetX() - 0.22450640303)
            unSourceId = 2;

        else if (tRobotPosition.GetY() <=  tRobotPosition.GetX() - 0.87727913472 &&
                 tRobotPosition.GetY() <= -tRobotPosition.GetX() + 0.22450640303 &&
                 tRobotPosition.GetY() >= -tRobotPosition.GetX() - 0.22450640303)
            unSourceId = 4;
    }

    return unSourceId;
}

/****************************************/
/****************************************/

void SeqLoopFunction::UpdateRobotPositions() {
    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
        cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                           pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

        m_tRobotStates[pcEpuck].cLastPosition = m_tRobotStates[pcEpuck].cPosition;
        m_tRobotStates[pcEpuck].cPosition = cEpuckPosition;
    }
}

/****************************************/
/****************************************/

void SeqLoopFunction::UpdateRobotColors() {
    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CColor cEpuckColor;
    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
        cEpuckColor = pcEpuck->GetLEDEquippedEntity().GetLED(10).GetColor();

        m_tRobotStates[pcEpuck].cColor = cEpuckColor;
    }
}

/****************************************/
/****************************************/

void SeqLoopFunction::InitRobotStates() {

    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
        cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                           pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

        m_tRobotStates[pcEpuck].cLastPosition = cEpuckPosition;
        m_tRobotStates[pcEpuck].cPosition = cEpuckPosition;
        m_tRobotStates[pcEpuck].cColor = CColor::BLACK;
        m_tRobotStates[pcEpuck].bItem = false;
    }
}

/****************************************/
/****************************************/

void SeqLoopFunction::InitSourceItems() {

    for (UInt32 unSources=1; unSources < 6; ++unSources) {
        m_tSourceItems[unSources] = 0;
    }
}

/****************************************/
/****************************************/

bool SeqLoopFunction::SelectColorOrder(UInt32 un_ColorOrderParam) {

    Real fRandomIndex;
    bool bBlueFirst = true;
    if (un_ColorOrderParam == 0)
        fRandomIndex = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
    else{
        fRandomIndex = (un_ColorOrderParam * 0.5) - (0.5/2);
    }

    if (fRandomIndex > 0.5)
        bBlueFirst = false;

    return bBlueFirst;

}

/****************************************/
/****************************************/

void SeqLoopFunction::AsignArenaColors(UInt32 un_NumberColorsParam) {

    if (un_NumberColorsParam == 1) {
        m_cTaskAsignedColor = CColor::BLACK;
        m_cTaskEvalColor = CColor::BLACK;
    }
    else{
        m_cTaskAsignedColor = CColor::BLUE;
        m_cTaskEvalColor = CColor::CYAN;
    }
}

/****************************************/
/****************************************/

CVector3 SeqLoopFunction::GetRandomPosition() {
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

REGISTER_LOOP_FUNCTIONS(SeqLoopFunction, "seq_loop_function");
