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
}

MVRunMT::~MVRunMT()
{

}

void MVRunMT::RecordEvent(const G4Event* event)
{
    MVEventInformation* info = dynamic_cast<MVEventInformation*>(event->GetUserInformation());

    // These are variables in a single event. Those with "f" are the variables for a MVRunMT object,
    auto strList = info->GetStrList();
    SINGLE_COUNTER SiPMPhotonCounter;
    SINGLE_COUNTER CPNCounter;
    SINGLE_COUNTER FVPathCounter;
    SINGLE_COUNTER EPNCounter;

    const MVPrimaryGeneratorAction* generatorAction
        = dynamic_cast<const MVPrimaryGeneratorAction*>
            (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
    if(generatorAction)
    {
        // G4cout << "<<<< This is not a master thread" << G4endl;
        G4ParticleGun* particleGun = generatorAction->GetParticleGun();
        fParticlePosition.push_back(particleGun->GetParticlePosition());
    }

    // Update fStrList; Translate indices for strList into indices for fStrList
    for(auto it : info->GetSiPMPhotonCounter())
    {
        if(!IsStringInList(strList[it.first], fStrList))
            fStrList.push_back(strList[it.first]);

        SiPMPhotonCounter[GetIndexOfString(strList[it.first], fStrList)] = it.second;
    }

    for(auto it : info->GetCPNCounter())
    {
        if(!IsStringInList(strList[it.first], fStrList))
            fStrList.push_back(strList[it.first]);

        CPNCounter[GetIndexOfString(strList[it.first], fStrList)] = it.second;
    }

    for(auto it : info->GetFVPathCounter())
    {
        if(!IsStringInList(strList[it.first], fStrList))
            fStrList.push_back(strList[it.first]);

        FVPathCounter[GetIndexOfString(strList[it.first], fStrList)] = it.second;
    }

    for(auto it : info->GetEPNCounter())
    {
        if(!IsStringInList(strList[it.first], fStrList))
            fStrList.push_back(strList[it.first]);

        EPNCounter[GetIndexOfString(strList[it.first], fStrList)] = it.second;
    }

    fSiPMPhotonCounter.push_back(SiPMPhotonCounter);
    fCPNCounter.push_back(CPNCounter);
    fFVPathCounter.push_back(FVPathCounter);
    fEPNCounter.push_back(EPNCounter);

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
}

void MVRunMT::Merge(const G4Run* run)
{
    const MVRunMT* localRun = dynamic_cast<const MVRunMT*>(run);
    auto strList = localRun->GetStrList();

    fParticlePosition.insert(fParticlePosition.end(), localRun->fParticlePosition.begin(), localRun->fParticlePosition.end());

    for(auto singleEventMap : localRun->fSiPMPhotonCounter)
    {
        SINGLE_COUNTER newSingleEventMap;
        for(auto it : singleEventMap)
        {
            if(!IsStringInList(strList[it.first], fStrList))
                fStrList.push_back(strList[it.first]);
            newSingleEventMap[GetIndexOfString(strList[it.first], fStrList)] = it.second;
        }
        fSiPMPhotonCounter.push_back(newSingleEventMap);
    }
    
    for(auto singleEventMap : localRun->fCPNCounter)
    {
        SINGLE_COUNTER newSingleEventMap;
        for(auto it : singleEventMap)
        {
            if(!IsStringInList(strList[it.first], fStrList))
                fStrList.push_back(strList[it.first]);
            newSingleEventMap[GetIndexOfString(strList[it.first], fStrList)] = it.second;
        }
        fCPNCounter.push_back(newSingleEventMap);
    }

    for(auto singleEventMap : localRun->fFVPathCounter)
    {
        SINGLE_COUNTER newSingleEventMap;
        for(auto it : singleEventMap)
        {
            if(!IsStringInList(strList[it.first], fStrList))
                fStrList.push_back(strList[it.first]);
            newSingleEventMap[GetIndexOfString(strList[it.first], fStrList)] = it.second;
        }
        fFVPathCounter.push_back(newSingleEventMap);
    }

    for(auto singleEventMap : localRun->fEPNCounter)
    {
        SINGLE_COUNTER newSingleEventMap;
        for(auto it : singleEventMap)
        {
            if(!IsStringInList(strList[it.first], fStrList))
                fStrList.push_back(strList[it.first]);
            newSingleEventMap[GetIndexOfString(strList[it.first], fStrList)] = it.second;
        }
        fEPNCounter.push_back(newSingleEventMap);
    }

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

    if(fParticleEnergy != localRun->fParticleEnergy)    fParticleEnergy = localRun->fParticleEnergy;
    if(fParticleName != localRun->fParticleName)    fParticleName = localRun->fParticleName;

    G4Run::Merge(run);
}