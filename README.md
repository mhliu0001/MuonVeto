# MuonVeto: Geant4-based MC project for Muon Veto design

## Dependencies

**geant4** version >= 11.0 (with support for OpenGL), **CMake** version >=3.16



## Usage

Source the `geant4.sh` and `geant4make.sh`  in the geant4 installation directory, and then run the following code in the terminal (with GUI):

```shell
mkdir build/
cd build/
cmake ../
make
./MuonVeto # or ./MuonVeto ${some macro file} for batch mode
```

The code can work both in interactive mode and batch mode.