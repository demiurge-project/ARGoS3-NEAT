// MPI
#include <mpi.h>

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

// Loop function
#include "../loop-functions/neat_loop_function.h"

#include <stdint.h>
#define UINT32_MAX (0xffffffff)

// GLOBAL VARIABLE
MPI::Intercomm g_com;
int g_nbProcess;
unsigned int g_unRandomSeed = 1;

/**
 * Function that launches the experiment and evaluates this last one.
 * @note: To launch your own program: you can use, for example, system("") which is a combination of fork(), exec() and waitpid().
 */
void launchARGoSAndEvaluate(NEAT::Population& pop, unsigned int num_runs_per_gen) {

   // Check
   if(num_runs_per_gen == 0) return;

   std::cout << "numgen: " << NEAT::num_gens << std::endl;

     std::cout << "numruns: " << NEAT::num_runs << std::endl;

   // The CSimulator class of ARGoS is a singleton.
   static argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();

   // Gets a reference to the loop functions (the evaluation is done by the loop fct)
   static CNeatLoopFunctions& cLoopFunctions = dynamic_cast<CNeatLoopFunctions&>(cSimulator.GetLoopFunctions());

   // Produces the different random seeds for the experiment
   argos::CRandom::CreateCategory("neat", g_unRandomSeed);
   argos::CRandom::CRNG* pRNG = argos::CRandom::CreateRNG("neat");
   std::vector<UInt32> vecRandomSeed;
   for(size_t i=0; i<num_runs_per_gen; i++) {
      vecRandomSeed.push_back( pRNG->Uniform(argos::CRange<UInt32>(0, UINT32_MAX)) );
   }

   int i=0;

   for(std::vector<Organism*>::iterator itOrg = (pop.organisms).begin(); itOrg != (pop.organisms).end(); ++itOrg, ++i) {

      std::cout << "\nOrganism[" << i << "]: " << std::endl;

      // Sets the neural network
      cLoopFunctions.ConfigureFromNetwork(*((*itOrg)->net));

      // Launches the experiment with the specified random seed, and get the score
      double dPerformance = 0.0;
      std::cout << "--> number runs per gen = " << num_runs_per_gen << std::endl;

      for(size_t j = 0; j < num_runs_per_gen; j++) {
         std::cout << "Random seed: " << vecRandomSeed[j] << std::endl;
         cSimulator.SetRandomSeed(vecRandomSeed[j]);
         cSimulator.Reset();
         cSimulator.Execute();
         dPerformance += cLoopFunctions.GetObjectiveFunction();
      }
      // Computes the average performance
      if(num_runs_per_gen > 0) {
         dPerformance /= num_runs_per_gen;
      }

      // The evaluation of this network is stored in the fitness
      (*itOrg)->fitness = dPerformance;

      std::cout << "--> Average fitness = " << (*itOrg)->fitness << std::endl;
   }
}

/**
 * Function that launches in parallel (with MPI) the experiment for each organism in the population and evaluates each one.
 */
void launchARGoSInParallelAndEvaluate(NEAT::Population& pop, unsigned int num_runs_per_gen) {

   // Check
   if(num_runs_per_gen == 0) return;

   // Produces the different random seeds for the experiment, initialized with the clock
   argos::CRandom::CreateCategory("neat", g_unRandomSeed);
   argos::CRandom::CRNG* pRNG = argos::CRandom::CreateRNG("neat");
   std::vector<UInt32> vecRandomSeed;
   for(size_t i=0; i<num_runs_per_gen; i++) {
      vecRandomSeed.push_back( pRNG->Uniform(argos::CRange<UInt32>(0, UINT32_MAX)) );
   }

   // Serialization: Genome -> string
   std::vector<std::string> vecStr;
   for(std::vector<Organism*>::iterator itOrg = (pop.organisms).begin(); itOrg != (pop.organisms).end(); ++itOrg) {

      std::stringstream s;
      std::string str;

      NEAT::Genome* g = (*itOrg)->gnome;
      g->print_to_file(s);
      str = s.str();
      size_t pos = str.find("\n")+1;
      str = str.substr(pos, str.find("genomeend")-pos);

      vecStr.push_back(str);
   }

   // MPI: Sends the random seed and the genome & Receives the fitness.
   int nId = 0, nId1=0;
   int nSize = vecStr.size();
   int nGroup = ceil((double) nSize/g_nbProcess);
   MPI::Status status;
   int nSource;
   bool cont = true;
   double dFitness;

   // The following for-loop takes into account the case where the #Processes ≠ #Organisms.
   for(int i=0; i < nGroup; i++) {

      // Sends the random seed and the genome (string) to all the child processes.
      for(int j=0; (j < g_nbProcess) && (nId < nSize); j++, nId++) {
      	g_com.Send(&cont, 1, MPI::BOOL, j, 1);
         g_com.Send(&vecRandomSeed[0], vecRandomSeed.size(), MPI::UNSIGNED, j, 1);
         g_com.Send(vecStr[nId].c_str(), vecStr[nId].length(), MPI::CHAR, j, 1);
      }

      // Receives the result of each process and store it in the fitness of each organism
      for(int j=0; (j < g_nbProcess) && (nId1 < nSize); j++, nId1++) {
         g_com.Recv(&dFitness, 1, MPI::DOUBLE, MPI::ANY_SOURCE, MPI::ANY_TAG, status);
         nSource = status.Get_source();
         (pop.organisms[nSource + (nId1-j)])->fitness = dFitness;
      }

   }
}

/**
 * Main program.
 */
int main(int argc, char *argv[]) {

   // Checks the arguments
   if (argc < 4) {
      std::cerr << "Arg1: A configuration file (.argos or .xml) is required for ARGoS." << std::endl;
      std::cerr << "Arg2: A NEAT parameters file (.ne file) is required to run the experiments." << std::endl;
      std::cerr << "Arg3: A starter genome file is required for the creation of the initial population." << std::endl;
      std::cerr << "Arg4 (optional): The number (natural number) of processes you want to launch. If none is specified, or if it's 0 or 1, there will be only one process." << std::endl;
      std::cerr << "Arg5 (non-optional if more than 1process!): the name of the binary file to launch in parallel." << std::endl;
      return -1;
   }

   // Checks the number of processes to launch
   g_nbProcess = 0;
   if (argc > 5) {
      try {
         g_nbProcess = atoi(argv[4]); //should check if it's negative
      } catch (const std::invalid_argument& err) {
         std::cerr << "Invalid argument: " << err.what() << std::endl;
         std::cerr << "There will be only one process!" << std::endl;
      }
   }

   // Intializes the random number generator
   time_t t;
   srand((unsigned) time(&t));
   g_unRandomSeed = rand();

   // Launches the program
   if(g_nbProcess > 1) { // in parallel

      std::cout << "PARALLEL RUN" << std::endl;

      // Initializes the MPI execution environment.
      MPI::Init();

      // Spawns a number of identical binaries.
      g_com = MPI::COMM_WORLD.Spawn(argv[5], (const char**) argv, g_nbProcess, MPI::Info(), 0);

      // Launches NEAT with the specified experiment
      launchNEAT(argv[2], argv[3], launchARGoSInParallelAndEvaluate);

      // Sends a signal to terminate the children.
      std::cout << "Parent: Terminate children" << std::endl;
      bool cont = false;
      for(int j=0; (j < g_nbProcess); j++) {
         g_com.Send(&cont, 1, MPI::BOOL, j, 1);
      }

      // Terminates MPI execution environment.
      MPI_Finalize();

   } else { // sequential

      std::cout << "SEQUENTIAL RUN" << std::endl;

      // Initialization of ARGoS
      argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
      cSimulator.SetExperimentFileName(argv[1]);
      cSimulator.LoadExperiment();

      // Launches NEAT with the specified experiment
      launchNEAT(argv[2], argv[3], launchARGoSAndEvaluate);

      // Disposes of ARGoS stuff
      cSimulator.Destroy();
   }

   return 0;
}
