#ifndef MVDetectorConstruction_H
#define MVDetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"
#include "G4VSolid.hh"
#include "MVGlobals.hh"
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
        MVDetectorConstruction(Config& config);
        ~MVDetectorConstruction();

        G4VPhysicalVolume* Construct() override;
        void ConstructSDandField() override;

        void UpdateGeometry();

        // Set material properties.
        void SetScintYield(G4double newScintYield);
        
        // Set detector constants
        void SetFiberCount(G4int newFiberCount);

        // Get methods
        inline G4double GetScintYield() const { return scintYield; }
        inline G4int GetFiberCount() const { return fiberCount; }

    private:
        Config fConfig;
        MVDetectorMessenger* fDetectorMessenger;

        // World
        G4double worldX;
        G4double worldY;
        G4double worldZ;

        // Total Box
        G4double wholeX;
        G4double wholeY;
        G4double wholeZ;

        // PS type
        G4int psType;

        // Uninstrumented area
        G4double uninstrumentedX;

        // Aluminum reflection layer
        G4double shellThickness;
        G4double reflectionThickness;

        // Fiber
        G4double fiberDiameter; // diameter of fiber
        G4int fiberCount; // number of fiber
        G4double claddingDepth; // thickness of cladding
        G4double uturnRadius; // fiber bending radius for UTurn configuration
        G4double FiberDepthUpper;
        G4double FiberDepthLower;

        // Groove
        G4double grooveWidth; // along y direction
        G4double grooveDepth; // along z direction

        // Groove pool
        G4double uturnPoolX;

        // SiPM
        G4double SiPMX;
        G4double SiPMY;
        G4double SiPMZ;
        
        // CSV file path
        std::string ps_scint;
        std::string fiber_abs;
        std::string fiber_emi;

        /* Parameters, these should NOT be set in SetDefaults(), **
        ** but should be CALCULATED from CalculateParameters().  */

        /* Parameters for PScint */
        G4double PScintX;
        G4double PScintY;
        G4double PScintZ;
        G4double PScintXTranslation;

        /* Parameters for grooves */

        // These are read directly from the design graph, determined by "fiber_count"21
        std::vector<G4double> straightGrooveLength; // full length of middle fiber part, along the x direction
        G4double mergedGrooveX; // full x length of merged groove
        G4double mergedGrooveY; // full y length of merged groove, on sides of pscint

        // These are calculated middle results
        G4double grooveYInterval; // nearest distance between adjacent grooves
        std::vector<G4double> obliqueGrooveLength; // length of oblique groove
        std::vector<G4double> obliqueGrooveAngle; // oblique angle of groove
        G4double unmergedTotalY; // full y length of merged groove, on the opposite side of "merged_groove_x"

        /* Parameters for fibers */
        std::vector<G4double> StraightFiberPartLength; // full length of middle fiber straight part (along z direction)
        std::vector<G4double> StraightFiberPartTranslationY; // bending radius of first bending part
        std::vector<G4double> ObliqueFiberPartTranslationX; // bending angle of first bending part
        std::vector<G4double> ObliqueFiberPartTranslationY; // full length of oblique straight fiber
        std::vector<G4double> ObliqueFiberPartLength;
        std::vector<G4double> ConnectingFiberPartRotRadius; // bending radius of second bending part
        std::vector<G4double> SiPMFiberPartRotRadius_0;
        std::vector<G4double> SiPMFiberPartRotRadius_1;
        std::vector<G4double> SiPMFiberPartAngle;
        std::vector<G4double> SiPMFiberPartTranslationX_0;
        std::vector<G4double> SiPMFiberPartTranslationY_0;
        std::vector<G4double> SiPMFiberPartTranslationX_1;
        std::vector<G4double> SiPMFiberPartTranslationY_1;

        // Material
        G4Material* Air;
        G4Material* PSAluminum;
        G4Material* PSPolystyrene;
        G4Material* SiPMGlass;
        G4Material* CladdingPMMA;
        G4Material* FiberPolystyrene;
        G4Material* GrooveGlue;

        // Material Properties
        G4double scintYield;

        // Optical surface
        G4OpticalSurface* AluminumSurface;

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

        // Solid construction of groove
        G4VSolid* GetGroove() const;
        G4VSolid* GetGrooveBending() const;
        G4VSolid* GetGrooveUTurn() const;
        G4VSolid* GetGrooveStraight() const;

        // Solid construction of fiber part
        G4VSolid* GetFiberPart(G4double diameter) const;
        G4VSolid* GetFiberPartBending() const;
        G4VSolid* GetFiberPartUTurn() const;
        G4VSolid* GetFiberPartStraight() const;
};

}
#endif