#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include <iostream>
#include "neat.h"
#include "network.h"
#include "population.h"
#include "organism.h"
//#include "genome.h"
//#include "species.h"

using namespace NEAT;

void launchNEAT(const char *neatParams, const char *startGenes, double (*fctExperiment)(Network&));
void launchNEAT(const char *neatParams, const char *startGenes, void (*fctExperiment)(Population&, unsigned int));
void launchNEAT(const char *trainingSet, const char *neatParams, const char *startGenes, void (*fctExperiment)(Population&, unsigned int, std::vector<std::string>&, unsigned int));

#endif
