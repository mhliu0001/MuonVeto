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
        std::vector<MVGlobalCounter<G4String>> runCounters;
        inline std::map<G4int, std::vector<G4double>> GetProcessSpectrum() const { return fProcessSpectrum; }
        inline std::vector<G4ThreeVector> GetParticlePosition() const { return fParticlePosition; }
        inline G4double GetParticleEnergy() const { return fParticleEnergy; }
        G4String GetParticleName() const { return fParticleName; }

    private:
        std::map<G4int, std::vector<G4double>> fProcessSpectrum;
        std::vector<G4ThreeVector> fParticlePosition;
        G4double fParticleEnergy = 0;
        G4String fParticleName;
};

}

#endif