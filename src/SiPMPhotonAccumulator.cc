/// !!!!!!CURRENTLY NOT USED!!!!!!

#include "SiPMPhotonAccumulator.hh"

#include "G4SystemOfUnits.hh"
#include "G4StepStatus.hh"
#include "G4Track.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Box.hh"
#include "G4GeometryTolerance.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4VisAttributes.hh"
#include "G4UImanager.hh"

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
        fpos.push_back(preStep->GetPosition());
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

void MuonVeto::SiPMPhotonAccumulator::Initialize(G4HCofThisEvent* HCE)
{
    PhotonCountMap = new G4THitsMap<G4int>(detector->GetName(), GetName());
    if ( HCID < 0 ) HCID = GetCollectionID(0);
    HCE->AddHitsCollection(HCID, (G4VHitsCollection*)PhotonCountMap);
    fpos.clear();
}

void MuonVeto::SiPMPhotonAccumulator::EndOfEvent(G4HCofThisEvent*)
{
    
}

void MuonVeto::SiPMPhotonAccumulator::clear()
{
    PhotonCountMap->clear();
    fpos.clear();
}

void MuonVeto::SiPMPhotonAccumulator::DrawAll()
{
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if(pVVisManager)
    {
        for(auto itr = fpos.begin(); itr != fpos.end(); itr++)
        {
            const G4ThreeVector pos = *itr;
            G4Circle circle(pos);
            G4cout  << "Drawing a point at " << pos << G4endl;
            circle.SetScreenSize(8.);
            circle.SetFillStyle(G4Circle::filled);
            G4Colour colour(1.,0.,0.);
            G4VisAttributes attribs(colour);
            circle.SetVisAttributes(attribs);
            pVVisManager->Draw(circle);
        }
    }
    else
    {
        G4cout  << "null pVVisManager" << G4endl;
    }
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




