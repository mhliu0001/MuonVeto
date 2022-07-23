#ifndef MVEventAction_h
#define MVEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "MVSteppingAction.hh"

namespace MuonVeto
{

class MVRunAction;

class MVEventAction : public G4UserEventAction
{
    public:
        MVEventAction();
        ~MVEventAction() override;

        void  BeginOfEventAction(const G4Event* ) override;
        void    EndOfEventAction(const G4Event* ) override;

    private:
        friend class MVSteppingAction;
        std::map<G4int, G4String> fCPNRecorder;
        std::map<G4int, G4String> fFVPathRecorder;
        std::map<G4int, G4String> fEPNRecorder;
};

}
#endif
