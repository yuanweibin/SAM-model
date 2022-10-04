#!/bin/sh

rm -r slurm*
#yhrun -n 1 blockMesh
#yhrun -n 1 decomposePar
yhbatch -n 24 -p bigdata job.sh
