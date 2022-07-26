#ifndef MVEventAction_h
#define MVEventAction_h 1

#include "G4UserEventAction.hh"
#include "MVSteppingAction.hh"
#include "MVGlobals.hh"

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
        RECORDER fCPNRecorder;
        RECORDER fFVPathRecorder;
        RECORDER fEPNRecorder;
        STRLIST fStrList;
};

}
#endif
