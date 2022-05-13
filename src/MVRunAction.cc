#include "MVRunAction.hh"
#include "G4Run.hh"
#include "MVRunMT.hh"
#include <numeric>
namespace MuonVeto
{

MVRunAction::MVRunAction(G4int SiPMCount): fSiPMCount(SiPMCount)
{

}

MVRunAction::~MVRunAction()
{

}

G4Run* MVRunAction::GenerateRun()
{
    return (new MVRunMT(fSiPMCount));
}

void MVRunAction::BeginOfRunAction(const G4Run*)
{

}

void MVRunAction::EndOfRunAction(const G4Run* aRun)
{
    if(isMaster)
    {
        G4cout << G4endl << "--------------------End of Global Run-----------------------" << G4endl;
        std::vector<G4int>* SiPMPhotonCount = ((MVRunMT*)aRun)->GetSiPMPhotonCount();
        for(int SiPMNb = 0; SiPMNb < fSiPMCount; SiPMNb++)
        {
            int eventCount = SiPMPhotonCount[SiPMNb].size();
            if (!eventCount)    break;

            std::vector<G4int> singleSiPMPhotonCount = SiPMPhotonCount[SiPMNb];

            int sum = std::accumulate(singleSiPMPhotonCount.begin(), singleSiPMPhotonCount.end(), 0);
            double mean = (double)sum / eventCount;
            G4cout << ">>> Mean photon number for SiPM " << SiPMNb << " is " << mean << G4endl;

            int squaredSum = std::accumulate(singleSiPMPhotonCount.begin(), singleSiPMPhotonCount.end(), 0, 
                [](int a, int b){return a+b*b;});
            double rms = std::sqrt(-mean*mean + (double)squaredSum/eventCount);
            G4cout << ">>> rms for SiPM " << SiPMNb << " is " << rms << G4endl;
        }
    }
    else
    {
        G4cout << G4endl << "--------------------End of Local Run-----------------------" <<  G4endl;
    }
}

}