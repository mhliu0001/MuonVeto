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
        G4int eventCount = SiPMPhotonCount[0].size();
        auto CPNCounter = MTRun->GetCPNCounter();
        auto FVPathCounter = MTRun->GetFVPathCounter();
        auto EPNCounter = MTRun->GetEPNCounter();

        G4cout << "This run consists of " << SiPMPhotonCount[0].size() << " events of " << particleName <<
            " with energy " << particleEnergy/GeV << " GeV" << " located at " << particlePosition << G4endl; 

        std::vector<G4double> mean;
        std::vector<G4double> rms;

        for(int SiPMNb = 0; SiPMNb < fSiPMCount; SiPMNb++)
        {
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
                /*
                if (itr > fPhotonCountCut)
                {
                    G4cerr << "Event is cut because this event has " << itr << " photons in SiPM_" << SiPMNb;
                    eventCount -= 1;
                    continue;
                }
                */
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
        if(CPNCounter.size() != 0)
        {
            auto CPNMeanAndRMS = GetMeanAndRMSOfCounter(CPNCounter, eventCount);
            G4cout << ">>> Creator Process List: " << G4endl;
            for(auto item : CPNMeanAndRMS[0]) 
                G4cout << "    " << item.first << ": " << item.second << " +- " << 
                (CPNMeanAndRMS[1].find(item.first) != CPNMeanAndRMS[1].end() ? (*CPNMeanAndRMS[1].find(item.first)).second : -1) << G4endl;
            
            delete[] CPNMeanAndRMS;
        }

        if(FVPathCounter.size() != 0)
        {
            auto FVPathMeanAndRMS = GetMeanAndRMSOfCounter(FVPathCounter, eventCount);
            G4cout << ">>> Final Volume Path List: " << G4endl;
            for(auto item : FVPathMeanAndRMS[0]) 
                G4cout << "    " << item.first << ": " << item.second << " +- " << 
                (FVPathMeanAndRMS[1].find(item.first) != FVPathMeanAndRMS[1].end() ? (*FVPathMeanAndRMS[1].find(item.first)).second : -1) << G4endl;
            
            delete[] FVPathMeanAndRMS;
        }

        if(EPNCounter.size() != 0)
        {
            auto EPNMeanAndRMS = GetMeanAndRMSOfCounter(EPNCounter, eventCount);
            G4cout << ">>> Ending Process List: " << G4endl;
            for(auto item : EPNMeanAndRMS[0]) 
                G4cout << "    " << item.first << ": " << item.second << " +- " << 
                (EPNMeanAndRMS[1].find(item.first) != EPNMeanAndRMS[1].end() ? (*EPNMeanAndRMS[1].find(item.first)).second : -1) << G4endl;
            
            delete[] EPNMeanAndRMS;
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

std::map<G4String, double>* MVRunAction::GetMeanAndRMSOfCounter(std::vector<std::map<G4String, G4int>> counter, G4int eventCount) const
{
    auto meanAndRMS = new std::map<G4String, double>[2];
    std::map<G4String, long long> sum;
    std::map<G4String, long long> squaredSum;
    for (auto singleCounter : counter)
    {
        for (auto item : singleCounter)
        {
            if(sum.find(item.first) != sum.end())
            {
                sum[item.first] += item.second;
                squaredSum[item.first] += (long long)item.second*item.second;
            }
            else
            {
                sum[item.first] = item.second;
                squaredSum[item.first] = (long long)item.second*item.second;
            }
        }
    }
    for(auto item : sum)
    {
        meanAndRMS[0][item.first] = (double)item.second / eventCount;
    }
    for(auto item : squaredSum)
    {
        meanAndRMS[1][item.first] = std::sqrt(-meanAndRMS[0][item.first]*meanAndRMS[0][item.first] + (double)item.second/eventCount);
    }

    return meanAndRMS;
}

}