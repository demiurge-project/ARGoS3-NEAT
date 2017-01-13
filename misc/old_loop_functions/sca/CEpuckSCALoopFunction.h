/* 
 * File:   CEpuckSCALoopFunction.cpp
 * Author: Gianpiero Francesca
 *
 * Created on 16 septembre 2011, 15.47
 */

#ifndef CEPUCKSCALOOPFUNCTIONS_H
#define	CEPUCKSCALOOPFUNCTIONS_H

#include "../neat_loop_function.h"
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>
 #include <argos3/plugins/simulator/entities/box_entity.h>

using namespace argos;

class CEpuckSCALoopFunction : public CNeatLoopFunctions {

    public:
        CEpuckSCALoopFunction();
        virtual ~CEpuckSCALoopFunction();

        virtual void Init(TConfigurationNode& t_tree);
        virtual void Reset();
        /*virtual void Destroy();
        virtual void PreStep();*/
        virtual void PostStep();
        //virtual bool IsExperimentFinished();

        virtual CColor GetFloorColor(const CVector2& c_position_on_plane);

    private:
        Real m_fRCircle, m_fEcartXCircle, m_fPosYCircle;
        Real m_fWSpot, m_fHSpot, m_fPosYSpot;
        CBoxEntity *m_pcBoxLeft, *m_pcBoxRight, *m_pcBoxCenter;
};

#endif	
/* CEpuckSCALoopFunction_H */
