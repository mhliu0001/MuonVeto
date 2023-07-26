#include "MVDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"
#include "G4Element.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4Torus.hh"
#include "G4Trd.hh"
#include "G4UnionSolid.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4SDParticleFilter.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4MultiUnion.hh"
#include "G4IntersectionSolid.hh"
// #include "G4VPrimitiveScorer.hh"
// #include "SiPMPhotonAccumulator.hh"
#include "SiPMSD.hh"
#include "MVDetectorMessenger.hh"
#include "G4RunManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "BambooUtils.hh"
#include <string>
#include <cmath>

using namespace MuonVeto;

MVDetectorConstruction::MVDetectorConstruction(Config &config) : fConfig(config)
{
    fDetectorMessenger = new MVDetectorMessenger(this);
    SetDefaults();
}

MVDetectorConstruction::~MVDetectorConstruction()
{
    delete fDetectorMessenger;
}

void MVDetectorConstruction::SetDefaults()
{
    // Experimental Hall
    worldX = 500 * cm;
    worldY = 200 * cm;
    worldZ = 200 * cm;

    // Pscint
    wholeX = 1000 * mm;
    wholeY = 100 * mm;
    wholeZ = 20 * mm;

    // Teflon reflection layer
    shellThickness = 1 * mm;
    reflectionThickness = 0.1 * mm;

    // Fiber
    fiberDiameter = 1. * mm;
    // fiber_count = fConfig.fiberCount;
    // fiber_bend_r = 750*mm; // Not used in new geometry
    // fiber_depth_upper = 1.5*mm; // Not used in new geometry
    // fiber_depth_lower = 3.0*mm; // Not used in new geometry
    claddingDepth = 0.015 * mm;

    // Groove
    grooveDepth = 4.5 * mm;
    grooveWidth = 1.5 * mm;
    FiberDepthLower = -grooveDepth / 2 + fiberDiameter / 2 + 0.1 * mm;
    FiberDepthUpper = grooveDepth / 2 - fiberDiameter / 2 - 0.1 * mm;

    // SiPM
    SiPMX = 1 * mm;
    SiPMY = 6 * mm;
    SiPMZ = 6 * mm;

    // Material Properties
    scintYield = 8000. / MeV;

    // Read from PS configuration file
    std::ifstream psConfigStream(fConfig.psConfig);
    json psConfig = json::parse(psConfigStream);
    psType = psConfig["ps_type"];
    fiberCount = psConfig["fiber_count"];
    wholeX = BambooUtils::evaluate(psConfig["whole_x"]);
    wholeY = BambooUtils::evaluate(psConfig["whole_y"]);
    uninstrumentedX = BambooUtils::evaluate(psConfig["uninstrumented_x"]);
    ps_scint = psConfig["ps_scint"];
    fiber_abs = psConfig["fiber_abs"];
    fiber_emi = psConfig["fiber_emi"];
}

void MVDetectorConstruction::DefineMaterials()
{
    G4NistManager *man = G4NistManager::Instance();

    // Definition of elements
    G4Element *C = man->FindOrBuildElement("C");
    G4Element *H = man->FindOrBuildElement("H");
    G4Element *O = man->FindOrBuildElement("O");
    G4Element *Al = man->FindOrBuildElement("Al");

    // Definition of PSPolystyrene, used in plastic scintillators
    // http://relics.org.cn:9092/lib/exe/fetch.php?media=montecarlo:psmuonveto:sp101.pdf
    PSPolystyrene = new G4Material("PSPolystyrene", 1.023 * g / cm3, 2, kStateSolid);
    PSPolystyrene->AddElement(C, 8);
    PSPolystyrene->AddElement(H, 8); //(C8H8)n
    // PSPolystyrene = man->FindOrBuildMaterial("G4_POLYSTYRENE");

    // Definition of PSAluminum, used in plastic scintillator reflective layers
    PSAluminum = new G4Material("PSAluminum", 2.699 * g / cm3, 1, kStateSolid);
    PSAluminum->AddElement(Al, 1.);

    // Definition of Air, used in world
    Air = man->FindOrBuildMaterial("G4_AIR");

    // Definition of SiPMGlass, used in SiPM
    SiPMGlass = man->FindOrBuildMaterial("G4_GLASS_PLATE");

    // Definition of FiberPMMA, used in WLS fiber cladding
    CladdingPMMA = new G4Material("CladdingPMMA", 1200 * kg / m3, 3);
    CladdingPMMA->AddElement(H, 8);
    CladdingPMMA->AddElement(C, 5);
    CladdingPMMA->AddElement(O, 2);

    // Definition of FiberPolystyrene, used in WLS fiber core
    FiberPolystyrene = new G4Material("FiberPolystyrene", 1050 * kg / m3, 2);
    FiberPolystyrene->AddElement(H, 0.5);
    FiberPolystyrene->AddElement(C, 0.5);

    // Definition of GrooveGlue, used in groove
    GrooveGlue = new G4Material("GrooveGlue", 1200 * kg / m3, 3); // Epoxy resin, (C11H12O3)n
    GrooveGlue->AddElement(H, 12);
    GrooveGlue->AddElement(C, 11);
    GrooveGlue->AddElement(O, 3);
}

void MVDetectorConstruction::DefineMaterialTables()
{
    // Table for PSPolystyrene
    G4MaterialPropertiesTable *pPSPolystyreneTable = new G4MaterialPropertiesTable();
    std::vector<G4double> pPSPolystyrenePhotonMomentum = {2.0 * eV, 3.0 * eV, 4.0 * eV};
    std::vector<G4double> pPSPolystyreneRIndex = {1.58, 1.58, 1.58};
    std::vector<G4double> pPSPolystyreneAbsLength = {210 * cm, 210 * cm, 210 * cm};
    pPSPolystyreneTable->AddProperty("RINDEX", pPSPolystyrenePhotonMomentum, pPSPolystyreneRIndex);
    pPSPolystyreneTable->AddProperty("ABSLENGTH", pPSPolystyrenePhotonMomentum, pPSPolystyreneAbsLength);
    pPSPolystyreneTable->AddConstProperty("SCINTILLATIONYIELD", scintYield);
    pPSPolystyreneTable->AddConstProperty("RESOLUTIONSCALE", 0.0); // Since we only want to get LCE, fluctuations in photons need not be considered
    pPSPolystyreneTable->AddConstProperty("SCINTILLATIONYIELD1", 1.0);
    pPSPolystyreneTable->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.40 * ns);
    std::vector<G4double> pPSPolystyreneComPhotonMomentum = {};
    std::vector<G4double> pPSPolystyreneComFactor = {};
    read_csv(ps_scint, pPSPolystyreneComPhotonMomentum, pPSPolystyreneComFactor);
    pPSPolystyreneTable->AddProperty("SCINTILLATIONCOMPONENT1", pPSPolystyreneComPhotonMomentum, pPSPolystyreneComFactor);
    PSPolystyrene->SetMaterialPropertiesTable(pPSPolystyreneTable);

    // Table for PSAluminum
    G4MaterialPropertiesTable *pPSAluminumTable = new G4MaterialPropertiesTable();
    std::vector<G4double> pPSAluminumPhotonMomentum = {2.0 * eV, 3.0 * eV, 4.0 * eV};
    std::vector<G4double> pPSAluminumReflectivity = {0.85, 0.85, 0.85};
    pPSAluminumTable->AddProperty("REFLECTIVITY", pPSAluminumPhotonMomentum, pPSAluminumReflectivity);
    PSAluminum->SetMaterialPropertiesTable(pPSAluminumTable);

    // Table for Air
    G4MaterialPropertiesTable *pAirTable = new G4MaterialPropertiesTable();
    pAirTable->AddProperty("RINDEX", "Air");
    Air->SetMaterialPropertiesTable(pAirTable);

    // Table for SiPMGlass
    G4MaterialPropertiesTable *pSiPMGlassTable = new G4MaterialPropertiesTable();
    std::vector<G4double> pSiPMGlassPhotonMomentum = {2.0 * eV, 3.0 * eV, 4.0 * eV};
    std::vector<G4double> pSiPMGlasRIndex = {1.5, 1.5, 1.5};
    pSiPMGlassTable->AddProperty("RINDEX", pSiPMGlassPhotonMomentum, pSiPMGlasRIndex);
    SiPMGlass->SetMaterialPropertiesTable(pSiPMGlassTable);

    // Table for CladdingPMMA
    G4MaterialPropertiesTable *pCladdingPMMATable = new G4MaterialPropertiesTable();
    std::vector<G4double> pCladdingPMMAPhotonMomentum = {2.0 * eV, 3.0 * eV, 4.0 * eV};
    std::vector<G4double> pCladdingPMMARIndex = {1.49, 1.49, 1.49};
    pCladdingPMMATable->AddProperty("RINDEX", pCladdingPMMAPhotonMomentum, pCladdingPMMARIndex);
    CladdingPMMA->SetMaterialPropertiesTable(pCladdingPMMATable);

    // Table for FiberPolystyrene
    G4MaterialPropertiesTable *pFiberPolystyreneTable = new G4MaterialPropertiesTable();
    std::vector<G4double> pFiberPolystyrenePhotonMomentum = {2.0 * eV, 3.0 * eV, 4.0 * eV};
    std::vector<G4double> pFiberPolystyreneRIndex = {1.60, 1.60, 1.60};
    pFiberPolystyreneTable->AddProperty("RINDEX", pFiberPolystyrenePhotonMomentum, pFiberPolystyreneRIndex);
    /*
    std::vector<G4double> pFiberPolystyreneAbsPhotonMomentum = {};
    std::vector<G4double> pFiberPolystyreneAbsFactor = {};
    read_csv(fiber_abs, pFiberPolystyreneAbsPhotonMomentum, pFiberPolystyreneAbsFactor);
    pFiberPolystyreneTable->AddProperty("WLSABSLENGTH", pFiberPolystyreneAbsPhotonMomentum, pFiberPolystyreneAbsFactor);
    std::vector<G4double> pFiberPolystyreneEmiPhotonMomentum = {};
    std::vector<G4double> pFiberPolystyreneEmiFactor = {};
    read_csv(fiber_emi, pFiberPolystyreneEmiPhotonMomentum, pFiberPolystyreneEmiFactor);
    pFiberPolystyreneTable->AddProperty("WLSCOMPONENT", pFiberPolystyreneEmiPhotonMomentum, pFiberPolystyreneEmiFactor);
    */
    // Guang Luo's code
    G4double photonEnergy[] =
        {2.00 * eV, 2.03 * eV, 2.06 * eV, 2.09 * eV, 2.12 * eV,
         2.15 * eV, 2.18 * eV, 2.21 * eV, 2.24 * eV, 2.27 * eV,
         2.30 * eV, 2.33 * eV, 2.36 * eV, 2.39 * eV, 2.42 * eV,
         2.45 * eV, 2.48 * eV, 2.51 * eV, 2.54 * eV, 2.57 * eV,
         2.60 * eV, 2.63 * eV, 2.66 * eV, 2.69 * eV, 2.72 * eV,
         2.75 * eV, 2.78 * eV, 2.81 * eV, 2.84 * eV, 2.87 * eV,
         2.90 * eV, 2.93 * eV, 2.96 * eV, 2.99 * eV, 3.02 * eV,
         3.05 * eV, 3.08 * eV, 3.11 * eV, 3.14 * eV, 3.17 * eV,
         3.20 * eV, 3.23 * eV, 3.26 * eV, 3.29 * eV, 3.32 * eV,
         3.35 * eV, 3.38 * eV, 3.41 * eV, 3.44 * eV, 3.47 * eV,
         3.50 * eV, 3.53 * eV, 3.56 * eV, 3.59 * eV, 3.62 * eV,
         3.65 * eV, 3.69 * eV, 3.72 * eV, 3.75 * eV, 6.20 * eV};
    G4double absWLSfiber[] =
        {2.40 * m, 2.00 * m, 2.00 * m, 1.90 * m, 1.80 * m, 1.70 * m, 1.60 * m, 1.50 * m, 1.45 * m, 1.40 * m,
         1.39 * m, 1.36 * m, 1.35 * m, 1.34 * m, 1.33 * m, 1.32 * m, 1.30 * m, 1.20 * m, 1.19 * m, 1.10 * m,
         1.00 * m, 0.50 * m, 0.29 * m, 0.10 * m, 0.05 * m, 20.0 * mm, 10.0 * mm, 8.30 * mm, 3.00 * mm, 2.00 * mm,
         2.0 * mm, 1.40 * mm, 1.30 * mm, 1.20 * mm, 1.10 * mm, 0.10 * mm, 0.80 * mm, 0.90 * mm, 1.00 * mm, 1.20 * mm,
         1.38 * mm, 1.8 * mm, 2.08 * mm, 3.08 * mm, 4.80 * mm, 5.05 * mm, 6.02 * mm, 7.01 * mm, 8.01 * mm, 9.01 * mm,
         10.0 * mm, 11.0 * mm, 15.0 * mm, 20.0 * mm, 25.0 * mm, 60.0 * mm, 95.0 * mm, 1.00 * m, 2.00 * m, 5.00 * m};
    G4double emissionFib[] =
        {0.05, 0.10, 0.30, 0.50, 0.75, 1.00, 1.50, 1.85, 2.30, 2.75,
         3.25, 3.80, 4.50, 5.20, 6.00, 7.00, 8.50, 9.50, 11.1, 12.4,
         12.9, 13.0, 12.8, 12.3, 11.1, 11.0, 12.0, 11.0, 17.0, 16.9,
         15.0, 5.00, 3.50, 2.00, 1.05, 0.50, 0.40, 0.30, 0.20, 0.10,
         0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05,
         0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05};
    pFiberPolystyreneTable->AddProperty("WLSABSLENGTH", photonEnergy, absWLSfiber, 60);
    pFiberPolystyreneTable->AddProperty("WLSCOMPONENT", photonEnergy, emissionFib, 60);
    pFiberPolystyreneTable->AddConstProperty("WLSTIMECONSTANT", 2.7 * ns);
    FiberPolystyrene->SetMaterialPropertiesTable(pFiberPolystyreneTable);

    // Table for GrooveGlue
    G4MaterialPropertiesTable *pGrooveGlueTable = new G4MaterialPropertiesTable();
    std::vector<G4double> pGrooveGluePhotonMomentum = {2.0 * eV, 3.0 * eV, 4.0 * eV};
    std::vector<G4double> pGrooveGlueRIndex = {1.53, 1.53, 1.53};
    pGrooveGlueTable->AddProperty("RINDEX", pGrooveGluePhotonMomentum, pGrooveGlueRIndex);
    GrooveGlue->SetMaterialPropertiesTable(pGrooveGlueTable);
}

void MVDetectorConstruction::DefineOpticalSurfaces()
{
    // Surface between LAB and teflon, if border definition is used; if skin definition is used,
    // this surface is still used, but not "op_teflon_air_surface".
    AluminumSurface = new G4OpticalSurface(
        "AluminumSurface", unified, groundfrontpainted, dielectric_dielectric);
    G4MaterialPropertiesTable *AluminumSurfaceTable = new G4MaterialPropertiesTable();
    std::vector<G4double> AluminumSurfacePhotonMomentum = {2.0 * eV, 3.0 * eV, 4.0 * eV};
    std::vector<G4double> AluminumSurfaceReflectivity = {0.85, 0.85, 0.85};
    AluminumSurfaceTable->AddProperty("REFLECTIVITY", AluminumSurfacePhotonMomentum, AluminumSurfaceReflectivity);
    AluminumSurface->SetMaterialPropertiesTable(AluminumSurfaceTable);
}

G4VPhysicalVolume *MVDetectorConstruction::ConstructDetector() const
{
    bool check_overlap = true;

    // World
    G4Box *WorldBox = new G4Box("WorldBox", worldX, worldY, worldZ);
    G4LogicalVolume *WorldLogicalVolume = new G4LogicalVolume(WorldBox, Air, "WorldLogicalVolume");
    G4VPhysicalVolume *World = new G4PVPlacement(0, G4ThreeVector(), WorldLogicalVolume, "World", NULL, false, 0);

    // The entire box
    G4Box *WholePSBox = new G4Box("WholePSBox", 0.5 * wholeX, 0.5 * wholeY, 0.5 * wholeZ);
    G4LogicalVolume *WholePSBoxLogicalVolume = new G4LogicalVolume(WholePSBox, PSAluminum, "WholePSLogicalVolume");
    new G4PVPlacement(0, G4ThreeVector(), WholePSBoxLogicalVolume, "WholePS", WorldLogicalVolume, false, 0, check_overlap);

    // Inside the shell
    G4double InteriorBoxX = 0.5 * wholeX - shellThickness;
    if(psType == 2)
        InteriorBoxX = 0.5 * wholeX - shellThickness + reflectionThickness;
    G4Box *InteriorBox = new G4Box("InteriorBox", InteriorBoxX, 0.5 * wholeY - shellThickness + reflectionThickness, 0.5 * wholeZ - shellThickness + reflectionThickness);
    G4LogicalVolume *InteriorLogicalVolume = new G4LogicalVolume(InteriorBox, Air, "InteriorLogicalVolume");
    new G4PVPlacement(0, G4ThreeVector(), InteriorLogicalVolume, "Interior", WholePSBoxLogicalVolume, false, 0, check_overlap);

    // Reflective layer
    G4Box *ReflectionLayerBox = new G4Box("ReflectionBox", 0.5 * PScintX + reflectionThickness, 0.5 * PScintY + reflectionThickness, 0.5 * PScintZ + reflectionThickness);
    G4VSolid *ReflectionLayerSolid = ReflectionLayerBox;
    if (psType == 1)
    {
        G4Box *ReflectionLayerHoleBox = new G4Box("ReflectionHoleBox", 0.5 * reflectionThickness + 0.01*mm, 0.5 * mergedGrooveY, 0.5 * grooveDepth);
        ReflectionLayerSolid = new G4SubtractionSolid("ReflectionLayerSolid_0", ReflectionLayerSolid, ReflectionLayerHoleBox,
                                                      G4Translate3D(G4ThreeVector(-0.5 * (PScintX + reflectionThickness) - 0.01*mm, 0, PScintZ / 2 - grooveDepth / 2)));
        ReflectionLayerSolid = new G4SubtractionSolid("ReflectionLayerSolid_1", ReflectionLayerSolid, ReflectionLayerHoleBox,
                                                      G4Translate3D(G4ThreeVector(0.5 * (PScintX + reflectionThickness) + 0.01*mm, 0, PScintZ / 2 - grooveDepth / 2)));
    }
    else if (psType == 2)
    {
        G4Box *ReflectionLayerHoleBox = new G4Box("ReflectionHoleBox", 0.5 * reflectionThickness + 0.01*mm, 0.5 * grooveWidth, 0.5 * grooveDepth);
        for (int fiberIndex = 0; fiberIndex < 2*fiberCount; ++fiberIndex)
        {
            ReflectionLayerSolid = new G4SubtractionSolid("ReflectionLayerSolid_"+std::to_string(fiberIndex), ReflectionLayerSolid, ReflectionLayerHoleBox,
                                                          G4Translate3D(G4ThreeVector(-0.5 * (PScintX + reflectionThickness) - 0.01*mm, StraightFiberPartTranslationY[fiberIndex], PScintZ / 2 - grooveDepth / 2)));
        }
    }

    // PS part
    G4VSolid *PScintSolid = new G4Box("PScintBox", 0.5 * PScintX, 0.5 * PScintY, 0.5 * PScintZ);

    // Grooves for fibers
    G4VSolid *GrooveSolid = GetGroove();

    // Fiber cladding
    G4VSolid *FiberCladdingSolid = GetFiberPart(fiberDiameter);
    if (psType == 2)
    {
        ReflectionLayerSolid = new G4SubtractionSolid("ReflectionLayerSolid_final", ReflectionLayerSolid, PScintSolid);
        PScintSolid = new G4SubtractionSolid("PScintSolid", PScintSolid, GrooveSolid, G4TranslateZ3D(PScintZ/2-grooveDepth/2));
        GrooveSolid = new G4SubtractionSolid("GrooveSolid_0", GrooveSolid, FiberCladdingSolid, G4Translate3D(G4ThreeVector(-PScintXTranslation, 0, - PScintZ / 2 + grooveDepth / 2)));
    }
    G4LogicalVolume *ReflectionLayerLogicalVolume = new G4LogicalVolume(ReflectionLayerSolid, PSAluminum, "ReflectionLogicalVolume");
    G4LogicalVolume *PScintLogicalVolume = new G4LogicalVolume(PScintSolid, PSPolystyrene, "PScintLogicalVolume");
    G4LogicalVolume *GrooveLogicalVolume = new G4LogicalVolume(GrooveSolid, GrooveGlue, "GrooveLogicalVolume");
    G4LogicalVolume *FiberCladdingLogicalVolume = new G4LogicalVolume(FiberCladdingSolid, CladdingPMMA, "FiberCladdingLogicalVolume");

    // Fiber core
    G4VSolid *FiberCoreSolid = GetFiberPart(fiberDiameter - 2 * claddingDepth);
    G4LogicalVolume *FiberCoreLogicalVolume = new G4LogicalVolume(FiberCoreSolid, FiberPolystyrene, "FiberCoreLogicalVolume");

    // Placement
    if(psType == 1)
    {
        new G4PVPlacement(0, G4ThreeVector(PScintXTranslation, 0, 0), ReflectionLayerLogicalVolume, "ReflectionLayer", InteriorLogicalVolume, false, 0, check_overlap);
        new G4PVPlacement(0, G4ThreeVector(), PScintLogicalVolume, "PScint", ReflectionLayerLogicalVolume, false, 0, check_overlap);
        new G4PVPlacement(G4TranslateZ3D(PScintZ / 2 - grooveDepth / 2), GrooveLogicalVolume, "Groove", PScintLogicalVolume, false, 0, check_overlap);
        new G4PVPlacement(0, G4ThreeVector(), FiberCladdingLogicalVolume, "FiberCladding", GrooveLogicalVolume, false, 0, check_overlap);
        new G4PVPlacement(0, G4ThreeVector(), FiberCoreLogicalVolume, "FiberCore", FiberCladdingLogicalVolume, false, 0, check_overlap);
    }
    else if (psType == 2)
    {
        new G4PVPlacement(0, G4ThreeVector(PScintXTranslation, 0, 0), ReflectionLayerLogicalVolume, "ReflectionLayer", InteriorLogicalVolume, false, 0, check_overlap);
        new G4PVPlacement(0, G4ThreeVector(PScintXTranslation, 0, 0), PScintLogicalVolume, "PScint", InteriorLogicalVolume, false, 0, check_overlap);
        new G4PVPlacement(0, G4ThreeVector(PScintXTranslation, 0, PScintZ / 2 - grooveDepth / 2), GrooveLogicalVolume, "Groove", InteriorLogicalVolume, false, 0, check_overlap);
        new G4PVPlacement(0, G4ThreeVector(), FiberCladdingLogicalVolume, "FiberCladding", InteriorLogicalVolume, false, 0, check_overlap);
        new G4PVPlacement(0, G4ThreeVector(), FiberCoreLogicalVolume, "FiberCore", FiberCladdingLogicalVolume, false, 0, check_overlap);
    }
    

    // SiPMs
    G4Box *SiPMBox = new G4Box("SiPMBox", 0.5 * SiPMX, 0.5 * SiPMY, 0.5 * SiPMZ);
    G4LogicalVolume *SiPMLogicalVolume_0 = new G4LogicalVolume(SiPMBox, SiPMGlass, "SiPMLogicalVolume_0");
    G4LogicalVolume *SiPMLogicalVolume_1 = new G4LogicalVolume(SiPMBox, SiPMGlass, "SiPMLogicalVolume_1");
    if (psType == 1 || psType == 0)
    {
        new G4PVPlacement(0, G4ThreeVector(PScintXTranslation - SiPMX / 2 - PScintX / 2 - reflectionThickness, 0, PScintZ / 2 - SiPMZ / 2),
                          SiPMLogicalVolume_0, "SiPM_0", InteriorLogicalVolume, false, 0, check_overlap);
        new G4PVPlacement(0, G4ThreeVector(PScintXTranslation + SiPMX / 2 + PScintX / 2 + reflectionThickness, 0, PScintZ / 2 - SiPMZ / 2),
                          SiPMLogicalVolume_1, "SiPM_1", InteriorLogicalVolume, false, 0, check_overlap);
    }
    else
    {
        new G4PVPlacement(0, G4ThreeVector(PScintXTranslation - SiPMX / 2 - PScintX / 2 - SiPMPScintDistance, 0, PScintZ / 2 - SiPMZ / 2),
                          SiPMLogicalVolume_0, "SiPM_0", InteriorLogicalVolume, false, 0, check_overlap);
    }

    // Reflective layer
    new G4LogicalSkinSurface("AluminumSurface", ReflectionLayerLogicalVolume, AluminumSurface);

    return World;
}

G4VSolid *MVDetectorConstruction::GetGroove() const
{
    switch (psType)
    {
    case 1:
        return GetGrooveBending();
    case 2:
        return GetGrooveUTurn();
    case 0:
        return GetGrooveStraight();
    default:
        throw std::runtime_error("MVDetectorConstruction::GetGroove: Invalid PSType provided!");
    }
}

G4VSolid *MVDetectorConstruction::GetGrooveBending() const
{
    G4MultiUnion *grooveSolid = new G4MultiUnion("grooveSolid");

    // Merged groove
    G4Trd *mergedGrooveSolid = new G4Trd(
        "mergedGrooveSolid",
        unmergedTotalY / 2,
        mergedGrooveY / 2,
        grooveDepth / 2,
        grooveDepth / 2,
        mergedGrooveX / 2);

    grooveSolid->AddNode(mergedGrooveSolid, G4Transform3D(
                                                (new G4RotationMatrix())->rotateX(90 * degree).rotateZ(90 * degree),
                                                G4ThreeVector(PScintX / 2 - mergedGrooveX / 2, 0, 0)));
    grooveSolid->AddNode(mergedGrooveSolid, G4Transform3D(
                                                (new G4RotationMatrix())->rotateX(90 * degree).rotateZ(-90 * degree),
                                                G4ThreeVector(-PScintX / 2 + mergedGrooveX / 2, 0, 0)));

    for (int grooveIndex = 0; grooveIndex < fiberCount / 2; ++grooveIndex)
    {
        G4Box *straightGrooveSolid = new G4Box(
            "straightGrooveSolid" + std::to_string(grooveIndex),
            straightGrooveLength[grooveIndex] / 2,
            grooveWidth / 2,
            grooveDepth / 2);
        G4Box *obliqueGrooveSolid = new G4Box(
            "obliqueGrooveSolid" + std::to_string(grooveIndex),
            obliqueGrooveLength[grooveIndex] / 2,
            grooveWidth / 2,
            grooveDepth / 2);

        G4Transform3D obliqueTransformPosX = G4Transform3D(
            (new G4RotationMatrix())->rotateZ(-obliqueGrooveAngle[grooveIndex]),
            G4ThreeVector(
                0.5 * straightGrooveLength[grooveIndex] + 0.5 * obliqueGrooveLength[grooveIndex] * std::cos(obliqueGrooveAngle[grooveIndex]) - 0.5 * grooveWidth * std::sin(obliqueGrooveAngle[grooveIndex]),
                0.5 * grooveWidth - 0.5 * obliqueGrooveLength[grooveIndex] * std::sin(obliqueGrooveAngle[grooveIndex]) - 0.5 * grooveWidth * std::cos(obliqueGrooveAngle[grooveIndex]),
                0));

        G4Transform3D obliqueTransformNegX = G4Transform3D(
            (new G4RotationMatrix())->rotateZ(obliqueGrooveAngle[grooveIndex]),
            G4ThreeVector(
                -0.5 * straightGrooveLength[grooveIndex] - 0.5 * obliqueGrooveLength[grooveIndex] * std::cos(obliqueGrooveAngle[grooveIndex]) + 0.5 * grooveWidth * std::sin(obliqueGrooveAngle[grooveIndex]),
                0.5 * grooveWidth - 0.5 * obliqueGrooveLength[grooveIndex] * std::sin(obliqueGrooveAngle[grooveIndex]) - 0.5 * grooveWidth * std::cos(obliqueGrooveAngle[grooveIndex]),
                0));

        G4UnionSolid *grooveOnlyPosZ = new G4UnionSolid(
            "grooveOnlyPosZ" + std::to_string(grooveIndex),
            straightGrooveSolid,
            obliqueGrooveSolid,
            obliqueTransformPosX);
        G4UnionSolid *groovePosZAndNegZ = new G4UnionSolid(
            "groovePosZAndNegZ" + std::to_string(grooveIndex),
            grooveOnlyPosZ,
            obliqueGrooveSolid,
            obliqueTransformNegX);

        grooveSolid->AddNode(
            groovePosZAndNegZ,
            G4TranslateY3D((fiberCount / 2 - 0.5 - grooveIndex) * (grooveYInterval + grooveWidth)));

        grooveSolid->AddNode(
            groovePosZAndNegZ,
            G4Transform3D(
                (new G4RotationMatrix())->rotateX(180 * degree),
                G4ThreeVector(0, -(fiberCount / 2 - 0.5 - grooveIndex) * (grooveYInterval + grooveWidth), 0)));
    }

    grooveSolid->Voxelize();
    return grooveSolid;
}

G4VSolid *MVDetectorConstruction::GetGrooveUTurn() const
{
    G4MultiUnion *grooveSolid = new G4MultiUnion("grooveSolid");
    G4Box *poolBox = new G4Box("poolBox", uturnPoolX / 2, PScintY / 2, grooveDepth / 2);
    grooveSolid->AddNode(poolBox, G4TranslateX3D(PScintX / 2 - uturnPoolX / 2));

    for (int grooveIndex = 0; grooveIndex < fiberCount * 2; ++grooveIndex)
    {
        G4Box *singleFiberGrooveBox = new G4Box(G4String("singleFiberGrooveBox") + std::to_string(grooveIndex), (PScintX - uturnPoolX) / 2, grooveWidth / 2, grooveDepth / 2);
        grooveSolid->AddNode(singleFiberGrooveBox, G4Translate3D(-uturnPoolX / 2, (grooveYInterval + grooveWidth) * (fiberCount - 0.5 - grooveIndex), 0));
    }

    grooveSolid->Voxelize();
    return grooveSolid;
}

G4VSolid *MVDetectorConstruction::GetGrooveStraight() const
{
    G4MultiUnion *grooveSolid = new G4MultiUnion("grooveSolid");
    for (int grooveIndex = 0; grooveIndex < fiberCount; ++grooveIndex)
    {
        G4Box *singleFiberGrooveBox = new G4Box(G4String("singleFiberGrooveBox") + std::to_string(grooveIndex), PScintX / 2, grooveWidth / 2, grooveDepth / 2);
        grooveSolid->AddNode(singleFiberGrooveBox, G4TranslateY3D(grooveYInterval * (fiberCount / 2 - 0.5 - grooveIndex) + grooveWidth * (fiberCount / 2 - 1 - grooveIndex)));
    }

    grooveSolid->Voxelize();
    return grooveSolid;
}

void MVDetectorConstruction::CalculateParameters()
{
    // Calculate parameters for grooves
    PScintY = wholeY - 2 * shellThickness;
    PScintZ = wholeZ - 2 * shellThickness;
    switch (psType)
    {
    case 1:
        PScintX = wholeX - 2 * shellThickness - 2 * uninstrumentedX;
        PScintXTranslation = 0;
        break;
    case 2:
        PScintX = wholeX - 2 * shellThickness - uninstrumentedX;
        PScintXTranslation = 0.5 * uninstrumentedX;
        break;
    case 0:
        PScintX = wholeX - 2 * shellThickness - 2 * uninstrumentedX;
        PScintXTranslation = 0;
        break;
    default:
        throw std::runtime_error("MVDetectorConstruction::CalculateParameters: PS type invalid or not implemented!");
        break;
    }
    if (psType == 1)
    {
        assert(fiberCount == 4 || fiberCount == 6);
        if (fiberCount == 4)
        {
            // Four fiber configuration
            straightGrooveLength.push_back(460.00 / 1000 * wholeX);
            straightGrooveLength.push_back(500.00 / 1000 * wholeX);
            assert(int(straightGrooveLength.size()) == fiberCount / 2);

            mergedGrooveX = 15.00 * mm;
            mergedGrooveY = 4.00 * mm;
        }
        else
        {
            // Six fiber configuration
            straightGrooveLength.push_back(440.00 / 1000 * wholeX);
            straightGrooveLength.push_back(460.00 / 1000 * wholeX);
            straightGrooveLength.push_back(500.00 / 1000 * wholeX);
            assert(int(straightGrooveLength.size()) == fiberCount / 2);

            mergedGrooveX = 22.00 * mm;
            mergedGrooveY = 5.00 * mm;
        }
        grooveYInterval = (PScintY - fiberCount * grooveWidth) / (fiberCount + 1);
        unmergedTotalY = mergedGrooveX / (PScintX / 2 - straightGrooveLength[0] / 2) * ((fiberCount - 1) * grooveYInterval + fiberCount * grooveWidth - mergedGrooveY) + mergedGrooveY;
        for (int grooveIndex = 0; grooveIndex < fiberCount / 2; ++grooveIndex)
        {
            G4double startingX = straightGrooveLength[grooveIndex] / 2;
            G4double endingX = PScintX / 2;
            G4double deltaX = endingX - startingX;

            G4double deltaY;
            if (grooveIndex == 0)
            {
                G4double startingY = (fiberCount / 2 - 0.5 - grooveIndex) * grooveYInterval + (fiberCount / 2 - grooveIndex) * grooveWidth;
                G4double endingY = mergedGrooveY / 2;
                deltaY = startingY - endingY;
                obliqueGrooveAngle.push_back(std::atan(deltaY / deltaX));
            }
            else
            {

                G4double lowerRightY = (unmergedTotalY - 2 * grooveWidth / std::cos(obliqueGrooveAngle[0])) / (fiberCount - 2) * (fiberCount / 2 - grooveIndex - 1);
                G4double upperLeftY = (fiberCount / 2 - 0.5 - grooveIndex) * grooveYInterval + (fiberCount / 2 - grooveIndex) * grooveWidth;
                G4double deltaXPrime = PScintX / 2 - mergedGrooveX - straightGrooveLength[grooveIndex] / 2;
                G4double deltaYPrime = upperLeftY - lowerRightY;

                obliqueGrooveAngle.push_back(
                    std::atan(deltaYPrime / deltaXPrime) +
                    std::asin(-grooveWidth / std::sqrt(deltaXPrime * deltaXPrime + deltaYPrime * deltaYPrime)));
            }
            obliqueGrooveLength.push_back(deltaX / std::cos(obliqueGrooveAngle[grooveIndex]));
        }
    }
    else if (psType == 2)
    {
        uturnPoolX = 7 * cm;
        uturnRadius = 6 * cm;
        grooveYInterval = uturnRadius * 2 / fiberCount - grooveWidth;
        if ((grooveYInterval + grooveWidth) * (fiberCount * 2 - 1) > PScintY - grooveWidth)
        {
            throw std::runtime_error("PScint: Too many fibers or PscintY too small!");
        }
        SiPMPScintDistance = 12 * cm;
    }
    else if (psType == 0)
    {
        grooveYInterval = (PScintY - grooveWidth * fiberCount) / (fiberCount + 1);
    }

    // Calculate parameters for fibers
    if (psType == 1)
    {
        for (int fiberIndex = 0; fiberIndex < fiberCount / 2; ++fiberIndex)
        {
            // shortcuts
            G4double halfTheta = obliqueGrooveAngle[fiberIndex] / 2.;
            G4double theta = obliqueGrooveAngle[fiberIndex];
            G4double df = fiberDiameter;
            G4double dg = grooveWidth;
            G4double r; // equivalent to ConnectingFiberPartRotRadius[fiberIndex]

            r = (dg - df / 2. * (1 + cos(halfTheta))) / (1 - cos(halfTheta));
            ConnectingFiberPartRotRadius.push_back(r);
            StraightFiberPartLength.push_back(straightGrooveLength[fiberIndex] - 2 * (r + df / 2.) * tan(halfTheta));
            StraightFiberPartTranslationY.push_back(grooveYInterval * (fiberCount / 2 - 0.5 - fiberIndex) + grooveWidth * (fiberCount / 2 - fiberIndex) - fiberDiameter / 2);
            ObliqueFiberPartLength.push_back(obliqueGrooveLength[fiberIndex] - (r + df / 2) * tan(halfTheta) - mergedGrooveX / cos(theta));
            ObliqueFiberPartTranslationX.push_back(StraightFiberPartLength[fiberIndex] / 2 + (r + df / 2) * sin(theta) -
                                                   (-ObliqueFiberPartLength[fiberIndex] / 2 * cos(theta) + df / 2 * sin(theta)));
            ObliqueFiberPartTranslationY.push_back(StraightFiberPartTranslationY[fiberIndex] - r + (r + df / 2) * cos(theta) -
                                                   (ObliqueFiberPartLength[fiberIndex] / 2 * sin(theta) + df / 2 * cos(theta)));

            G4double SiPMFiberXCenterLeft = PScintX / 2. - mergedGrooveX - df / 2. * sin(theta);
            G4double SiPMFiberXCenterRight = PScintX / 2.;
            G4double dx = SiPMFiberXCenterRight - SiPMFiberXCenterLeft;
            G4double SiPMFiberYCenterLeft;
            G4double SiPMFiberYCenterRight = mergedGrooveY / (fiberCount / 2. + 1) * (fiberCount / 4. - fiberIndex - 0.5);
            G4double dy;
            if (fiberIndex == 0)
            {
                SiPMFiberYCenterLeft = unmergedTotalY / 2. - df / 2. * cos(theta);
            }
            else
            {
                G4double grooveLowerRightY = (unmergedTotalY - 2. * grooveWidth / std::cos(obliqueGrooveAngle[0])) / (fiberCount - 2) * (fiberCount / 2. - fiberIndex - 1.);
                SiPMFiberYCenterLeft = grooveLowerRightY + dg / cos(theta) - df / 2. * cos(theta);
            }
            dy = SiPMFiberYCenterLeft - SiPMFiberYCenterRight;
            G4double r0, r1, phi;
            for (phi = 0.001 * degree; phi < 90 * degree; phi += 0.0001 * degree)
            {
                G4double det = (sin(theta + phi) - sin(theta)) * (1 - cos(theta + phi)) - sin(theta + phi) * (cos(theta) - cos(theta + phi));
                r0 = ((1 - cos(theta + phi)) * dx - sin(theta + phi) * dy) / det;
                r1 = ((sin(theta + phi) - sin(theta)) * dy - (cos(theta) - cos(theta + phi)) * dx) / det;
                assert(abs((sin(theta + phi) - sin(theta)) * r0 + sin(theta + phi) * r1 - dx) < 1e-8);
                assert(abs((cos(theta) - cos(theta + phi)) * r0 + (1 - cos(theta + phi)) * r1 - dy) < 1e-8);
                if (r0 > 3 * cm && r1 > 3 * cm)
                {
                    //G4cout << phi / degree << " degree is ok" << G4endl;
                    break;
                }
            }
            if (phi > 90 * degree)
            {
                throw std::runtime_error("MVDetectorConstruction::CalculateParameters: no phi is possible for fiber " + std::to_string(fiberIndex));
            }
            SiPMFiberPartAngle.push_back(phi);
            SiPMFiberPartRotRadius_0.push_back(r0);
            SiPMFiberPartRotRadius_1.push_back(r1);
            SiPMFiberPartTranslationX_0.push_back(SiPMFiberXCenterLeft - r0 * sin(theta));
            SiPMFiberPartTranslationY_0.push_back(SiPMFiberYCenterLeft - r0 * cos(theta));
            SiPMFiberPartTranslationX_1.push_back(SiPMFiberXCenterRight);
            SiPMFiberPartTranslationY_1.push_back(SiPMFiberYCenterRight + r1);
        }
        // length check
        assert(StraightFiberPartLength.size() == fiberCount / 2);
        assert(StraightFiberPartTranslationY.size() == fiberCount / 2);
        assert(ObliqueFiberPartTranslationX.size() == fiberCount / 2);
        assert(ObliqueFiberPartTranslationY.size() == fiberCount / 2);
        assert(ObliqueFiberPartLength.size() == fiberCount / 2);
        assert(ConnectingFiberPartRotRadius.size() == fiberCount / 2);
        assert(SiPMFiberPartRotRadius_0.size() == fiberCount / 2);
        assert(SiPMFiberPartRotRadius_1.size() == fiberCount / 2);
        assert(SiPMFiberPartAngle.size() == fiberCount / 2);
        assert(SiPMFiberPartTranslationX_0.size() == fiberCount / 2);
        assert(SiPMFiberPartTranslationY_0.size() == fiberCount / 2);
        assert(SiPMFiberPartTranslationX_1.size() == fiberCount / 2);
        assert(SiPMFiberPartTranslationY_1.size() == fiberCount / 2);
    }
    else if (psType == 2)
    {
        StraightFiberPartLength.push_back(PScintX - uturnPoolX);
        for (int fiberIndex = 0; fiberIndex < fiberCount; ++fiberIndex)
        {
            StraightFiberPartTranslationY.push_back((grooveYInterval + grooveWidth) * (fiberCount - 0.5 - fiberIndex));
            StraightFiberPartTranslationY.push_back((grooveYInterval + grooveWidth) * (-fiberIndex - 0.5));
            FiberDepth.push_back(PScintZ / 2 - fiberDiameter * (fiberIndex + 0.5));
            SiPMFiberPartAngle.push_back(atan((StraightFiberPartTranslationY[2*fiberIndex]-SiPMY/6.)/SiPMPScintDistance)*2);
            SiPMFiberPartRotRadius_0.push_back(SiPMPScintDistance/2/sin(SiPMFiberPartAngle[fiberIndex]));
            if(SiPMFiberPartRotRadius_0[fiberIndex] < 3*cm)
            {
                throw std::runtime_error("MVDetectorConstruction::CalculateParameters: Radius too small for bending fiber!");
            }
            //G4cout << SiPMFiberPartRotRadius_0[fiberIndex] << G4endl;
        }
        assert(StraightFiberPartLength.size() == 1);
        assert(StraightFiberPartTranslationY.size() == fiberCount * 2);
        assert(FiberDepth.size() == fiberCount);
        assert(SiPMFiberPartAngle.size() == fiberCount);
        assert(SiPMFiberPartRotRadius_0.size() == fiberCount);
    }
}

G4VSolid *MVDetectorConstruction::GetFiberPart(G4double diameter) const
{
    switch (psType)
    {
    case 0:
        return GetFiberPartStraight(diameter);
    case 1:
        return GetFiberPartBending(diameter);
    case 2:
        return GetFiberPartUTurn(diameter);
    default:
        throw std::runtime_error("MVDetectorConstruction::GetFiberPart: PS Type not implemented!");
    }
}

G4VSolid *MVDetectorConstruction::GetFiberPartStraight(G4double diameter) const
{
    throw std::runtime_error("MVDetectorConstruction::GetFiberPartStraight: Not implemented!");
}

G4VSolid *MVDetectorConstruction::GetFiberPartBending(G4double diameter) const
{
    // Counter so that names are unique
    static int id = 0;

    G4MultiUnion *FiberParts = new G4MultiUnion("FiberParts" + std::to_string(id));
    for (int fiberIndex = 0; fiberIndex < fiberCount / 2; ++fiberIndex)
    {
        // Straight part, in the middle
        G4Tubs *StraightFiberPart = new G4Tubs("StraightFiberPart" + std::to_string(fiberIndex) + std::to_string(id),
                                               0, diameter / 2, StraightFiberPartLength[fiberIndex] / 2 + 0.1 * mm, 0, 360 * degree);
        FiberParts->AddNode(StraightFiberPart, G4Transform3D((new G4RotationMatrix())->rotateY(90 * degree),
                                                             G4ThreeVector(0, StraightFiberPartTranslationY[fiberIndex],
                                                                           FiberDepthUpper)));
        FiberParts->AddNode(StraightFiberPart, G4Transform3D((new G4RotationMatrix())->rotateY(90 * degree),
                                                             G4ThreeVector(0, -StraightFiberPartTranslationY[fiberIndex],
                                                                           FiberDepthLower)));
        // Oblique part, on both sides
        G4Tubs *ObliqueFiberPart = new G4Tubs("ObliqueFiberPart" + std::to_string(fiberIndex) + std::to_string(id),
                                              0, diameter / 2, ObliqueFiberPartLength[fiberIndex] / 2 + 0.1 * mm, 0, 360 * degree);
        FiberParts->AddNode(ObliqueFiberPart, G4Transform3D((new G4RotationMatrix())->rotateY(90 * degree).rotateZ(-obliqueGrooveAngle[fiberIndex]),
                                                            G4ThreeVector(ObliqueFiberPartTranslationX[fiberIndex],
                                                                          ObliqueFiberPartTranslationY[fiberIndex],
                                                                          FiberDepthUpper)));
        FiberParts->AddNode(ObliqueFiberPart, G4Transform3D((new G4RotationMatrix())->rotateY(90 * degree).rotateZ(obliqueGrooveAngle[fiberIndex]),
                                                            G4ThreeVector(-ObliqueFiberPartTranslationX[fiberIndex],
                                                                          ObliqueFiberPartTranslationY[fiberIndex],
                                                                          FiberDepthUpper)));
        FiberParts->AddNode(ObliqueFiberPart, G4Transform3D((new G4RotationMatrix())->rotateY(90 * degree).rotateZ(obliqueGrooveAngle[fiberIndex]),
                                                            G4ThreeVector(ObliqueFiberPartTranslationX[fiberIndex],
                                                                          -ObliqueFiberPartTranslationY[fiberIndex],
                                                                          FiberDepthLower)));
        FiberParts->AddNode(ObliqueFiberPart, G4Transform3D((new G4RotationMatrix())->rotateY(90 * degree).rotateZ(-obliqueGrooveAngle[fiberIndex]),
                                                            G4ThreeVector(-ObliqueFiberPartTranslationX[fiberIndex],
                                                                          -ObliqueFiberPartTranslationY[fiberIndex],
                                                                          FiberDepthLower)));

        // Connecting parts, on both sides
        G4Torus *ConnectingFiberPart = new G4Torus("ConnectingFiberPart" + std::to_string(fiberIndex) + std::to_string(id),
                                                   0, diameter / 2, ConnectingFiberPartRotRadius[fiberIndex], 0, obliqueGrooveAngle[fiberIndex] + 0.1 * degree);
        FiberParts->AddNode(ConnectingFiberPart, G4Transform3D((new G4RotationMatrix())->rotateZ(90 * degree - obliqueGrooveAngle[fiberIndex]),
                                                               G4ThreeVector(StraightFiberPartLength[fiberIndex] / 2,
                                                                             StraightFiberPartTranslationY[fiberIndex] - ConnectingFiberPartRotRadius[fiberIndex],
                                                                             FiberDepthUpper)));
        FiberParts->AddNode(ConnectingFiberPart, G4Transform3D((new G4RotationMatrix())->rotateZ(90 * degree),
                                                               G4ThreeVector(-StraightFiberPartLength[fiberIndex] / 2,
                                                                             StraightFiberPartTranslationY[fiberIndex] - ConnectingFiberPartRotRadius[fiberIndex],
                                                                             FiberDepthUpper)));
        FiberParts->AddNode(ConnectingFiberPart, G4Transform3D((new G4RotationMatrix())->rotateZ(-90 * degree),
                                                               G4ThreeVector(StraightFiberPartLength[fiberIndex] / 2,
                                                                             -StraightFiberPartTranslationY[fiberIndex] + ConnectingFiberPartRotRadius[fiberIndex],
                                                                             FiberDepthLower)));
        FiberParts->AddNode(ConnectingFiberPart, G4Transform3D((new G4RotationMatrix())->rotateZ(-90 * degree - obliqueGrooveAngle[fiberIndex]),
                                                               G4ThreeVector(-StraightFiberPartLength[fiberIndex] / 2,
                                                                             -StraightFiberPartTranslationY[fiberIndex] + ConnectingFiberPartRotRadius[fiberIndex],
                                                                             FiberDepthLower)));

        // SiPM fiber part, on both sides
        G4Torus *SiPMFiberPart_0 = new G4Torus("SiPMFiberPart_0" + std::to_string(fiberIndex) + std::to_string(id),
                                               0, diameter / 2, SiPMFiberPartRotRadius_0[fiberIndex], 0, SiPMFiberPartAngle[fiberIndex] + 0.1 * degree);
        G4Torus *SiPMFiberPart_1 = new G4Torus("SiPMFiberPart_1" + std::to_string(fiberIndex) + std::to_string(id),
                                               0, diameter / 2, SiPMFiberPartRotRadius_1[fiberIndex], 0, SiPMFiberPartAngle[fiberIndex] + obliqueGrooveAngle[fiberIndex]);
        FiberParts->AddNode(SiPMFiberPart_0, G4Transform3D((new G4RotationMatrix())->rotateZ(90. * degree - obliqueGrooveAngle[fiberIndex] - SiPMFiberPartAngle[fiberIndex]),
                                                           G4ThreeVector(SiPMFiberPartTranslationX_0[fiberIndex],
                                                                         SiPMFiberPartTranslationY_0[fiberIndex],
                                                                         FiberDepthUpper)));
        FiberParts->AddNode(SiPMFiberPart_1, G4Transform3D((new G4RotationMatrix())->rotateZ(-90 * degree - SiPMFiberPartAngle[fiberIndex] - obliqueGrooveAngle[fiberIndex]),
                                                           G4ThreeVector(SiPMFiberPartTranslationX_1[fiberIndex],
                                                                         SiPMFiberPartTranslationY_1[fiberIndex],
                                                                         FiberDepthUpper)));
        FiberParts->AddNode(SiPMFiberPart_0, G4Transform3D((new G4RotationMatrix())->rotateZ(90. * degree + obliqueGrooveAngle[fiberIndex]),
                                                           G4ThreeVector(-SiPMFiberPartTranslationX_0[fiberIndex],
                                                                         SiPMFiberPartTranslationY_0[fiberIndex],
                                                                         FiberDepthUpper)));
        FiberParts->AddNode(SiPMFiberPart_1, G4Transform3D((new G4RotationMatrix())->rotateZ(-90. * degree),
                                                           G4ThreeVector(-SiPMFiberPartTranslationX_1[fiberIndex],
                                                                         SiPMFiberPartTranslationY_1[fiberIndex],
                                                                         FiberDepthUpper)));
        FiberParts->AddNode(SiPMFiberPart_0, G4Transform3D((new G4RotationMatrix())->rotateZ(-90. * degree + obliqueGrooveAngle[fiberIndex]),
                                                           G4ThreeVector(SiPMFiberPartTranslationX_0[fiberIndex],
                                                                         -SiPMFiberPartTranslationY_0[fiberIndex],
                                                                         FiberDepthLower)));
        FiberParts->AddNode(SiPMFiberPart_1, G4Transform3D((new G4RotationMatrix())->rotateZ(90. * degree),
                                                           G4ThreeVector(SiPMFiberPartTranslationX_1[fiberIndex],
                                                                         -SiPMFiberPartTranslationY_1[fiberIndex],
                                                                         FiberDepthLower)));
        FiberParts->AddNode(SiPMFiberPart_0, G4Transform3D((new G4RotationMatrix())->rotateZ(-90. * degree - obliqueGrooveAngle[fiberIndex] - SiPMFiberPartAngle[fiberIndex]),
                                                           G4ThreeVector(-SiPMFiberPartTranslationX_0[fiberIndex],
                                                                         -SiPMFiberPartTranslationY_0[fiberIndex],
                                                                         FiberDepthLower)));
        FiberParts->AddNode(SiPMFiberPart_1, G4Transform3D((new G4RotationMatrix())->rotateZ(90. * degree - SiPMFiberPartAngle[fiberIndex] - obliqueGrooveAngle[fiberIndex]),
                                                           G4ThreeVector(-SiPMFiberPartTranslationX_1[fiberIndex],
                                                                         -SiPMFiberPartTranslationY_1[fiberIndex],
                                                                         FiberDepthLower)));
    }

    FiberParts->Voxelize();

    // Finally intersect with the whole PScint solid to avoid overlapping
    /*
    G4VSolid *PScintBox = G4SolidStore::GetInstance()->GetSolid("PScintBox");
    G4IntersectionSolid *FinalSolid = new G4IntersectionSolid("FinalParts" + std::to_string(id), PScintBox, FiberParts);
    */
    // return FinalSolid;
    ++id;
    return FiberParts;
}

G4VSolid *MVDetectorConstruction::GetFiberPartUTurn(G4double diameter) const
{
    static int id = 0;

    G4MultiUnion *FiberParts = new G4MultiUnion("FiberParts" + std::to_string(id));

    for (int fiberIndex = 0; fiberIndex < fiberCount; ++fiberIndex)
    {
        G4Tubs *StraightFiberPart = new G4Tubs("StraightFiberPart" + std::to_string(fiberIndex) + std::to_string(id),
                                               0, diameter / 2, StraightFiberPartLength[0] / 2 + 0.1 * mm, 0, 360 * degree);
        FiberParts->AddNode(StraightFiberPart, G4Transform3D((new G4RotationMatrix())->rotateY(90 * degree),
                                                             G4ThreeVector(-uturnPoolX / 2 + PScintXTranslation, StraightFiberPartTranslationY[2 * fiberIndex],
                                                                           FiberDepth[fiberIndex])));
        FiberParts->AddNode(StraightFiberPart, G4Transform3D((new G4RotationMatrix())->rotateY(90 * degree),
                                                             G4ThreeVector(-uturnPoolX / 2 + PScintXTranslation, StraightFiberPartTranslationY[2 * fiberIndex + 1],
                                                                           FiberDepth[fiberIndex])));
        
        G4Torus *UTurnFiberPart = new G4Torus("UTurnFiberPart" + std::to_string(fiberIndex) + std::to_string(id),
                                              0, diameter / 2, uturnRadius, 0 * degree, 180 * degree);
        FiberParts->AddNode(UTurnFiberPart, G4Transform3D((new G4RotationMatrix)->rotateZ(-90*degree),
                                                          G4ThreeVector(PScintX / 2 - uturnPoolX + PScintXTranslation,
                                                                        0.5 * (StraightFiberPartTranslationY[2 * fiberIndex] + StraightFiberPartTranslationY[2 * fiberIndex + 1]),
                                                                        FiberDepth[fiberIndex])));
        G4Torus *SiPMFiberPart_0 = new G4Torus("SiPMFiberPart_0" + std::to_string(fiberIndex) + std::to_string(id),
                                               0, diameter / 2, SiPMFiberPartRotRadius_0[fiberIndex], 0, SiPMFiberPartAngle[fiberIndex]);
        FiberParts->AddNode(SiPMFiberPart_0, G4Transform3D((new G4RotationMatrix())->rotateZ(90*degree),
                                                           G4ThreeVector(-PScintX/2 + PScintXTranslation,
                                                                         StraightFiberPartTranslationY[2*fiberIndex] - SiPMFiberPartRotRadius_0[fiberIndex],
                                                                         FiberDepth[fiberIndex])));
        FiberParts->AddNode(SiPMFiberPart_0, G4Transform3D((new G4RotationMatrix())->rotateZ(-90*degree),
                                                           G4ThreeVector(-PScintX/2 + PScintXTranslation - SiPMPScintDistance,
                                                                         SiPMY / 6. + SiPMFiberPartRotRadius_0[fiberIndex],
                                                                         FiberDepth[fiberIndex])));
        G4Torus *SiPMFiberPart_1 = new G4Torus("SiPMFiberPart_1" + std::to_string(fiberIndex) + std::to_string(id),
                                               0, diameter / 2, SiPMFiberPartRotRadius_0[fiberCount-fiberIndex-1], 0, SiPMFiberPartAngle[fiberCount-fiberIndex-1]);
        FiberParts->AddNode(SiPMFiberPart_1, G4Transform3D((new G4RotationMatrix())->rotateZ(-90*degree-SiPMFiberPartAngle[fiberCount-fiberIndex-1]),
                                                           G4ThreeVector(-PScintX/2 + PScintXTranslation,
                                                                         StraightFiberPartTranslationY[2*fiberIndex+1] + SiPMFiberPartRotRadius_0[fiberCount-fiberIndex-1],
                                                                         FiberDepth[fiberIndex])));
        FiberParts->AddNode(SiPMFiberPart_1, G4Transform3D((new G4RotationMatrix())->rotateZ(90*degree-SiPMFiberPartAngle[fiberCount-fiberIndex-1]),
                                                           G4ThreeVector(-PScintX/2 + PScintXTranslation - SiPMPScintDistance,
                                                                         -SiPMY / 6. - SiPMFiberPartRotRadius_0[fiberCount-fiberIndex-1],
                                                                         FiberDepth[fiberIndex])));
    }

    FiberParts->Voxelize();

    return FiberParts;
}

G4VPhysicalVolume *MVDetectorConstruction::Construct()
{
    DefineMaterials();
    DefineMaterialTables();
    DefineOpticalSurfaces();
    CalculateParameters();
    return ConstructDetector();
}

void MVDetectorConstruction::ConstructSDandField()
{
    // Sensitive detectors
    G4String SiPM_name = "SiPM";

    // Filter for optical photon
    G4SDParticleFilter *photon_filter =
        new G4SDParticleFilter("photon_filter", "opticalphoton");

    // Use SiPMSD as the sensitive detector
    SiPMSD *SiPMSD_0 = new SiPMSD(SiPM_name + "_0", "collection_0", YZ_plus);
    SiPMSD_0->SetFilter(photon_filter);
    G4SDManager::GetSDMpointer()->AddNewDetector(SiPMSD_0);
    SetSensitiveDetector("SiPMLogicalVolume_0", SiPMSD_0);

    if (psType != 2)
    {
        SiPMSD *SiPMSD_1 = new SiPMSD(SiPM_name + "_1", "collection_1", YZ_minus);
        SiPMSD_1->SetFilter(photon_filter);
        G4SDManager::GetSDMpointer()->AddNewDetector(SiPMSD_1);
        SetSensitiveDetector("SiPMLogicalVolume_1", SiPMSD_1);
    }

    /*
    // Definition for SiPM_0 (the upper one)
    // Using SiPMPhotonAccumulator as the primitive scorer
    G4MultiFunctionalDetector* SiPM_0 = new G4MultiFunctionalDetector(SiPM_name+"_0");
    G4VPrimitiveScorer* photon_counter_in = new SiPMPhotonAccumulator("photon_counter_0", XY_minus);
    photon_counter_in->SetFilter(photon_filter);
    SiPM_0->RegisterPrimitive(photon_counter_in);

    G4SDManager::GetSDMpointer()->AddNewDetector(SiPM_0);
    SetSensitiveDetector("SiPM_0_log", SiPM_0);

    // Definition for SiPM_1 (the lower one)
    G4MultiFunctionalDetector* SiPM_1 = new G4MultiFunctionalDetector(SiPM_name+"_1");
    G4VPrimitiveScorer* photon_counter_out = new SiPMPhotonAccumulator("photon_counter_1", XY_plus);
    photon_counter_out->SetFilter(photon_filter);
    SiPM_1->RegisterPrimitive(photon_counter_out);

    G4SDManager::GetSDMpointer()->AddNewDetector(SiPM_1);
    SetSensitiveDetector("SiPM_1_log", SiPM_1);
    */
}

void MuonVeto::MVDetectorConstruction::UpdateGeometry()
{
    G4SolidStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4PhysicalVolumeStore::GetInstance()->Clean();

    auto runManager = G4RunManager::GetRunManager();
    runManager->DefineWorldVolume(Construct());
    runManager->GeometryHasBeenModified();
}

void MVDetectorConstruction::SetScintYield(G4double newScintYield)
{
    scintYield = newScintYield;
    auto PSPolystreneTable = PSPolystyrene->GetMaterialPropertiesTable();
    PSPolystreneTable->RemoveConstProperty("SCINTILLATIONYIELD");
    PSPolystreneTable->AddConstProperty("SCINTILLATIONYIELD", scintYield);
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void MVDetectorConstruction::SetFiberCount(G4int newFiberCount)
{
    fiberCount = newFiberCount;
    UpdateGeometry();
}