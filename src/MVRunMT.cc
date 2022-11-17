#include "MVRunMT.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4VHitsCollection.hh"
#include "G4Event.hh"
#include "MVGenerator.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "MVEventInformation.hh"
#include <string>
#include <exception>
#include "G4SystemOfUnits.hh"

using namespace MuonVeto;

MVRunMT::MVRunMT():
    genInfos("MVGeneratorInformation", "Generator information"), 
    trackLengths("trackLength", "Track Length in whole_solid"), 
    Edeps("Edep", "Energy Deposition of primary particle")
{
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

    for(int counter_index = 0; counter_index < 4; counter_index++)
    {
        runCounters[counter_index].AppendSingle(info->counters[counter_index]);
    }
    genInfos.Update(info->genInfo);
    trackLengths.Update(info->trackLength);
    Edeps.Update(info->Edep);
    

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

    for(int counter_index = 0; counter_index < 4; counter_index++)
    {
        runCounters[counter_index].Merge(localRunCounters[counter_index]);
    }
    genInfos.Merge(localRun->genInfos);
    trackLengths.Merge(localRun->trackLengths);
    Edeps.Merge(localRun->Edeps);

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

    G4Run::Merge(run);
}