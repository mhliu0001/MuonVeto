#include "MVSteppingAction.hh"
#include "MVEventAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4VProcess.hh"
#include "G4String.hh"
#include "MVGlobals.hh"
#include "G4SolidStore.hh"
#include "G4Box.hh"
#include "G4GeometryTolerance.hh"

using namespace MuonVeto;

MVSteppingAction::MVSteppingAction(MVEventAction* eventAction): fEventAction(eventAction)
{
    auto solidStore = G4SolidStore::GetInstance();
    G4Box* whole_solid = dynamic_cast<G4Box*>(solidStore->GetSolid("whole_solid"));
    if(!whole_solid)
    {
        throw std::runtime_error("MVSteppingAction: whole_solid not found!");
    }
    fwhole_x = whole_solid->GetXHalfLength();
    fwhole_y = whole_solid->GetYHalfLength();
    fwhole_z = whole_solid->GetZHalfLength();
    kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
}

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
    G4String particleName = track->GetParticleDefinition()->GetParticleName();
    const G4int trackID = track->GetTrackID();
    if(trackID == 1)
    {
        G4ThreeVector preStepPos = aStep->GetPreStepPoint()->GetPosition();
        G4ThreeVector postStepPos = aStep->GetPostStepPoint()->GetPosition();
        if(
               abs(preStepPos.x())-fwhole_x < kCarTolerance
            && abs(preStepPos.y())-fwhole_y < kCarTolerance
            && abs(preStepPos.z())-fwhole_z < kCarTolerance
            && abs(postStepPos.x())-fwhole_x < kCarTolerance
            && abs(postStepPos.y())-fwhole_y < kCarTolerance
            && abs(postStepPos.z())-fwhole_z < kCarTolerance
        )
        {
            fEventAction->eventInformation->trackLength.constant += track->GetStepLength();
        }
    }

    G4double edepStep = aStep->GetTotalEnergyDeposit();
    if(edepStep > 0)
    {
        fEventAction->eventInformation->Edep.constant += edepStep;
    }

    if(track->GetParticleDefinition()->GetParticleName() != "opticalphoton")    return;
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

    fEventAction->eventInformation->counters[0].UpdateRecorder(trackID, CPN);
    fEventAction->eventInformation->counters[1].UpdateRecorder(trackID, FVPath);
    fEventAction->eventInformation->counters[2].UpdateRecorder(trackID, EPN);

    if(fEventAction->fConfig.spectrumAnalysis)
        fEventAction->fEnergyRecorder[trackID] = track->GetTotalEnergy();
    /*
    if(!IsStringInList(CPN, fEventAction->fStrList))   fEventAction->fStrList.push_back(CPN);
    if(!IsStringInList(FVPath, fEventAction->fStrList))    fEventAction->fStrList.push_back(FVPath);
    if(!IsStringInList(EPN, fEventAction->fStrList))    fEventAction->fStrList.push_back(EPN);

    fEventAction->fCPNRecorder[trackID] = GetIndexOfString(CPN, fEventAction->fStrList);
    fEventAction->fFVPathRecorder[trackID] = GetIndexOfString(FVPath, fEventAction->fStrList);
    fEventAction->fEPNRecorder[trackID] = GetIndexOfString(EPN, fEventAction->fStrList);
    */
}