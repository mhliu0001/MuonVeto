#ifndef MVEventAction_h
#define MVEventAction_h 1

#include "G4UserEventAction.hh"
#include "MVEventInformation.hh"
#include "MVSteppingAction.hh"
#include "MVGlobals.hh"

namespace MuonVeto
{

class MVRunAction;

class MVEventAction : public G4UserEventAction
{
    public:
        MVEventAction(const Config& config);
        ~MVEventAction() override;

        void  BeginOfEventAction(const G4Event* ) override;
        void    EndOfEventAction(const G4Event* ) override;
        MVEventInformation* eventInformation;
        std::map<G4int, G4double> fEnergyRecorder;

    private:
        friend class MVSteppingAction;
        const Config fConfig;
};

}
#endif