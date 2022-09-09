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
    fScintYieldCmd->SetParameterName("scintYield", false);
    fScintYieldCmd->SetDefaultUnit("/MeV");
    fScintYieldCmd->SetUnitCandidates("/eV /keV /MeV /GeV");
    fScintYieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fFiberCountCmd = new G4UIcmdWithAnInteger("/detector/fiberCount", this);
    fFiberCountCmd->SetGuidance("Set fiber count.");
    fFiberCountCmd->SetGuidance("Possible values: 4 or 6.");
    fFiberCountCmd->SetParameterName("fiberCount", false);
    fFiberCountCmd->SetRange("fiberCount == 4 || fiberCount == 6");
    fFiberCountCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

MVDetectorMessenger::~MVDetectorMessenger()
{
    delete fScintYieldCmd;
    delete fFiberCountCmd;
    delete fDetectorDirectory;
}

void MVDetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValues)
{
    if(command == fScintYieldCmd)
    {
        fDetector->SetScintYield(fScintYieldCmd->GetNewDoubleValue(newValues));
    }
    if(command == fFiberCountCmd)
    {
        fDetector->SetFiberCount(fFiberCountCmd->GetNewIntValue(newValues));
    }
}

G4String MVDetectorMessenger::GetCurrentValue(G4UIcommand* command)
{
    G4String cv; // current value
    if(command == fScintYieldCmd)
    {
        cv = fScintYieldCmd->ConvertToString(fDetector->GetScintYield());
    }
    else if(command == fFiberCountCmd)
    {
        cv = fFiberCountCmd->ConvertToString(fDetector->GetFiberCount());
    }
    return cv;
}