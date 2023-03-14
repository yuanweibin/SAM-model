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

1. Make sure your system can run OpenFOAM and has OpenFOAM's enviormental variables, e.g. `$FOAM_SRC`, `$WM_PROJECT_USER_DIR`, etc.
You can test it by typing
  
   ```shell
   echo $WM_PROJECT_USER_DIR;
   echo $FOAM_SRC;
   ```
   

2. Copy the existing models to `$WM_PROJECT_USER_DIR`

   ```shell
   cd $WM_PROJECT_USER_DIR;
   mkdir src;
   cd src;
   cp -r $FOAM_SRC/TurbulenceModels .;
   cd TurbulenceModels;
   ```
   
3. You need modify some files before creating you own `.lib` file,
   ```shell
   vi incompressible/Make/files;
   ```
   change the last line `LIB = $(FOAM_LIBBIN)/libincompressibleTurbulenceModels` to
   ```shell
   LIB = $(FOAM_USER_LIBBIN)/libincompressibleTurbulenceModels
   ```
   Similary, open another file
   ```shell
   vi compressible/Make/files;
   ```
   change the last line `LIB = $(FOAM_LIBBIN)/libcompressibleTurbulenceModels` to
   ```shell
   LIB = $(FOAM_USER_LIBBIN)/libcompressibleTurbulenceModels
   ```

4. compile the codes by
   ```shell
   ./Allmake
   ``` 
   
5. Now, you can add your own model. Here we use `SpalartAllmarasDNS` as an example.
   Copy the code prodived by this repository to `$WM_PROJECT_USER_DIR/src/TurbulenceModels/turbulenceModels/RAS`.
   When you type the command,
   ```shell
   ls $WM_PROJECT_USER_DIR/src/TurbulenceModels/turbulenceModels/RAS/SpalartAllmarasDNS
   ``` 
   you shold see the below result,
   ```shell
   SpalartAllmarasDNS.C  SpalartAllmarasDNS.H
   ``` 
   
6. Let openFOAM know that you have added a new model.
   ```shell
   cd $WM_PROJECT_USER_DIR/src/TurbulenceModels;
   vi incompressible/turbulentTransportModels/turbulentTransportModels.C;
   ``` 
   add below two lines to RAS models part
   ```cpp
   #include "SpalartAllmarasDNS.H"
   makeRASModel(SpalartAllmarasDNS);
   ``` 

   The file should look this,
   ```cpp
   //............ 
   #include "SpalartAllmaras.H"
   makeRASModel(SpalartAllmaras);
   
   #include "SpalartAllmarasDNS.H"
   makeRASModel(SpalartAllmarasDNS);
   //............
   ```
   
   Similarly, changing the compressible sovler,
   ```shell
   cd $WM_PROJECT_USER_DIR/src/TurbulenceModels;
   vi compressible/turbulentFluidThermoModels/turbulentFluidThermoModels.C;
   ```
   add below two lines to RAS models part
   ```cpp
   #include "SpalartAllmarasDNS.H"
   makeRASModel(SpalartAllmarasDNS);
   ```
   
7. Modify the `Allwmake` file by
   ```shell
   vi Allwmake;
   ```
   Add the code `wmakeLnInclude -u turbulenceModels` before `wmake $targetType turbulenceModels`.
   So it looks like
   ```shell
   #............
   wmakeLnInclude -u turbulenceModels
   
   wmake $targetType turbulenceModels
   #............
   ```

8. Recompile by 
   ```shell
   ./Allwmake
   ```
   
9. Congratulations! You can use the new model named `SpalartAllmarasDNS` in openFOAM by changing the keyword in `constant/turbulenceProperties`!
   If you want to add more models for this repository, you need to repeat the steps 5 to 8.

# Contact me

If you have any problems, feel free to contact me: yxb5132@psu.edu
