#include "MVRunAction.hh"
#include "G4Run.hh"
#include "MVRunMT.hh"
#include <numeric>
#include "G4SystemOfUnits.hh"
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

        // Run Conditions
        G4double particleEnergy = ((MVRunMT*)aRun)->GetParticleEnergy();
        G4ThreeVector particlePosition = ((MVRunMT*)aRun)->GetParticlePosition();
        G4String particleName = ((MVRunMT*)aRun)->GetParticleName();
        std::vector<G4int>* SiPMPhotonCount = ((MVRunMT*)aRun)->GetSiPMPhotonCount();

        G4cout << "This run consists of " << SiPMPhotonCount[0].size() << " events of " << particleName <<
            " with energy " << particleEnergy/GeV << " GeV" << " located at " << particlePosition << G4endl; 

        std::vector<G4double> mean;
        std::vector<G4double> rms;
        for(int SiPMNb = 0; SiPMNb < fSiPMCount; SiPMNb++)
        {
            int eventCount = SiPMPhotonCount[SiPMNb].size();
            if (!eventCount)    break;

            std::vector<G4int> singleSiPMPhotonCount = SiPMPhotonCount[SiPMNb];

            int sum = std::accumulate(singleSiPMPhotonCount.begin(), singleSiPMPhotonCount.end(), 0);
            mean.push_back((double)sum / eventCount);
            G4cout << ">>> Mean photon number for SiPM " << SiPMNb << " is " << mean[SiPMNb] << G4endl;

            int squaredSum = std::accumulate(singleSiPMPhotonCount.begin(), singleSiPMPhotonCount.end(), 0, 
                [](int a, int b){return a+b*b;});
            rms.push_back(std::sqrt(-mean[SiPMNb]*mean[SiPMNb] + (double)squaredSum/eventCount));
            G4cout << ">>> rms for SiPM " << SiPMNb << " is " << rms[SiPMNb] << G4endl;
        }
        
        // Output for .csv
        G4cout << ">>>> " << particlePosition.z() << ",";
        for(int SiPMNb = 0; SiPMNb < fSiPMCount; SiPMNb++)
        {
            G4cout << mean[SiPMNb] << "," << rms[SiPMNb] << (SiPMNb==fSiPMCount-1? "" : ",");
        }
        G4cout << G4endl;
    }
    else
    {
        G4cout << G4endl << "--------------------End of Local Run-----------------------" <<  G4endl;
    }
}

}