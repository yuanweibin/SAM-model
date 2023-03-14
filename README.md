# Read me

Welcome!

This project gives three functions, i.e., fv1, fv2 and fw, of SAM model.
To change SA model code to SAM model code, users can replace these functions by linear iinterpolation.

There are two floders in this project.

1. '/Functions/': It constains the fv1, fv2 and fw obtained by the NN. 

2. '/OpenFOAMcases/src/': OpenFOAM's source code of different model

    * '/OpenFOAMcases/src/SAM': OpenFOAM's source code of SAM model for simpleFoam and pimpleFoam.

    * '/OpenFOAMcases/src/SAMcom': OpenFOAM's source code of SAM model for rhoSimpleFoam and rhoPimpleFoam.

    * '/OpenFOAMcases/src/NASA_SA': OpenFOAM's source code of standard SA model for simpleFoam and pimpleFoam.

    * '/OpenFOAMcases/src/NASA_SAcom': OpenFOAM's source code of standard SA model for rhoSimpleFoam and rhoPimpleFoam.

    * p.s. To use this code in OpemFOAM, reader should use 'wmake' to compile the code and include the .so file in the 'controlDict' file, i.e., adding "libs (*.so);" in this file.

    * p.s.s. The default SA model in OpenFOAM have some difference with standard SA model (given by NASA's TMR website).

3. '/OpenFOAMcases/caseName': the setup files for different cases.

# How to use

Before using SAM model in openFOAM, you should learn how to create a now model in OpenFOAM.
Here, I will show you the whole process.

1. Make sure your system can run OpenFOAM and has OpenFOAM's enviormental variables, e.g. $FOAM_SRC, $WM_PROJECT_USER_DIR, etc.
You can test it by typing
   
   `
   echo $WM_PROJECT_USER_DIR;
   echo $FOAM_SRC;
   `
   

2. Copy the existing models to $WM_PROJECT_USER_DIR

   `
   cd $WM_PROJECT_USER_DIR
   mkdir src
   cd src
   cp -r $FOAM_SRC/TurbulenceModels .
   cd TurbulenceModels
   `
   
3. Create you own `.lib` file by 
   

# Contact me

If you have any problems, feel free to contact me: yxb5132@psu.edu
