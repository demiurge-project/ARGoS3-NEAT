#include "experiments.h"
#include <stdio.h>
#include <iostream>

/**
 * Function that launches NEAT: load all the useful parameters for NEAT, create the first population based on the starter genome,
 * call the experiment and evaluation defined by the user, and create the next generations.
 * @params: neatParams - file containing the useful parameters for NEAT.
 *	    startGenes - file containing the structure of the starter genome.
 *	    (*fctExperiment)(Network&) - function defined by the user that launches the experiment on a network, and return the evaluation on this last one.
 * */
void launchNEAT(const char *neatParams, const char *startGenes, double (*fctExperiment)(Network&), unsigned int seed) {

   // Useful variables
   Population* pop = NULL;
   Genome* startGenome = NULL;
   char curword[20];
   int id;

   if (seed == 0) {
     // Random Setup: Seed the random-number generator with current time.
     srand((unsigned)time(NULL));
   } else {
     srand(seed);
   }

   // Load all the useful parameters for NEAT
   NEAT::load_neat_params(neatParams,true);

   // Create the starter genome
   std::ifstream iFile(startGenes,std::ios::in);
   iFile >> curword;
   iFile >> id;
   startGenome = new Genome(id,iFile);
   iFile.close();

   // For each run
   for(int run=0; run < NEAT::num_runs; run++) {

      // Set the new population based on the starter genome.
      pop = new Population(startGenome, NEAT::pop_size);
      // For each generation
      for(int g=1; g <= NEAT::num_gens; g++) {
         printf("\n Generation %d\n", g);

         // Evaluate each organism on a test
         id=1;
         for(std::vector<Organism*>::iterator org = (pop->organisms).begin(); org != (pop->organisms).end(); ++org, id++) {

            printf("Organism: %d\n", id);

            // Launch the experiment with the specified network passed in argument, and evaluate this last one
            (*org)->fitness = (*fctExperiment)(*((*org)->net));

            printf("Fitness: %f\n", (*org)->fitness);
         }

         // Write the result in a file.
         if(NEAT::print_every!=0 && (g % NEAT::print_every)==0) {
            sprintf(curword, "gen/gen_%d_%d", run, g);
            pop->print_to_file_by_species(curword);
            //pop->print_species_champs_tofiles();
         }

         // Reproduction and creation of the next generation is done by NEAT
         pop->epoch(g);
      }

      // Todo: Post-Evaluation

      // Delete the population
      delete pop;
   }

   // Delete the genome
   delete startGenome;
}

/**
 * Function that launches NEAT: load all the useful parameters for NEAT, create the first population based on the starter genome,
 * call the experiment and evaluation defined by the user, and create the next generations.
 * @params: neatParams - file containing the useful parameters for NEAT.
 *	    startGenes - file containing the structure of the starter genome.
 *	    (*fctExperiment)(Population&) - function defined by the user that launches the experiment on a whole population.
 *		  The evaluation of each organism can be done in parallel. This parallelism is left to the user to define it.
 * */
void launchNEAT(const char *neatParams, const char *startGenes, void (*fctExperiment)(Population&, unsigned int),
                unsigned int seed) {

   // Useful variables
   Population* pop = NULL;
   Genome* startGenome = NULL;
   char curword[20];
   int id;

  if (seed == 0) {
    // Random Setup: Seed the random-number generator with current time.
    srand((unsigned)time(NULL));
  } else {
    srand(seed);
  }

   // Load all the useful parameters for NEAT
   NEAT::load_neat_params(neatParams,true);

   // Create the starter genome
   std::ifstream iFile(startGenes, std::ios::in);
   iFile >> curword;
   iFile >> id;
   startGenome = new Genome(id,iFile);
   iFile.close();

   // For each run
   for(int run=1; run <= NEAT::num_runs; run++) {
      // Set the new population based on the starter genome.
      pop = new Population(startGenome, NEAT::pop_size);

      // For each generation
      for(int g=1; g <= NEAT::num_gens; g++) {

         // Launch the experiment <NEAT::num_runs_per_gen> times with the specified population
         // passed in argument, and evaluate each organism in this last one.
         (*fctExperiment)(*pop, NEAT::num_runs_per_gen);

         // Write the result in a file.
         if(NEAT::print_every!=0 && (g % NEAT::print_every)==0) {
            sprintf(curword, "gen/gen_%d_%d", run, g);
            pop->print_to_file_by_species(curword);
            sprintf(curword, "gen/gen_%d_%d_champ", run, g);
            pop->print_champ_to_file(curword);
            //pop->print_species_champs_tofiles();
         }

         // Reproduction and creation of the next generation is done by NEAT
         pop->epoch(g);
      }
      // Post-Evaluation of the last population
      if(NEAT::num_runs_post_eval != 0) {
         std::cout << "\nPost-Evaluation" << std::endl;
         (*fctExperiment)(*pop, NEAT::num_runs_post_eval);
      }

      // Write the result in a file.
      sprintf(curword, "gen/gen_last_%d", run);
      pop->print_to_file_by_species(curword);
      sprintf(curword, "gen/gen_last_%d_champ", run);
      pop->print_champ_to_file(curword);
      //pop->print_species_champs_tofiles();

      // Delete the population
      delete pop;
   }

   // Delete the genome
   delete startGenome;
}
