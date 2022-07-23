#ifndef MVRunMT_h
#define MVRunMT_h 1

#include "G4Run.hh"
#include <vector>
#include "G4ThreeVector.hh"

namespace MuonVeto
{

class MVRunMT : public G4Run
{
    public:
        MVRunMT(G4int SiPMCount);
        ~MVRunMT() override;
        void RecordEvent(const G4Event*) override;
        void Merge(const G4Run*) override;
        inline std::vector<G4int>* GetSiPMPhotonCount() const { return fSiPMPhotonCount; }
        inline std::vector<std::map<G4String, G4int>> GetCPNCounter() const { return fCPNCounter; }
        inline std::vector<std::map<G4String, G4int>> GetFVPathCounter() const { return fFVPathCounter; }
        inline std::vector<std::map<G4String, G4int>> GetEPNCounter() const { return fEPNCounter; }
        inline G4ThreeVector GetParticlePosition() const { return fParticlePosition; }
        inline G4double GetParticleEnergy() const { return fParticleEnergy; }
        G4String GetParticleName() const { return fParticleName; }

    private:
        G4int fSiPMCount;
        std::vector<G4int>* fSiPMPhotonCount;
        std::vector<std::map<G4String, G4int>> fCPNCounter;
        std::vector<std::map<G4String, G4int>> fFVPathCounter;
        std::vector<std::map<G4String, G4int>> fEPNCounter;
        G4ThreeVector fParticlePosition;
        G4double fParticleEnergy = 0;
        G4String fParticleName;
};

}

#endif