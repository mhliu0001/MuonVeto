#ifndef MVRunMT_h
#define MVRunMT_h 1

#include "G4Run.hh"
#include <vector>

namespace MuonVeto
{

class MVRunMT : public G4Run
{
    public:
        MVRunMT(G4int SiPMCount);
        ~MVRunMT() override;
        void RecordEvent(const G4Event*) override;
        void Merge(const G4Run*) override;
        void MergeSiPMPhotonCount();
        inline std::vector<G4int>* GetSiPMPhotonCount(){ return fSiPMPhotonCount; };
    
    private:
        G4int fSiPMCount;
        std::vector<G4int>* fSiPMPhotonCount;
};

}

#endif