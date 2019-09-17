// Standard C++ Library
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>

// NEAT
#include "../NEAT/experiments.h"

// ARGOS
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/visualization/visualization.h>

// Controller
#include "../NEATController.h"

// Loop function
#include <argos3/demiurge/loop-functions/CoreLoopFunctions.h>

/**
 * Function that launches the experiment and evaluates this last one.
 * @note: To launch your own program: you can use, for example, system("") which is a combination of fork(), exec() and waitpid().
 */
void launchARGoSAndEvaluate(NEAT::Population& pop, unsigned int num_runs_per_gen) {

   // The CSimulator class of ARGoS is a singleton.
   static argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();

   // Get a reference to the loop functions (the evaluation is done by the loop fct)
   static CoreLoopFunctions& cLoopFunctions = dynamic_cast<CoreLoopFunctions&>(cSimulator.GetLoopFunctions());

   // Produce the different random seeds for the experiment
   argos::CRandom::CreateCategory("neat", 1);
   argos::CRandom::CRNG* pRNG = argos::CRandom::CreateRNG("neat");
   std::vector<UInt32> vecRandomSeed;
   for(size_t i=0; i<num_runs_per_gen; i++) {
      vecRandomSeed.push_back( pRNG->Uniform(argos::CRange<UInt32>(0, UINT32_MAX)) );
   }

   int i=0;

   for(std::vector<Organism*>::iterator itOrg = (pop.organisms).begin(); itOrg != (pop.organisms).end(); ++itOrg, ++i) {

      std::cout << "\nOrganism[" << i << "]: " << std::endl;

      // Set the neural network
      CSpace::TMapPerType cEntities = cSimulator.GetSpace().GetEntitiesByType("controller");
      for (CSpace::TMapPerType::iterator it = cEntities.begin(); it != cEntities.end(); ++it) {
          CControllableEntity* pcEntity = any_cast<CControllableEntity*>(it->second);
          try {
              CEPuckNEATController& cController = dynamic_cast<CEPuckNEATController&>(pcEntity->GetController());
              cController.SetNetwork(*((*itOrg)->net));
          } catch (std::exception& ex) {
              LOGERR << "Error while setting network: " << ex.what() << std::endl;
          }
      }

      // Launch the experiment with the specified random seed, and get the score
      double dPerformance = 0.0;
      for(size_t j = 0; j < num_runs_per_gen; j++) {
         std::cout << "Random seed: " << vecRandomSeed[j] << std::endl;
         cSimulator.SetRandomSeed(vecRandomSeed[j]);
         cSimulator.Reset();
         cSimulator.Execute();
         dPerformance += cLoopFunctions.GetObjectiveFunction();
      }

      // Compute the average performance
      if(num_runs_per_gen > 0) {
         dPerformance /= num_runs_per_gen;
      }

      // The evaluation of this network is stored in the fitness
      (*itOrg)->fitness = dPerformance;

      std::cout << "--> Average fitness = " << (*itOrg)->fitness << std::endl;
   }
}

/**
 * Function that launches the experiment and evaluates this last one.
 * @note: To launch your own program: you can use, for example, system("") which is a combination of fork(), exec() and waitpid().
 * TODO: set the random seed
 */
/*double launchARGoSAndEvaluate(NEAT::Network& net) {

   // The CSimulator class of ARGoS is a singleton.
   static argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();

   // Get a reference to the loop functions (the evaluation is done by the loop fct)
   static CoreLoopFunctions& cLoopFunctions = dynamic_cast<CoreLoopFunctions&>(cSimulator.GetLoopFunctions());

   // Launch the experiment with the correct network
   cSimulator.Reset();
   cLoopFunctions.ConfigureFromNetwork(net);
   cSimulator.Execute();

   // Return the evaluation of this network
   return cLoopFunctions.getPerformance();
}*/


/**
 * Main program.
 */
int main(int argc, char *argv[]) {

   // Check the arguments
   if (argc < 4) {
      std::cerr << "Arg1: A configuration file (.argos or .xml) is required for ARGoS." << std::endl;
      std::cerr << "Arg2: A NEAT parameters file (.ne file) is required to run the experiments." << std::endl;
      std::cerr << "Arg3: A starter genome file is required for the creation of the initial population." << std::endl;
      std::cerr << "Arg4 (optional): The seed (unsigned integer) used to initialize NEAT. If none is specified, then NEAT will initialize with the current time" << std::endl;
      return -1;
   }

  // Checks the seed (if it exists)
  int seed = 0;
  if (argc >= 5) {
    try {
      seed = atoi(argv[4]);
    }
    catch (const std::invalid_argument& err) {
      std::cerr << "Invalid argument: " << err.what() << std::endl;
      std::cerr << "The seed will be left at 0!" << std::endl;
    }
  }

   // Initialization of ARGoS
   argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
   cSimulator.SetExperimentFileName(argv[1]);
   cSimulator.LoadExperiment();

   // Launch NEAT with the specified experiment
   launchNEAT(argv[2], argv[3], launchARGoSAndEvaluate, seed);
   std::cout << "/*   End of evolution...   */" << std::endl;
   std::cout << "/*   Output files writen in gen/ folder   */" << std::endl;
   // Dispose of ARGoS stuff
   cSimulator.Destroy();

   return 0;
}
