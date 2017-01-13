/* 
 * File:   CEpuckSCALoopFunction.cpp
 * Author: Gianpiero Francesca
 *
 * Created on 16 septembre 2011, 15.47
 */

#include "CEpuckSCALoopFunction.h"
#include <argos3/plugins/simulator/entities/box_entity.h>

CEpuckSCALoopFunction::CEpuckSCALoopFunction() : 
    m_fRCircle(0.30),
    m_fEcartXCircle(3),
    m_fPosYCircle(0),
    m_fWSpot(0.60),
    m_fHSpot(0.15),
    m_fPosYSpot(-3),
    m_pcBoxLeft(NULL), 
    m_pcBoxRight(NULL), 
    m_pcBoxCenter(NULL) {
}

CEpuckSCALoopFunction::~CEpuckSCALoopFunction() {
}

void CEpuckSCALoopFunction::Init(TConfigurationNode& t_tree) {

    /* Initialize the objective function */
    //setIsTheObjectiveFunctionAlreadyMinimizing(true);

    try {
        GetNodeAttributeOrDefault(t_tree, "rCircle", m_fRCircle, m_fRCircle);
        GetNodeAttributeOrDefault(t_tree, "ecartXCircle", m_fEcartXCircle, m_fEcartXCircle);
        GetNodeAttributeOrDefault(t_tree, "posYCircle", m_fPosYCircle, m_fPosYCircle);

        GetNodeAttributeOrDefault(t_tree, "wSpot", m_fWSpot , m_fWSpot);
        GetNodeAttributeOrDefault(t_tree, "hSpot", m_fHSpot, m_fHSpot);
        GetNodeAttributeOrDefault(t_tree, "posYSpot", m_fPosYSpot, m_fPosYSpot);
    } catch(CARGoSException& ex) {
        THROW_ARGOSEXCEPTION_NESTED("Error parsing loop functions!", ex);
    }

    CQuaternion angleWall;

    // Center
    angleWall.FromEulerAngles(CRadians::PI_OVER_TWO, CRadians::ZERO, CRadians::ZERO);
    m_pcBoxCenter = new CBoxEntity("wall_center", 
        CVector3(0, m_fPosYSpot-m_fHSpot/2-0.05/2, 0.0), 
        angleWall,
        false,
        CVector3(0.05, m_fWSpot+0.05, 0.2));
    AddEntity(*m_pcBoxCenter);

    // Left
    angleWall.FromEulerAngles(CRadians::ZERO, CRadians::ZERO, CRadians::ZERO);
    m_pcBoxLeft = new CBoxEntity("wall_left", 
        CVector3(-m_fWSpot/2-0.05/4, m_fPosYSpot-0.05/2, 0.0), 
        angleWall,
        false,
        CVector3(0.05, m_fHSpot+0.05, 0.2));
    AddEntity(*m_pcBoxLeft);

    // Right
    m_pcBoxRight = new CBoxEntity("wall_right", 
        CVector3(m_fWSpot/2+0.05/4, m_fPosYSpot-0.05/2, 0.0),
        angleWall,
        false,
        CVector3(0.05, m_fHSpot+0.05, 0.2));
    AddEntity(*m_pcBoxRight);

    CNeatLoopFunctions::Init(t_tree);
}

void CEpuckSCALoopFunction::Reset() {
    CNeatLoopFunctions::Reset();
}

/*void CEpuckSCALoopFunction::Destroy() {
}

void CEpuckSCALoopFunction::PreStep() {
}*/

void CEpuckSCALoopFunction::PostStep() {

    CNeatLoopFunctions::PostStep();

    //Update score: +1 if one epuck is in the zone.
    CVector3 pos;
    CSpace::TMapPerType m_tEPuckEntityMap = GetSpace().GetEntitiesByType("epuck");

    for(CSpace::TMapPerType::iterator it = m_tEPuckEntityMap.begin(); it != m_tEPuckEntityMap.end(); ++it) {
        CEPuckEntity& myPuck = *(any_cast<CEPuckEntity*>(it->second));
        pos = myPuck.GetEmbodiedEntity().GetPosition();

        if (pos.GetX() > -m_fWSpot/2 && pos.GetX() < m_fWSpot/2 && pos.GetY() > -m_fHSpot/2+m_fPosYSpot && pos.GetY() < m_fHSpot/2+m_fPosYSpot) {
            m_dPerformance++;
        }
    }

}

/*bool CEpuckSCALoopFunction::IsExperimentFinished() {
    return CNeatLoopFunctions::IsExperimentFinished();
}*/

CColor CEpuckSCALoopFunction::GetFloorColor(const CVector2& c_position_on_plane) {

    if ((c_position_on_plane - CVector2(m_fEcartXCircle, m_fPosYCircle)).Length() < m_fRCircle)
        return CColor::BLACK;
    if ((c_position_on_plane - CVector2(-m_fEcartXCircle, m_fPosYCircle)).Length() < m_fRCircle)
        return CColor::BLACK;

    if( c_position_on_plane.GetX() > -m_fWSpot/2 && c_position_on_plane.GetX() < m_fWSpot/2
            && c_position_on_plane.GetY() > -m_fHSpot/2+m_fPosYSpot && c_position_on_plane.GetY() < m_fHSpot/2+m_fPosYSpot)
        return CColor::WHITE;

    return CColor::GRAY50;  
}

REGISTER_LOOP_FUNCTIONS(CEpuckSCALoopFunction, "sca_loop_functions")
