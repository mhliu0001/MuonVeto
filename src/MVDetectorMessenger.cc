#include "MVDetectorMessenger.hh"
#include "MVDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

using namespace MuonVeto;

MVDetectorMessenger::MVDetectorMessenger(MVDetectorConstruction* detector):
    fDetector(detector)
{
    fDetectorDirectory = new G4UIdirectory("/detector/");
    fDetectorDirectory->SetGuidance("Detector Construction control commands.");

    new G4UnitDefinition("PerElectronVolt", "/eV", "ScintYield", 1/eV);
    new G4UnitDefinition("PerKiloElectronVolt", "/keV", "ScintYield", 1/keV);
    new G4UnitDefinition("PerMegaElectronVolt", "/MeV", "ScintYield", 1/MeV);
    new G4UnitDefinition("PerGigaElectronVolt", "/GeV", "ScintYield", 1/GeV);
    fScintYieldCmd = new G4UIcmdWithADoubleAndUnit("/detector/scintYield", this);
    fScintYieldCmd->SetGuidance("Set scintillation yield for the plastic scintillator.");
    fScintYieldCmd->SetGuidance("Default unit is /MeV. Default value is 8000/MeV.");
    fScintYieldCmd->SetParameterName("ScintYield", false);
    fScintYieldCmd->SetDefaultUnit("/MeV");
    fScintYieldCmd->SetUnitCandidates("/eV /keV /MeV /GeV");
    fScintYieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

MVDetectorMessenger::~MVDetectorMessenger()
{
    delete fScintYieldCmd;
    delete fDetectorDirectory;
}

void MVDetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValues)
{
    if(command == fScintYieldCmd)
    {
        fDetector->SetScintYield(fScintYieldCmd->GetNewDoubleValue(newValues));
    }
}

G4String MVDetectorMessenger::GetCurrentValue(G4UIcommand* command)
{
    G4String cv; // current value
    if(command == fScintYieldCmd)
    {
        cv = fScintYieldCmd->ConvertToString(fDetector->GetScintYield());
    }
    return cv;
}