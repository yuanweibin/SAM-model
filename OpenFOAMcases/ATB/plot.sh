#!/bin/bash

reconstructPar -latestTime
#postProcess -func columnAverage -latestTime
#postProcess -func "grad(U)" -latestTime
#postProcess -func vorticity -latestTime
#postProcess -func "mag(vorticity)" -latestTime
rhoSimpleFoam -postProcess -func wallShearStress -latestTime
postProcess -func surfaces -latestTime
