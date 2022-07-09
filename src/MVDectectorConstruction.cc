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
#include "G4MultiUnion.hh"
#include "G4IntersectionSolid.hh"
//#include "G4VPrimitiveScorer.hh"
//#include "SiPMPhotonAccumulator.hh"
#include "SiPMSD.hh"
#include <string>
#include <cmath>

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
    pscint_y = 20*mm;
    pscint_z = 2000*mm;

    // Teflon reflection layer
    teflon_depth = 1*mm;

    // Fiber
    fiber_d = 1.5*mm;
    fiber_count = 4;
    fiber_bend_r = 740*mm;
    fiber_depth = 2*mm;
    cladding_depth_1 = 0.015*mm;
    cladding_depth_2 = 0.045*mm;

    // Groove
    groove_depth = fiber_depth + fiber_d/2 + 0.1*mm;
    groove_width = fiber_d + 0.5*mm;

    // SiPM
    SiPM_width = 10*mm;
    SiPM_length = 10*mm;
    SiPM_depth = 1*mm;
    air_interval = 1*mm;

}

void MuonVeto::MVDetectorConstruction::DefineMaterials()
{
    G4NistManager* man = G4NistManager::Instance();

    // Definition of elements
    G4Element* C = man->FindOrBuildElement("C");
    G4Element* H = man->FindOrBuildElement("H");
    G4Element* O = man->FindOrBuildElement("O");
    
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

    // Definition of glass, used in SiPM
    glass = man->FindOrBuildMaterial("G4_GLASS_PLATE");
    G4MaterialPropertiesTable* glass_table = new G4MaterialPropertiesTable();
    std::vector<G4double> glass_energy = {2.8*eV, 2.9*eV, 3.0*eV};
    std::vector<G4double> glass_rindex = {1.5, 1.5, 1.5};
    glass_table->AddProperty("RINDEX", glass_energy, glass_rindex);
    glass->SetMaterialPropertiesTable(glass_table);

    // Definition of PMMA, used in fiber
    PMMA = new G4Material("PMMA", 1190*kg/m3, 3);
    PMMA->AddElement(H, 8);
    PMMA->AddElement(C, 5);
    PMMA->AddElement(O, 2);

    // Definition of Pethylene_1 (Fluorinated Polyethylene), used in the outer fiber cladding
    Pethylene_1 = new G4Material("Pethylene_1", 1400*kg/m3, 2);
    Pethylene_1->AddElement(H, 4);
    Pethylene_1->AddElement(C, 2);

    // Definition of Pethylene_2 (Polyethylene), used in the inner fiber cladding
    Pethylene_2 = new G4Material("Pethylene_2", 1200*kg/m3, 2);
    Pethylene_2->AddElement(H, 4);
    Pethylene_2->AddElement(C, 2);

}

void MuonVeto::MVDetectorConstruction::DefineMaterialTables()
{
    /*************Table of LAB*************/

    G4MaterialPropertiesTable* MPTscint = new G4MaterialPropertiesTable();

    // Absorption
    G4double scintABSEnergy[502] = {1.3778*eV, 1.3793*eV, 1.3808*eV, 1.3824*eV, 1.3839*eV, 1.3855*eV, 1.3870*eV, 1.3886*eV, 1.3901*eV, 1.3917*eV, 1.3933*eV, 1.3948*eV, 1.3964*eV, 1.3980*eV, 1.3995*eV, 1.4011*eV, 1.4027*eV, 1.4043*eV, 1.4059*eV, 1.4075*eV, 1.4091*eV, 1.4107*eV, 1.4123*eV, 1.4139*eV, 1.4155*eV, 1.4171*eV, 1.4188*eV, 1.4204*eV, 1.4220*eV, 1.4237*eV, 1.4253*eV, 1.4269*eV, 1.4286*eV, 1.4302*eV, 1.4319*eV, 1.4335*eV, 1.4352*eV, 1.4368*eV, 1.4385*eV, 1.4402*eV, 1.4419*eV, 1.4435*eV, 1.4452*eV, 1.4469*eV, 1.4486*eV, 1.4503*eV, 1.4520*eV, 1.4537*eV, 1.4554*eV, 1.4571*eV, 1.4588*eV, 1.4605*eV, 1.4623*eV, 1.4640*eV, 1.4657*eV, 1.4675*eV, 1.4692*eV, 1.4709*eV, 1.4727*eV, 1.4744*eV, 1.4762*eV, 1.4779*eV, 1.4797*eV, 1.4815*eV, 1.4833*eV, 1.4850*eV, 1.4868*eV, 1.4886*eV, 1.4904*eV, 1.4922*eV, 1.4940*eV, 1.4958*eV, 1.4976*eV, 1.4994*eV, 1.5012*eV, 1.5030*eV, 1.5049*eV, 1.5067*eV, 1.5085*eV, 1.5104*eV, 1.5122*eV, 1.5140*eV, 1.5159*eV, 1.5177*eV, 1.5196*eV, 1.5215*eV, 1.5233*eV, 1.5252*eV, 1.5271*eV, 1.5290*eV, 1.5309*eV, 1.5328*eV, 1.5347*eV, 1.5366*eV, 1.5385*eV, 1.5404*eV, 1.5423*eV, 1.5442*eV, 1.5461*eV, 1.5481*eV, 1.5500*eV, 1.5519*eV, 1.5539*eV, 1.5558*eV, 1.5578*eV, 1.5597*eV, 1.5617*eV, 1.5637*eV, 1.5657*eV, 1.5676*eV, 1.5696*eV, 1.5716*eV, 1.5736*eV, 1.5756*eV, 1.5776*eV, 1.5796*eV, 1.5816*eV, 1.5837*eV, 1.5857*eV, 1.5877*eV, 1.5897*eV, 1.5918*eV, 1.5938*eV, 1.5959*eV, 1.5979*eV, 1.6000*eV, 1.6021*eV, 1.6041*eV, 1.6062*eV, 1.6083*eV, 1.6104*eV, 1.6125*eV, 1.6146*eV, 1.6167*eV, 1.6188*eV, 1.6209*eV, 1.6230*eV, 1.6252*eV, 1.6273*eV, 1.6294*eV, 1.6316*eV, 1.6337*eV, 1.6359*eV, 1.6380*eV, 1.6402*eV, 1.6424*eV, 1.6446*eV, 1.6467*eV, 1.6489*eV, 1.6511*eV, 1.6533*eV, 1.6555*eV, 1.6578*eV, 1.6600*eV, 1.6622*eV, 1.6644*eV, 1.6667*eV, 1.6689*eV, 1.6712*eV, 1.6734*eV, 1.6757*eV, 1.6779*eV, 1.6802*eV, 1.6825*eV, 1.6848*eV, 1.6871*eV, 1.6894*eV, 1.6917*eV, 1.6940*eV, 1.6963*eV, 1.6986*eV, 1.7010*eV, 1.7033*eV, 1.7056*eV, 1.7080*eV, 1.7103*eV, 1.7127*eV, 1.7151*eV, 1.7175*eV, 1.7198*eV, 1.7222*eV, 1.7246*eV, 1.7270*eV, 1.7294*eV, 1.7318*eV, 1.7343*eV, 1.7367*eV, 1.7391*eV, 1.7416*eV, 1.7440*eV, 1.7465*eV, 1.7489*eV, 1.7514*eV, 1.7539*eV, 1.7564*eV, 1.7589*eV, 1.7614*eV, 1.7639*eV, 1.7664*eV, 1.7689*eV, 1.7714*eV, 1.7740*eV, 1.7765*eV, 1.7791*eV, 1.7816*eV, 1.7842*eV, 1.7867*eV, 1.7893*eV, 1.7919*eV, 1.7945*eV, 1.7971*eV, 1.7997*eV, 1.8023*eV, 1.8049*eV, 1.8076*eV, 1.8102*eV, 1.8129*eV, 1.8155*eV, 1.8182*eV, 1.8209*eV, 1.8235*eV, 1.8262*eV, 1.8289*eV, 1.8316*eV, 1.8343*eV, 1.8370*eV, 1.8398*eV, 1.8425*eV, 1.8452*eV, 1.8480*eV, 1.8507*eV, 1.8535*eV, 1.8563*eV, 1.8591*eV, 1.8619*eV, 1.8647*eV, 1.8675*eV, 1.8703*eV, 1.8731*eV, 1.8759*eV, 1.8788*eV, 1.8816*eV, 1.8845*eV, 1.8874*eV, 1.8902*eV, 1.8931*eV, 1.8960*eV, 1.8989*eV, 1.9018*eV, 1.9048*eV, 1.9077*eV, 1.9106*eV, 1.9136*eV, 1.9165*eV, 1.9195*eV, 1.9225*eV, 1.9255*eV, 1.9285*eV, 1.9315*eV, 1.9345*eV, 1.9375*eV, 1.9405*eV, 1.9436*eV, 1.9466*eV, 1.9497*eV, 1.9528*eV, 1.9558*eV, 1.9589*eV, 1.9620*eV, 1.9651*eV, 1.9683*eV, 1.9714*eV, 1.9745*eV, 1.9777*eV, 1.9808*eV, 1.9840*eV, 1.9872*eV, 1.9904*eV, 1.9936*eV, 1.9968*eV, 2.0000*eV, 2.0032*eV, 2.0065*eV, 2.0097*eV, 2.0130*eV, 2.0163*eV, 2.0195*eV, 2.0228*eV, 2.0261*eV, 2.0295*eV, 2.0328*eV, 2.0361*eV, 2.0395*eV, 2.0428*eV, 2.0462*eV, 2.0496*eV, 2.0530*eV, 2.0564*eV, 2.0598*eV, 2.0632*eV, 2.0771*eV, 2.0875*eV, 2.0981*eV, 2.1088*eV, 2.1197*eV, 2.1306*eV, 2.1416*eV, 2.1528*eV, 2.1640*eV, 2.1754*eV, 2.1869*eV, 2.1986*eV, 2.2103*eV, 2.2222*eV, 2.2342*eV, 2.2464*eV, 2.2587*eV, 2.2711*eV, 2.2836*eV, 2.2963*eV, 2.3091*eV, 2.3221*eV, 2.3352*eV, 2.3485*eV, 2.3619*eV, 2.3755*eV, 2.3892*eV, 2.4031*eV, 2.4172*eV, 2.4314*eV, 2.4458*eV, 2.4603*eV, 2.4750*eV, 2.4900*eV, 2.5051*eV, 2.5203*eV, 2.5358*eV, 2.5514*eV, 2.5673*eV, 2.5833*eV, 2.5996*eV, 2.6160*eV, 2.6327*eV, 2.6496*eV, 2.6667*eV, 2.6840*eV, 2.7015*eV, 2.7193*eV, 2.7373*eV, 2.7556*eV, 2.7740*eV, 2.7928*eV, 2.8118*eV, 2.8311*eV, 2.8506*eV, 2.8704*eV, 2.8904*eV, 2.8972*eV, 2.9040*eV, 2.9108*eV, 2.9176*eV, 2.9245*eV, 2.9314*eV, 2.9384*eV, 2.9454*eV, 2.9524*eV, 2.9594*eV, 2.9665*eV, 2.9736*eV, 2.9808*eV, 2.9880*eV, 2.9952*eV, 3.0024*eV, 3.0097*eV, 3.0170*eV, 3.0244*eV, 3.0318*eV, 3.0392*eV, 3.0467*eV, 3.0542*eV, 3.0617*eV, 3.0693*eV, 3.0769*eV, 3.0846*eV, 3.0923*eV, 3.1000*eV, 3.1078*eV, 3.1156*eV, 3.1234*eV, 3.1313*eV, 3.1000*eV, 3.1039*eV, 3.1078*eV, 3.1117*eV, 3.1156*eV, 3.1195*eV, 3.1234*eV, 3.1274*eV, 3.1313*eV, 3.1353*eV, 3.1392*eV, 3.1432*eV, 3.1472*eV, 3.1512*eV, 3.1552*eV, 3.1592*eV, 3.1633*eV, 3.1673*eV, 3.1714*eV, 3.1754*eV, 3.1795*eV, 3.1836*eV, 3.1877*eV, 3.1918*eV, 3.1959*eV, 3.2000*eV, 3.2041*eV, 3.2083*eV, 3.2124*eV, 3.2166*eV, 3.2208*eV, 3.2250*eV, 3.2292*eV, 3.2334*eV, 3.2376*eV, 3.2418*eV, 3.2461*eV, 3.2503*eV, 3.2546*eV, 3.2589*eV, 3.2632*eV, 3.2675*eV, 3.2718*eV, 3.2761*eV, 3.2804*eV, 3.2848*eV, 3.2891*eV, 3.2935*eV, 3.2979*eV, 3.3023*eV, 3.3067*eV, 3.3111*eV, 3.3155*eV, 3.3199*eV, 3.3244*eV, 3.3289*eV, 3.3333*eV, 3.3378*eV, 3.3423*eV, 3.3468*eV, 3.3514*eV, 3.3559*eV, 3.3604*eV, 3.3650*eV, 3.3696*eV, 3.3741*eV, 3.3787*eV, 3.3834*eV, 3.3880*eV, 3.3926*eV, 3.3973*eV, 3.4019*eV, 3.4066*eV, 3.4113*eV, 3.4160*eV, 3.4207*eV, 3.4254*eV, 3.4302*eV, 3.4349*eV, 3.4397*eV, 3.4444*eV, 3.4492*eV, 3.4540*eV, 3.4589*eV, 3.4637*eV, 3.4685*eV, 3.4734*eV, 3.4783*eV, 3.4831*eV, 3.4880*eV, 3.4930*eV, 3.4979*eV, 3.5028*eV, 3.5078*eV, 3.5127*eV, 3.5177*eV, 3.5227*eV, 3.5277*eV, 3.5328*eV, 3.5378*eV, 3.5429*eV, 3.5479*eV, 3.5530*eV, 3.5581*eV, 3.5632*eV, 3.5683*eV, 3.5735*eV, 3.5786*eV, 3.5838*eV, 6.2000*eV, 10.3300*eV, 15.500*eV};
	G4double scintABSLength[502] = {0.2919*m,  0.3005*m, 0.3176*m, 0.3441*m, 0.3873*m, 0.3902*m, 0.3959*m, 0.4209*m, 0.4422*m, 0.4531*m, 0.4600*m, 0.4718*m, 0.4652*m, 0.4396*m, 0.4214*m, 0.3918*m, 0.3653*m, 0.3409*m, 0.3169*m, 0.2999*m, 0.2751*m, 0.2570*m, 0.2437*m, 0.2357*m, 0.2337*m, 0.2357*m, 0.2465*m, 0.2665*m, 0.2888*m, 0.3165*m, 0.3487*m, 0.3872*m, 0.4388*m, 0.4955*m, 0.5627*m, 0.6195*m, 0.7014*m, 0.7829*m, 0.8771*m, 0.9682*m, 1.0587*m, 1.1284*m, 1.2284*m, 1.3007*m, 1.3541*m, 1.3977*m, 1.4458*m, 1.5217*m, 1.6116*m, 1.6694*m, 1.6499*m, 1.6705*m, 1.7253*m, 1.7555*m, 1.7929*m, 1.8192*m, 1.8041*m, 1.8179*m, 1.7608*m, 1.7770*m, 1.8262*m, 1.8372*m, 1.8307*m, 1.8275*m, 1.7740*m, 1.7716*m, 1.7626*m, 1.6933*m, 1.6630*m, 1.6509*m, 1.6775*m, 1.7281*m, 1.7449*m, 1.7531*m, 1.8066*m, 1.8211*m, 1.7991*m, 1.8307*m, 1.8850*m, 1.9106*m, 1.9728*m, 2.0210*m, 2.0472*m, 2.0249*m, 2.0569*m, 2.0882*m, 2.2132*m, 2.2721*m, 2.2622*m, 2.2370*m, 2.3279*m, 2.4254*m, 2.5777*m, 2.6245*m, 2.7147*m, 2.7578*m, 2.8973*m, 3.0392*m, 3.2215*m, 3.1744*m, 3.2435*m, 3.5948*m, 3.6273*m, 3.7258*m, 3.7419*m, 3.1045*m, 3.0818*m, 3.162*m, 3.1076*m, 3.1634*m, 3.2586*m, 3.2378*m, 3.2867*m, 3.1667*m, 3.2902*m, 2.4514*m, 2.3186*m, 2.0723*m, 2.9251*m, 2.8870*m, 2.6630*m, 2.4269*m, 2.3520*m, 2.0751*m, 2.9531*m, 2.7754*m, 2.5388*m, 2.3052*m, 2.0798*m, 1.8668*m, 1.6911*m, 1.5107*m, 1.3701*m, 1.2460*m, 1.1709*m, 1.0945*m, 1.0318*m, 0.9973*m, 0.9784*m, 0.9731*m, 0.9795*m, 0.9813*m, 0.9984*m, 1.0302*m, 1.0580*m, 1.0817*m, 1.1033*m, 1.1291*m, 1.1450*m, 1.1488*m, 1.1510*m, 1.1585*m, 1.1723*m, 1.2023*m, 1.2568*m, 1.3062*m, 1.3928*m, 1.5107*m, 1.6201*m, 1.7584*m, 1.8742*m, 2.0016*m, 2.1300*m, 2.2880*m, 2.4220*m, 2.6022*m, 2.7289*m, 2.8436*m, 3.0164*m, 3.1305*m, 3.2516*m, 3.2844*m, 3.3979*m, 3.4749*m, 3.5362*m, 3.4565*m, 3.2927*m, 3.1437*m, 2.9836*m, 2.7261*m, 2.4891*m, 2.3022*m, 2.1627*m, 2.0344*m, 1.9602*m, 1.9261*m, 1.9543*m, 2.0593*m, 2.2274*m, 2.4379*m, 2.6391*m, 2.7799*m, 2.9038*m, 3.1571*m, 3.3781*m, 3.6047*m, 3.8103*m, 3.9790*m, 2.1435*m, 2.3150*m, 2.4629*m, 2.4476*m, 2.6294*m, 2.7692*m, 2.7867*m, 2.9978*m, 2.1055*m, 2.1663*m, 2.3138*m, 2.4187*m, 2.6723*m, 2.8704*m, 2.0126*m, 2.3112*m, 2.3964*m, 2.4439*m, 2.2582*m, 2.1693*m, 2.3964*m, 2.6921*m, 2.9416*m, 2.5001*m, 2.8685*m, 2.3014*m, 2.1511*m, 2.2709*m, 2.4689*m, 2.5555*m, 2.5120*m, 2.2507*m, 2.4689*m, 2.4475*m, 2.6780*m, 2.7235*m, 2.6107*m, 2.2709*m, 2.1906*m, 2.5555*m, 2.1906*m, 2.0449*m, 1.8594*m, 1.8957*m, 1.6749*m, 1.2060*m, 1.8970*m, 1.3138*m, 1.9930*m, 1.8535*m, 1.8535*m, 1.8176*m, 1.6792*m, 1.6750*m, 1.7648*m, 1.8088*m, 1.8900*m, 1.0560*m, 2.1006*m, 2.1106*m, 2.3084*m, 2.1869*m, 2.2337*m, 2.0757*m, 2.1510*m, 1.3246*m, 1.4757*m, 1.6477*m, 1.8837*m, 1.2356*m, 2.4042*m, 2.6072*m, 1.7353*m, 2.9232*m, 2.2709*m, 2.5120*m, 2.87264*m, 1.95264*m, 1.58576*m, 1.38944*m, 1.49776*m, 1.30832*m, 1.13968*m, 1.71232*m, 1.23504*m, 1.8168*m, 1.30832*m, 1.92608*m, 1.19872*m, 2.5624*m, 2.15936*m, 1.69888*m, 1.37952*m, 1.42624*m, 1.63344*m, 1.86528*m, 2.1064*m, 2.19488*m, 1.30384*m, 1.23504*m, 1.03056*m, 1.08496*m, 1.09984*m, 1.5176*m, 1.28848*m, 1.82336*m, 1.05824*m, 2.2376*m, 3.14288*m, 2.43184*m, 2.44576*m, 2.88896*m, 3.02208*m, 3.25472*m, 3.4072*m, 3.17872*m, 3.26928*m, 3.83232*m, 3.40224*m, 1.21712*m, 3.58144*m, 3.96304*m, 2.78912*m, 2.47616*m, 2.71184*m, 2.5024*m, 2.432*m,  2.85456*m, 2.99776*m, 3.31344*m, 3.44656*m, 2.3232*m, 3.39888*m, 3.06416*m, 3.58672*m, 3.31136*m, 2.33328*m, 3.23728*m, 2.17328*m, 2.53008*m, 2.7384*m, 1.35072*m, 2.7976*m, 2.64288*m, 2.41792*m, 2.3008*m, 2.55232*m, 2.81904*m, 2.312*m,  2.42464*m, 2.1144*m, 2.2448*m, 2.0176*m, 2.47376*m, 2.0568*m, 2.10496*m, 2.98416*m, 2.19648*m, 2.25216*m, 2.64288*m, 2.25264*m, 2.69088*m, 2.27312*m, 2.11184*m, 1.94992*m, 1.48224*m, 1.73392*m, 1.6928*m, 1.9984*m, 1.9491*m, 1.6101*m, 1.8526*m, 1.1005*m, 1.5001*m, 1.8324*m, 1.758*m, 1.7928*m, 1.3715*m, 1.0455*m, 0.7664*m, 0.5591*m, 0.4065*m, 0.2934*m, 0.2101*m, 0.1529*m, 0.1133*m, 0.0849*m, 0.0650*m, 0.0503*m, 0.0393*m, 0.0314*m, 0.0255*m, 0.07207*m,  0.06294*m,  0.05539*m,  0.04875*m,  0.04280*m,  0.03801*m,  0.03402*m,  0.03047*m,  0.02733*m,  0.02468*m,  0.02243*m,  0.02003*m,  0.01824*m,  0.01666*m,  0.01527*m,  0.01403*m,  0.01295*m,  0.01197*m,  0.01113*m,  0.01037*m,  0.00966*m,  0.00906*m,  0.00853*m,  0.00806*m,  0.00763*m,  0.00723*m,  0.00688*m,  0.00656*m,  0.00627*m,  0.00601*m,  0.00576*m,  0.00553*m,  0.00532*m,  0.00512*m,  0.00495*m,  0.00478*m,  0.00462*m,  0.00449*m,  0.00436*m,  0.00424*m,  0.00412*m,  0.00401*m,  0.00391*m,  0.00381*m,  0.00371*m,  0.00362*m,  0.00354*m,  0.00346*m,  0.00338*m,  0.00331*m,  0.00324*m,  0.00317*m,  0.00310*m,  0.00303*m,  0.00297*m,  0.00291*m,  0.00285*m,  0.00280*m,  0.00274*m,  0.00268*m,  0.00263*m,  0.00258*m,  0.00253*m,  0.00248*m,  0.00244*m,  0.00239*m,  0.00235*m,  0.00231*m,  0.00228*m,  0.00224*m,  0.00221*m,  0.00218*m,  0.00215*m,  0.00210*m,  0.00207*m,  0.00204*m,  0.00202*m,  0.00199*m,  0.00196*m,  0.00194*m,  0.00191*m,  0.00189*m,  0.00186*m,  0.00183*m,  0.00181*m,  0.00178*m,  0.00175*m,  0.00171*m,  0.00168*m,  0.00164*m,  0.00161*m,  0.00156*m,  0.00152*m,  0.00148*m,  0.00143*m,  0.00138*m,  0.00132*m,  0.00126*m,  0.00121*m,  0.00115*m,  0.00108*m,  0.00102*m,  0.00095*m,  0.00088*m,  0.00077*m,  0.00070*m,  0.00062*m,  0.00055*m,  0.00048*m,  0.000001*m, 0.000001*m, 0.000001*m};
	MPTscint->AddProperty("ABSLENGTH", scintABSEnergy, scintABSLength, 502);

    // Refraction index
    G4double scintRefIndexEnergy[18] = {1.55*eV, 1.79505*eV, 2.10499*eV, 2.27077*eV, 2.55111*eV, 2.84498*eV, 3.06361*eV, 4.13281*eV, 6.20*eV, 6.526*eV, 6.889*eV, 7.294*eV, 7.75*eV, 8.267*eV, 8.857*eV, 9.538*eV, 10.33*eV, 15.5*eV};
    G4double Pscintrindex = 1.52;
    G4double scintRefIndex[18] = {Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex, Pscintrindex};
	MPTscint->AddProperty("RINDEX",   scintRefIndexEnergy, scintRefIndex, 18);

    // Reemission? I don't know what process this is
    G4double scintReemEnergy[28] = {1.55000*eV, 1.73987*eV, 2.15315*eV, 2.42867*eV, 2.45963*eV, 2.49129*eV, 2.52368*eV, 2.55682*eV, 2.59075*eV, 2.62547*eV, 2.66104*eV, 2.69747*eV, 2.73481*eV, 2.77308*eV, 2.81231*eV, 2.85255*eV, 2.89384*eV, 2.93621*eV, 2.97972*eV, 3.02439*eV, 3.07030*eV, 3.21588*eV, 3.80628*eV, 5.87347*eV, 6.20000*eV, 7.75000*eV, 10.33*eV, 15.5*eV};
    G4double scintReem[28] = {0, 0, 0.0587, 0.1141, 0.1259, 0.1215, 0.116, 0.1372, 0.1344, 0.1409, 0.1417, 0.1712, 0.1936, 0.1969, 0.2203, 0.2354, 0.3004, 0.4165, 0.6353, 0.8022, 0.8004, 0.8006, 0.8, 0.45, 0.42, 0.40, 0.40, 0.40};
	
    // Rayleigh scattering
    G4double scintRayEnergy[11] = {1.55*eV, 1.7714*eV, 2.102*eV, 2.255*eV, 2.531*eV, 2.884*eV, 3.024*eV, 4.133*eV, 6.20*eV, 10.33*eV, 15.5*eV};
    G4double scintRayLength[11] = {500.0*m, 300.0*m, 170.0*m, 100.0*m, 62.0*m, 42.0*m, 30.0*m, 7.6*m, 0.85*m, 0.85*m, 0.85*m};
    MPTscint->AddProperty("RAYLEIGH", scintRayEnergy, scintRayLength, 11);

    // Scintillation
    G4double scintComEnergy[275] = {1.550000*eV, 2.066401*eV, 2.069851*eV, 2.073312*eV, 2.076791*eV, 2.080272*eV, 2.083771*eV, 2.087279*eV, 2.090801*eV, 2.094329*eV, 2.097869*eV, 2.101431*eV, 2.104992*eV, 2.108571*eV, 2.112170*eV, 2.115770*eV, 2.119390*eV, 2.123019*eV, 2.126660*eV, 2.130310*eV, 2.133979*eV, 2.137658*eV, 2.141350*eV, 2.145062*eV, 2.148771*eV, 2.152501*eV, 2.156252*eV, 2.160000*eV, 2.163769*eV, 2.167559*eV, 2.171351*eV, 2.175160*eV, 2.178978*eV, 2.182822*eV, 2.186671*eV, 2.190530*eV, 2.194411*eV, 2.198301*eV, 2.202209*eV, 2.206119*eV, 2.210059*eV, 2.214001*eV, 2.217961*eV, 2.221940*eV, 2.225928*eV, 2.229931*eV, 2.233948*eV, 2.237980*eV, 2.242031*eV, 2.246092*eV, 2.250172*eV, 2.254259*eV, 2.258360*eV, 2.262489*eV, 2.266620*eV, 2.270771*eV, 2.274941*eV, 2.279118*eV, 2.283319*eV, 2.287532*eV, 2.291759*eV, 2.295999*eV, 2.300262*eV, 2.304541*eV, 2.308828*eV, 2.313139*eV, 2.317458*eV, 2.321802*eV, 2.326162*eV, 2.330529*eV, 2.334922*eV, 2.339318*eV, 2.343749*eV, 2.348191*eV, 2.352642*eV, 2.357110*eV, 2.361599*eV, 2.366110*eV, 2.370629*eV, 2.375179*eV, 2.379741*eV, 2.384308*eV, 2.388911*eV, 2.393522*eV, 2.398151*eV, 2.402789*eV, 2.407458*eV, 2.412142*eV, 2.416848*eV, 2.421572*eV, 2.426311*eV, 2.431058*eV, 2.435838*eV, 2.440628*eV, 2.445451*eV, 2.450278*eV, 2.455130*eV, 2.460000*eV, 2.464890*eV, 2.469800*eV, 2.474729*eV, 2.479678*eV, 2.484651*eV, 2.489640*eV, 2.494649*eV, 2.499683*eV, 2.504732*eV, 2.509801*eV, 2.514892*eV, 2.520003*eV, 2.525139*eV, 2.530292*eV, 2.535461*eV, 2.540661*eV, 2.545882*eV, 2.551120*eV, 2.556380*eV, 2.561661*eV, 2.566958*eV, 2.572289*eV, 2.577630*eV, 2.583000*eV, 2.588402*eV, 2.593811*eV, 2.599248*eV, 2.604708*eV, 2.610191*eV, 2.615703*eV, 2.621232*eV, 2.626779*eV, 2.632361*eV, 2.637961*eV, 2.643591*eV, 2.649239*eV, 2.654911*eV, 2.660607*eV, 2.666328*eV, 2.672068*eV, 2.677839*eV, 2.683640*eV, 2.689461*eV, 2.695312*eV, 2.701178*eV, 2.707081*eV, 2.712998*eV, 2.718952*eV, 2.724927*eV, 2.730929*eV, 2.736962*eV, 2.743011*eV, 2.749098*eV, 2.755200*eV, 2.761342*eV, 2.767499*eV, 2.773702*eV, 2.779908*eV, 2.786160*eV, 2.792441*eV, 2.798737*eV, 2.805068*eV, 2.811428*eV, 2.817823*eV, 2.824241*eV, 2.830688*eV, 2.837172*eV, 2.843671*eV, 2.850208*eV, 2.856781*eV, 2.863378*eV, 2.869998*eV, 2.876663*eV, 2.883352*eV, 2.890072*eV, 2.896831*eV, 2.903607*eV, 2.910429*eV, 2.917283*eV, 2.924163*eV, 2.931068*eV, 2.938013*eV, 2.944990*eV, 2.952001*eV, 2.959053*eV, 2.966131*eV, 2.973243*eV, 2.980390*eV, 2.987570*eV, 2.994793*eV, 3.002043*eV, 3.009329*eV, 3.016650*eV, 3.023999*eV, 3.031399*eV, 3.038828*eV, 3.046286*eV, 3.053804*eV, 3.061343*eV, 3.068920*eV, 3.076534*eV, 3.084178*eV, 3.091884*eV, 3.099613*eV, 3.107372*eV, 3.115179*eV, 3.123032*eV, 3.130910*eV, 3.138843*eV, 3.146809*eV, 3.154807*eV, 3.162862*eV, 3.170950*eV, 3.179080*eV, 3.187251*eV, 3.195473*eV, 3.203729*eV, 3.212027*eV, 3.220369*eV, 3.228763*eV, 3.237192*eV, 3.245657*eV, 3.254183*eV, 3.262737*eV, 3.271353*eV, 3.280007*eV, 3.288706*eV, 3.297451*eV, 3.306252*eV, 3.315091*eV, 3.323969*eV, 3.332912*eV, 3.341886*eV, 3.350917*eV, 3.360006*eV, 3.369144*eV, 3.378323*eV, 3.387552*eV, 3.396832*eV, 3.406163*eV, 3.415536*eV, 3.424979*eV, 3.434465*eV, 3.444014*eV, 3.453596*eV, 3.463252*eV, 3.472952*eV, 3.482696*eV, 3.492505*eV, 3.502380*eV, 3.512300*eV, 3.522277*eV, 3.532311*eV, 3.542412*eV, 3.552561*eV, 3.562758*eV, 3.573034*eV, 3.583359*eV, 3.593745*eV, 3.604190*eV, 3.614697*eV, 3.625275*eV, 3.635905*eV, 3.646587*eV, 3.657353*eV, 3.668172*eV, 3.679056*eV, 3.690015*eV, 3.701017*eV, 3.712097*eV, 3.723254*eV, 3.734456*eV, 3.745748*eV, 3.757098*eV, 6.200000*eV, 10.33*eV, 15.5*eV};
	G4double scintComponent[275] = {0.000000, 0.007298, 0.007011, 0.007932, 0.008065, 0.007699, 0.006620, 0.007727, 0.007464, 0.007931, 0.006825, 0.007623, 0.006487, 0.007494, 0.007604, 0.007250, 0.007187, 0.008465, 0.007539, 0.007951, 0.007393, 0.008746, 0.008358, 0.008536, 0.008132, 0.009147, 0.009598, 0.008566, 0.008370, 0.009756, 0.007964, 0.008557, 0.009199, 0.009928, 0.008394, 0.009174, 0.009142, 0.009985, 0.009611, 0.009985, 0.011081, 0.010671, 0.010177, 0.012099, 0.010819, 0.011596, 0.012298, 0.012099, 0.012604, 0.013004, 0.013502, 0.012587, 0.014589, 0.015733, 0.015105, 0.014145, 0.015762, 0.015455, 0.015944, 0.015391, 0.016797, 0.018667, 0.018206, 0.018382, 0.020014, 0.019940, 0.021778, 0.021521, 0.023801, 0.024522, 0.024905, 0.025821, 0.028128, 0.026988, 0.029616, 0.028540, 0.029964, 0.031531, 0.034344, 0.033769, 0.036688, 0.037003, 0.038282, 0.039943, 0.042146, 0.043553, 0.044054, 0.046702, 0.045442, 0.049577, 0.053079, 0.050783, 0.056636, 0.056936, 0.058559, 0.061420, 0.064514, 0.065756, 0.068394, 0.073655, 0.074814, 0.080707, 0.083768, 0.087875, 0.089284, 0.095072, 0.099422, 0.107914, 0.112227, 0.118243, 0.122406, 0.125939, 0.131992, 0.136012, 0.140968, 0.147579, 0.156020, 0.158363, 0.167771, 0.168055, 0.175895, 0.179084, 0.185346, 0.193068, 0.198061, 0.202359, 0.206894, 0.212571, 0.218799, 0.223242, 0.229322, 0.239725, 0.250068, 0.257243, 0.264742, 0.281645, 0.294740, 0.309784, 0.322450, 0.343039, 0.363025, 0.375340, 0.400378, 0.421838, 0.435316, 0.452269, 0.475444, 0.489341, 0.505680, 0.524780, 0.530979, 0.542752, 0.552351, 0.562429, 0.567725, 0.572789, 0.578060, 0.588238, 0.587622, 0.584193, 0.593162, 0.601844, 0.613819, 0.628391, 0.642112, 0.656654, 0.673388, 0.704065, 0.727850, 0.764664, 0.791312, 0.832467, 0.870620, 0.894459, 0.929799, 0.955303, 0.977617, 0.987266, 0.999449, 1.000000, 0.994073, 0.971962, 0.958778, 0.934527, 0.902721, 0.870857, 0.838456, 0.805928, 0.785872, 0.756622, 0.733803, 0.724502, 0.711707, 0.722673, 0.723408, 0.739724, 0.761128, 0.784760, 0.803470, 0.818395, 0.834321, 0.828638, 0.812963, 0.788427, 0.761730, 0.705279, 0.642877, 0.582818, 0.508845, 0.435086, 0.372081, 0.304945, 0.248959, 0.200416, 0.156939, 0.128059, 0.102210, 0.082023, 0.062593, 0.051869, 0.042981, 0.035078, 0.028940, 0.024279, 0.020112, 0.016616, 0.016298, 0.014177, 0.012852, 0.012006, 0.011478, 0.009994, 0.009320, 0.008654, 0.008841, 0.008425, 0.008513, 0.008608, 0.007599, 0.008116, 0.007355, 0.006986, 0.006304, 0.005700, 0.005496, 0.004889, 0.004423, 0.003786, 0.003303, 0.003402, 0.003424, 0.002978, 0.002702, 0.002367, 0.002698, 0.002239, 0.002125, 0.002578, 0.002259, 0.002102, 0.002050, 0.002311, 0.001994, 0.001809, 0.001690, 0.001965, 0.001909, 0.002218, 0.001887, 0.001981, 0.002153, 0.002269, 0.000000, 0.000000, 0.000000};
	MPTscint->AddConstProperty("SCINTILLATIONYIELD", 8000./MeV);
    MPTscint->AddConstProperty("RESOLUTIONSCALE",1.0);
    MPTscint->AddProperty("SCINTILLATIONCOMPONENT1", scintComEnergy, scintComponent, 275);
    MPTscint->AddConstProperty("SCINTILLATIONYIELD1", 1.0);
    MPTscint->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 1.50*ns); // This may be wrong, because I don't understand "ReemissionFASTTIMECONSTANT"

    LAB->SetMaterialPropertiesTable(MPTscint);
    LAB->GetIonisation()->SetBirksConstant(0.125*mm/MeV);

    /*************Table of PMMA*************/

    G4MaterialPropertiesTable* mptWLSfiber = new G4MaterialPropertiesTable();

    // Refraction index
    G4double photonEnergy[] =
        {2.00*eV,2.03*eV,2.06*eV,2.09*eV,2.12*eV,
        2.15*eV,2.18*eV,2.21*eV,2.24*eV,2.27*eV,
        2.30*eV,2.33*eV,2.36*eV,2.39*eV,2.42*eV,
        2.45*eV,2.48*eV,2.51*eV,2.54*eV,2.57*eV,
        2.60*eV,2.63*eV,2.66*eV,2.69*eV,2.72*eV,
        2.75*eV,2.78*eV,2.81*eV,2.84*eV,2.87*eV,
        2.90*eV,2.93*eV,2.96*eV,2.99*eV,3.02*eV,
        3.05*eV,3.08*eV,3.11*eV,3.14*eV,3.17*eV,
        3.20*eV,3.23*eV,3.26*eV,3.29*eV,3.32*eV,
        3.35*eV,3.38*eV,3.41*eV,3.44*eV,3.47*eV,
        3.50*eV,3.53*eV,3.56*eV,3.59*eV,3.62*eV,
        3.65*eV,3.69*eV,3.72*eV,3.75*eV,6.20*eV};
    G4double refractiveIndexWLSfiber[] =
        { 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
        1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
        1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
        1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
        1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
        1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60};
    const G4int nEntries = sizeof(photonEnergy)/sizeof(G4double);
    mptWLSfiber->AddProperty("RINDEX",photonEnergy,refractiveIndexWLSfiber,nEntries);

    // Wavelength shifting
    G4double absWLSfiber[] =
        {2.40*m,2.00*m,2.00*m,1.90*m,1.80*m,1.70*m,1.60*m,1.50*m,1.45*m,1.40*m,
        1.39*m,1.36*m,1.35*m,1.34*m,1.33*m,1.32*m,1.30*m,1.20*m,1.19*m,1.10*m,
        1.00*m,0.50*m,0.29*m,0.10*m,0.05*m,20.0*mm,10.0*mm,8.30*mm,3.00*mm,2.00*mm,
        2.0*mm,1.40*mm,1.30*mm,1.20*mm,1.10*mm,0.10*mm, 0.80*mm, 0.90*mm, 1.00*mm, 1.20*mm,
        1.38*mm,1.8*mm,2.08*mm,3.08*mm,4.80*mm,5.05*mm, 6.02*mm, 7.01*mm, 8.01*mm, 9.01*mm,
        10.0*mm,11.0*mm,15.0*mm,20.0*mm,25.0*mm,60.0*mm,95.0*mm,1.00*m,2.00*m,5.00*m};
    G4double emissionFib[] =
        {0.05, 0.10, 0.30, 0.50, 0.75, 1.00, 1.50, 1.85, 2.30, 2.75,
        3.25, 3.80, 4.50, 5.20, 6.00, 7.00, 8.50, 9.50, 11.1, 12.4,
        12.9, 13.0, 12.8, 12.3, 11.1, 11.0, 12.0, 11.0, 17.0, 16.9,
        15.0, 5.00, 3.50, 2.00, 1.05, 0.50, 0.40, 0.30, 0.20, 0.10,
        0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05,
        0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05};
    mptWLSfiber->AddProperty("WLSABSLENGTH",photonEnergy,absWLSfiber,nEntries);
    mptWLSfiber->AddProperty("WLSCOMPONENT",photonEnergy,emissionFib,nEntries);
    mptWLSfiber->AddConstProperty("WLSTIMECONSTANT", 0.5*ns);

    PMMA->SetMaterialPropertiesTable(mptWLSfiber);

    /*************Table of Pethylene_1*************/

    G4MaterialPropertiesTable* mptClad_1 = new G4MaterialPropertiesTable();

    G4double refractiveIndexClad_1[] =
        { 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
        1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
        1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
        1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
        1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
        1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42};
    
    G4double absClad[] =
        {20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
        20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
        20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
        20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
        20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
        20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m};

    mptClad_1->AddProperty("RINDEX",photonEnergy,refractiveIndexClad_1,nEntries);
    mptClad_1->AddProperty("ABSLENGTH",photonEnergy,absClad,nEntries);

    Pethylene_1->SetMaterialPropertiesTable(mptClad_1);

    /*************Table of Pethylene_2*************/

    G4MaterialPropertiesTable* mptClad_2 = new G4MaterialPropertiesTable();

    G4double refractiveIndexClad_2[] =
        { 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
        1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
        1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
        1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
        1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
        1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49};

    mptClad_2->AddProperty("RINDEX",photonEnergy,refractiveIndexClad_2,nEntries);
    mptClad_2->AddProperty("ABSLENGTH",photonEnergy,absClad,nEntries);

    Pethylene_2->SetMaterialPropertiesTable(mptClad_2);
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

    // Groove for fiber
    G4SubtractionSolid* pscint_subtraction_solid;
    for(int groove_index = 0; groove_index < fiber_count; groove_index++)
    {
        G4double straight_groove_x_position = pscint_x/2 - pscint_x/(fiber_count+1)*(groove_index+1);
        G4double bend_groove_end_x_position = SiPM_length/2 - SiPM_length/(fiber_count+1)*(groove_index+1);
        G4bool is_upper_groove = groove_index < fiber_count / 2;
        G4double delta_x = std::abs(straight_groove_x_position - bend_groove_end_x_position);
        G4double bend_center_angle = std::acos(
            ((fiber_bend_r - delta_x) + std::sqrt((fiber_bend_r - delta_x)*(fiber_bend_r - delta_x)-4*fiber_bend_r*groove_width/2))
            /2/(fiber_bend_r-groove_width/2)
        );
        G4double straight_groove_length = pscint_z - 2*std::sin(bend_center_angle)*fiber_bend_r;

        G4Box* straight_groove = new G4Box("straight_groove_"+std::to_string(groove_index), groove_width/2, groove_depth/2+0.01*mm, straight_groove_length+0.01*mm);
        G4Tubs* bend_groove = new G4Tubs("bend_groove_"+std::to_string(groove_index), fiber_bend_r-groove_width/2, fiber_bend_r+groove_width/2, groove_depth/2+0.01*mm, 0, bend_center_angle);

        G4RotationMatrix* bend_groove_rot_upper = new G4RotationMatrix();
        bend_groove_rot_upper->rotateX(90*degree);
        G4RotationMatrix* bend_groove_rot_lower = new G4RotationMatrix();
        bend_groove_rot_lower->rotateX(-90*degree);
        G4ThreeVector bend_groove_translate_upper(-fiber_bend_r, 0, straight_groove_length/2);
        G4ThreeVector bend_groove_translate_lower(-fiber_bend_r, 0, -straight_groove_length/2);
        G4Transform3D straight_groove_transform = G4Transform3D();
        G4Transform3D bend_groove_transform_upper(*bend_groove_rot_upper, bend_groove_translate_upper);
        G4Transform3D bend_groove_transform_lower(*bend_groove_rot_lower, bend_groove_translate_lower);
        
        G4MultiUnion* groove_union_solid = new G4MultiUnion("groove_union_solid_"+std::to_string(groove_index));
        groove_union_solid->AddNode(straight_groove, straight_groove_transform);
        groove_union_solid->AddNode(bend_groove, bend_groove_transform_upper);
        groove_union_solid->AddNode(bend_groove, bend_groove_transform_lower);
        
        groove_union_solid->Voxelize();
        
        G4RotationMatrix* groove_rot = new G4RotationMatrix();
        groove_rot->rotateZ(180*(1-is_upper_groove)*degree);
        G4Transform3D groove_transform(*groove_rot, G4ThreeVector(straight_groove_x_position,pscint_y/2-groove_depth/2,0));

        if(groove_index == 0)
            pscint_subtraction_solid = new G4SubtractionSolid("groove_solid_"+std::to_string(groove_index), pscint_solid, groove_union_solid, groove_transform);
        else
            pscint_subtraction_solid = new G4SubtractionSolid("groove_solid_"+std::to_string(groove_index), pscint_subtraction_solid, groove_union_solid, groove_transform);
    }

    G4LogicalVolume* pscint_log = new G4LogicalVolume(pscint_subtraction_solid, LAB, "pscint_log");
	G4VPhysicalVolume* pscint_phys = new G4PVPlacement(0,G4ThreeVector(0*mm,0*mm,0*mm), pscint_log,"pscint_phys",experimentalHall_log,false,0,checkOverlaps);

    // Fibers, notice that fiber_count must be an even number here
    for(int fiber_index = 0; fiber_index < fiber_count; fiber_index++)
    {
        G4double straight_fiber_x_position = pscint_x/2 - pscint_x/(fiber_count+1)*(fiber_index+1);

        // Most outer layer
        G4IntersectionSolid* fiber_solid = GetFiberPart("fiber_solid_"+std::to_string(fiber_index), fiber_d, pscint_solid, fiber_index);
        G4LogicalVolume* fiber_log = new G4LogicalVolume(fiber_solid, Pethylene_1, "fiber_log_"+std::to_string(fiber_index));
        new G4PVPlacement(0,G4ThreeVector(straight_fiber_x_position,0,0),fiber_log, "fiber_phys_"+std::to_string(fiber_index), experimentalHall_log, false, fiber_index, checkOverlaps);
    
        // Outer cladding
        G4IntersectionSolid* cladding_1_solid = GetFiberPart("fiber_outer_cladding_solid"+std::to_string(fiber_index), fiber_d - 2*cladding_depth_1, pscint_solid, fiber_index);        
        G4LogicalVolume* cladding_1_log = new G4LogicalVolume(cladding_1_solid, Pethylene_2, "fiber_outer_cladding_log_"+std::to_string(fiber_index));
        new G4PVPlacement(0,G4ThreeVector(0,0,0),cladding_1_log, "fiber_outer_cladding_phys_"+std::to_string(fiber_index), fiber_log, false, fiber_index, checkOverlaps);

        // Inner cladding
        G4IntersectionSolid* cladding_2_solid = GetFiberPart("fiber_inner_cladding_solid"+std::to_string(fiber_index), fiber_d - 2*cladding_depth_1 - 2*cladding_depth_2, pscint_solid, fiber_index);        
        G4LogicalVolume* cladding_2_log = new G4LogicalVolume(cladding_2_solid, PMMA, "fiber_outer_cladding_log_"+std::to_string(fiber_index));
        new G4PVPlacement(0,G4ThreeVector(0,0,0),cladding_2_log, "fiber_inner_cladding_phys_"+std::to_string(fiber_index), cladding_1_log, false, fiber_index, checkOverlaps);    
    }

    // SiPM on both sides
    G4Box* SiPM_solid = new G4Box("SiPM_solid", 0.5*SiPM_length, 0.5*SiPM_width, 0.5*SiPM_depth);
    G4LogicalVolume* SiPM_0_log = new G4LogicalVolume(SiPM_solid, glass, "SiPM_0_log");
    G4LogicalVolume* SiPM_1_log = new G4LogicalVolume(SiPM_solid, glass, "SiPM_1_log");
    new G4PVPlacement(0, G4ThreeVector(0*mm, pscint_y/2-fiber_depth, 0.5*pscint_z+air_interval), SiPM_0_log, "SiPM_0_phys", experimentalHall_log, false, 0, checkOverlaps);
    new G4PVPlacement(0, G4ThreeVector(0*mm, pscint_y/2-fiber_depth, -0.5*pscint_z-air_interval), SiPM_1_log, "SiPM_1_phys", experimentalHall_log, false, 1, checkOverlaps);

    // Optical surfaces
    new G4LogicalBorderSurface(
        "LAB_teflon_surface", pscint_phys, shell_phys, op_LAB_teflon_surface
    );

    new G4LogicalBorderSurface(
        "teflon_air_surface", experimentalHall_phys, shell_phys, op_teflon_air_surface
    );

    return experimentalHall_phys;
}

G4IntersectionSolid* MuonVeto::MVDetectorConstruction::GetFiberPart(const G4String &name, G4double diameter, G4Box* pscint_solid, G4int fiber_index) const
{   
    // counter so that names are unique
    static G4int id = 0;

    // parameter calculation
    G4double straight_fiber_x_position = pscint_x/2 - pscint_x/(fiber_count+1)*(fiber_index+1);
    G4double bend_fiber_end_x_position = SiPM_length/2 - SiPM_length/(fiber_count+1)*(fiber_index+1);
    G4bool is_upper_fiber = fiber_index < fiber_count / 2;
    G4double delta_x = std::abs(straight_fiber_x_position - bend_fiber_end_x_position);
    G4double bend_center_angle = std::acos(
        ((fiber_bend_r - delta_x) + std::sqrt((fiber_bend_r - delta_x)*(fiber_bend_r - delta_x)-4*fiber_bend_r*fiber_d/2))
        /2/(fiber_bend_r-fiber_d/2)
    );
    G4double straight_fiber_length = pscint_z - 2*std::sin(bend_center_angle)*fiber_bend_r;

    // basis geometry
    G4Tubs* straight_fiber = new G4Tubs("straight_fiber_"+std::to_string(id)+std::to_string(fiber_index), 0, diameter/2, straight_fiber_length/2+0.01*mm, 0, 2*M_PI);
    G4Torus* bend_fiber = new G4Torus("bend_fiber_"+std::to_string(id)+std::to_string(fiber_index), 0, diameter/2, fiber_bend_r, 0, bend_center_angle);

    // transformation calculation
    G4RotationMatrix* bend_fiber_rot_upper = new G4RotationMatrix();
    bend_fiber_rot_upper->rotateX(90*degree);
    G4RotationMatrix* bend_fiber_rot_lower = new G4RotationMatrix();
    bend_fiber_rot_lower->rotateX(-90*degree);
    G4ThreeVector bend_fiber_translate_upper(-fiber_bend_r, 0, straight_fiber_length/2);
    G4ThreeVector bend_fiber_translate_lower(-fiber_bend_r, 0, -straight_fiber_length/2);
    G4Transform3D straight_fiber_transform = G4Transform3D();
    G4Transform3D bend_fiber_transform_upper(*bend_fiber_rot_upper, bend_fiber_translate_upper);
    G4Transform3D bend_fiber_transform_lower(*bend_fiber_rot_lower, bend_fiber_translate_lower);
    
    // union three parts into one
    G4MultiUnion* fiber_union_solid = new G4MultiUnion("fiber_union_solid_"+std::to_string(id)+std::to_string(fiber_index));
    fiber_union_solid->AddNode(straight_fiber, straight_fiber_transform);
    fiber_union_solid->AddNode(bend_fiber, bend_fiber_transform_upper);
    fiber_union_solid->AddNode(bend_fiber, bend_fiber_transform_lower);
    fiber_union_solid->Voxelize();
    
    // intersect with pscint
    G4RotationMatrix* fiber_rot = new G4RotationMatrix();
    fiber_rot->rotateZ(180*(1-is_upper_fiber)*degree);
    G4Transform3D fiber_transform(*fiber_rot, G4ThreeVector(0,pscint_y/2-fiber_depth,0));

    id++;
    return new G4IntersectionSolid(name, pscint_solid, fiber_union_solid, fiber_transform);
}


G4VPhysicalVolume* MuonVeto::MVDetectorConstruction::Construct()
{
    DefineMaterials();
    DefineMaterialTables();
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

    // Use SiPMSD as the sensitive detector
    SiPMSD* SiPMSD_0  = new SiPMSD(SiPM_name+"_0", "collection_0", XY_minus);
    SiPMSD_0->SetFilter(photon_filter);
    G4SDManager::GetSDMpointer()->AddNewDetector(SiPMSD_0);
    SetSensitiveDetector("SiPM_0_log", SiPMSD_0);

    SiPMSD* SiPMSD_1  = new SiPMSD(SiPM_name+"_1", "collection_1", XY_plus);
    SiPMSD_1->SetFilter(photon_filter);
    G4SDManager::GetSDMpointer()->AddNewDetector(SiPMSD_1);
    SetSensitiveDetector("SiPM_1_log", SiPMSD_1);

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
