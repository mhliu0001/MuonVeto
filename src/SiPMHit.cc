#include "SiPMHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

namespace MuonVeto
{

G4ThreadLocal G4Allocator<SiPMHit>* SiPMHitAllocator = nullptr;

SiPMHit::SiPMHit()
{}

SiPMHit::~SiPMHit() {}


G4bool SiPMHit::operator==(const SiPMHit& right) const
{
    return this == &right;
}

void SiPMHit::Draw()
{
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if(pVVisManager)
    {
        G4Circle circle(fPos);
        circle.SetScreenSize(8.);
        circle.SetFillStyle(G4Circle::filled);
        G4Colour colour(1.,0.,0.);
        G4VisAttributes attribs(colour);
        circle.SetVisAttributes(attribs);
        pVVisManager->Draw(circle);
    }
}

void SiPMHit::Print()
{
    G4cout
        << "  trackID: " << fTrackID << " chamberNb: " << fSiPMNb
        << " Position: "
        << std::setw(7) << G4BestUnit( fPos,"Length")
        << G4endl;
}

}
