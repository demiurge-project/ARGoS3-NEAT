# Description

The code maintained on this repo allows one to use NEAT and the ARGoS3 simulator together (and on a cluster using OpenMPI):
- [Evolving Neural Networks through Augmenting Topologies](http://nn.cs.utexas.edu/downloads/papers/stanley.ec02.pdf)
- [ARGoS3](http://iridia.ulb.ac.be/~mdorigo/HomePageDorigo/thesis/phd/PinciroliPhDThesis.pdf)

In this directory, you will find at least:

* build: directory created by the cmake command, it contains all the executables/libraries,...
* CMakeLists: file containing the cmake instructions.
* config: directory containing the argos and neat configuration files, and the starter genomes needed by NEAT.
* controllers: directory containing the Epuck's Neural Network Controller.
* doc: directory containing some documentation.
* gen: directory where the results will be saved when we launch the evolutionary process.
* License: license file.
* loop_functions: directory containing the definition of the different loop-functions.
* NEAT: directory containing the NEAT source code (version 1.2.1).
* program: directory containing the main source code.


# Required

1. Operating System: Linux or MacOSX. Windows is not supported.

2. ARGoS3: The parallel and multi-engine simulator for heterogeneous swarm robotics ARGoS3 is required. Check the following [link](https://github.com/ilpincy/argos3 to install ARGoS3).
    * On MacOSX, you can install it directly by typing:
```
$ brew tap ilpincy/argos3 
$ brew install argos3
```

3. OpenMPI: The Message Passing Interface (MPI) library.
    * on MacOSX:
```
$ brew install open-mpi
```
    * on Linux (quick install):
```
$ sudo apt-get install openmpi-bin openmpi-common openssh-client openssh-server libopenmpi1.3 libopenmpi-dbg libopenmpi-dev
```


# Compiling

After compiling and installing ARGoS. 
From the neat directory, you need to type the following commands to build everything:
```bash
$ mkdir build
$ cd build
$ cmake ../
$ make
$ cd ..
```

# Command to launch

1. To launch the Evolutionary Process which uses NEAT and ARGoS (with the epuck robot):
    * Sequential
```
$ build/program/main config/neat.argos config/neatParams.ne config/<<startgenes>>
```
where <<startgenes>> is the stater genome file, which contains the definition of the 1st genome: for now, use ''evostickstartgenes'' (all inputs connected to outputs) or ''epuckstartgenes'' (all inputs disconnected).
   * Parallel
```
$ build/program/main config/neat.argos config/neatParams.ne config/<<startgenes>> <<n>> build/program/prog
```
where <<startgenes>> is the stater genome file, which contains the definition of the 1st genome: for now, use ''epuckstartgenes'' (all inputs connected to outputs) or ''epuckstartgenes0'' (all inputs disconnected).
where <<n>> is the nb of processes.

2. To launch a specific genome in ARGoS:
```bash
$ argos3 -c config/neat-trial.argos
```


# Create your own experiment

- If you want to create a new experiment with the current epuck's controller, which uses 8 proximity sensors, 8 light sensors, 3 ground sensors, 3 range-and-bearing sensors, a bias unit as inputs and 2 wheel actuators as outputs, you just need to create a new loop-function (which will evaluate the neural network), and possibly a new argos configuration file. Apart from those 2, You don’t need to create/change anything else.

- If instead you want to use another robot or the epuck with a different set of inputs/outputs, you will need to create your own controller, starter genome, and a new argos configuration file, in addition to the loop-function (if you want to create your own experiment).

- If you want to use just NEAT without the simulator ARGoS. You will need to modify the main program: in the main method, you will need to initialize your own experiment, then call the method launchNEAT(…) by passing your own defined method as a parameter.
launchNEAT(…) is a method that expects at least 3 arguments: the neat parameters file, the starter genome and your function that launches your experiment and evaluates an organism/network or population on this one. This method will set the evolutionary process and will call your method in which you are supposed to launch your experiment and evaluate the organisms/networks. After calling your method, launchNEAT(…) will evolve the population for the next generation.

Your method should accept only one parameter NEAT::Network& or NEAT::Population&.
-> If your method has NEAT::Network& as a parameter: you should, after launching the experiment and evaluating this network on it, return the fitness value.
-> If your method has NEAT::Population& as a parameter: you should launch the experiment for each organism and evaluate each one of them. Your method doesn’t need to return anything.

