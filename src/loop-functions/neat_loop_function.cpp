#include "neat_loop_function.h"

#include <limits>

/****************************************/
/****************************************/

CNeatLoopFunctions::CNeatLoopFunctions() :
   m_cArenaSideX(-1.0f, 1.0f),
   m_cArenaSideY(-1.0f, 1.0f),
   m_cRadiusPos(0.0f,1.2f),
   m_cOrientation(CRadians::ZERO, CRadians::TWO_PI),
   m_unNumberRobots(1),
   m_fDistributionRadius(1),
   m_pcRng(NULL),
   m_nTrial(1) {
}

/****************************************/
/****************************************/

CNeatLoopFunctions::~CNeatLoopFunctions() {
    RemoveArena();
}

/****************************************/
/****************************************/

void CNeatLoopFunctions::Init(TConfigurationNode& t_tree) {

    m_pcRng = CRandom::CreateRNG("argos");
    TConfigurationNode cParametersNode;
    try {
      cParametersNode = GetNode(t_tree, "params");
      GetNodeAttributeOrDefault(cParametersNode, "number_robots", m_unNumberRobots, (UInt32) 1);
      GetNodeAttributeOrDefault(cParametersNode, "build_arena", m_bBuildArena, (bool) false);
      GetNodeAttributeOrDefault(cParametersNode, "number_edges", m_unNumberEdges, (UInt32) 3);
      GetNodeAttributeOrDefault(cParametersNode, "number_boxes_per_edge", m_unNumberBoxes, (UInt32) 1);
      GetNodeAttributeOrDefault(cParametersNode, "lenght_boxes", m_fLenghtBoxes, (Real) 0.20);
      GetNodeAttributeOrDefault(cParametersNode, "piecewise_config", m_unPwConfig, (UInt32) 0);
      GetNodeAttributeOrDefault(cParametersNode, "piecewise_exp", m_unPwExp, (UInt32) 0);
      GetNodeAttributeOrDefault(cParametersNode, "piecewise_time", m_unPwTime, (UInt32) 1200);
    } catch(std::exception e) {
      LOGERR << e.what() << std::endl;
    }

    if (m_bBuildArena == true)
    {
        m_fDistributionRadius = GetArenaRadious();
        PositionArena();
    }
   // Add the robots in the space.
   PositionRobots();

   // Process any trial information.
  // try {

      //GetNodeAttribute(t_tree, "trial", m_nTrial);

      // We should reset, only if there is a parameter trial in the argos configuration file.
      // The reason is that in the main code, we're already calling reset(), but not when we're launching argos with the command line.
      //Reset();
  // } catch(CARGoSException& ex) {}
}

/****************************************/
/****************************************/
//
void CNeatLoopFunctions::Reset() {
    MoveRobots();
}

/****************************************/
/****************************************/

void CNeatLoopFunctions::MoveRobots() {
    CEPuckEntity* pcEpuck;
    bool bPlaced = false;
    UInt32 unTrials;
    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
      pcEpuck = any_cast<CEPuckEntity*>(it->second);
      // Choose position at random
      unTrials = 0;
      do {
         ++unTrials;
         CVector3 cEpuckPosition = GetRandomPosition();
         bPlaced = MoveEntity(pcEpuck->GetEmbodiedEntity(),
                              cEpuckPosition,
                              CQuaternion().FromEulerAngles(m_pcRng->Uniform(CRange<CRadians>(CRadians::ZERO,CRadians::TWO_PI)),
                              CRadians::ZERO,CRadians::ZERO),false);
      } while(!bPlaced && unTrials < 100);
      if(!bPlaced) {
         THROW_ARGOSEXCEPTION("Can't place robot");
      }
    }
}

/****************************************/
/****************************************/
//
void CNeatLoopFunctions::PositionRobots() {
    CEPuckEntity* pcEpuck;
    bool bPlaced = false;
    UInt32 unTrials;
    for(UInt32 i = 1; i < m_unNumberRobots + 1; ++i) {
      std::ostringstream id;
      id << "epuck" << i;
      pcEpuck = new CEPuckEntity(id.str().c_str(),
                                 "epuck_nn_controller",
                                 CVector3(0,0,0),
                                 CQuaternion().FromEulerAngles(CRadians::ZERO,CRadians::ZERO,CRadians::ZERO));
      AddEntity(*pcEpuck);
      m_pvecEpucks.push_back(pcEpuck);
      CEPuckNNController* pcController = &dynamic_cast<CEPuckNNController&>(pcEpuck->GetControllableEntity().GetController());
      m_pvecControllers.push_back(pcController);
      // Choose position at random
      unTrials = 0;
      do {
         ++unTrials;
         CVector3 cEpuckPosition = GetRandomPosition();
         bPlaced = MoveEntity((*pcEpuck).GetEmbodiedEntity(),
                              cEpuckPosition,
                              CQuaternion().FromEulerAngles(m_pcRng->Uniform(CRange<CRadians>(CRadians::ZERO,CRadians::TWO_PI)),
                              CRadians::ZERO,CRadians::ZERO),false);
      } while(!bPlaced && unTrials < 100);
      if(!bPlaced) {
         THROW_ARGOSEXCEPTION("Can't place robot");
      }
    }
}

/****************************************/
/****************************************/

void CNeatLoopFunctions::ConfigureFromNetwork(NEAT::Network& net) {
   for(UInt32 i=0; i < m_pvecControllers.size(); i++) {
      m_pvecControllers[i]->SetNetwork(net);
   }
}

void CNeatLoopFunctions::RemoveRobots() {
  for(UInt32 i = 1; i < m_unNumberRobots + 1; ++i) {
    std::ostringstream id;
    id << "epuck" << i;
    RemoveEntity(id.str().c_str());
  }
}

/****************************************/
/****************************************/

void CNeatLoopFunctions::PositionArena() {
  CArenaEntity* pcArena;
    pcArena = new CArenaEntity("arena",
                               CVector3(0,0,0),
                               CQuaternion().FromEulerAngles(CRadians::ZERO,CRadians::ZERO,CRadians::ZERO), // TODO
                               CVector3(0.01,m_fLenghtBoxes,0.1),
                               "leds",
                               m_unNumberBoxes,
                               m_unNumberEdges,
                               0.017f,
                               1.0f);
    AddEntity(*pcArena);
    m_pcArena = pcArena;
}

/****************************************/
/****************************************/

void CNeatLoopFunctions::RemoveArena() {
    std::ostringstream id;
    id << "arena";
    RemoveEntity(id.str().c_str());
}

/****************************************/
/****************************************/

Real CNeatLoopFunctions::GetArenaRadious() {

    Real fRadious;
    fRadious =  (m_fLenghtBoxes*m_unNumberBoxes) / (2 * Tan(CRadians::PI / m_unNumberEdges));
    //fRadious = fRadious - 0.10; // Avoids to place robots close the walls.
    fRadious = fRadious - 0.65; // Reduced cluster at the begining

    return fRadious;
}

/****************************************/
/****************************************/

bool CNeatLoopFunctions::IsEven(UInt32 unNumber) {
    bool even;
    if((unNumber%2)==0)
       even = true;
    else
       even = false;

    return even;
}

/****************************************/
/****************************************/
