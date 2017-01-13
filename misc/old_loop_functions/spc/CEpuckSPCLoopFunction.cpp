/* 
 * File:   CEpuckSPCLoopFunction.cpp
 * Author: Gianpiero Francesca
 *
 * Created on 16 septembre 2011, 15.47
 */

#include "CEpuckSPCLoopFunction.h"
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <set>

CEpuckSPCLoopFunction::CEpuckSPCLoopFunction() { //: m_vecInitPos(20) {
}

CEpuckSPCLoopFunction::~CEpuckSPCLoopFunction() {
}

void CEpuckSPCLoopFunction::Init(TConfigurationNode& t_tree) {

    /* Initialize the objective function */
    //setIsTheObjectiveFunctionAlreadyMinimizing(true);

    spot1.Set(0.6, 0);
    spot2.Set(-0.6, 0);
    squareRadius = 0.3f * 0.3f;
    side = 0.6f;
    m_bOptEval = false;
    m_fDoptA = 0.08f;
    m_fDoptP = 0.06f;
    m_unNumPoints = 1000;

    GetNodeAttributeOrDefault(GetNode(t_tree, "optimal_area_coverage"), "value", m_fDoptA, m_fDoptA);
    GetNodeAttributeOrDefault(GetNode(t_tree, "optimal_perimeter_coverage"), "value", m_fDoptP, m_fDoptP);
    GetNodeAttributeOrDefault(GetNode(t_tree, "num_points"), "value", m_unNumPoints, m_unNumPoints);

    //FillInitialPositionsForOptimalEval();

    CNeatLoopFunctions::Init(t_tree);
}

/*void CEpuckSPCLoopFunction::FillInitialPositionsForOptimalEval(){
    m_vecInitPos[0] = CVector3(spot1.GetX()-side/2.0f+0.1f,     spot1.GetY()-side/2.0f+0.1f,    0);
    m_vecInitPos[1] = CVector3(spot1.GetX()-side/2.0f+0.1f+(side-0.2f)/2.0f,    spot1.GetY()-side/2.0f+0.1f,    0);
    m_vecInitPos[2] = CVector3(spot1.GetX()-side/2.0f+0.1f+(side-0.2f),     spot1.GetY()-side/2.0f+0.1f,    0);
    m_vecInitPos[3] = CVector3(spot1.GetX()-side/2.0f+0.1f,     spot1.GetY()-side/2.0f+0.1f+(side-0.2f)/2.0f,   0);
    m_vecInitPos[4] = CVector3(spot1.GetX()-side/2.0f+0.1f+(side-0.2f)/2.0f,    spot1.GetY()-side/2.0f+0.1f+(side-0.2f)/2.0f,   0);
    m_vecInitPos[5] = CVector3(spot1.GetX()-side/2.0f+0.1f+(side-0.2f),     spot1.GetY()-side/2.0f+0.1f+(side-0.2f)/2.0f,   0);
    m_vecInitPos[6] = CVector3(spot1.GetX()-side/2.0f+0.1f,     spot1.GetY()-side/2.0f+0.1f+(side-0.2f),    0);
    m_vecInitPos[7] = CVector3(spot1.GetX()-side/2.0f+0.1f+(side-0.2f)/2.0f,    spot1.GetY()-side/2.0f+0.1f+(side-0.2f),    0);
    m_vecInitPos[8] = CVector3(spot1.GetX()-side/2.0f+0.1f+(side-0.2f),     spot1.GetY()-side/2.0f+0.1f+(side-0.2f),    0);

    Real radius = Sqrt(squareRadius);
    for(int i = 0; i < 9; i++){
        m_vecInitPos[i+9] = CVector3(spot2.GetX()+radius*Cos(i*CRadians::TWO_PI/9), spot2.GetY()+radius*Sin(i*CRadians::TWO_PI/9), 0);
    }
    m_vecInitPos[18] = CVector3(0, 1, 0);
    m_vecInitPos[19] = CVector3(0, -1, 0);
}*/

void CEpuckSPCLoopFunction::Reset() {
    CNeatLoopFunctions::Reset();
}

/*void CEpuckSPCLoopFunction::Destroy() {
}

void CEpuckSPCLoopFunction::PreStep() {
    CNeatLoopFunctions::PreStep();
}

void CEpuckSPCLoopFunction::PostStep() {
    CNeatLoopFunctions::PostStep();
}

bool CEpuckSPCLoopFunction::IsExperimentFinished() {
    return CNeatLoopFunctions::IsExperimentFinished();
}*/

void CEpuckSPCLoopFunction::PostExperiment() {
    m_dPerformance = ( (0.85/m_fDoptA + 0.6/m_fDoptP) - CalculateObjectiveFunction() );
    CNeatLoopFunctions::PostExperiment();
}

CColor CEpuckSPCLoopFunction::GetFloorColor(const CVector2& c_position_on_plane) {
    CRange<Real> rangeXSquare(spot1.GetX() - side/2.0f,spot1.GetX() + side/2.0f);
    CRange<Real> rangeYSquare(spot1.GetY() - side/2.0f,spot1.GetY() + side/2.0f);

    if (rangeXSquare.WithinMinBoundIncludedMaxBoundIncluded(c_position_on_plane.GetX()) &&
            rangeYSquare.WithinMinBoundIncludedMaxBoundIncluded(c_position_on_plane.GetY())) {
        return CColor::WHITE;
    } else if ((c_position_on_plane - spot2).SquareLength() < squareRadius) {
        return CColor::BLACK;
    } else{
        return CColor::GRAY50;
    } 
}

Real CEpuckSPCLoopFunction::CalculateObjectiveFunction() {
    int i;
    CVector2 point;
    Real dA=0, dP=0;
    CSpace::TMapPerType m_tEPuckEntityMap = GetSpace().GetEntitiesByType("epuck");

    // White square area
    for(i = 0; i < m_unNumPoints; i++){

        Real d = 0.85;
        point = RandomPointOnSquareArea(spot1, side);

        for (CSpace::TMapPerType::iterator itEPuckEntity = m_tEPuckEntityMap.begin(); itEPuckEntity != m_tEPuckEntityMap.end(); itEPuckEntity++) {
            CEPuckEntity* cEPuckEntity = any_cast<CEPuckEntity*> ((*itEPuckEntity).second);
            CVector2 cps = cEPuckEntity->GetEmbodiedEntity().GetPosition().ProjectOntoXY(cps);
            if(IsOnSquareArea(cps, spot1, side)){
                if((point - cps).Length() < d){
                    d = (point - cps).Length();
                }
            }
        }

        dA += d;
    }
    dA /= m_unNumPoints;

    // Black circle area
    for(i = 0; i < m_unNumPoints; i++){

        Real d = 0.6;
        point = RandomPointOnCirclePerimeter(spot2, Sqrt(squareRadius));
        
        for (CSpace::TMapPerType::iterator itEPuckEntity = m_tEPuckEntityMap.begin(); itEPuckEntity != m_tEPuckEntityMap.end(); itEPuckEntity++) {
            CEPuckEntity* cEPuckEntity = any_cast<CEPuckEntity*> ((*itEPuckEntity).second);
            CVector2 cps = cEPuckEntity->GetEmbodiedEntity().GetPosition().ProjectOntoXY(cps);
            if(IsOnCirclePerimeter(cps, 0.04f, spot2, Sqrt(squareRadius))){
                if((point - cps).Length() < d){
                    d = (point - cps).Length();
                }
            }
        }

        dP += d;
    }

    dP /= m_unNumPoints;
    Real performance = (dA/m_fDoptA) + (dP/m_fDoptP);

    return performance;
}

CVector2 CEpuckSPCLoopFunction::RandomPointOnSquareArea(CVector2 centre, Real side){
    return CVector2(m_pcRNG->Uniform(CRange<Real>(centre.GetX() - side/2.0f,centre.GetX() + side/2.0f)),
                    m_pcRNG->Uniform(CRange<Real>(centre.GetY() - side/2.0f,centre.GetY() + side/2.0f)));
}

CVector2 CEpuckSPCLoopFunction::RandomPointOnCirclePerimeter(CVector2 centre, Real radius){
    CRadians angle = m_pcRNG->Uniform(CRange<CRadians>(CRadians::ZERO,CRadians::TWO_PI));
    return CVector2(centre.GetX() + Cos(angle)*radius, centre.GetY() + Sin(angle)* radius);
}

bool CEpuckSPCLoopFunction::IsOnSquareArea(CVector2 point, CVector2 centre, Real side){
    CRange<Real> rangeXSquare(centre.GetX() - side/2.0f,centre.GetX() + side/2.0f);
    CRange<Real> rangeYSquare(centre.GetY() - side/2.0f,centre.GetY() + side/2.0f);
    if (rangeXSquare.WithinMinBoundIncludedMaxBoundIncluded(point.GetX()) &&
            rangeYSquare.WithinMinBoundIncludedMaxBoundIncluded(point.GetY())) {
        return true;
    }
    return false;
}

bool CEpuckSPCLoopFunction::IsOnCirclePerimeter(CVector2 point, Real robotRadius, CVector2 centre, Real radius) {
    CRange<Real> acceptanceRange(radius - robotRadius, radius + robotRadius);
    if(acceptanceRange.WithinMinBoundIncludedMaxBoundIncluded((point - centre).Length())){
        return true;
    }
    return false;
}


REGISTER_LOOP_FUNCTIONS(CEpuckSPCLoopFunction, "spc_loop_functions");
