#ifndef MVRunAction_h
#define MVRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <vector>

class G4Run;

namespace MuonVeto
{

/// Run action class

class MVRunAction : public G4UserRunAction
{
    public:
        MVRunAction(G4int SiPMCount);
        ~MVRunAction();

        void BeginOfRunAction(const G4Run* run) override;
        void EndOfRunAction(const G4Run* run) override;
        void AddSiPMPhotonCount(const G4int* photonCount);
        void ClearSiPMPhotonCount();
    
    private:
        G4int fSiPMCount;
        std::vector<G4int>* fSiPMPhotonCount; 
};

}


#endif