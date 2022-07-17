#include "MVRunAction.hh"
#include "G4Run.hh"
#include "MVRunMT.hh"
#include <numeric>
#include "G4SystemOfUnits.hh"
#include <string>
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
        MVRunMT* MTRun = (MVRunMT*)aRun;

        G4double particleEnergy = MTRun->GetParticleEnergy();
        G4ThreeVector particlePosition = MTRun->GetParticlePosition();
        G4String particleName = MTRun->GetParticleName();
        std::vector<G4int>* SiPMPhotonCount = MTRun->GetSiPMPhotonCount();

        G4cout << "This run consists of " << SiPMPhotonCount[0].size() << " events of " << particleName <<
            " with energy " << particleEnergy/GeV << " GeV" << " located at " << particlePosition << G4endl; 

        std::vector<G4double> mean;
        std::vector<G4double> rms;

        for(int SiPMNb = 0; SiPMNb < fSiPMCount; SiPMNb++)
        {
            int eventCount = SiPMPhotonCount[SiPMNb].size();
            if (!eventCount)    break;

            std::vector<G4int> singleSiPMPhotonCount = SiPMPhotonCount[SiPMNb];

            long long sum = 0;
            long long squaredSum = 0;

            char FileName[100];

            sprintf(FileName,"SiPM_%d",SiPMNb);
            FILE* pf=fopen(FileName,"a");
            if(!pf) G4cerr << "Open file failed! " << G4endl;

            for (auto itr : singleSiPMPhotonCount)
            {
                if (itr > photonCountCut)
                {
                    G4cerr << "Event is cut because this event has " << itr << " photons in SiPM_" << SiPMNb;
                    eventCount -= 1;
                    continue;
                }
                if(pf) fprintf(pf, "%f,%f,%f,%d\n", double(particlePosition.z()/cm), double(particlePosition.x()/cm), double(particleEnergy/GeV), itr);

                sum += ((long long) itr);
                squaredSum += ((long long) itr) * ((long long) itr);
                
            }
            if(pf) fclose(pf);

            mean.push_back((double)sum / eventCount);
            rms.push_back(std::sqrt(-mean[SiPMNb]*mean[SiPMNb] + (double)squaredSum/eventCount));
            G4cout << ">>> Mean photon number for SiPM " << SiPMNb << " is " << mean[SiPMNb] << G4endl;
            G4cout << ">>> rms for SiPM " << SiPMNb << " is " << rms[SiPMNb] << G4endl;
        }

        // Trajectory message
        if(MTRun->GetCPNCounter().size() != 0)
        {
            for (auto itr : MTRun->GetCPNCounter())
            {
                int eventCount = 0;
                long long sum = 0;
                long long squaredSum = 0;
                for (auto single : itr.second)
                {
                    ++eventCount;
                    sum += single;
                    squaredSum += single*single;
                }
                double mean = (double) sum / eventCount;
                double rms = std::sqrt(-mean*mean + (double)squaredSum/eventCount);
                G4cout << ">>> Creator Process Name " << itr.first << ": " << mean << "+-" << rms << G4endl;
            }
        }

        if(MTRun->GetFVPathCounter().size() != 0)
        {
            for (auto itr : MTRun->GetFVPathCounter())
            {
                int eventCount = 0;
                long long sum = 0;
                long long squaredSum = 0;
                for (auto single : itr.second)
                {
                    ++eventCount;
                    sum += single;
                    squaredSum += single*single;
                }
                double mean = (double) sum / eventCount;
                double rms = std::sqrt(-mean*mean + (double)squaredSum/eventCount);
                G4cout << ">>> Final Volume Path " << itr.first << ": " << mean << "+-" << rms << G4endl;
            }
        }

        if(MTRun->GetEPNCounter().size() != 0)
        {
            for (auto itr : MTRun->GetEPNCounter())
            {
                int eventCount = 0;
                long long sum = 0;
                long long squaredSum = 0;
                for (auto single : itr.second)
                {
                    ++eventCount;
                    sum += single;
                    squaredSum += single*single;
                }
                double mean = (double) sum / eventCount;
                double rms = std::sqrt(-mean*mean + (double)squaredSum/eventCount);
                G4cout << ">>> Ending Process Name " << itr.first << ": " << mean << "+-" << rms << G4endl;
            }
        }
        
        // Output for .csv

        G4int particleNb = -1;
        if(particleName == "mu-")   particleNb=0;
        if(particleName == "gamma")   particleNb=1;

        G4cout << ">>>> " << particlePosition.z() << "," << particlePosition.x() << "," << particleNb << "," << particleEnergy/GeV << ",";
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