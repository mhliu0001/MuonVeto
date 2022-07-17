#ifndef MVDetectorConstruction_H
#define MVDetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"
class G4VPhysicalVolume;
class G4Material;
class G4OpticalSurface;
class G4IntersectionSolid;
class G4Box;

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
        G4double cladding_depth_1; // thickness of the outer cladding
        G4double cladding_depth_2; // thickness of the inner cladding

        // Groove
        G4double groove_width; // along y direction
        G4double groove_length; // along x direction
        G4double groove_depth; // along z direction

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
        G4Material* Pethylene_1;
        G4Material* Pethylene_2;
        G4Material* groove_mat;

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

        // Solid construction of fiber part
        G4IntersectionSolid* GetFiberPart(const G4String &name, G4double diameter, G4Box* pscint_solid, G4int fiber_index) const;

};

}
#endif