#!/bin/bash

reconstructPar -latestTime
#postProcess -func columnAverage -latestTime
#postProcess -func sample -latestTime
postProcess -func "grad(U)" -latestTime
postProcess -func surfaces -latestTime
