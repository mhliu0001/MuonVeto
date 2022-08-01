#include "MVSteppingAction.hh"
#include "MVEventAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4VProcess.hh"
#include "G4String.hh"
#include "MVGlobals.hh"

using namespace MuonVeto;

MVSteppingAction::MVSteppingAction(MVEventAction* eventAction): fEventAction(eventAction){}

MVSteppingAction::~MVSteppingAction(){}

static G4String Path(const G4TouchableHandle& th)
{
    std::ostringstream oss;
    /*
    G4int depth = th->GetHistoryDepth();
    for (G4int i = depth; i >= 0; --i)
    {
        oss << th->GetVolume(i)->GetName()
            << ':' << th->GetCopyNumber(i);
        if (i != 0) oss << '/';
    }
    */
    oss << th->GetVolume(0)->GetName() << ':' << th->GetCopyNumber(0);
    return oss.str();
}

void MVSteppingAction::UserSteppingAction(const G4Step* aStep)
{
    const G4Track* track = aStep->GetTrack();
    if(track->GetParticleDefinition()->GetParticleName() != "opticalphoton")    return;
    const G4int trackID = track->GetTrackID();
    const G4VProcess* CP = track->GetCreatorProcess();
    const G4String CPN = (!CP ? "None": CP->GetProcessName());
    G4String FVPath;
    G4String EPN;
    const G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
    if (track->GetCurrentStepNumber() > 0)
    {
        FVPath = Path(track->GetTouchableHandle());
        const G4VProcess* PDS = postStepPoint->GetProcessDefinedStep();
        EPN = (!PDS ? "None" : PDS->GetProcessName());
    }

    fEventAction->fCPNRecorder[trackID] = CPN;
    fEventAction->fFVPathRecorder[trackID] = FVPath;
    fEventAction->fEPNRecorder[trackID] = EPN;

    /*
    if(!IsStringInList(CPN, fEventAction->fStrList))   fEventAction->fStrList.push_back(CPN);
    if(!IsStringInList(FVPath, fEventAction->fStrList))    fEventAction->fStrList.push_back(FVPath);
    if(!IsStringInList(EPN, fEventAction->fStrList))    fEventAction->fStrList.push_back(EPN);

    fEventAction->fCPNRecorder[trackID] = GetIndexOfString(CPN, fEventAction->fStrList);
    fEventAction->fFVPathRecorder[trackID] = GetIndexOfString(FVPath, fEventAction->fStrList);
    fEventAction->fEPNRecorder[trackID] = GetIndexOfString(EPN, fEventAction->fStrList);
    */
}