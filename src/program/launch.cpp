/**
 * @file <argos3/core/simulator/main.cpp>
 *
 * @author Ken Hasselmann - <khasselm@ulb.ac.be>
 *
 * @package ARGoS3-AutoMoDe
 *
 * @license MIT License
 */

#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/utility/plugins/dynamic_loading.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/entity/entity.h>
#include <argos3/core/simulator/entity/controllable_entity.h>
//#include <argos3/core/simulator/query_plugins.h>
#include <argos3/core/simulator/argos_command_line_arg_parser.h>
#include "../NEATController.h"
#include <argos3/demiurge/loop-functions/CoreLoopFunctions.h>

using namespace argos;

/**
 * @brief The standard main() function to run the ARGoS simulator.
 *
 * This main() function provides the basic logic to run the ARGoS
 * simulator: parses the command line, loads the experiment, runs the
 * simulation and disposes all the data.
 *
 * @param n_argc the number of command line arguments given at the shell.
 * @param ppch_argv the actual command line arguments.
 * @return 0 if everything OK; 1 in case of errors.
 */
int main(int n_argc, char** ppch_argv) {

  std::string strGenome;
  UInt32 unSeed = 0;
  bool bCommandLineGenomeFound = false;
  std::vector<std::string> vecCommandLineGenome;

  try {

    int nCurrentArgument = 1;
		while(!bCommandLineGenomeFound && nCurrentArgument < n_argc) {
			if(strcmp(ppch_argv[nCurrentArgument], "--cl-genome") == 0) {
				bCommandLineGenomeFound = true;
				nCurrentArgument++;
				while (nCurrentArgument < n_argc) {
					vecCommandLineGenome.push_back(std::string(ppch_argv[nCurrentArgument]));
					nCurrentArgument++;
				}
				// Do not take the genome configuration into account in the standard command line parsing.
				n_argc = n_argc - vecCommandLineGenome.size() - 1;
			}
			nCurrentArgument++;
		}
    /* Create a new instance of the simulator */
    static CSimulator& cSimulator = CSimulator::GetInstance();

    /* Configure the command line options */
    CARGoSCommandLineArgParser cACLAP;
    cACLAP.AddArgument<std::string>('g', "genome", "genome file for your robots", strGenome);
    cACLAP.AddArgument<UInt32>('s', "seed", "", unSeed);
    /* Parse command line */
    cACLAP.Parse(n_argc, ppch_argv);

    if (!bCommandLineGenomeFound && strGenome.empty()) {
      THROW_ARGOSEXCEPTION("A genome is needed!");
    }

    switch(cACLAP.GetAction()) {
      case CARGoSCommandLineArgParser::ACTION_RUN_EXPERIMENT: {
        CDynamicLoading::LoadAllLibraries();
        cSimulator.SetExperimentFileName(cACLAP.GetExperimentConfigFile());
        cSimulator.SetRandomSeed(unSeed);
        cSimulator.LoadExperiment();

        //TConfigurationNode& cConfigRoot = cSimulator.GetConfigurationRoot();
        //SetNodeAttribute(GetNode(cConfigRoot, "controllers"), "genome_file", strGenome);
        // Duplicate the finite state machine and pass it to all robots.
        CSpace::TMapPerType cEntities = cSimulator.GetSpace().GetEntitiesByType("controller");
        for (CSpace::TMapPerType::iterator it = cEntities.begin(); it != cEntities.end(); ++it) {
            CControllableEntity* pcEntity = any_cast<CControllableEntity*>(it->second);
            try {
                CEPuckNEATController& cController = dynamic_cast<CEPuckNEATController&> (pcEntity->GetController());
                if (!bCommandLineGenomeFound) {
                  cController.LoadNetwork(strGenome);
                } else {
                  cController.LoadNetworkFromCommandLine(vecCommandLineGenome);
                }
            } catch (std::exception& ex) {
                LOGERR << "Error while casting: " << ex.what() << std::endl;
            }
        }

        cSimulator.Execute();

        CoreLoopFunctions& cLoopFunctions = dynamic_cast<CoreLoopFunctions&>(cSimulator.GetLoopFunctions());
				Real fObjectiveFunction = cLoopFunctions.GetObjectiveFunction();
				std::cout << "Score " << fObjectiveFunction << std::endl;

        break;
      }
      case CARGoSCommandLineArgParser::ACTION_QUERY:
          CDynamicLoading::LoadAllLibraries();
          //QueryPlugins(cACLAP.GetQuery());
          break;
      case CARGoSCommandLineArgParser::ACTION_SHOW_HELP:
          cACLAP.PrintUsage(LOG);
          break;
      case CARGoSCommandLineArgParser::ACTION_SHOW_VERSION:
          cACLAP.PrintVersion();
          break;
      case CARGoSCommandLineArgParser::ACTION_UNKNOWN:
          /* Should never get here */
          break;
  }
  /* Done, destroy stuff */
    cSimulator.Destroy();
   }
   catch(std::exception& ex) {
      /* A fatal error occurred: dispose of data, print error and exit */
      LOGERR << ex.what() << std::endl;
#ifdef ARGOS_THREADSAFE_LOG
      LOG.Flush();
      LOGERR.Flush();
#endif
      return 1;
   }
   /* Everything's ok, exit */
   return 0;
}
