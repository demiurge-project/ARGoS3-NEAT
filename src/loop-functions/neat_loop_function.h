/* The NN controller */
#include "../controllers/epuck_nn/epuck_nn_controller.h"

/* ARGoS-related headers */
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/utility/math/rng.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

/* NEAT */
#include "../NEAT/neat.h"
#include "../NEAT/network.h"

/****************************************/
/****************************************/

using namespace argos;

class CNeatLoopFunctions : public CLoopFunctions {

public:

   CNeatLoopFunctions();
   virtual ~CNeatLoopFunctions();

   virtual void Init(TConfigurationNode& t_node);
   virtual void Reset();

   /* Configures the robot controller from the network */
   void ConfigureFromNetwork(NEAT::Network& c_net);

   /*
    * This method should return the fitness of the controller.
    */
   virtual Real GetObjectiveFunction() = 0;

   /*
    * Return a random position.
    */
   virtual CVector3 GetRandomPosition() = 0;

protected:
    /* Move/Place randomly the epucks in the arena */
    void MoveRobots();

    // Range of the arena.
    CRange<Real> m_cArenaSideX, m_cArenaSideY;
    CRange<Real> m_cRadiusPos;
    CRange<CRadians> m_cOrientation;

    // Vector of Epuck entities.
    std::vector<CEPuckEntity*> m_pvecEpucks;

    /*
    * The number of robots to be placed for the experiment.
    */
    UInt32 m_unNumberRobots;

    /*
    * The radius of the circle in which the robots will be distributed.
    */
    Real m_fDistributionRadius;

    CRandom::CRNG* m_pcRng;

    int m_nTrial;

    void PositionRobots();
    void RemoveRobots();
    // Vector of Epuck controllers.
    std::vector<CEPuckNNController*> m_pvecControllers;

};
