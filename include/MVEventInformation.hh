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
        inline SINGLE_COUNTER GetSiPMPhotonCounter() { return fSiPMPhotonCounter; }
        inline SINGLE_COUNTER GetCPNCounter() { return fCPNCounter; }
        inline SINGLE_COUNTER GetFVPathCounter() { return fFVPathCounter; }
        inline SINGLE_COUNTER GetEPNCounter() { return fEPNCounter; }
        inline std::map<G4int, std::vector<G4double>> GetProcessSpectrum() { return fProcessSpectrum; }
        inline STRLIST GetStrList() { return fStrList; }
        inline void SetSiPMPhotonCounter(SINGLE_COUNTER SiPMPhotonCounter) { fSiPMPhotonCounter = SiPMPhotonCounter; }
        inline void SetCPNCounter(SINGLE_COUNTER CPNCounter) { fCPNCounter = CPNCounter; }
        inline void SetFVPathCounter(SINGLE_COUNTER FVPathCounter) { fFVPathCounter = FVPathCounter; }
        inline void SetEPNCounter(SINGLE_COUNTER EPNCounter) { fEPNCounter = EPNCounter; }
        inline void SetProcessSpectrum(std::map<G4int, std::vector<G4double>> processSpectrum) { fProcessSpectrum = processSpectrum; }
        inline void SetStrList(STRLIST strList) { fStrList = strList; }
    
    private:
        SINGLE_COUNTER fSiPMPhotonCounter;
        SINGLE_COUNTER fCPNCounter;
        SINGLE_COUNTER fFVPathCounter;
        SINGLE_COUNTER fEPNCounter;
        std::map<G4int, std::vector<G4double>> fProcessSpectrum;
        STRLIST fStrList;
};

}

#endif