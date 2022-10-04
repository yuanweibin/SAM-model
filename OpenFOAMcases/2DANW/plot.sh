#!/bin/bash

reconstructPar -latestTime
#postProcess -func columnAverage -latestTime
#postProcess -func sample -latestTime
#simpleFoam -postProcess -func wallShearStress -latestTime
postProcess -func surfaces -latestTime
