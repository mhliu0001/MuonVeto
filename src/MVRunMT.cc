#include "MVRunMT.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4VHitsCollection.hh"
#include "G4Event.hh"
#include "MVPrimaryGeneratorAction.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "MVEventInformation.hh"
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

    MVEventInformation* info = dynamic_cast<MVEventInformation*>(event->GetUserInformation());
    fCPNCounter.push_back(info->GetCPNCounter());
    fFVPathCounter.push_back(info->GetFVPathCounter());
    fEPNCounter.push_back(info->GetEPNCounter());
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

    fCPNCounter.insert(
        fCPNCounter.end(),
        localRun->fCPNCounter.begin(),
        localRun->fCPNCounter.end()
    );

    fFVPathCounter.insert(
        fFVPathCounter.end(),
        localRun->fFVPathCounter.begin(),
        localRun->fFVPathCounter.end()
    );

    fEPNCounter.insert(
        fEPNCounter.end(),
        localRun->fEPNCounter.begin(),
        localRun->fEPNCounter.end()
    );

    if(fParticleEnergy != localRun->fParticleEnergy)    fParticleEnergy = localRun->fParticleEnergy;
    if(fParticlePosition != localRun->fParticlePosition)    fParticlePosition = localRun->fParticlePosition;
    if(fParticleName != localRun->fParticleName)    fParticleName = localRun->fParticleName;

    G4Run::Merge(run);
}