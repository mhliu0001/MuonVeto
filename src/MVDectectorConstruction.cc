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
#include "G4UnionSolid.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4SDParticleFilter.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSFlatSurfaceCurrent.hh"

MuonVeto::MVDetectorConstruction::MVDetectorConstruction()
{
    SetDefaults();
}

MuonVeto::MVDetectorConstruction::~MVDetectorConstruction()
{

}

void MuonVeto::MVDetectorConstruction::SetDefaults()
{
    // Experimental Hall
    expHall_x = 200*cm;
    expHall_y = 200*cm;
    expHall_z = 500*cm;

    // Pscint
    pscint_x = 200*mm;
    pscint_y = 40*mm;
    pscint_z = 2000*mm;

    // Teflon reflection layer
    teflon_depth = 1*mm;

    // Fiber
    rTrunk = 500*mm;
    aPMT = 740*mm;
    bPMT = 28.3*mm;
    hPMT = 750*mm;

    // Groove
    groove_depth = 10*mm;
    groove_width = 4*mm;

    // SiPM
    SiPM_width = 40*mm;
    SiPM_length = 200*mm;
    SiPM_depth = 1*mm;
    air_interval = 10*mm;

}

void MuonVeto::MVDetectorConstruction::DefineMaterials()
{
    G4NistManager* man = G4NistManager::Instance();

    // Definition of elements
    G4Element* C = man->FindOrBuildElement("C");
    G4Element* H = man->FindOrBuildElement("H");

    // Definition of teflon, used in outer shell(reflection layer)
    teflon = man->FindOrBuildMaterial("G4_TEFLON");

    // Definition of air, used in groove and world
    air = man->FindOrBuildMaterial("G4_AIR");
    G4MaterialPropertiesTable* air_table = new G4MaterialPropertiesTable();
    air_table->AddProperty("RINDEX", "Air");
    air->SetMaterialPropertiesTable(air_table);

    // Definition of LAB, used in pscint
    LAB = new G4Material("LAB", 1.05*g/cm3, 2);
    LAB->AddElement(C,0.47);
    LAB->AddElement(H,0.53);
    G4MaterialPropertiesTable* LAB_table = new G4MaterialPropertiesTable();
    std::vector<G4double> LAB_energy = {2.8*eV, 2.9*eV, 3.0*eV};
    std::vector<G4double> LAB_rindex = {1.58, 1.58, 1.58};
    std::vector<G4double> LAB_abslength = {2.*m, 2.*m, 2.*m};
    std::vector<G4double> LAB_scint = {0.0, 1.0, 0.0};
    LAB_table->AddConstProperty("SCINTILLATIONYIELD", 8000./MeV);
    LAB_table->AddProperty("RINDEX", LAB_energy, LAB_rindex);
    LAB_table->AddProperty("ABSLENGTH", LAB_energy, LAB_abslength);
    LAB_table->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.*ns);
    LAB_table->AddProperty("SCINTILLATIONCOMPONENT1", LAB_energy, LAB_scint);
    LAB_table->AddConstProperty("SCINTILLATIONYIELD1", 1.0);
    LAB_table->AddConstProperty("RESOLUTIONSCALE", 1.0);
    LAB->SetMaterialPropertiesTable(LAB_table);

    // Definition of glass, used in SiPM
    glass = man->FindOrBuildMaterial("G4_GLASS_PLATE");
    G4MaterialPropertiesTable* glass_table = new G4MaterialPropertiesTable();
    std::vector<G4double> glass_energy = {2.8*eV, 2.9*eV, 3.0*eV};
    std::vector<G4double> glass_rindex = {1.5, 1.5, 1.5};
    glass_table->AddProperty("RINDEX", glass_energy, glass_rindex);
    glass->SetMaterialPropertiesTable(glass_table);
}

void MuonVeto::MVDetectorConstruction::DefineOpticalSurfaces()
{
    // Surface between LAB and teflon
    op_LAB_teflon_surface = new G4OpticalSurface(
        "op_LAB_teflon_surface", unified, groundfrontpainted, dielectric_dielectric
    );
    G4MaterialPropertiesTable* LAB_teflon_table = new G4MaterialPropertiesTable();
    std::vector<G4double> LAB_teflon_energy = {2.8*eV, 2.9*eV, 3.0*eV};
    std::vector<G4double> LAB_teflon_reflectivity = {0.85, 0.85, 0.85};
    LAB_teflon_table->AddProperty("REFLECTIVITY", LAB_teflon_energy, LAB_teflon_reflectivity);
    op_LAB_teflon_surface->SetMaterialPropertiesTable(LAB_teflon_table);

    // Surface between air and teflon
    op_teflon_air_surface = new G4OpticalSurface(
        "op_teflon_air_surface", unified, ground, dielectric_dielectric
    );
    G4MaterialPropertiesTable* teflon_air_table = new G4MaterialPropertiesTable();
    std::vector<G4double> teflon_air_energy = {2.8*eV, 2.9*eV, 3.0*eV};
    std::vector<G4double> teflon_air_reflectivity = {0.85, 0.85, 0.85};
    teflon_air_table->AddProperty("REFLECTIVITY", teflon_air_energy, teflon_air_reflectivity);
    op_teflon_air_surface->SetMaterialPropertiesTable(LAB_teflon_table);
}

G4VPhysicalVolume* MuonVeto::MVDetectorConstruction::ConstructDetector() const
{
    G4bool checkOverlaps = true;

    // Experimental Hall
    G4Box* experimentalHall_box = new G4Box("World", expHall_x, expHall_y, expHall_z);
    G4LogicalVolume* experimentalHall_log = new G4LogicalVolume(experimentalHall_box, air, "World");
    G4VPhysicalVolume* experimentalHall_phys = new G4PVPlacement(0, G4ThreeVector(0*mm,0*mm,0*mm), experimentalHall_log, "World", NULL, false, 0);

    // Outer teflon relection layer (shell) and the plastic scintillator (Pscint) in the middle (made of LAB)
    G4Box* whole_solid = new G4Box("whole_solid",0.5*(pscint_x+teflon_depth),0.5*(pscint_y+teflon_depth),0.5*pscint_z);
    G4Box* pscint_solid = new G4Box("pscint_solid",0.5*pscint_x,0.5*pscint_y,0.5*pscint_z);
    G4SubtractionSolid* shell_solid = new G4SubtractionSolid("shell_solid", whole_solid, pscint_solid);
    G4LogicalVolume* shell_log = new G4LogicalVolume(shell_solid,teflon,"shell_log",0,0,0);
    G4VPhysicalVolume* shell_phys =new G4PVPlacement(0,G4ThreeVector(0*mm,0*mm,0*mm),shell_log,"shell_log1",experimentalHall_log,false,0,checkOverlaps);
    G4LogicalVolume* pscint_log = new G4LogicalVolume(pscint_solid, LAB, "pscint_log");

    // Groove for fiber
    G4Box* groove_solid = new G4Box("groove_solid", 0.5*groove_width, 0.5*groove_depth, 0.5*pscint_z);
    G4LogicalVolume* groove_log = new G4LogicalVolume(groove_solid, air, "groove_log");
    G4VPhysicalVolume* groove_phys = new G4PVPlacement(0,G4ThreeVector(0*mm, 0.5*(pscint_y-groove_depth),0*mm), groove_log, "groove_phys", pscint_log, false, 0, checkOverlaps);
	G4VPhysicalVolume* pscint_phys =new G4PVPlacement(0,G4ThreeVector(0*mm,0*mm,0*mm), pscint_log,"pscint_phys",experimentalHall_log,false,0,checkOverlaps);

    // SiPM on both sides
    G4Box* SiPM_solid = new G4Box("SiPM_solid", 0.5*SiPM_length, 0.5*SiPM_width, 0.5*SiPM_depth);
    G4LogicalVolume* SiPM_0_log = new G4LogicalVolume(SiPM_solid, glass, "SiPM_0_log");
    G4LogicalVolume* SiPM_1_log = new G4LogicalVolume(SiPM_solid, glass, "SiPM_1_log");
    new G4PVPlacement(0, G4ThreeVector(0*mm, 0*mm, 0.5*pscint_z+air_interval), SiPM_0_log, "SiPM_0_phys", experimentalHall_log, false, 0, checkOverlaps);
    new G4PVPlacement(0, G4ThreeVector(0*mm, 0*mm, -0.5*pscint_z-air_interval), SiPM_1_log, "SiPM_1_phys", experimentalHall_log, false, 1, checkOverlaps);

    // Optical surfaces
    new G4LogicalBorderSurface(
        "LAB_teflon_surface", pscint_phys, shell_phys, op_LAB_teflon_surface
    );
    new G4LogicalBorderSurface(
        "teflon_air_surface", groove_phys, shell_phys, op_teflon_air_surface
    );

    return experimentalHall_phys;
}

G4VPhysicalVolume* MuonVeto::MVDetectorConstruction::Construct()
{
    DefineMaterials();
    DefineOpticalSurfaces();
    return ConstructDetector();
}

void MuonVeto::MVDetectorConstruction::ConstructSDandField()
{
    // Sensitive detectors

    G4String SiPM_name = "SiPM";
    
    // Filter for optical photon
    G4SDParticleFilter* photon_filter =
        new G4SDParticleFilter("photon_filter","opticalphoton");

    // Definition for SiPM_0 (the upper one)
    // Using G4PSFlatSurfaceCurrent as the primitive scorer
    // This may cause problems, as it only counts the current in the -Z surface!
    G4MultiFunctionalDetector* SiPM_0 = new G4MultiFunctionalDetector(SiPM_name+"_0");
    G4VPrimitiveScorer* photon_counter_in = new G4PSFlatSurfaceCurrent("photon_counter_0", 1);
    photon_counter_in->SetFilter(photon_filter);
    SiPM_0->RegisterPrimitive(photon_counter_in);

    G4SDManager::GetSDMpointer()->AddNewDetector(SiPM_0);
    SetSensitiveDetector("SiPM_0_log", SiPM_0);

    // Definition for SiPM_1 (the lower one)
    G4MultiFunctionalDetector* SiPM_1 = new G4MultiFunctionalDetector(SiPM_name+"_1");
    G4VPrimitiveScorer* photon_counter_out = new G4PSFlatSurfaceCurrent("photon_counter_1", 2);
    photon_counter_out->SetFilter(photon_filter);
    SiPM_1->RegisterPrimitive(photon_counter_out);

    G4SDManager::GetSDMpointer()->AddNewDetector(SiPM_1);
    SetSensitiveDetector("SiPM_1_log", SiPM_1);
}
