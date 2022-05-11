#include "SiPMPhotonAccumulator.hh"

#include "G4SystemOfUnits.hh"
#include "G4StepStatus.hh"
#include "G4Track.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Box.hh"
#include "G4GeometryTolerance.hh"

MuonVeto::SiPMPhotonAccumulator::SiPMPhotonAccumulator(G4String name, SiPM_surface surface): 
    G4VPrimitiveScorer(name), HCID(-1), fSurface(surface)
{

}

MuonVeto::SiPMPhotonAccumulator::~SiPMPhotonAccumulator()
{

}

G4bool MuonVeto::SiPMPhotonAccumulator::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    G4StepPoint* preStep = aStep->GetPreStepPoint();
    G4VPhysicalVolume* physVol = preStep->GetPhysicalVolume();
    G4VSolid* solid = physVol->GetLogicalVolume()->GetSolid();
    G4Box* SiPM_solid = dynamic_cast<G4Box*>(solid);

    if (IsSelectedSurface(aStep, SiPM_solid))
    {
        G4int index = GetIndex(aStep);
        G4int count = 1;
        PhotonCountMap->add(index, count);
    }
    return true;
}

G4bool MuonVeto::SiPMPhotonAccumulator::IsSelectedSurface(G4Step* aStep, G4Box* SiPM_solid)
{
    G4TouchableHandle theTouchable = 
        aStep->GetPreStepPoint()->GetTouchableHandle();
    G4double kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();

    if (aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary ){
        // Entering Geometry
        G4ThreeVector stppos1 = aStep->GetPreStepPoint()->GetPosition();
        G4ThreeVector localpos1 = 
            theTouchable->GetHistory()->GetTopTransform().TransformPoint(stppos1);

        switch (fSurface)
        {
        case XY_minus:
            if(std::fabs( localpos1.z() + SiPM_solid->GetZHalfLength())<kCarTolerance ){
                return true;
            }
            break;
        case XY_plus:
            if(std::fabs( localpos1.z() - SiPM_solid->GetZHalfLength())<kCarTolerance ){
                return true;
            }
            break;
        case XZ_minus:
            if(std::fabs( localpos1.y() + SiPM_solid->GetYHalfLength())<kCarTolerance ){
                return true;
            }
            break;
        case XZ_plus:
            if(std::fabs( localpos1.y() - SiPM_solid->GetYHalfLength())<kCarTolerance ){
                return true;
            }
            break;
        case YZ_minus:
            if(std::fabs( localpos1.x() + SiPM_solid->GetXHalfLength())<kCarTolerance ){
                return true;
            }
            break;
        case YZ_plus:
            if(std::fabs( localpos1.x() - SiPM_solid->GetXHalfLength())<kCarTolerance ){
                return true;
            }
            break;
        default:
            break;
        }
    }
    return false;
}

void MuonVeto::SiPMPhotonAccumulator::Initialize(G4HCofThisEvent* HCE)
{
    PhotonCountMap = new G4THitsMap<G4int>(detector->GetName(), GetName());
    if ( HCID < 0 ) HCID = GetCollectionID(0);
    HCE->AddHitsCollection(HCID, (G4VHitsCollection*)PhotonCountMap);
}

void MuonVeto::SiPMPhotonAccumulator::EndOfEvent(G4HCofThisEvent* HCE)
{

}

void MuonVeto::SiPMPhotonAccumulator::clear()
{
    PhotonCountMap->clear();
}

void MuonVeto::SiPMPhotonAccumulator::DrawAll()
{

}

void MuonVeto::SiPMPhotonAccumulator::PrintAll()
{
    G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
    G4cout << " PrimitiveScorer " << GetName() <<G4endl; 
    G4cout << " Number of entries " << PhotonCountMap->entries() << G4endl;
    std::map<G4int,G4int*>::iterator itr = PhotonCountMap->GetMap()->begin();
    for(; itr != PhotonCountMap->GetMap()->end(); itr++) 
    {
        G4cout << "  copy no.: " << itr->first << " photon count : " ;
        G4cout <<  *(itr->second);
    }
    G4cout << G4endl;
}




