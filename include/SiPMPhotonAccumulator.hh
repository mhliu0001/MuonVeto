/// SiPMPhotonAccumulator.hh: Primitive scorer for SiPM
/// SiPM is defined as a G4Box 
/// A surface is defined as a scoring surface, and optical photons coming inside the surface
/// are accumulated.

#ifndef SiPMPhotonAccumulator_h
#define SiPMPhotonAccumulator_h 1

#include "globals.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4THitsMap.hh"

#include "G4Box.hh"

namespace MuonVeto
{

typedef enum { XY_plus, XY_minus, XZ_plus, XZ_minus, YZ_plus, YZ_minus} SiPM_surface;

class SiPMPhotonAccumulator: public G4VPrimitiveScorer
{
    public:
        SiPMPhotonAccumulator(G4String name, SiPM_surface surface);
        virtual ~SiPMPhotonAccumulator();

    protected:
        virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;
        G4bool IsSelectedSurface(G4Step*, G4Box*);

    public:
        virtual void Initialize(G4HCofThisEvent*) override;
        virtual void EndOfEvent(G4HCofThisEvent*) override;
        virtual void clear() override;
        virtual void DrawAll() override;
        virtual void PrintAll() override;

    private:
        G4int HCID;
        SiPM_surface fSurface;
        G4THitsMap<G4int>* PhotonCountMap;
        G4ThreeVectorList fpos;
};

}

#endif