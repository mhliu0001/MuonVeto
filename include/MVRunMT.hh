#ifndef MVRunMT_h
#define MVRunMT_h 1

#include "G4Run.hh"
#include <vector>
#include "G4ThreeVector.hh"
#include "MVGlobals.hh"

namespace MuonVeto
{

class MVRunMT : public G4Run
{
    public:
        MVRunMT();
        ~MVRunMT() override;
        void RecordEvent(const G4Event*) override;
        void Merge(const G4Run*) override;
        inline COUNTER GetSiPMPhotonCounter() const { return fSiPMPhotonCounter; }
        inline COUNTER GetCPNCounter() const { return fCPNCounter; }
        inline COUNTER GetFVPathCounter() const { return fFVPathCounter; }
        inline COUNTER GetEPNCounter() const { return fEPNCounter; }
        inline STRLIST GetStrList() const { return fStrList; }
        inline G4ThreeVector GetParticlePosition() const { return fParticlePosition; }
        inline G4double GetParticleEnergy() const { return fParticleEnergy; }
        G4String GetParticleName() const { return fParticleName; }

    private:
        COUNTER fSiPMPhotonCounter;
        COUNTER fCPNCounter;
        COUNTER fFVPathCounter;
        COUNTER fEPNCounter;
        STRLIST fStrList;
        G4ThreeVector fParticlePosition;
        G4double fParticleEnergy = 0;
        G4String fParticleName;
};

}

#endif