/**
  * @file <loop-functions/example/ChocolateLCNLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#include "ChocolateLCNLoopFunc.h"

/****************************************/
/****************************************/

ChocolateLCNLoopFunction::ChocolateLCNLoopFunction() {
  m_fSensingRange = 0.25;
  m_fCommunicationDistance = 0.35;
  m_unNumberPoints = 100000;
  m_cArenaCenter = CVector2(0,0);
  m_fObjectiveFunction = 0;
}

/****************************************/
/****************************************/

ChocolateLCNLoopFunction::ChocolateLCNLoopFunction(const ChocolateLCNLoopFunction& orig) {}

/****************************************/
/****************************************/

ChocolateLCNLoopFunction::~ChocolateLCNLoopFunction() {}

/****************************************/
/****************************************/

void ChocolateLCNLoopFunction::Destroy() {}

/****************************************/
/****************************************/

void ChocolateLCNLoopFunction::Reset() {
  AutoMoDeLoopFunctions::Reset();
}

/****************************************/
/****************************************/

argos::CColor ChocolateLCNLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {
  return CColor::GRAY50;
}

/****************************************/
/****************************************/

void ChocolateLCNLoopFunction::PostExperiment() {
  m_fObjectiveFunction = ComputeObjectiveFunction();
}

/****************************************/
/****************************************/

Real ChocolateLCNLoopFunction::GetObjectiveFunction() {
  return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void ChocolateLCNLoopFunction::AddNeighs(std::vector<CNetAgent> &agents, std::vector<CNetAgent>::iterator ag) {
	for (std::vector<CNetAgent>::iterator neigh = agents.begin(); neigh != agents.end(); ++neigh) {
		if (neigh->unClusterID != 0)
         continue;
		if ( Distance(ag->cPosition, neigh->cPosition) < m_fCommunicationDistance ) {
			neigh->unClusterID = ag->unClusterID;
			AddNeighs(agents, neigh);
		}
	}
}

/****************************************/
/****************************************/

Real ChocolateLCNLoopFunction::ComputeObjectiveFunction() {
  std::vector<CNetAgent> agents;
  CSpace::TMapPerType m_tEPuckEntityMap = GetSpace().GetEntitiesByType("epuck");

  CVector2 cEpuckPosition;

  /* create a vector with the agents positions (using the objects CNetAgents) */
  for (CSpace::TMapPerType::iterator itEPuckEntity = m_tEPuckEntityMap.begin(); itEPuckEntity != m_tEPuckEntityMap.end(); itEPuckEntity++) {
     CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*> (itEPuckEntity->second);
     cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                        pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());
     agents.push_back(CNetAgent(cEpuckPosition));
  }

  /* Cluster the agents in groups with maximum distance MAX_COMM_DIST */
  UInt32 maxUsedID = 0;
  for (std::vector<CNetAgent>::iterator ag = agents.begin(); ag != agents.end(); ++ag){
     if (ag->unClusterID != 0)
        continue;
     ag->unClusterID = ++maxUsedID;
     AddNeighs(agents, ag);
  }

  /* Determine the biggest group */
  size_t maxGroupSize = 0;
  UInt32 biggestGroupID = 0;
  for (UInt32 i = 1; i <= maxUsedID; i++){
     size_t size = 0;
     for (std::vector<CNetAgent>::iterator ag = agents.begin(); ag != agents.end(); ++ag){
        if (ag->unClusterID == i) {
           size++;
        }
     }

     if (maxGroupSize < size){
        maxGroupSize = size;
        biggestGroupID = i;
     }
  }

  /* create a vector of positions of only the elements in the biggest group */
  /* and at the same time, calculate the extreme coordinates of the group */
  std::vector<CVector2> biggestGroup;
  Real min_x = std::numeric_limits<double>::max();
  Real min_y = std::numeric_limits<double>::max();
  Real max_x = 0;
  Real max_y = 0;
  for (std::vector<CNetAgent>::iterator ag = agents.begin(); ag != agents.end(); ++ag){
     if (ag->unClusterID == biggestGroupID) {
        min_x = (ag->cPosition.GetX() < min_x)? ag->cPosition.GetX() : min_x;
        min_y = (ag->cPosition.GetY() < min_y)? ag->cPosition.GetY() : min_y;
        max_x = (ag->cPosition.GetX() > max_x)? ag->cPosition.GetX() : max_x;
        max_y = (ag->cPosition.GetY() > max_y)? ag->cPosition.GetY() : max_y;
        biggestGroup.push_back(ag->cPosition);
     }
  }

  /* calculate the bounding box sizes (ranges) */
  //Real width  = max_x - min_x + 2*SENSING_DIST;
  //Real height = max_y - min_y + 2*SENSING_DIST;
  CRange<Real> width( min_x - m_fSensingRange, max_x + m_fSensingRange);
  CRange<Real> height(min_y - m_fSensingRange, max_y + m_fSensingRange);

  /* Monte-Carlo sampling to estimate the ratio of the bounding box that is covered by the sensing range */
  Real avg = 0;
  for (size_t i = 0; i < (UInt32)Ceil((width.GetSpan()*height.GetSpan())*m_unNumberPoints); i++) {
     Real rx = m_pcRng->Uniform(width);
     Real ry = m_pcRng->Uniform(height);
     CVector2 rndPoint(rx, ry);

     for (std::vector<CVector2>::iterator pos = biggestGroup.begin(); pos != biggestGroup.end(); ++pos) {
        if (Distance(rndPoint, *pos) <= m_fSensingRange && Distance(rndPoint, m_cArenaCenter) < m_fDistributionRadius) {
           avg++;
           break;
        }
     }
  }

  /* Compute the coverage-ratio */
  avg = avg / Ceil((width.GetSpan()*height.GetSpan())*m_unNumberPoints);
  /* Multiply the area size (bounding box) by the coverage-ratio */
  Real performance = width.GetSpan() * height.GetSpan() * avg;

  return performance;
}

/****************************************/
/****************************************/

CVector3 ChocolateLCNLoopFunction::GetRandomPosition() {
  Real temp;
  Real a = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
  Real  b = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
  // If b < a, swap them
  if (b < a) {
    temp = a;
    a = b;
    b = temp;
  }
  Real fPosX = b * m_fDistributionRadius * cos(2 * CRadians::PI.GetValue() * (a/b));
  Real fPosY = b * m_fDistributionRadius * sin(2 * CRadians::PI.GetValue() * (a/b));

  return CVector3(fPosX, fPosY, 0);
}

REGISTER_LOOP_FUNCTIONS(ChocolateLCNLoopFunction, "chocolate_lcn_loop_functions");
