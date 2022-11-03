#ifndef MVEventInformation_h
#define MVEventInformation_h 1

#include "G4VUserEventInformation.hh"
#include "MVGlobals.hh"

namespace MuonVeto
{

class MVEventInformation : public G4VUserEventInformation
{
    public:
        MVEventInformation();
        ~MVEventInformation() override;
        virtual void Print() const override;
        std::vector<MVSingleCounter<G4String>> counters;
        std::map<G4String, G4int> SiPMPhotonCounter;
        std::map<G4int, std::vector<G4double>> processSpectrum;
};

}

#endif