// MPI
#include <mpi.h>

// Standard C++ Library
#include <iostream>
#include <vector>
#include <string>

// NEAT
#include "../NEAT/genome.h"

// ARGOS
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/utility/plugins/dynamic_loading.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

// Controller
#include "../NEATController.h"

// Loop function
#include <argos3/demiurge/loop-functions/CoreLoopFunctions.h>


/**
 * Child process: Initializes the MPI execution environment, ARGoS, and waits for the parent to give the random seed and the genome (string).
 * Once it has received all the necessary information, it creates the genome/network, and launches the experiment. At the end of the experiment,
 * it sends back the fitness to the parent.
 */
int main(int argc, char* argv[]) {

   // Initializes the MPI execution environment.
   MPI::Init();

   // Gets the parent intercommunicator of current spawned process.
   MPI::Intercomm parent_comm = MPI::Comm::Get_parent();

   // Gets the rank of the process.
   int id = MPI::COMM_WORLD.Get_rank();

   std::cout << "Hello, I'm the process " << id << "."<< std::endl;

   // Initialization of ARGoS
   argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
   argos::CDynamicLoading::LoadAllLibraries();
   cSimulator.SetExperimentFileName(argv[2]);
   cSimulator.LoadExperiment();
   static CoreLoopFunctions& cLoopFunctions = dynamic_cast<CoreLoopFunctions&>(cSimulator.GetLoopFunctions());

   // Waiting for the parent to give us some work to do.
   while(true) {

      // If we received an end signal, we get out of the while loop.
      bool cont;
      parent_comm.Recv(&cont, 1, MPI::BOOL, 0, 1);
      if(!cont) {
         std::cout << "ID" << id << ": END" << std::endl;
         break;
      }

      // Receiving the vector of random seeds
      std::vector<UInt32> vecRandomSeed;
      //vecRandomSeed.resize(NEAT::num_runs_per_gen); // Not correct: NEAT::num_runs_per_gen will always = 1
      MPI::Status status;
      parent_comm.Probe(0, 1, status);
      int nNum_runs_per_gen = status.Get_count(MPI::UNSIGNED);
      vecRandomSeed.resize(nNum_runs_per_gen);
      parent_comm.Recv(&vecRandomSeed[0], nNum_runs_per_gen, MPI::UNSIGNED, 0, 1);

      // Receiving the genome as a string
      //MPI::Status status;
      parent_comm.Probe(0, 1, status);
      int l = status.Get_count(MPI::CHAR);
      char *buf = new char[l];
      parent_comm.Recv(buf, l, MPI::CHAR, 0, 1, status);
      std::string strGenome(buf, l);
      delete[] buf;

      // Deserialize: String -> Genome
      std::vector<NEAT::Trait*> vecTraits;
      std::vector<NEAT::NNode*> vecNodes;
      std::vector<NEAT::Gene*> vecGenes;
      size_t i = 0, j = strGenome.find("\n",i);
      std::string str;

      while(j != std::string::npos) {

         str = strGenome.substr(i,j-i);

         if(str.compare(0,5,"trait") == 0) { //Trait

            str = str.substr(6);
            NEAT::Trait* pTrait = new NEAT::Trait(str.c_str());
            vecTraits.push_back(pTrait);

         } else if(str.compare(0,4,"node") == 0) { //Node

            str = str.substr(5);
            NEAT::NNode* pNode = new NEAT::NNode(str.c_str(), vecTraits);
            vecNodes.push_back(pNode);

         } else if(str.compare(0,4,"gene") == 0) { //Gene

            str = str.substr(5);
            NEAT::Gene* pGene = new NEAT::Gene(str.c_str(), vecTraits, vecNodes);
            vecGenes.push_back(pGene);

         }

         i = j+1;
         j = strGenome.find("\n",i);
      }

      // Create the new genome, and the network
      NEAT::Genome* genome = new NEAT::Genome(id, vecTraits, vecNodes, vecGenes);
      NEAT::Network* net = genome->genesis(genome->genome_id);

      // Launch the experiment with the correct random seed and network,
      // and evaluate the average fitness
      CSpace::TMapPerType cEntities = cSimulator.GetSpace().GetEntitiesByType("controller");
      for (CSpace::TMapPerType::iterator it = cEntities.begin(); it != cEntities.end(); ++it) {
          CControllableEntity* pcEntity = any_cast<CControllableEntity*>(it->second);
          try {
              CEPuckNEATController& cController = dynamic_cast<CEPuckNEATController&>(pcEntity->GetController());
              cController.SetNetwork(*net);
          } catch (std::exception& ex) {
              LOGERR << "Error while setting network: " << ex.what() << std::endl;
          }
      }

      double dFitness = 0.0;
      try {
        for(int j = 0; j < nNum_runs_per_gen; j++) {
          std::cout << "ID" << id << ": Random Seed received: " << vecRandomSeed[j] << std::endl;
          cSimulator.SetRandomSeed(vecRandomSeed[j]);
          cSimulator.Reset();
          cSimulator.Execute();
          dFitness += cLoopFunctions.GetObjectiveFunction();
        }
      } catch(...) {
        std::cout << "ERROR: ARGoS" << std::endl;
        std::cout << "j=" << j << std::endl;
        std::cout << "length=" << vecRandomSeed.size() << std::endl;
        std::cout << "seed=" << vecRandomSeed[j] << std::endl;
      }

      // Compute the average fitness
      if(nNum_runs_per_gen > 0) {
         dFitness /= nNum_runs_per_gen;
      }

      // Send the fitness to the parent.
      std::cout << "ID" << id << " --> Average fitness = " << dFitness << std::endl;
      parent_comm.Send(&dFitness, 1, MPI::DOUBLE, 0, 1);

      // Removing stuffs
      delete genome;
      delete net;
   }

   // Dispose of ARGoS stuff
   cSimulator.Destroy();

   // Terminates MPI execution environment.
   MPI_Finalize();

   return 0;
}
