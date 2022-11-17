#ifndef MVSteppingAction_h
#define MVSteppingAction_h 1

#include "G4UserSteppingAction.hh"

#include "G4TouchableHandle.hh"

namespace MuonVeto
{

class MVEventAction;

class MVSteppingAction : public G4UserSteppingAction
{
    public:
        MVSteppingAction(MVEventAction*);
        ~MVSteppingAction();
        
        void UserSteppingAction(const G4Step*) override;
    
    private:
        MVEventAction* fEventAction;
        G4double fwhole_x;
        G4double fwhole_y;
        G4double fwhole_z;
        G4double kCarTolerance;
};

}

#endif