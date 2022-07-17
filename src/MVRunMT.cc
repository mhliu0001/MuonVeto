#include "MVRunMT.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4VHitsCollection.hh"
#include "G4Event.hh"
#include "MVPrimaryGeneratorAction.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4RichTrajectory.hh"
#include "G4AttValue.hh"
#include <string>

using namespace MuonVeto;

MVRunMT::MVRunMT(G4int SiPMCount): fSiPMCount(SiPMCount)
{
    std::vector<G4int>* SiPMPhotonCount = new std::vector<G4int>[SiPMCount];
    fSiPMPhotonCount = SiPMPhotonCount;

    const MVPrimaryGeneratorAction* generatorAction
        = static_cast<const MVPrimaryGeneratorAction*>
            (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
    if(generatorAction)
    {
        // G4cout << "<<<< This is not a master thread" << G4endl;
        G4ParticleGun* particleGun = generatorAction->GetParticleGun();
        fParticleEnergy = particleGun->GetParticleEnergy();
        fParticlePosition = particleGun->GetParticlePosition();
        fParticleName = particleGun->GetParticleDefinition()->GetParticleName();
    }

}

MVRunMT::~MVRunMT()
{
    delete[] fSiPMPhotonCount;
}

void MVRunMT::RecordEvent(const G4Event* event)
{
    G4HCofThisEvent* hce = event->GetHCofThisEvent();
    G4SDManager* SDMan = G4SDManager::GetSDMpointer();

    // Get the photon count of each SiPM
    for (int SiPMNb = 0; SiPMNb < fSiPMCount; SiPMNb++)
    {
        G4int SiPMCID = SDMan->GetCollectionID("collection_"+std::to_string(SiPMNb));
        G4VHitsCollection* SiPMHC = hce->GetHC(SiPMCID);
        fSiPMPhotonCount[SiPMNb].push_back(SiPMHC->GetSize() ? SiPMHC->GetSize() : 0);
    }

    G4TrajectoryContainer *trajectoryContainer = event->GetTrajectoryContainer();
    G4int n_trajectories = 0;
    std::map<G4String, G4int> CPNCounter; // Creator Process Name
    std::map<G4String, G4int> FVPathCounter; // Final Volume Path
    std::map<G4String, G4int> EPNCounter; // Ending Process Name
    if (trajectoryContainer)
    {
        n_trajectories = trajectoryContainer->entries();
        for (int i = 0; i < n_trajectories; ++i)
        {
            G4RichTrajectory *single_trajectory = static_cast<G4RichTrajectory *>((*trajectoryContainer)[i]);
            if(single_trajectory->GetParticleName() != "opticalphoton") continue;

            auto attValues = single_trajectory->CreateAttValues();
            for(auto single_attValue : *attValues)
            {
                if(single_attValue.GetName() == G4String("CPN"))
                {
                    auto iter = CPNCounter.find(single_attValue.GetValue());
                    if(iter == CPNCounter.end()) CPNCounter[single_attValue.GetValue()] = 1;
                    else ++CPNCounter[single_attValue.GetValue()];
                }
                if(single_attValue.GetName() == G4String("FVPath"))
                {
                    auto iter = FVPathCounter.find(single_attValue.GetValue());
                    if(iter == FVPathCounter.end()) FVPathCounter[single_attValue.GetValue()] = 1;
                    else ++FVPathCounter[single_attValue.GetValue()];
                }
                if(single_attValue.GetName() == G4String("EPN"))
                {
                    auto iter = EPNCounter.find(single_attValue.GetValue());
                    if(iter == EPNCounter.end()) EPNCounter[single_attValue.GetValue()] = 1;
                    else ++EPNCounter[single_attValue.GetValue()];
                }
            }
        }
        for(auto itr : CPNCounter) fCPNCounter[itr.first].push_back(itr.second);
        for(auto itr : FVPathCounter) fFVPathCounter[itr.first].push_back(itr.second);
        for(auto itr : EPNCounter) fEPNCounter[itr.first].push_back(itr.second);

    }
}

void MVRunMT::Merge(const G4Run* run)
{
    const MVRunMT* localRun = dynamic_cast<const MVRunMT*>(run);
    for (int SiPMNb = 0; SiPMNb < fSiPMCount; SiPMNb++)
    {
        fSiPMPhotonCount[SiPMNb].insert(
            fSiPMPhotonCount[SiPMNb].end(),
            localRun->fSiPMPhotonCount[SiPMNb].begin(),
            localRun->fSiPMPhotonCount[SiPMNb].end()
        );
    }

    for(auto it : localRun->fCPNCounter)
    {
        fCPNCounter[it.first].insert(
            fCPNCounter[it.first].end(),
            it.second.begin(),
            it.second.end()
        );
    }

    for(auto it : localRun->fFVPathCounter)
    {
        fFVPathCounter[it.first].insert(
            fFVPathCounter[it.first].end(),
            it.second.begin(),
            it.second.end()
        );
    }

    for(auto it : localRun->fEPNCounter)
    {
        fEPNCounter[it.first].insert(
            fEPNCounter[it.first].end(),
            it.second.begin(),
            it.second.end()
        );
    }

    if(fParticleEnergy != localRun->fParticleEnergy)    fParticleEnergy = localRun->fParticleEnergy;
    if(fParticlePosition != localRun->fParticlePosition)    fParticlePosition = localRun->fParticlePosition;
    if(fParticleName != localRun->fParticleName)    fParticleName = localRun->fParticleName;

    G4Run::Merge(run);
}