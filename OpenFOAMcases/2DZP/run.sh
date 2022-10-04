#!/bin/sh

rm -r slurm*
#yhrun -n 1 blockMesh
yhrun -n 1 decomposePar -latestTime
yhbatch -n 48 -p bigdata zpSAM.sh
