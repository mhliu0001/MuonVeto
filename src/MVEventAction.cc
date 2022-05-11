#include "MVEventAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"
#include "G4SDManager.hh"
#include "G4THitsMap.hh"

MuonVeto::MVEventAction::MVEventAction()
{}

MuonVeto::MVEventAction::~MVEventAction()
{}

void MuonVeto::MVEventAction::BeginOfEventAction(const G4Event*)
{}

void MuonVeto::MVEventAction::EndOfEventAction(const G4Event* event)
{
    // get number of stored trajectories

    G4TrajectoryContainer* trajectoryContainer = event->GetTrajectoryContainer();
    G4int n_trajectories = 0;
    if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

    G4int eventID = event->GetEventID();
    G4cout << ">>> Event: " << eventID  << G4endl;
    if ( trajectoryContainer ) 
    {
        G4cout << "    " << n_trajectories
            << " trajectories stored in this event." << G4endl;
    }
    G4HCofThisEvent* hce = event->GetHCofThisEvent();
    G4SDManager* SDMan = G4SDManager::GetSDMpointer();
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
        G4cout << "SiPM_0 with copyNb " << copyNb << ": " << count << " photons " << G4endl;
    }
    for (auto itr = SiPM_1_hm->GetMap()->begin(); itr != SiPM_1_hm->GetMap()->end(); itr++) 
    {
        G4int copyNb  = (itr->first);
        G4int count = *(itr->second);
        G4cout << "SiPM_1 with copyNb " << copyNb << ": " << count << " photons " << G4endl;
    }
}