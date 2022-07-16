// This file is for the use of a customized trajectory

#ifndef MVTrackingAction_h
#define MVTrackingAction_h 1

#include "G4UserTrackingAction.hh"

namespace MuonVeto
{

class MVTrackingAction : public G4UserTrackingAction
{
    public:
        MVTrackingAction();
        ~MVTrackingAction();
        
        void PreUserTrackingAction(const G4Track*) override;
        void PostUserTrackingAction(const G4Track*) override;
};


}

#endif