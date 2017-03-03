#!/usr/bin/python3

import argparse
import subprocess
import os
import re
import shlex
###############################################################################
# This script launches several runs in parallel in a SGE Cluster, and
# each run is parallelized using MPI.  Execute without parameters to see usage.
###############################################################################

########################### CONFIGURATION POINTS ##############################

NEAT_DIR = "/home/khasselm/argos3-neat"

QUEUE=short
#MACHINE=opteron2216 #rack 1
MACHINE=xeon5410 #rack 2
#MACHINE=opteron6128 #rack 3
#MACHINE=opteron6272 #rack 4
#MACHINE=xeon2680 #rack 5



################# Do not change unless you know what you do ###################

p = argparse.ArgumentParser(description='runs a run multiple times in paralell using mpi, python paralell_run_mpi.py')
p.add_argument('-d', '--dir', help='the execdir', required=True)
p.add_argument('-n', '--nbjobs', help='number of parallel jobs', type=int, required=True)
p.add_argument('-e', '--exp', help='the experiment xml', required=True)
p.add_argument('-p', '--params', help='the file for the parameters of the evolution', required=True)
p.add_argument('-s', '--startgenes', help='the file for the starting genome', required=True)
p.add_argument('-m', '--machine', help='the machine to run to', default=MACHINE)
p.add_argument('-q', '--queue', help='the queue to run to', default=QUEUE)


def run_neat(args):
    data = {"jobname": "neatevo-%s" % (args.dir).replace(' ',''),
            "machine": args.machine,
            "queue": args.queue,
            "execdir": args.dir,
            "nbjob": args.nbjobs,
            "neatdir": NEAT_DIR,
            "experiment": args.exp,
            "params": args.params,
            "startgenes": args.startgenes,
    }
    script = """#!/bin/bash
#$ -N %(jobname)s
#$ -l %(machine)s
#$ -l %(queue)s
#$ -m base
#      b     Mail is sent at the beginning of the job.
#      e     Mail is sent at the end of the job.
#      a     Mail is sent when the job is aborted or rescheduled.
#      s     Mail is sent when the job is suspended.
#$ -o %(execdir)s/argos3-neat.stdout
#$ -e %(execdir)s/argos3-neat.stderr
#$ -cwd
#$ -pe mpi %(nbjob)s
#$ -binding linear:256

USERNAME=`whoami`
NEATDIR=%(neatdir)s
COMMAND="$NEATDIR/bin/train %(experiment)s %(params)s %(startgenes)s %(nbjob)s $NEATDIR/bin/scheduler"

mkdir -p %(execdir)s/gen
cd %(execdir)s
echo "$COMMAND"
eval $COMMAND
$RET=$?
echo $RET
exit $RET"""
    pro = subprocess.run("qsub "+(script % data), stdout=subprocess.PIPE)


if __name__ == "__main__":
    args = p.parse_args()
    run_neat(args)
