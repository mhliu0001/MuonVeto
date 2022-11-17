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
        MVEventConstantVector<MVGeneratorInformation> genInfos;
        MVEventConstantVector<G4double> trackLengths;
        MVEventConstantVector<G4double> Edeps;
        inline std::map<G4int, std::vector<G4double>> GetProcessSpectrum() const { return fProcessSpectrum; }

    private:
        std::map<G4int, std::vector<G4double>> fProcessSpectrum;

};

}

#endif