// MPI
#include <mpi.h>

// Standard C++ Library
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <stdexcept>
#include <stdio.h>


// NEAT
#include "../NEAT/experiments.h"

// ARGOS
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/utility/plugins/dynamic_loading.h>
#include <argos3/core/simulator/visualization/visualization.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>

// Controller
#include "../NEATController.h"

// Loop function
#include <argos3/demiurge/loop-functions/CoreLoopFunctions.h>

#include <stdint.h>
#include <unistd.h>
#define UINT32_MAX (0xffffffff)

// GLOBAL VARIABLE
MPI::Intercomm g_com;
unsigned int nbProcess;
unsigned int g_unRandomSeed = 1;

std::string exec(const char* cmd) {
  char buffer[128];
  std::string result = "";
  FILE* pipe = popen(cmd, "r");
  if (!pipe) throw std::runtime_error("popen() failed!");
  try {
      while (fgets(buffer, sizeof buffer, pipe) != NULL) {
          result += buffer;
      }
  } catch (...) {
      pclose(pipe);
      throw;
  }
  pclose(pipe);
  return result;
}

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

const std::string transformOneLine(const std::string &s) {
  std::vector<std::string> elems = split(s, '\n');
  std::vector<std::string>::iterator it;
  std::stringstream ssOneLine;
  for (it = elems.begin(); it != elems.end(); it++) {
    ssOneLine << *it << ' ';
  }
  return ssOneLine.str();
}

signed int extractPerformance(std::string output) {
    std::vector<std::string> elements;
    elements = split(output, '\n');
    std::vector<std::string>::iterator it;
    elements = split(elements.at(elements.size()-2), ' ');

    return atoi((elements.at(elements.size()-1)).c_str());
}

/**
 * Function that launches the experiment and evaluates this last one.
 * @note: To launch your own program: you can use, for example, system("") which is a combination of fork(), exec() and waitpid().
 */
void launchARGoSAndEvaluate(NEAT::Population& pop, unsigned int num_runs_per_gen, std::vector<std::string> &experiment_files) {
  std::cout << "Beginning of evaluation procedure" << std::endl;
   // Check
  if(num_runs_per_gen == 0) return;

  std::cout << "numgen: " << NEAT::num_gens << std::endl;
  std::cout << "numruns: " << NEAT::num_runs << std::endl;

  // Produces the different random seeds for the experiment
  std::vector<UInt32> vecRandomSeed;
  for(size_t i=0; i<num_runs_per_gen; i++) {
    vecRandomSeed.push_back(rand());
  }

  // Sample epuck model (contained in experiment file)
  std::vector<UInt32> vecRandomExpFileIndexes;
  for(size_t i=0; i<num_runs_per_gen; i++) {
    vecRandomExpFileIndexes.push_back(rand() % experiment_files.size());

  }

  // Iterating over all genomes
  int i=0;
  for(std::vector<Organism*>::iterator itOrg = (pop.organisms).begin(); itOrg != (pop.organisms).end(); ++itOrg, ++i) {

    std::cout << "\nOrganism[" << i << "]: " << std::endl;

    double dPerformance = 0.0;
    // Each genome is evaluate one or multiple times, and the performance is averaged.
    for(size_t j = 0; j < num_runs_per_gen; j++) {
      std::cout << "Exp file: " << experiment_files[vecRandomExpFileIndexes[j]] << "; Random seed: " << vecRandomSeed[j] << std::endl;

      std::stringstream ssGenome;
      ((*itOrg)->gnome)->print_to_file(ssGenome);
      std::cout << "Genome = " << std::endl;
      std::cout << transformOneLine(ssGenome.str()) << std::endl;

      // Create command to execute
      std::stringstream ssCommandLine;
      ssCommandLine << "/home/aligot/Desktop/Arena/NEAT-mmts/bin/NEAT-launch";
      ssCommandLine << " -c " << experiment_files[vecRandomExpFileIndexes[j]];
      ssCommandLine << " -s " << vecRandomSeed[j];
      ssCommandLine << " --cl-genome " << transformOneLine(ssGenome.str());
      const std::string temp = ssCommandLine.str();

      std::string output = exec(temp.c_str());
      std::cout << "Perf = " << extractPerformance(output) << std::endl;
      dPerformance += extractPerformance(output);
    }
    // Computes the average performance
    if(num_runs_per_gen > 0) {
        dPerformance /= num_runs_per_gen;
    }

    // The evaluation of this network is stored in the fitness
    (*itOrg)->fitness = dPerformance;

    std::cout << "--> Average fitness = " << (*itOrg)->fitness << std::endl;
  }
  //cSimulator.Destroy();
}

/**
 * Function that launches in parallel (with MPI) the experiment for each organism in the population and evaluates each one.
 */
void launchARGoSInParallelAndEvaluate(NEAT::Population& pop, unsigned int num_runs_per_gen, std::vector<std::string> &experiment_files) {

   // Check
   if(num_runs_per_gen == 0) return;

   // Produces the different random seeds for the experiment, initialized with the clock
   argos::CRandom::CRNG* pRNG = argos::CRandom::CreateRNG("neat");
   std::vector<UInt32> vecRandomSeed;
   for(size_t i=0; i<num_runs_per_gen; i++) {
      vecRandomSeed.push_back( pRNG->Uniform(argos::CRange<UInt32>(0, UINT32_MAX)) );
   }

   // Serialization: Genome -> string
   std::vector<std::string> vecStrGenomes;
   for(std::vector<Organism*>::iterator itOrg = (pop.organisms).begin(); itOrg != (pop.organisms).end(); ++itOrg) {

      std::stringstream ssGenome;
      std::string strGenome;
      ((*itOrg)->gnome)->print_to_file(ssGenome);
      strGenome = transformOneLine(ssGenome.str());
      vecStrGenomes.push_back(strGenome);
   }

   // MPI: Sends the random seed and the genome & Receives the fitness.
   int nId = 0, nId1=0;
   int nSize = vecStrGenomes.size();
   int nGroup = ceil((double) nSize/nbProcess);
   MPI::Status status;
   int nSource;
   bool cont = true;
   double dFitness;

   // The following for-loop takes into account the case where the #Processes ≠ #Organisms.
   for(int i=0; i < nGroup; i++) {

      // Sends the random seed and the genome (string) to all the child processes.
      for(int j=0; (j < nbProcess) && (nId < nSize); j++, nId++) {
      	g_com.Send(&cont, 1, MPI::BOOL, j, 1);
         g_com.Send(&vecRandomSeed[0], vecRandomSeed.size(), MPI::UNSIGNED, j, 1);
         g_com.Send(vecStrGenomes[nId].c_str(), vecStrGenomes[nId].length(), MPI::CHAR, j, 1);
      }

      // Receives the result of each process and store it in the fitness of each organism
      for(int j=0; (j < nbProcess) && (nId1 < nSize); j++, nId1++) {
         g_com.Recv(&dFitness, 1, MPI::DOUBLE, MPI::ANY_SOURCE, MPI::ANY_TAG, status);
         nSource = status.Get_source();
         (pop.organisms[nSource + (nId1-j)])->fitness = dFitness;
      }

   }
}


static void show_usage(std::string name) {
  std::cerr << "Mandatory arguments:\n"
            << "\t-cf,--config-folder\tSpecify the folder containing the configuration files (.argos)\n"
            << "\t-p,--params\t\tSpecify the NEAT parameters file (.ne)\n"
            << "\t-sg,--start-genome\tSpecify the starter genome file for the creation of the initial population\n"
            << "Optional arguments:\n"
            << "\t-s,--seed\t\tSpecify the seed for the design process\n"
            << "\t-np,--nb-processes\tSpecify the number of process to launch (if more than 1)\n"
            << "\t-b,--binary\t\tSpecify the scheduling binary file if more than 1 process in parallel.\n"
            << std::endl;
}

/**
 * Main program.
 */
int main(int argc, char *argv[]) {

  if (argc < 6) {
    show_usage(argv[0]);
    return 1;
  }
  std::vector <std::string> sources;
  std::string destination;
  const char * configFolder; // AKA the training set
  const char * neatParams;
  const char * startGenome;
  unsigned int seed = 0;
  unsigned int nbProcess = 0;
  const char * binaryProcess;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if ((arg == "-cf") || (arg == "--config-folder")) {
      if (i + 1 < argc) {
        configFolder = argv[i+1];
      } else {
        std::cerr << "-cf,--config-folder option requires one argument." << std::endl;
        return 1;
      }
    } else if ((arg == "-p") || (arg == "--params")) {
      if (i + 1 < argc) {
        neatParams = argv[i+1];
      } else {
        std::cerr << "-p,--params option requires one argument." << std::endl;
        return 1;
      }
    } else if ((arg == "-sg") || (arg == "--start-genome")) {
      if (i + 1 < argc) {
        startGenome = argv[i+1];
      } else {
        std::cerr << "-sg,--start-genome option requires one argument." << std::endl;
        return 1;
      }
    } else if ((arg == "-s") || (arg == "--seed")) {
      if (i + 1 < argc) {
        try {
          seed = atoi(argv[i+1]);
        } catch (const std::invalid_argument& e) {
          std::cerr << "Invalid argument: " << e.what() << std::endl;
          return 1;
        }
      } else {
        std::cerr << "-s,--seed option requires one argument." << std::endl;
        return 1;
      }
    } else if ((arg == "-np") || (arg == "--nb-processes")) {
      if (i + 1 < argc) {
        try {
          nbProcess = atoi(argv[i+1]);
        } catch (const std::invalid_argument& e) {
          std::cerr << "Invalid argument: " << e.what() << std::endl;
          return 1;
        }
      } else {
        std::cerr << "-np,--num-processes option requires one argument." << std::endl;
        return 1;
      }
    } else if ((arg == "-b") || (arg == "--binary")) {
      if (i + 1 < argc) {
        binaryProcess = argv[i+1];
      } else {
        std::cerr << "-b,--binary option requires one argument." << std::endl;
        return 1;
      }
    }
  }

  // Intializes the random number generator
  time_t t;
  if (seed > 0) {
    srand((unsigned) seed);
  } else {
    srand((unsigned) time(&t));
  }

  // Launches the program
  if(nbProcess > 1) { // in parallel
    std::cout << "PARALLEL RUN" << std::endl;

    // Initializes the MPI execution environment.
    MPI::Init();

    // Spawns a number of identical binaries.
    g_com = MPI::COMM_WORLD.Spawn(binaryProcess, (const char**) argv, nbProcess, MPI::Info(), 0);

    // Launches NEAT with the specified experiment
    launchNEAT(configFolder, neatParams, startGenome, launchARGoSInParallelAndEvaluate);

    // Sends a signal to terminate the children.
    std::cout << "Parent: Terminate children" << std::endl;
    bool cont = false;
    for(int j=0; (j < nbProcess); j++) {
      g_com.Send(&cont, 1, MPI::BOOL, j, 1);
    }

    // Terminates MPI execution environment.
    MPI_Finalize();

  } else { // sequential
    std::cout << "SEQUENTIAL RUN" << std::endl;

    // Initialization of ARGoS
    //argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
    //argos::CDynamicLoading::LoadAllLibraries();
    //cSimulator.SetExperimentFileName(argv[1]);
    //cSimulator.LoadExperiment();

    // Launches NEAT with the specified experiment
    launchNEAT(configFolder, neatParams, startGenome, launchARGoSAndEvaluate);

    // Disposes of ARGoS stuff
    //cSimulator.Destroy();
  }

   return 0;
}
