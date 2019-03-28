/* The NN controller */
#include "../controllers/epuck_nn/epuck_nn_controller.h"

/* ARGoS-related headers */
#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/utility/math/rng.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>
#include <argos3/plugins/simulator/entities/box_entity.h>
#include <argos3/plugins/robots/arena/simulator/arena_entity.h>

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

   /*
    * Return the radious of the arena.
    */
   Real GetArenaRadious();

protected:

   /*
    * Method used to remove the robots from the arena.
    */
   void RemoveRobots();

   /*
    * Method used to reallocate the robots.
    * The position is given by the method GetRandomPosition().
    */
   void MoveRobots();

   /*
    * Method used to create and distribute the robots.
    * The position is given by the method GetRandomPosition().
    */
   void PositionRobots();

   /*
    * Method used to create and distribute the Arena.
    */
   void PositionArena();

   /*
    * Method used to remove the arena from the arena.
    */
   void RemoveArena();

   /*
    * Method used to deternmine wheter a number is even.
    */
   bool IsEven(UInt32 unNumber);

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

    /*
     * Build the arena with the arena_entity plugin.
     */
    bool m_bBuildArena;

    /*
     * The number of edges in the arena used in the experiment.
     */
    UInt32 m_unNumberEdges;

    /*
     * The number of boxes in each edge of the arena used in the experiment.
     */
    UInt32 m_unNumberBoxes;

    /*
     * The lenght of the boxes used in the experiment.
     */
    Real m_fLenghtBoxes;

    /*
     * The arena used in the experiment.
     */
    CArenaEntity* m_pcArena;

    /*
     * The position of the boxes in the arena used in the experiment.
     */

    CRandom::CRNG* m_pcRng;

    /*
     * Transition time in sequence experiments
     */
    UInt32 m_unNumerColors;

    /*
     * Selector of the order of the color in the arena
     * (0:Random, 1:First Blue, 2: FirstRed)
     */
    UInt32 m_unColorOrder;

    /*
     * Selector of the task executed in blue arena
     */
    UInt32 m_unBlueTask;

    /*
     * Selector of the task executed in red arena
     */
    UInt32 m_unRedTask;

    /*
     * Task to be evaluated in sequence experiments
     * (0:Overall (TODO), 1: Blue, 2: Red)
     */
    UInt32 m_unEvalTask;

    /*
     * Alpha value for linearly combine the objetive functions
     * [0,2,4,5,6,8] --- But could be extended as Real
     */
    Real m_fAlphaValue;

    /*
     * Transition time in sequence experiments
     */
    UInt32 m_unTrnTime;

    // Vector of Epuck controllers.
    std::vector<CEPuckNNController*> m_pvecControllers;

};
