#include "MVTrackingAction.hh"
#include "G4Track.hh"
#include "G4RichTrajectory.hh"
#include "G4TrackingManager.hh"

using namespace MuonVeto;

MVTrackingAction::MVTrackingAction(){}

MVTrackingAction::~MVTrackingAction(){}

void MVTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
    G4RichTrajectory* aRichTrajectory = new G4RichTrajectory(aTrack);
    fpTrackingManager->SetTrajectory(aRichTrajectory);
}

void MVTrackingAction::PostUserTrackingAction(const G4Track* aTrack){}