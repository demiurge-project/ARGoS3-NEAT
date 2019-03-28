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
    m_cTaskAsignedColorBlue = CColor::BLACK;
    m_cTaskEvalColorBlue = CColor::BLACK;
    m_cTaskAsignedColorRed = CColor::BLACK;
    m_cTaskEvalColorRed = CColor::BLACK;
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
    m_tSourceOperation.clear();
    m_tSourceReparation.clear();
    m_tSourceRestoring.clear();
    m_tArenaPoints.clear();
}

/****************************************/
/****************************************/

void SeqLoopFunction::Init(TConfigurationNode& t_tree) {
    CNeatLoopFunctions::Init(t_tree);

    m_bBlueFirst = SelectColorOrder(m_unColorOrder);

    AsignArenaColors(m_unNumerColors);

    InitRobotStates();
    InitSources();

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
    m_tSourceOperation.clear();
    m_tSourceReparation.clear();
    m_tSourceRestoring.clear();
    m_tArenaPoints.clear();

    InitRobotStates();
    InitSources();
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
        m_fObjectiveFunction = (m_fAlphaValue)*GetNormalizedScore(m_fObjectiveFunctionBlue, m_unBlueTask)
                              +(1-m_fAlphaValue)*GetNormalizedScore(m_fObjectiveFunctionRed, m_unRedTask);
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
            m_cArenaColor = m_cTaskAsignedColorBlue;
        else
            m_cArenaColor = m_cTaskAsignedColorRed;
    }

    if (m_unClock == m_unTrnTime) {
        if (m_bBlueFirst)
            m_cArenaColor = m_cTaskAsignedColorRed;
        else
            m_cArenaColor = m_cTaskAsignedColorBlue;
    }

    if (m_unClock == 1 || m_unClock == m_unTrnTime) {
        m_pcArena->SetArenaColor(m_cArenaColor);
        for (UInt32 unSources=1; unSources < 9; ++unSources) {
            if (m_tSourceOperation[unSources] > m_unClock)
                m_pcArena->SetBoxColor(2,unSources,CColor::GREEN);
            else
                m_pcArena->SetBoxColor(2,unSources,CColor::BLACK);
        }
    }
    else {
        for (UInt32 unSources=1; unSources < 9; ++unSources) {
            if (m_tSourceOperation[unSources] == m_unClock)
                m_pcArena->SetBoxColor(2,unSources,CColor::BLACK);
        }
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
            }
        }
        else{
            if (m_unClock > m_unTrnTime){
                m_fObjectiveFunctionBlue += GetScore(m_unBlueTask);
            }
            //else if (m_unRedTask == 2){ // Enable for restore
            //    ExecuteRestore();
            //}
        }
    }

    else if (m_unEvalTask == 2) {
        if (!m_bBlueFirst){
            if (m_unClock <= m_unTrnTime){
                m_fObjectiveFunctionRed += GetScore(m_unRedTask);
            }
        }
        else{
            if (m_unClock > m_unTrnTime){
                m_fObjectiveFunctionRed += GetScore(m_unRedTask);
            }
            //else if (m_unBlueTask == 2){ // Enable for restore
            //    ExecuteRestore();
            //}
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
        unScore = -GetManufactureScore();
        break;
    case 1:
        unScore = -GetTransportScore();
        break;
    case 2:
        unScore = -GetCleaningScore();
        break;
    case 3:
        unScore = -GetPickUpScore();
        break;
    case 4:
        unScore = -GetDispersionScore();
        break;
    case 5:
        unScore = -GetRechargeScore();
        break;
    /*case 0:
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
    case 6:
        unScore = GetColorStopScore();
        break;
    case 7:
        unScore = GetColorAllBlackScore();
        break;
    case 8:
        unScore = -GetColorForageScore();
        break;
    case 9:
        unScore = GetColorAggregationScore();
        break;*/
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
        fNormalizedScore = fScore;
        break;
    case 1:
        fNormalizedScore = fScore;
        break;
    case 2:
        fNormalizedScore = fScore;
        break;
    case 3:
        fNormalizedScore = fScore;
        break;
    case 4:
        fNormalizedScore = fScore;
        break;
    case 5:
        fNormalizedScore = fScore;
        break;
    /*case 0:
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
    case 6:
        fNormalizedScore = fScore/12000; // 12000 =  (Mission time / 2) * Number of robots
        break;
    case 7:
        fNormalizedScore = fScore/12000;
        break;
    case 8:
        fNormalizedScore = (180+fScore)/180; // 180 = (MaxItemsPerRobot * 20)
        break;
    case 9:
        fNormalizedScore = (fScore-104)/(115900); // 104 = Min score obtained with aggregation, 714 = Max score obtained with aggregation, 610 = Max-Min
        break;*/
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

        if (m_cArenaColor == m_cTaskAsignedColorBlue &&
            it->second.cColor != m_cTaskEvalColorBlue)
            unScore+=1;

        else if (m_cArenaColor == m_cTaskAsignedColorRed &&
                 it->second.cColor != m_cTaskEvalColorRed)
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

Real SeqLoopFunction::GetRechargeScore() {

    UpdateRobotPositions();

    Real unScore = 0;
    TRobotStateMap::iterator it;
    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {
        if (IsRobotInDock(it->second.cPosition))
            unScore+=1;
    }

    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetRefillScore() {

    //if (m_unClock == m_unTrnTime || m_unClock == 2*m_unTrnTime){

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

    //}
    //else
        //return 0;

}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetSurveillanceScore() {

    Real unNumberPoints = 1000;

    if (m_tArenaPoints.empty()) {
        GetArenaPoints(unNumberPoints);
    }

    UpdateRobotPositions();

    CVector2 cRandomPoint;

    Real fminDistance = 1.96;
    Real fExpectedDistance = 0;
    Real unScore;

    TRobotStateMap::iterator it;
    TArenaPoints::iterator jt;

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {
        Real d = (it->second.cPosition - it->second.cLastPosition).Length();
        if (d > 0.0005){
            it->second.bMoving = true;
        }
    }

    for(jt = m_tArenaPoints.begin(); jt != m_tArenaPoints.end(); ++jt){
        cRandomPoint = *jt;
        for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {
            if (it->second.bMoving == true) {
                it->second.bMoving = false;
                Real distance = (cRandomPoint - it->second.cPosition).Length();
                if(distance < fminDistance)
                    fminDistance = distance;
            }
        }
        fExpectedDistance = fExpectedDistance + fminDistance;
    }

    fExpectedDistance = fExpectedDistance / unNumberPoints;

    unScore = fExpectedDistance;

    return unScore;


}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetTransportScore() {

    UpdateRobotPositions();

    bool bInNest;
    UInt32 unInSource = 0;
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
            unInSource = IsRobotInSourceID(it->second.cPosition);
            if (unInSource != 0){
                //if (m_tSourceOperation[unInSource] > m_unClock){
                    it->second.bItem = true;
                //}
            }
        }
    }

    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetManufactureScore() {

    UpdateRobotPositions();

    UInt32 unInSource = 0;
    Real unScore = 0;
    TRobotStateMap::iterator it;

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {

        unInSource = IsRobotInSourceID(it->second.cPosition);
        if (unInSource != 0){
            if (m_tSourceOperation[unInSource] > m_unClock){
                m_tSourceItems[unInSource] += 1;
            }
            if (m_tSourceItems[unInSource] == 100){
                m_tSourceItems[unInSource] = 0;
                unScore+=1;
            }
        }
    }

    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetRestoreScore() {

    UpdateRobotPositions();

    UInt32 unInSource = 0;
    UInt32 unExpTime = 2*m_unTrnTime;
    Real unScore = 0;
    TRobotStateMap::iterator it;

    for (UInt32 unSources=1; unSources < 9; ++unSources) {
        m_tSourceRestoring[unSources] = 0;
    }

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {

        unInSource = IsRobotInSourceID(it->second.cPosition);
        if (unInSource != 0){
            if (m_tSourceOperation[unInSource] <= m_unClock){
                m_tSourceRestoring[unInSource] += 1;
            }
        }
    }

    for (UInt32 unSources=1; unSources < 9; ++unSources) {

        if (m_tSourceOperation[unSources] <= m_unClock){

            if (m_tSourceRestoring[unSources] >= 2)
                m_tSourceReparation[unSources] += 1;
            else
                m_tSourceReparation[unSources] = 0;

            if (m_tSourceReparation[unSources] == 50){
                m_tSourceReparation[unSources] = 0;
                m_tSourceOperation[unSources] = GetRandomTime(m_unClock, unExpTime);
                m_pcArena->SetBoxColor(2,unSources,CColor::GREEN);
                unScore+=1;
            }
        }
    }


    return unScore;
}

/****************************************/
/****************************************/

void SeqLoopFunction::ExecuteRestore() {

    UpdateRobotPositions();

    UInt32 unInSource = 0;
    UInt32 unExpTime = 2*m_unTrnTime;
    TRobotStateMap::iterator it;

    for (UInt32 unSources=1; unSources < 9; ++unSources) {
        m_tSourceRestoring[unSources] = 0;
    }

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {

        unInSource = IsRobotInSourceID(it->second.cPosition);
        if (unInSource != 0){
            if (m_tSourceOperation[unInSource] <= m_unClock){
                m_tSourceRestoring[unInSource] += 1;
            }
        }
    }

    for (UInt32 unSources=1; unSources < 9; ++unSources) {

        if (m_tSourceOperation[unSources] <= m_unClock){

            if (m_tSourceRestoring[unSources] >= 2)
                m_tSourceReparation[unSources] += 1;
            else
                m_tSourceReparation[unSources] = 0;

            if (m_tSourceReparation[unSources] == 50){
                m_tSourceReparation[unSources] = 0;
                m_tSourceOperation[unSources] = GetRandomTime(m_unClock, unExpTime);
                m_pcArena->SetBoxColor(2,unSources,CColor::GREEN);
            }
        }
    }
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetPickUpScore() {

    UpdateRobotPositions();

    bool bInNest;
    Real unScore = 0;
    TRobotStateMap::iterator it;

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {

        if (it->second.bMaterial == true){
            bInNest = IsRobotInNest(it->second.cPosition);
            if (bInNest == false) {
                unScore+=1;
                it->second.bMaterial = false;
            }
        }
        else {
            bInNest = IsRobotInNest(it->second.cPosition);
            if (bInNest){
                    it->second.bMaterial = true;
            }
        }
    }

    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetDispersionScore() {

    UpdateRobotPositions();

    Real unScore = 0;
    Real fMinDistance = 1.96;
    Real fDistance = 0;
    TRobotStateMap::iterator it, jt;

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {
        for (jt = m_tRobotStates.begin(); jt != it; ++jt) {
            fDistance = (it->second.cPosition - jt->second.cPosition).Length();
            if(fDistance < fMinDistance)
                fMinDistance = fDistance;
        }
    }

    unScore = fMinDistance;

    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetCleaningScore() {

    UpdateRobotPositions();

    UInt32 unInSource = 0;
    Real unScore = 0;
    TRobotStateMap::iterator it;

    for (UInt32 unSources=1; unSources < 9; ++unSources) {
        m_tSourceRestoring[unSources] = 0;
    }

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {

        unInSource = IsRobotInSourceID(it->second.cPosition);
        if (unInSource != 0){
            if (m_tSourceOperation[unInSource] <= m_unClock){
                m_tSourceRestoring[unInSource] += 1;
            }
        }
    }

    for (UInt32 unSources=1; unSources < 9; ++unSources) {

        if (m_tSourceOperation[unSources] <= m_unClock){

            if (m_tSourceRestoring[unSources] >= 2)
                m_tSourceReparation[unSources] += 1;
            else
                m_tSourceReparation[unSources] = 0;

            if (m_tSourceReparation[unSources] == 50){
                m_tSourceReparation[unSources] = 0;
                unScore+=1;
            }
        }
    }


    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetColorStopScore() {

    UpdateRobotPositions();
    UpdateRobotColors();

    CColor cMimicryColor = CColor::BLACK;
    if (m_cArenaColor == m_cTaskAsignedColorBlue)
        cMimicryColor = m_cTaskEvalColorBlue;
    else if (m_cArenaColor == m_cTaskAsignedColorRed)
        cMimicryColor = m_cTaskEvalColorRed;

    Real unScore = 0;
    TRobotStateMap::iterator it;
    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {

        if (it->second.cColor == cMimicryColor) {
            Real d = (it->second.cPosition - it->second.cLastPosition).Length();
            if (d > 0.0005)
                unScore+=1;
        }
        else
            unScore+=1;
    }

    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetColorAllBlackScore() {

    UpdateRobotPositions();
    UpdateRobotColors();

    CColor cMimicryColor = CColor::BLACK;
    if (m_cArenaColor == m_cTaskAsignedColorBlue)
        cMimicryColor = m_cTaskEvalColorBlue;
    else if (m_cArenaColor == m_cTaskAsignedColorRed)
        cMimicryColor = m_cTaskEvalColorRed;

    bool bInSource;
    Real unScore = 0;
    TRobotStateMap::iterator it;
    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {

        if (it->second.cColor == cMimicryColor) {
            bInSource = IsRobotInSource(it->second.cPosition);
            if (!bInSource)
                unScore+=1;
        }
        else
            unScore+=1;
    }

    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetColorForageScore() {

    UpdateRobotPositions();
    UpdateRobotColors();

    CColor cMimicryColor = CColor::BLACK;
    if (m_cArenaColor == m_cTaskAsignedColorBlue)
        cMimicryColor = m_cTaskEvalColorBlue;
    else if (m_cArenaColor == m_cTaskAsignedColorRed)
        cMimicryColor = m_cTaskEvalColorRed;

    bool bInNest;
    bool bInSource;
    Real unScore = 0;
    TRobotStateMap::iterator it;

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {

        if (it->second.cColor == cMimicryColor) {

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
    }

    return unScore;
}

/****************************************/
/****************************************/

Real SeqLoopFunction::GetColorAggregationScore() {

    UpdateRobotPositions();
    UpdateRobotColors();

    CColor cMimicryColor = CColor::BLACK;
    if (m_cArenaColor == m_cTaskAsignedColorBlue)
        cMimicryColor = m_cTaskEvalColorBlue;
    else if (m_cArenaColor == m_cTaskAsignedColorRed)
        cMimicryColor = m_cTaskEvalColorRed;

    Real unScore = 0;
    Real d = 0;
    Real fCounter = 0;
    TRobotStateMap::iterator it, jt;

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {
        for (jt = m_tRobotStates.begin(); jt != it; ++jt) {
            d = (it->second.cPosition - jt->second.cPosition).Length();
            unScore+=d;

            if (it->second.cColor == cMimicryColor || jt->second.cColor == cMimicryColor)
                fCounter+=1;
        }
    }

    unScore = unScore / Max(fCounter,1.0);

    return unScore;
}

/****************************************/
/****************************************/

argos::CColor SeqLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {

    if (c_position_on_plane.GetX() >= -0.125 && c_position_on_plane.GetX() <= 0.125){
        if (c_position_on_plane.GetY() >= 0.65033 || c_position_on_plane.GetY() <= -0.65033)
            return CColor::BLACK;
        else if (c_position_on_plane.GetY() >= -0.125 && c_position_on_plane.GetY() <= 0.125)
            return CColor::WHITE;
    }

    else if (c_position_on_plane.GetY() >= -0.125 && c_position_on_plane.GetY() <= 0.125){
        if (c_position_on_plane.GetX() >= 0.65033 || c_position_on_plane.GetX() <= -0.65033)
            return CColor::BLACK;
    }
    else if (c_position_on_plane.GetY() <=  c_position_on_plane.GetX() + 0.1767766953 &&
             c_position_on_plane.GetY() >=  c_position_on_plane.GetX() - 0.1767766953){
        if ( c_position_on_plane.GetY() >= -c_position_on_plane.GetX() + 0.9267766094 ||
             c_position_on_plane.GetY() <= -c_position_on_plane.GetX() - 0.9267766094)
            return CColor::BLACK;
    }

    else if (c_position_on_plane.GetY() <= -c_position_on_plane.GetX() + 0.1767766953 &&
             c_position_on_plane.GetY() >= -c_position_on_plane.GetX() - 0.1767766953){
        if ( c_position_on_plane.GetY() >=  c_position_on_plane.GetX() + 0.9267766094 ||
             c_position_on_plane.GetY() <=  c_position_on_plane.GetX() - 0.9267766094)
            return CColor::BLACK;
    }

    return CColor::GRAY50;
}

/****************************************/
/****************************************/

bool SeqLoopFunction::IsRobotInNest (CVector2 tRobotPosition) {

    if (tRobotPosition.GetX() >= -0.16 && tRobotPosition.GetX() <= 0.16 &&
        tRobotPosition.GetY() >= -0.16 && tRobotPosition.GetY() <= 0.16)
        return true;

    return false;
}

/****************************************/
/****************************************/

bool SeqLoopFunction::IsRobotInDock (CVector2 tRobotPosition){

    if (tRobotPosition.Length() >= 0.65033) {

        if (tRobotPosition.GetX() >= -0.16 && tRobotPosition.GetX() <= 0.16){
            if (tRobotPosition.GetY() >= 0.61533)
                return false;
            else if (tRobotPosition.GetY() <= -0.61533)
                return false;
        }
        else if (tRobotPosition.GetY() >= -0.16 && tRobotPosition.GetY() <= 0.16){
            if (tRobotPosition.GetX() >= 0.61533)
                return false;
            else if (tRobotPosition.GetX() <= -0.61533)
                return false;
        }
        else if (tRobotPosition.GetY() <=  tRobotPosition.GetX() + 0.22450640303 &&
                 tRobotPosition.GetY() >=  tRobotPosition.GetX() - 0.22450640303){
            if ( tRobotPosition.GetY() >= -tRobotPosition.GetX() + 0.87727913472)
                return false;
            else if (tRobotPosition.GetY() <= -tRobotPosition.GetX() - 0.87727913472)
                return false;
        }

        else if (tRobotPosition.GetY() <= -tRobotPosition.GetX() + 0.22450640303 &&
                 tRobotPosition.GetY() >= -tRobotPosition.GetX() - 0.22450640303){
            if ( tRobotPosition.GetY() >=  tRobotPosition.GetX() + 0.87727913472)
                return false;
            else if (tRobotPosition.GetY() <=  tRobotPosition.GetX() - 0.87727913472)
                return false;
        }

        return true;
    }

    return false;
}

/****************************************/
/****************************************/

bool SeqLoopFunction::IsRobotInSource (CVector2 tRobotPosition){

    if (tRobotPosition.Length() >= 0.6) {

        if (tRobotPosition.GetX() >= -0.16 && tRobotPosition.GetX() <= 0.16){
            if (tRobotPosition.GetY() >= 0.61533)
                return true;
            else if (tRobotPosition.GetY() <= -0.61533)
                return true;
        }
        else if (tRobotPosition.GetY() >= -0.16 && tRobotPosition.GetY() <= 0.16){
            if (tRobotPosition.GetX() >= 0.61533)
                return true;
            else if (tRobotPosition.GetX() <= -0.61533)
                return true;
        }
        else if (tRobotPosition.GetY() <=  tRobotPosition.GetX() + 0.22450640303 &&
                 tRobotPosition.GetY() >=  tRobotPosition.GetX() - 0.22450640303){
            if ( tRobotPosition.GetY() >= -tRobotPosition.GetX() + 0.87727913472)
                return true;
            else if (tRobotPosition.GetY() <= -tRobotPosition.GetX() - 0.87727913472)
                return true;
        }

        else if (tRobotPosition.GetY() <= -tRobotPosition.GetX() + 0.22450640303 &&
                 tRobotPosition.GetY() >= -tRobotPosition.GetX() - 0.22450640303){
            if ( tRobotPosition.GetY() >=  tRobotPosition.GetX() + 0.87727913472)
                return true;
            else if (tRobotPosition.GetY() <=  tRobotPosition.GetX() - 0.87727913472)
                return true;
        }
    }

    return false;
}

/****************************************/
/****************************************/

UInt32 SeqLoopFunction::IsRobotInSourceID (CVector2 tRobotPosition){

    UInt32 unSourceId = 0;

    if (tRobotPosition.Length() >= 0.6) {

        if (tRobotPosition.GetX() >= -0.16 && tRobotPosition.GetX() <= 0.16){
            if (tRobotPosition.GetY() >= 0.61533)
                unSourceId = 2;
            else if (tRobotPosition.GetY() <= -0.61533)
                unSourceId = 6;
        }
        else if (tRobotPosition.GetY() >= -0.16 && tRobotPosition.GetY() <= 0.16){
            if (tRobotPosition.GetX() >= 0.61533)
                unSourceId = 8;
            else if (tRobotPosition.GetX() <= -0.61533)
                unSourceId = 4;
        }
        else if (tRobotPosition.GetY() <=  tRobotPosition.GetX() + 0.22450640303 &&
                 tRobotPosition.GetY() >=  tRobotPosition.GetX() - 0.22450640303){
            if ( tRobotPosition.GetY() >= -tRobotPosition.GetX() + 0.87727913472)
                unSourceId = 1;
            else if (tRobotPosition.GetY() <= -tRobotPosition.GetX() - 0.87727913472)
                unSourceId = 5;
        }

        else if (tRobotPosition.GetY() <= -tRobotPosition.GetX() + 0.22450640303 &&
                 tRobotPosition.GetY() >= -tRobotPosition.GetX() - 0.22450640303){
            if ( tRobotPosition.GetY() >=  tRobotPosition.GetX() + 0.87727913472)
                unSourceId = 3;
            else if (tRobotPosition.GetY() <=  tRobotPosition.GetX() - 0.87727913472)
                unSourceId = 7;
        }
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
        m_tRobotStates[pcEpuck].bMaterial = false;
        m_tRobotStates[pcEpuck].bMoving = false;
    }
}

/****************************************/
/****************************************/

void SeqLoopFunction::InitSources() {
    UInt32 unExpTime = 2*m_unTrnTime;

    for (UInt32 unSources=1; unSources < 9; ++unSources) {
        m_tSourceItems[unSources] = 0;
        m_tSourceOperation[unSources] = GetRandomTime(1, unExpTime);
        m_tSourceRestoring[unSources] = 0;
        m_tSourceReparation[unSources] = 0;
    }

    m_tSourceOperation[2] = unExpTime;

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

    if (un_NumberColorsParam == 0) {
        m_cTaskAsignedColorBlue = CColor::BLACK;
        m_cTaskEvalColorBlue = CColor::BLACK;
        m_cTaskAsignedColorRed = CColor::BLACK;
        m_cTaskEvalColorRed = CColor::BLACK;
    }
    else if (un_NumberColorsParam == 1) {
        m_cTaskAsignedColorBlue = CColor::BLACK;
        m_cTaskEvalColorBlue = CColor::BLACK;
        m_cTaskAsignedColorRed = CColor::RED;
        m_cTaskEvalColorRed = CColor::MAGENTA;
    }
    else{
        m_cTaskAsignedColorBlue = CColor::BLUE;
        m_cTaskEvalColorBlue = CColor::CYAN;
        m_cTaskAsignedColorRed = CColor::RED;
        m_cTaskEvalColorRed = CColor::MAGENTA;
    }
}

/****************************************/
/****************************************/

void SeqLoopFunction::GetArenaPoints(UInt32 unNumberPoints) {

        CVector2 cRandomPoint;

        for(UInt32 unPoint = 0; unPoint < unNumberPoints; unPoint++){

            cRandomPoint = GetRandomArenaPoint();
            m_tArenaPoints.push_back(cRandomPoint);
        }
}

/****************************************/
/****************************************/

CVector2 SeqLoopFunction::GetRandomArenaPoint() {

  Real a = m_pcRng->Uniform(CRange<Real>(0.0f, 1000.0f));
  Real b = m_pcRng->Uniform(CRange<Real>(0.0f, 1000.0f));

  CVector2 cArenaPoint;
  cArenaPoint.FromPolarCoordinates((a/1000)*0.98, (b/1000)*CRadians::TWO_PI);

  return cArenaPoint;
}

/****************************************/
/****************************************/

CVector3 SeqLoopFunction::GetRandomPosition() {
    Real temp;
    Real a = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
    Real b = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
    Real c = m_pcRng->Uniform(CRange<Real>(-1.0f, 1.0f));
    Real d = m_pcRng->Uniform(CRange<Real>(-1.0f, 1.0f));
    // If b < a, swap them
    if (b < a) {
      temp = a;
      a = b;
      b = temp;
    }
    m_fDistributionRadius = 0.4;
    Real fPosX = (c * m_fDistributionRadius / 2) + m_fDistributionRadius * cos(2 * CRadians::PI.GetValue() * (a/b));
    Real fPosY = (d * m_fDistributionRadius / 2) + m_fDistributionRadius * sin(2 * CRadians::PI.GetValue() * (a/b));

    return CVector3(fPosX, fPosY, 0);
}

/****************************************/
/****************************************/

UInt32 SeqLoopFunction::GetRandomTime(UInt32 unMin, UInt32 unMax) {
  UInt32 unFailureAt = m_pcRng->Uniform(CRange<UInt32>(unMin, unMax));
  return unFailureAt;
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(SeqLoopFunction, "seq_loop_function");
