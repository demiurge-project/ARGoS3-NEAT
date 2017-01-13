/* 
 * File:   CEpuckSPCLoopFunction.cpp
 * Author: Gianpiero Francesca
 *
 * Created on 16 septembre 2011, 15.47
 */

#ifndef CEpuckSPCLoopFunctionS_H
#define	CEpuckSPCLoopFunctionS_H

#include "../neat_loop_function.h"
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

using namespace argos;

class CEpuckSPCLoopFunction : public CNeatLoopFunctions {

   public:
      CEpuckSPCLoopFunction();
      virtual ~CEpuckSPCLoopFunction();

      virtual void Init(TConfigurationNode& t_tree);
      virtual void Reset();
      /*virtual void Destroy();
      virtual void PreStep();
      virtual void PostStep();
      virtual bool IsExperimentFinished();*/
      virtual void PostExperiment();

      virtual CColor GetFloorColor(const CVector2& c_position_on_plane);

      virtual Real CalculateObjectiveFunction();

      virtual CVector2 RandomPointOnSquareArea(CVector2 centre, Real side);
      virtual bool IsOnSquareArea(CVector2 point, CVector2 centre, Real side);
      virtual CVector2 RandomPointOnCirclePerimeter(CVector2 centre, Real ratius);
      virtual bool IsOnCirclePerimeter(CVector2 point, Real robotRatius,CVector2 centre, Real ratius );
      
      //void FillInitialPositionsForOptimalEval();

   private:
      //std::vector<CVector3> m_vecInitPos;

      CVector2 spot1, spot2;
      Real squareRadius;
      Real side;
      bool m_bOptEval;
      Real m_fDoptA;
      Real m_fDoptP;
      UInt32 m_unNumPoints;
};

#endif
/* CEpuckSPCLoopFunction_H */
