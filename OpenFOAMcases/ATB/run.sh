#!/bin/sh

rm -r slurm*
#yhrun -n 1 blockMesh
#yhrun -n 1 decomposePar -latestTime
yhbatch -n 24 -p bigdata ATB.sh
