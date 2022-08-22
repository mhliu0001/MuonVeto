#ifndef MVRunAction_h
#define MVRunAction_h 1

#include "G4UserRunAction.hh"
#include "MVGlobals.hh"

class G4Run;

namespace MuonVeto
{

/// Run action class

class MVRunAction : public G4UserRunAction
{
    public:
        MVRunAction(const Config& config);
        ~MVRunAction();

        G4Run* GenerateRun() override;
        void BeginOfRunAction(const G4Run* run) override;
        void EndOfRunAction(const G4Run* run) override;
    
    private:
        const Config fConfig;
        // G4int fPhotonCountCut = 100000;
        std::map<G4int, G4double>* GetMeanAndRMSOfCounter(COUNTER counter, G4int eventCount) const;

};

}


#endif