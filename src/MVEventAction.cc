#include "MVEventAction.hh"

#include "MVRunAction.hh"
#include "MVEventInformation.hh"
#include "MVGenerator.hh"
#include "G4RunManager.hh"
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
#include <algorithm>

using namespace MuonVeto;

MVEventAction::MVEventAction(const Config& config) : fConfig(config)
{}

MVEventAction::~MVEventAction()
{}

void MVEventAction::BeginOfEventAction(const G4Event *)
{
    eventInformation = new MVEventInformation;
    fpEventManager->SetUserInformation(eventInformation);
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
    const MVGenerator *generator = dynamic_cast<const MVGenerator*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());

    MVEventInformation* info = dynamic_cast<MVEventInformation*> (event->GetUserInformation());
    for (int counter_index = 0; counter_index < 3; counter_index++)
        info->counters[counter_index].UpdateCounter();
    info->genInfo.constant.name = generator->GetCurrentParticleName();
    info->genInfo.constant.theta = generator->GetCurrentTheta();
    info->genInfo.constant.phi = generator->GetCurrentPhi();
    info->genInfo.constant.energy = generator->GetCurrentEnergy();
    info->genInfo.constant.position = generator->GetCurrentParticlePosition();
    info->genInfo.constant.momentumDir = generator->GetCurrentMomentumDirection();
    
    G4int SiPMCID_0 = SDMan->GetCollectionID("collection_0");
    G4int SiPMCID_1 = SDMan->GetCollectionID("collection_1");
    G4VHitsCollection *SiPMHC_0 = hce->GetHC(SiPMCID_0);
    G4VHitsCollection *SiPMHC_1 = hce->GetHC(SiPMCID_1);
    std::map<G4String, G4int> SiPMPhotonCounter;
    SiPMPhotonCounter["SiPM_0"] = SiPMHC_0->GetSize();
    SiPMPhotonCounter["SiPM_1"] = SiPMHC_1->GetSize();
    info->counters[3].UpdateCounter(SiPMPhotonCounter);
    info->Print();

    // Spectrum analysis
    /*
    if(fConfig.spectrumAnalysis)
    {
        std::map<G4int, std::vector<G4double>> processSpectrum;
        for(auto singlePhoton : fEnergyRecorder)
        {
            if(processSpectrum.find(GetIndexOfString(fCPNRecorder[singlePhoton.first], fStrList)) == processSpectrum.end())
            {
                std::vector<G4double> singleProcessSpectrum = {singlePhoton.second};
                processSpectrum.insert(std::make_pair(GetIndexOfString(fCPNRecorder[singlePhoton.first], fStrList), singleProcessSpectrum));
            }
            else
            {
                processSpectrum[GetIndexOfString(fCPNRecorder[singlePhoton.first], fStrList)].push_back(singlePhoton.second);
            }
        }
        info->processSpectrum = processSpectrum;
        fEnergyRecorder.clear();
    }
    */

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