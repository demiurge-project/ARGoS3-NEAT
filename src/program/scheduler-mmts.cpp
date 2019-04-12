// MPI
#include <mpi.h>

// Standard C++ Library
#include <iostream>
#include <vector>
#include <string>

// NEAT
#include "../NEAT/genome.h"


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

      // Receiving the experiment file
      parent_comm.Probe(0, 1, status);


      // Receiving the genome as a string
      parent_comm.Probe(0, 1, status);
      int l = status.Get_count(MPI::CHAR);
      char *buf = new char[l];
      parent_comm.Recv(buf, l, MPI::CHAR, 0, 1, status);
      std::string strGenome(buf, l);
      delete[] buf;

      // Launch the experiment with the correct random seed and network,
      // and evaluate the average fitness


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

   // Terminates MPI execution environment.
   MPI_Finalize();

   return 0;
}
