// MPI
#include <mpi.h>

// Standard C++ Library
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>

// NEAT
#include "../NEAT/genome.h"

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

double extractPerformance(std::string output) {
    std::vector<std::string> elements;
    elements = split(output, '\n');
    elements = split(elements.at(elements.size()-2), ' ');

    return atof((elements.at(elements.size()-1)).c_str());
}

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
      std::vector<unsigned int> vecRandomSeed;
      MPI::Status status;
      parent_comm.Probe(0, 1, status);
      int nNum_runs_per_gen = status.Get_count(MPI::UNSIGNED);
      vecRandomSeed.resize(nNum_runs_per_gen);
      parent_comm.Recv(&vecRandomSeed[0], nNum_runs_per_gen, MPI::UNSIGNED, 0, 1);

      // Receiving all configuration files combined into one string
      parent_comm.Probe(0, 1, status);
      int l = status.Get_count(MPI::CHAR);
      char *buf = new char[l];
      parent_comm.Recv(buf, l, MPI::CHAR, 0, 1, status);
      std::string strCombinedConfigFile(buf, l);
      delete[] buf;
      // Spliting the long string into individual configuration files
      std::vector<std::string> vecConfigFiles = split(strCombinedConfigFile, ';');

      // Receiving the genome as a string
      parent_comm.Probe(0, 1, status);
      l = status.Get_count(MPI::CHAR);
      char *buf2 = new char[l];
      parent_comm.Recv(buf2, l, MPI::CHAR, 0, 1, status);
      std::string strGenome(buf2, l);
      delete[] buf2;

      //std::cout << vecConfigFiles.back() << std::endl;
      
      // Launch the experiment with the correct configuration file, random seed, and genome.
      double dFitness = 0.0;
      for(int j = 0; j < nNum_runs_per_gen; j++) {
        //std::cout << "ID" << id << " received config file: " << split(vecConfigFiles[j],'/').back() << " and seed: " << vecRandomSeed[j] << std::endl;

        // Create command to execute
        std::stringstream ssCommandLine;
        ssCommandLine << "/home/aligot/MMTS/NEAT-mmts/bin/NEAT-launch";
        ssCommandLine << " -c " << vecConfigFiles[j];
        ssCommandLine << " -s " << vecRandomSeed[j];
        ssCommandLine << " --cl-genome " << transformOneLine(strGenome);
        const std::string temp = ssCommandLine.str();

        std::string output = exec(temp.c_str());
        std::cout << "ID" << id << " Perf = " << extractPerformance(output) << std::endl;
        dFitness += extractPerformance(output);
      }

      // Compute the average fitness
      if(nNum_runs_per_gen > 0) {
         dFitness /= nNum_runs_per_gen;
      }

      // Send the fitness to the parent.
      //std::cout << "ID" << id << " --> Average fitness = " << dFitness << std::endl;
      parent_comm.Send(&dFitness, 1, MPI::DOUBLE, 0, 1);
   }

   // Terminates MPI execution environment.
   MPI_Finalize();

   return 0;
}
