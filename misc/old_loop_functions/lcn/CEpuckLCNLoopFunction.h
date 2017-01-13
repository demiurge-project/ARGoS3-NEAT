/*
 * File:   CEpuckLCNLoopFunction.h
 * Author: Andreagiovanni Reina
 *
 * Created on 12th February 2014
 */

#ifndef CEPUCKLCNLOOPFUNCTIONS_H
#define	 CEPUCKLCNLOOPFUNCTIONS_H

#include "../neat_loop_function.h"
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

#include <list>

using namespace argos;

class CEpuckLCNLoopFunction : public CNeatLoopFunctions {
	private:
		struct CNetAgent {
			CVector2 pos;
			UInt32 clusterID;

			CNetAgent(CVector2 p) {
				pos = p;
				clusterID = 0;
			}
		};

	public:
		CEpuckLCNLoopFunction();
		CEpuckLCNLoopFunction(const CEpuckLCNLoopFunction& orig);
		virtual ~CEpuckLCNLoopFunction();

    	virtual void Init(TConfigurationNode& t_tree);
    	virtual void Reset();
    	/*virtual void Destroy();
    	virtual void PreStep();
    	virtual void PostStep();
    	virtual bool IsExperimentFinished();*/
    	virtual void PostExperiment();

    	virtual CColor GetFloorColor(const CVector2& c_position_on_plane);
    
    	virtual Real CalculateObjectiveFunction();

    	void AddNeighs(std::list<CNetAgent> &agents, std::list<CNetAgent>::iterator ag);

    private:
		Real maxCommunicationDistance;
		Real sensingRange;
		UInt32 samplingsPerMeter2;
		CVector2 arenaCenter;
		Real arenaRadius;
};

#endif	/* CEpuckLCNLoopFunction_H */

