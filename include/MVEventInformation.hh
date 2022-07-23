#ifndef MVEventInformation_h
#define MVEventInformation_h 1

#include "G4VUserEventInformation.hh"
#include "globals.hh"

namespace MuonVeto
{

class MVEventInformation : public G4VUserEventInformation
{
    public:
        MVEventInformation();
        ~MVEventInformation() override;
        inline std::map<G4String, G4int> GetSiPMPhotonCounter() { return fSiPMPhotonCounter; }
        inline std::map<G4String, G4int> GetCPNCounter() { return fCPNCounter; }
        inline std::map<G4String, G4int> GetFVPathCounter() { return fFVPathCounter; }
        inline std::map<G4String, G4int> GetEPNCounter() { return fEPNCounter; }
        inline void SetSiPMPhotonCounter(std::map<G4String, G4int> SiPMPhotonCounter) { fSiPMPhotonCounter = SiPMPhotonCounter; }
        inline void SetCPNCounter(std::map<G4String, G4int> CPNCounter) { fCPNCounter = CPNCounter; }
        inline void SetFVPathCounter(std::map<G4String, G4int> FVPathCounter) { fFVPathCounter = FVPathCounter; }
        inline void SetEPNCounter(std::map<G4String, G4int> EPNCounter) { fEPNCounter = EPNCounter; }
        void Print() const override;
    
    private:
        std::map<G4String, G4int> fSiPMPhotonCounter;
        std::map<G4String, G4int> fCPNCounter;
        std::map<G4String, G4int> fFVPathCounter;
        std::map<G4String, G4int> fEPNCounter;
};

}

#endif