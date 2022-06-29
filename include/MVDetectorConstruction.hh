#ifndef MVDetectorConstruction_H
#define MVDetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"
class G4VPhysicalVolume;
class G4Material;
class G4OpticalSurface;

namespace MuonVeto
{

class MVDetectorConstruction : public G4VUserDetectorConstruction
{
    public:
        MVDetectorConstruction();
        ~MVDetectorConstruction();

        G4VPhysicalVolume* Construct() override;
        void ConstructSDandField() override;

    private:
        // Experimental Hall
        G4double expHall_x;
        G4double expHall_y;
        G4double expHall_z;

        // Pscint
        G4double pscint_x;
        G4double pscint_y;
        G4double pscint_z;

        // Teflon reflection layer
        G4double teflon_depth;

        // Fiber
        G4double fiber_d; // diameter of fiber
        G4int fiber_count; // number of fiber
        G4double fiber_bend_r; // radius of bending fiber
        G4double fiber_depth; // depth from the surface of Pscint to center of fiber

        // Groove
        G4double groove_depth;
        G4double groove_width;

        // SiPM
        G4double SiPM_width; // along y direction
        G4double SiPM_length; // along x direction
        G4double SiPM_depth; // along z direction
        G4double air_interval; // distance between SiPM and Pscint

        // Material
        G4Material* air;
        G4Material* teflon;
        G4Material* LAB;
        G4Material* glass;
        G4Material* PMMA;

        // Optical surface
        G4OpticalSurface* op_LAB_teflon_surface;
        G4OpticalSurface* op_teflon_air_surface;

        // Definition of materials and optical surfaces
        void DefineMaterials();
        void DefineMaterialTables();
        void DefineOpticalSurfaces();

        // Initialization
        void SetDefaults();

        // Real detector construction
        G4VPhysicalVolume* ConstructDetector() const;

};

}
#endif