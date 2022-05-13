#include "SiPMSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4Box.hh"
#include "G4GeometryTolerance.hh"

namespace MuonVeto
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SiPMSD::SiPMSD(const G4String& name, const G4String& hitsCollectionName, const SiPM_surface surface)
    : G4VSensitiveDetector(name), fSurface(surface)
{
    collectionName.insert(hitsCollectionName);
}

SiPMSD::~SiPMSD()
{}

void SiPMSD::Initialize(G4HCofThisEvent* hce)
{
    // Create hits collection

    fHitsCollection
        = new SiPMHitsCollection(SensitiveDetectorName, collectionName[0]);

    // Add this collection in hce

    G4int hcID
        = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection( hcID, fHitsCollection );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool SiPMSD::ProcessHits(G4Step* aStep,
                                     G4TouchableHistory*)
{
    G4StepPoint* preStep = aStep->GetPreStepPoint();
    G4VPhysicalVolume* physVol = preStep->GetPhysicalVolume();
    G4VSolid* solid = physVol->GetLogicalVolume()->GetSolid();
    G4Box* SiPM_solid = dynamic_cast<G4Box*>(solid);

    if (!IsSelectedSurface(aStep, SiPM_solid)) return false;

    SiPMHit* newHit = new SiPMHit();

    newHit->SetTrackID  (aStep->GetTrack()->GetTrackID());
    newHit->SetSiPMNb(aStep->GetPreStepPoint()->GetTouchableHandle()
                                                ->GetCopyNumber());
    newHit->SetPos (aStep->GetPreStepPoint()->GetPosition());

    fHitsCollection->insert( newHit );

    //newHit->Print();

    return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SiPMSD::EndOfEvent(G4HCofThisEvent*)
{
    if ( verboseLevel>1 ) {
        G4int nofHits = fHitsCollection->entries();
        G4cout << G4endl
                << "-------->Hits Collection: in this event they are " << nofHits
                << " hits in the tracker chambers: " << G4endl;
        for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
    }
}

G4bool SiPMSD::IsSelectedSurface(G4Step* aStep, G4Box* SiPM_solid)
{
    G4TouchableHandle theTouchable = 
        aStep->GetPreStepPoint()->GetTouchableHandle();
    G4double kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();

    if (aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary ){
        // Entering Geometry
        G4ThreeVector preStepPos = aStep->GetPreStepPoint()->GetPosition();
        G4ThreeVector postStepPos = aStep->GetPostStepPoint()->GetPosition();

        G4ThreeVector preStepLocalPos = 
            theTouchable->GetHistory()->GetTopTransform().TransformPoint(preStepPos);
        G4ThreeVector postStepLocalPos = 
            theTouchable->GetHistory()->GetTopTransform().TransformPoint(postStepPos);

        switch (fSurface)
        {
        case XY_minus:
            if(std::fabs( preStepLocalPos.z() + SiPM_solid->GetZHalfLength())<kCarTolerance 
                && postStepLocalPos.z() > preStepLocalPos.z()){
                return true;
            }
            break;
        case XY_plus:
            if(std::fabs( preStepLocalPos.z() - SiPM_solid->GetZHalfLength())<kCarTolerance
                && postStepLocalPos.z() < preStepLocalPos.z()){
                return true;
            }
            break;
        case XZ_minus:
            if(std::fabs( preStepLocalPos.y() + SiPM_solid->GetYHalfLength())<kCarTolerance 
                && postStepLocalPos.y() > preStepLocalPos.y()){
                return true;
            }
            break;
        case XZ_plus:
            if(std::fabs( preStepLocalPos.y() - SiPM_solid->GetYHalfLength())<kCarTolerance
                && postStepLocalPos.y() < preStepLocalPos.y()){
                return true;
            }
            break;
        case YZ_minus:
            if(std::fabs( preStepLocalPos.x() + SiPM_solid->GetXHalfLength())<kCarTolerance
                && postStepLocalPos.x() > preStepLocalPos.x()){
                return true;
            }
            break;
        case YZ_plus:
            if(std::fabs( preStepLocalPos.x() - SiPM_solid->GetXHalfLength())<kCarTolerance
                && postStepLocalPos.x() < preStepLocalPos.x()){
                return true;
            }
            break;
        default:
            break;
        }
    }
    return false;
}
}

