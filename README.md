# Details and Cases of SAM Model

## Introduction

The SAM (Spalart-Allmaras with Machine learning) model is a modified version of the Spalart-Allmaras turbulence model that uses machine learning to improve the accuracy of flow simulations.
The details about the SAM model are available in our publication: [Bin et al., AIAA, 2023](https://doi.org/10.2514/1.J062870).

This repository provides three parts of the SAM model:

1. `Functions`: Contains the `fv1`, `fv2`, and `fw` functions obtained by the neural network. Users can replace these functions with linear interpolation to change SA model code to SAM model code.
2. `OpenFOAMsrc`: Contains the OpenFOAM's source code of SAM model (`SplartAllmarasDNS`) and standard SA model (`SplartAllmarasNASA`), which agrees with the details of the SA model documented in the [NASA TMR website](https://turbmodels.larc.nasa.gov/spalart.html).
3. `OpenFOAMcases`: Contains 12 different cases that have been tested by the SAM model. We provide the setup files of OpenFOAM for these cases so that users can directly run them with OpenFOAM.

## Usage

### Prerequisites

Before using the SAM model in OpenFOAM, make sure your system can run OpenFOAM and has OpenFOAM's environmental variables, such as `$FOAM_SRC` and `$WM_PROJECT_USER_DIR`. You can test this by typing:

```shell
echo $WM_PROJECT_USER_DIR;
echo $FOAM_SRC;
```

### Adding the SAM Model to OpenFOAM

1. Copy the existing models to `$WM_PROJECT_USER_DIR`

   ```shell
   cd $WM_PROJECT_USER_DIR
   mkdir src
   cd src
   cp -r $FOAM_SRC/TurbulenceModels .
   cd TurbulenceModels
   ```
   
2. Modify some files before creating your own `.lib` file. Run the following commands to change the last line of the `Make/files` file in the `incompressible` and `compressible` folders:
   ```shell
   vi incompressible/Make/files
   LIB = $(FOAM_USER_LIBBIN)/libincompressibleTurbulenceModels
   
   vi compressible/Make/files
   LIB = $(FOAM_USER_LIBBIN)/libcompressibleTurbulenceModels

   vi turbulenceModels/Make/files
   LIB = $(FOAM_USER_LIBBIN)/libturbulenceModels
   ```

3. Compile the codes by typing:
   ```shell
   ./Allwmake
   ``` 
   
4. Add your own model. For example, to add the SpalartAllmarasDNS model, copy the code provided by this repository to `$WM_PROJECT_USER_DIR/src/TurbulenceModels/turbulenceModels/RAS`. Type the following command to check that the code has been copied correctly:
   ```shell
   ls $WM_PROJECT_USER_DIR/src/TurbulenceModels/turbulenceModels/RAS/SpalartAllmarasDNS
   ``` 
   
5. Let OpenFOAM know that you have added a new model by modifying the `incompressible/turbulentTransportModels/turbulentTransportModels.C` and `compressible/turbulentFluidThermoModels/turbulentFluidThermoModels.C` files. Add the following lines to the RAS models section of the turbulentTransportModels.C file:
   ```cpp
   #include "SpalartAllmarasDNS.H"
   makeRASModel(SpalartAllmarasDNS);
   ``` 
   Add the following lines to the RAS models section of the `turbulentFluidThermoModels.C` file:
   ```cpp
   #include "SpalartAllmarasDNS.H"
   makeRASModel(SpalartAllmarasDNS);
   ``` 
   
6. Modify the `Allwmake` file by
   ```shell
   vi Allwmake
   ```
   Add the code `wmakeLnInclude -u turbulenceModels` before `wmake $targetType turbulenceModels`.
   So it looks like:
   ```shell
   #...
   wmakeLnInclude -u turbulenceModels
   
   wmake $targetType turbulenceModels
   #...
   ```

8. Recompile by running: 
   ```shell
   ./Allwmake
   ```
   
9. Congratulations! You can now use the new model named `SpalartAllmarasDNS` in OpenFOAM by changing the keyword in `constant/turbulenceProperties`! If you want to add more models from this repository, you need to repeat steps 5 to 8.

## Conclusion
In summary, this repository provides the SAM model, which is an extension of the standard SA model. It includes the necessary functions and OpenFOAM source code for users to easily incorporate the SAM model into their simulations. Additionally, we provide 12 OpenFOAM cases that have been tested with the SAM model, along with setup files for users to run these cases directly.

We hope that this repository is helpful for researchers and engineers who are interested in turbulence modeling and simulation. If you have any questions or encounter any issues while using the SAM model, please feel free to contact us at beanyuanwei@gmail.com. Thank you for your interest in our work!
