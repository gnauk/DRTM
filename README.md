## DRTM
A physically based differentiable radiative transfer model (DRTM). Designed for remote sensing applications, DRTM enables efficient simulation and retrieval of optical properties (e.g., reflectance/transmittance spectra) and biochemical constituents (e.g., chlorophyll) from multispectral/hyperspectral imagery.


## Usage 
### 1、 Compile Mitsuba3 
Before using this project, users must first build Mitsuba3 from the **mitsuba3 folder**. Follow the official [Mitsuba3 documentation](https://mitsuba.readthedocs.io/en/latest/src/getting_started/compiling.html) for compilation instructions.
Note: The version in the mitsuba3 folder differs from the official release. The enhancement details will be added subsequently.

### 2、 Run UseCases 
The **usecases folder** contains ready-to-run examples demonstrating key functionalities. For scene generation, users may leverage existing 3D model architectures like LESS or Eradiate model.

## Usecases Introduction

### 1、Maize Scene
    The imagery simulation usecase.
    The multiple layer inversion usecase.
    The inversion usecase with the integration the DRTM and ProspectD models.
### 2、HET09_JBS_SUM Scene
    The imagery simulation usecase.
    The inversion usecase with one tree species.
    
### 3、The ProspectD code with DRJIT
     The ProspectD code with DRJIT to support the differentiation ability.

## Others
  Note: DRTM is now in code mode, dostn't has the GUI. The current implementation targets Mitsuba3 v3.5.0 due to certain code compatibility issues.

  The develop of DRTM model is on the road.

## reference

