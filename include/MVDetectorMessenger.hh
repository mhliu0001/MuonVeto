#ifndef MVDetectorMessenger_h
#define MVDetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"

namespace MuonVeto
{

class MVDetectorConstruction;

class MVDetectorMessenger : public G4UImessenger
{
    public:
        MVDetectorMessenger(MVDetectorConstruction* detector);
        ~MVDetectorMessenger();
        void SetNewValue(G4UIcommand* command, G4String newValues);
        G4String GetCurrentValue(G4UIcommand* command);

    private:
        MVDetectorConstruction* fDetector;
        G4UIdirectory* fDetectorDirectory;
        G4UIcmdWithADoubleAndUnit* fScintYieldCmd;
        G4UIcmdWithAnInteger* fFiberCountCmd;
};


}

#endif