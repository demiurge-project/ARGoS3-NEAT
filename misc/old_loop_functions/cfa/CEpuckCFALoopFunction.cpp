/* 
 * File:   CEpuckCFALoopFunction.cpp
 * Author: Gianpiero Francesca
 *
 * Created on 16 septembre 2011, 15.47
 */

#include "CEpuckCFALoopFunction.h"
#include <set>

CEpuckCFALoopFunction::CEpuckCFALoopFunction() : 
	m_fRadius(0.0f) {
}

CEpuckCFALoopFunction::~CEpuckCFALoopFunction() {
}

void CEpuckCFALoopFunction::Init(TConfigurationNode& t_tree) {

	/* Initialize the objective function */
	//setIsTheObjectiveFunctionAlreadyMinimizing(true);

	try {
		TConfigurationNode t_node = GetNode(t_tree, "ground_circles");
		Real x, y;
		GetNodeAttribute(t_node, "c1_x", x);
		GetNodeAttribute(t_node, "c1_y", y);
		m_vC1.Set(x,y);
		GetNodeAttribute(t_node, "c2_x", x);
		GetNodeAttribute(t_node, "c2_y", y);
		m_vC2.Set(x,y);
		GetNodeAttribute(t_node, "c3_x", x);
		GetNodeAttribute(t_node, "c3_y", y);
		m_vC3.Set(x,y);
		GetNodeAttribute(t_node, "radius", m_fRadius);
	} catch(CARGoSException& ex) {
		THROW_ARGOSEXCEPTION_NESTED("Error initializing controller", ex);
	}

	CNeatLoopFunctions::Init(t_tree);
}

void CEpuckCFALoopFunction::Reset() {
    CNeatLoopFunctions::Reset();
}

/*void CEpuckCFALoopFunction::Destroy() {
}

void CEpuckCFALoopFunction::PreStep() {
    CNeatLoopFunctions::PreStep();
}

void CEpuckCFALoopFunction::PostStep() {
	CNeatLoopFunctions::PostStep();
}

bool CEpuckCFALoopFunction::IsExperimentFinished() {
    return CNeatLoopFunctions::IsExperimentFinished();
}*/

void CEpuckCFALoopFunction::PostExperiment() {
	m_dPerformance = ((2*1.25) - GetExpectedDistance()) * 100;
	if (m_dPerformance < 0)
		m_dPerformance = 0;
	CNeatLoopFunctions::PostExperiment();
}

CColor CEpuckCFALoopFunction::GetFloorColor(const CVector2& c_position_on_plane) {

	CVector2 vCurrentPoint(c_position_on_plane.GetX(), c_position_on_plane.GetY());

	Real d = (m_vC1 - vCurrentPoint).Length();
	if (d <= m_fRadius) {
		return CColor::BLACK;
	}

	d = (m_vC2 - vCurrentPoint).Length();
	if (d <= m_fRadius) {
		return CColor::BLACK;
	}

	d = (m_vC3 - vCurrentPoint).Length();
	if (d <= m_fRadius) {
		return CColor::BLACK;
	}

	return CColor::GRAY50;
}

Real CEpuckCFALoopFunction::GetExpectedDistance() {

	// Total distance between each selected random point and its closest robots
	Real d = 0;
	// Expected distance
	Real e = 0;

	size_t i;
	for (i=0; i<10000; ++i) {
		CVector2 vRandomPoint = GetRandomPointInArena();
		d += GetClosestEpuckDistanceFromPoint(vRandomPoint);
	}
	e = d/i;
	return e;
}

// Old version
// Note from Brian: It won't work if all the robots are in the black zone
// Correction: if (it == cEpucks.end()) { return -1.0 } 
// a distance cannot be negative so, if we receive a negative distance, we can directly say that the expected distance = 0
// need to modify GetExpectedDistance() to take into account this fact
/*Real CEpuckCFALoopFunction::GetClosestEpuckDistanceFromPoint(CVector2 &vRandomPoint) {

    CSpace::TMapPerType& cEpucks = GetSpace().GetEntitiesByType("epuck");
    CSpace::TMapPerType::iterator it;

	// Select the first robot that is not on a black area and assume it's the closest robot from vRandomPoint
	it = cEpucks.begin();
	CVector2 vClosestEpuckPos;
	do {
		// if(it == cEpucks.end()) return -1.0;
		CEPuckEntity& e = *any_cast<CEPuckEntity*>(it->second);
		vClosestEpuckPos.Set(e.GetEmbodiedEntity().GetPosition().GetX(), e.GetEmbodiedEntity().GetPosition().GetY());
		it++;
	} while (IsOnBlackArea(vClosestEpuckPos));
	Real min_distance  = (vRandomPoint - vClosestEpuckPos).Length();

	// Loop over the other robots and check whether they're closer or not
	for (;it != cEpucks.end(); ++it) {
		CEPuckEntity& cEpuck = *any_cast<CEPuckEntity*>(it->second);
		CVector2 vEpuckPos(cEpuck.GetEmbodiedEntity().GetPosition().GetX(), cEpuck.GetEmbodiedEntity().GetPosition().GetY());

		// if the robot is on the black area, it doesn't count
		if (!IsOnBlackArea(vEpuckPos)) {
			Real distance = (vRandomPoint - vEpuckPos).Length();
			// Is the robot closer ? If so, update the min_distance value
			if (distance < min_distance) {
				min_distance = distance;
			}
		}
	}

	return min_distance;
}*/

// New version by Brian
Real CEpuckCFALoopFunction::GetClosestEpuckDistanceFromPoint(CVector2 &vRandomPoint) {
	CSpace::TMapPerType& cEpucks = GetSpace().GetEntitiesByType("epuck");
	CVector2 vEpuckPos;
    Real min_distance = (2*1.25); // Sets to the maximum distance which is approx. the diameter

    for(CSpace::TMapPerType::iterator it = cEpucks.begin(); it != cEpucks.end(); it++) {
    	CEPuckEntity& e = *any_cast<CEPuckEntity*>(it->second);
		vEpuckPos.Set(e.GetEmbodiedEntity().GetPosition().GetX(), e.GetEmbodiedEntity().GetPosition().GetY());
		if(!IsOnBlackArea(vEpuckPos)) {
			Real distance = (vRandomPoint - vEpuckPos).Length();
			if(distance < min_distance) {
				min_distance = distance;
			}
		}
    }

    return min_distance;
}

// Old version
/*CVector2 CEpuckCFALoopFunction::GetRandomPointInArena() {
	CVector2 vRetPoint;
	
	// Select a random point that is in the Circle centered in 0,0 of radius 1.25
	do {
		Real x = m_pcRNG->Uniform(CRange<Real>(-1.25f, 1.25f));
		Real y = m_pcRNG->Uniform(CRange<Real>(-1.25f, 1.25f));
		vRetPoint.Set(x, y);
	}
	while( (CVector2(0,0) - vRetPoint).Length() > 1.25f ); // if the point is not in the circle, pick another point

	return vRetPoint;
}*/

// New version by Brian
CVector2 CEpuckCFALoopFunction::GetRandomPointInArena() {
	return CVector2(m_pcRNG->Uniform(CRange<Real>(0.0f, 1.25f)), m_pcRNG->Uniform(m_cOrientation));
}

bool CEpuckCFALoopFunction::IsOnBlackArea(CVector2 &vEpuckPos) {

	/*
	 a point is in the circle if the distance between the center and
	 the point is smaller or equal than the circle's radius
	*/
	Real d1 = (m_vC1 - vEpuckPos).Length();
	Real d2 = (m_vC2 - vEpuckPos).Length();
	Real d3 = (m_vC3 - vEpuckPos).Length();
	if (d1 <= m_fRadius || d2 <= m_fRadius || d3 <= m_fRadius) {
		return true;
	}
	return false;
}


REGISTER_LOOP_FUNCTIONS(CEpuckCFALoopFunction, "cfa_loop_functions");
