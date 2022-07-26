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

MVRunMT::MVRunMT()
{
    const MVPrimaryGeneratorAction* generatorAction
        = dynamic_cast<const MVPrimaryGeneratorAction*>
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

}

void MVRunMT::RecordEvent(const G4Event* event)
{
    MVEventInformation* info = dynamic_cast<MVEventInformation*>(event->GetUserInformation());

    auto strList = info->GetStrList();
    SINGLE_COUNTER SiPMPhotonCounter;
    SINGLE_COUNTER CPNCounter;
    SINGLE_COUNTER FVPathCounter;
    SINGLE_COUNTER EPNCounter;

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
}

void MVRunMT::Merge(const G4Run* run)
{
    const MVRunMT* localRun = dynamic_cast<const MVRunMT*>(run);
    auto strList = localRun->GetStrList();

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

    if(fParticleEnergy != localRun->fParticleEnergy)    fParticleEnergy = localRun->fParticleEnergy;
    if(fParticlePosition != localRun->fParticlePosition)    fParticlePosition = localRun->fParticlePosition;
    if(fParticleName != localRun->fParticleName)    fParticleName = localRun->fParticleName;

    G4Run::Merge(run);
}