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
#include <exception>
#include "G4SystemOfUnits.hh"

using namespace MuonVeto;

MVRunMT::MVRunMT()
{
    // Record information about the generator which does not vary in the run
    // Currently the particle name and the energy is recorded here.
    // TODO: Fix generator information
    fParticleEnergy = 0;
    fParticleName = "FIXME";
    /*
    const MVPrimaryGeneratorAction* generatorAction
        = dynamic_cast<const MVPrimaryGeneratorAction*>
            (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
    if(generatorAction)
    {
        // G4cout << "<<<< This is not a master thread" << G4endl;
        G4ParticleGun* particleGun = generatorAction->GetParticleGun();
        fParticleEnergy = particleGun->GetParticleEnergy();
        fParticleName = particleGun->GetParticleDefinition()->GetParticleName();
    }
    */

    // Initialize runCounters
    runCounters.push_back(MVGlobalCounter<G4String>("CPNCounter", "Creator Process Name list"));
    runCounters.push_back(MVGlobalCounter<G4String>("FVPathCounter", "Final Volume Path list"));
    runCounters.push_back(MVGlobalCounter<G4String>("EPNCounter", "Ending Process Name list"));
    runCounters.push_back(MVGlobalCounter<G4String>("SiPMPhotonCounter", "SiPM Photon Count"));
}

MVRunMT::~MVRunMT()
{

}

void MVRunMT::RecordEvent(const G4Event* event)
{
    MVEventInformation* info = dynamic_cast<MVEventInformation*>(event->GetUserInformation());

    // These are variables in a single event. Those with "f" are the variables for a MVRunMT object,

    const MVPrimaryGeneratorAction* generatorAction
        = dynamic_cast<const MVPrimaryGeneratorAction*>
            (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
    if(generatorAction)
    {
        // G4cout << "<<<< This is not a master thread" << G4endl;
        G4ParticleGun* particleGun = generatorAction->GetParticleGun();
        fParticlePosition.push_back(particleGun->GetParticlePosition());
    }
    else
    {
        fParticlePosition.push_back(G4ThreeVector(0,0,0));
    }
    for(int counter_index = 0; counter_index < 4; counter_index++)
    {
        runCounters[counter_index].AppendSingle(info->counters[counter_index]);
    }

    /*
    // Spectrum
    for(auto singleProcess : info->GetProcessSpectrum())
    {
        if(!IsStringInList(strList[singleProcess.first], fStrList))
            fStrList.push_back(strList[singleProcess.first]);

        std::vector<G4double> singleProcessSpectrum;
        if(fProcessSpectrum.find(GetIndexOfString(strList[singleProcess.first], fStrList)) != fProcessSpectrum.end())
            singleProcessSpectrum = fProcessSpectrum[GetIndexOfString(strList[singleProcess.first], fStrList)];
        for(auto energy : singleProcess.second)
            singleProcessSpectrum.push_back(energy);

        fProcessSpectrum.insert_or_assign(GetIndexOfString(strList[singleProcess.first], fStrList), singleProcessSpectrum);
    }
    */
}

void MVRunMT::Merge(const G4Run* run)
{
    const MVRunMT* localRun = dynamic_cast<const MVRunMT*>(run);
    auto localRunCounters = localRun->runCounters;

    fParticlePosition.insert(fParticlePosition.end(), localRun->fParticlePosition.begin(), localRun->fParticlePosition.end());

    for(int counter_index = 0; counter_index < 4; counter_index++)
    {
        runCounters[counter_index].Merge(localRunCounters[counter_index]);
    }
   

    /*
    for(auto singleProcess : localRun->fProcessSpectrum)
    {
        if(!IsStringInList(strList[singleProcess.first], fStrList))
            fStrList.push_back(strList[singleProcess.first]);
        
        std::vector<G4double> singleProcessSpectrum;
        if(fProcessSpectrum.find(GetIndexOfString(strList[singleProcess.first], fStrList)) != fProcessSpectrum.end())
            singleProcessSpectrum = fProcessSpectrum[GetIndexOfString(strList[singleProcess.first], fStrList)];
        for(auto energy : singleProcess.second)
            singleProcessSpectrum.push_back(energy);

        fProcessSpectrum.insert_or_assign(GetIndexOfString(strList[singleProcess.first], fStrList), singleProcessSpectrum);
    }
    */

    if(fParticleEnergy != localRun->fParticleEnergy)    fParticleEnergy = localRun->fParticleEnergy;
    if(fParticleName != localRun->fParticleName)    fParticleName = localRun->fParticleName;

    G4Run::Merge(run);
}