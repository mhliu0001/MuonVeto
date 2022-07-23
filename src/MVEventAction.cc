#include "MVEventAction.hh"

#include "MVRunAction.hh"
#include "MVEventInformation.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"
#include "G4SDManager.hh"
#include "G4THitsMap.hh"
#include "G4GeometryTolerance.hh"
#include "G4UnitsTable.hh"
#include "SiPMPhotonAccumulator.hh"
#include "G4UImanager.hh"
#include "G4AttValue.hh"

using namespace MuonVeto;

MVEventAction::MVEventAction()
{}

MVEventAction::~MVEventAction()
{}

void MVEventAction::BeginOfEventAction(const G4Event *)
{
    fpEventManager->SetUserInformation(new MVEventInformation);
}

void MVEventAction::EndOfEventAction(const G4Event *event)
{
    
    // get number of stored trajectories

    G4TrajectoryContainer *trajectoryContainer = event->GetTrajectoryContainer();
    G4int n_trajectories = 0;
    if (trajectoryContainer)
    {
        n_trajectories = trajectoryContainer->entries();
    }

    G4int eventID = event->GetEventID();
    G4cout << ">>> Event: " << eventID << G4endl;
    if (trajectoryContainer)
    {
        G4cout << ">>> " << n_trajectories
               << " trajectories stored in this event." << G4endl;
    }
    
    G4HCofThisEvent *hce = event->GetHCofThisEvent();
    G4SDManager *SDMan = G4SDManager::GetSDMpointer();

    G4int SiPMCID_0 = SDMan->GetCollectionID("collection_0");
    G4int SiPMCID_1 = SDMan->GetCollectionID("collection_1");
    G4VHitsCollection *SiPMHC_0 = hce->GetHC(SiPMCID_0);
    G4VHitsCollection *SiPMHC_1 = hce->GetHC(SiPMCID_1);
    std::map<G4String, G4int> SiPMPhotonCounter;
    SiPMPhotonCounter["SiPM_0"] = SiPMHC_0->GetSize();
    SiPMPhotonCounter["SiPM_1"] = SiPMHC_1->GetSize();

    std::map<G4String, G4int> CPNCounter;
    std::map<G4String, G4int> FVPathCounter;
    std::map<G4String, G4int> EPNCounter;

    for(auto track : fCPNRecorder)
    {
        if(CPNCounter.find(track.second) != CPNCounter.end())   ++CPNCounter[track.second];
        else CPNCounter[track.second] = 1; 
    }
    for(auto track : fFVPathRecorder)
    {
        if(FVPathCounter.find(track.second) != FVPathCounter.end())   ++FVPathCounter[track.second];
        else FVPathCounter[track.second] = 1; 
    }
    for(auto track : fEPNRecorder)
    {
        if(EPNCounter.find(track.second) != EPNCounter.end())   ++EPNCounter[track.second];
        else EPNCounter[track.second] = 1;
    }

    MVEventInformation* info = dynamic_cast<MVEventInformation*> (event->GetUserInformation());
    info->SetSiPMPhotonCounter(SiPMPhotonCounter);
    info->SetCPNCounter(CPNCounter);
    info->SetFVPathCounter(FVPathCounter);
    info->SetEPNCounter(EPNCounter);
    info->Print();

    /*
    G4int SiPM_0_id = SDMan->GetCollectionID("SiPM_0/photon_counter_0");
    G4int SiPM_1_id = SDMan->GetCollectionID("SiPM_1/photon_counter_1");

    G4THitsMap<G4int>* SiPM_0_hm =
        (G4THitsMap<G4int>*)(hce->GetHC(SiPM_0_id));
    G4THitsMap<G4int>* SiPM_1_hm =
        (G4THitsMap<G4int>*)(hce->GetHC(SiPM_1_id));
    if(!SiPM_0_hm || !SiPM_1_hm)
    {
        G4cout << "    " << "HitsMap not found!" << G4endl;
        return;
    }
    for (auto itr = SiPM_0_hm->GetMap()->begin(); itr != SiPM_0_hm->GetMap()->end(); itr++)
    {
        G4int copyNb  = (itr->first);
        G4int count = *(itr->second);
        G4cout << "    " << "SiPM_0 with copyNb " << copyNb << ": " << count << " photons " << G4endl;
    }
    for (auto itr = SiPM_1_hm->GetMap()->begin(); itr != SiPM_1_hm->GetMap()->end(); itr++)
    {
        G4int copyNb  = (itr->first);
        G4int count = *(itr->second);
        G4cout << "    " << "SiPM_1 with copyNb " << copyNb << ": " << count << " photons " << G4endl;
    }
    */
}