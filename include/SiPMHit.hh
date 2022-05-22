/// SiPMHit.hh: Declaration of a SiPMHit
/// SiPMHit stores the following information:
/// TrackID, SiPMNb, pos

#ifndef SiPMHit_h
#define SiPMHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

namespace MuonVeto
{

class SiPMHit : public G4VHit
{
    public:
        SiPMHit();
        SiPMHit(const SiPMHit&) = default;
        ~SiPMHit() override;

        // operators
        SiPMHit& operator=(const SiPMHit&) = default;
        G4bool operator==(const SiPMHit&) const;

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        // methods from base class
        void Draw() override;
        void Print() override;

        // Set methods
        inline void SetTrackID  (G4int track)      { fTrackID = track; };
        inline void SetSiPMNb(G4int SiPMNb)      { fSiPMNb = SiPMNb; };
        inline void SetPos      (G4ThreeVector xyz){ fPos = xyz; };

        // Get methods
        inline G4int GetTrackID() const     { return fTrackID; };
        inline G4int GetSetSiPMNb() const   { return fSiPMNb; };
        inline G4ThreeVector GetPos() const { return fPos; };

    private:
        G4int         fTrackID = -1;
        G4int         fSiPMNb = -1;
        G4ThreeVector fPos;
};

typedef G4THitsCollection<SiPMHit> SiPMHitsCollection;

extern G4ThreadLocal G4Allocator<SiPMHit>* SiPMHitAllocator;

inline void* SiPMHit::operator new(size_t)
{
    if(!SiPMHitAllocator)
        SiPMHitAllocator = new G4Allocator<SiPMHit>;
    return (void *) SiPMHitAllocator->MallocSingle();
}

inline void SiPMHit::operator delete(void *hit)
{
    SiPMHitAllocator->FreeSingle((SiPMHit*) hit);
}

}

#endif