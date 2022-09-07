#ifndef MVDetectorConstruction_H
#define MVDetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"
class G4VPhysicalVolume;
class G4Material;
class G4OpticalSurface;
class G4MultiUnion;
class G4Box;
class G4MaterialPropertiesTable;

namespace MuonVeto
{
class MVDetectorMessenger;

class MVDetectorConstruction : public G4VUserDetectorConstruction
{
    public:
        MVDetectorConstruction();
        ~MVDetectorConstruction();

        G4VPhysicalVolume* Construct() override;
        void ConstructSDandField() override;

        void UpdateGeometry();

        // Set material properties.
        void SetScintYield(G4double newScintYield);

        // Get methods
        inline G4double GetScintYield() { return scintYield; }

    private:
        MVDetectorMessenger* fDetectorMessenger;

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
        // G4double fiber_bend_r; // radius of bending fiber, not used in new geometry
        // G4double fiber_depth_upper; // depth from the surface of Pscint to center of upper fiber, not used in new geometry
        // G4double fiber_depth_lower; // depth from the surface of Pscint to center of lower fiber(This is intended to avoid overlapping), not used in new geometry
        G4double cladding_depth_1; // thickness of the outer cladding
        G4double cladding_depth_2; // thickness of the inner cladding

        // Groove
        G4double groove_width; // along x direction
        G4double groove_depth; // along y direction

        // SiPM
        G4double SiPM_width; // along y direction
        G4double SiPM_length; // along x direction
        G4double SiPM_depth; // along z direction
        G4double air_interval; // distance between SiPM and Pscint

        /* Parameters, these should NOT be set in SetDefaults(), **
        ** but should be CALCULATED from CalculateParameters().  */

        /* Parameters for grooves */

        // These are read directly from the design graph, determined by "fiber_count"
        std::vector<G4double> straight_groove_length; // full length of middle fiber part, along the z direction
        G4double merged_groove_z; // full z length of merged groove
        G4double merged_groove_x; // full x length of merged groove, on sides of pscint

        // These are calculated middle results
        G4double x_interval_between_grooves; // nearest distance between adjacent grooves
        std::vector<G4double> oblique_groove_length; // length of oblique groove
        std::vector<G4double> oblique_groove_angle; // oblique angle of groove
        G4double unmerged_total_x; // full x length of merged groove, on the opposite side of "merged_groove_x"

        /* Parameters for fibers */
        std::vector<G4double> fiber_straight_1_length; // full length of middle fiber straight part (along z direction)
        std::vector<G4double> fiber_radius_1; // bending radius of first bending part
        std::vector<G4double> fiber_angle_1; // bending angle of first bending part
        std::vector<G4double> fiber_straight_2_length; // full length of oblique straight fiber
        std::vector<G4double> fiber_radius_2; // bending radius of second bending part
        std::vector<G4double> fiber_angle_2; // bending angle of second bending part


        // Material
        G4Material* air;
        G4Material* teflon;
        G4Material* LAB;
        G4Material* glass;
        G4Material* PMMA;
        G4Material* Pethylene_1;
        G4Material* Pethylene_2;
        G4Material* groove_mat;

        // Material Properties
        G4double scintYield;

        // Optical surface
        G4OpticalSurface* op_LAB_teflon_surface;
        G4OpticalSurface* op_teflon_air_surface;

        // Definition of materials and optical surfaces
        void DefineMaterials();
        void DefineMaterialTables();
        void DefineOpticalSurfaces();

        // Initialization
        void SetDefaults();

        // Calculate parameters for fibers and grooves
        void CalculateParameters();

        // Real detector construction
        G4VPhysicalVolume* ConstructDetector() const;

        // Solid construction of fiber part
        G4MultiUnion* GetFiberPart(const G4String &name, G4double diameter, G4Box* pscint_solid, G4int fiber_index) const;

};

}
#endif