#include "MVEventAction.hh"

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
#include "G4RichTrajectory.hh"
#include "G4AttValue.hh"

MuonVeto::MVEventAction::MVEventAction()
{}

MuonVeto::MVEventAction::~MVEventAction()
{}

void MuonVeto::MVEventAction::BeginOfEventAction(const G4Event *)
{}

void MuonVeto::MVEventAction::EndOfEventAction(const G4Event *event)
{
    // get number of stored trajectories

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
    }

    G4int eventID = event->GetEventID();
    G4cout << ">>> Event: " << eventID << G4endl;
    if (trajectoryContainer)
    {
        G4cout << ">>> " << n_trajectories
               << " trajectories stored in this event." << G4endl;
        G4cout << ">>> Creator Process Name list: " << G4endl;
        for(auto it : CPNCounter)
        {
            G4cout << "    " << it.first << ": " << it.second << G4endl;
        }
        G4cout << ">>> Final Volume Path list: " << G4endl;
        for(auto it : FVPathCounter)
        {
            G4cout << "    " << it.first << ": " << it.second << G4endl;
        }
        G4cout << ">>> Ending Process Name list: " << G4endl;
        for(auto it : EPNCounter)
        {
            G4cout << "    " << it.first << ": " << it.second << G4endl;
        }
    }
    G4HCofThisEvent *hce = event->GetHCofThisEvent();
    G4SDManager *SDMan = G4SDManager::GetSDMpointer();

    // Get the photon count of each SiPM
    G4int SiPMCID_0 = SDMan->GetCollectionID("collection_0");
    G4int SiPMCID_1 = SDMan->GetCollectionID("collection_1");

    G4VHitsCollection *SiPMHC_0 = hce->GetHC(SiPMCID_0);
    G4cout << ">>> "
           << "SiPM_0: "
           << SiPMHC_0->GetSize() << " photons" << G4endl;
    G4VHitsCollection *SiPMHC_1 = hce->GetHC(SiPMCID_1);
    G4cout << ">>> "
           << "SiPM_1: "
           << SiPMHC_1->GetSize() << " photons" << G4endl;

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