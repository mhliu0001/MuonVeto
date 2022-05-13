/// SiPMSD.hh: Declaration of a SiPM sensitive detector
/// SiPMSD must be a G4Box in the current version
///   When constructed, the sensitive detector name, the collection name
/// and a surface direction should be specified.
///   The surface direction can be XY_plus, XY_minus, XZ_plus, XZ_minus,
/// YZ_plus, YZ_minus
///   SiPMSD stores the particle information when it comes inside the 
/// G4Box from the surface specified. 

#ifndef SiPMSD_h
#define SiPMSD_h 1

#include "G4VSensitiveDetector.hh"

#include "SiPMHit.hh"

class G4Step;
class G4HCofThisEvent;
class G4Box;

namespace MuonVeto
{

// SiPM_surface: six sides can be specified
typedef enum { XY_plus, XY_minus, XZ_plus, XZ_minus, YZ_plus, YZ_minus} SiPM_surface;

/// Tracker sensitive detector class
///
/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step with non zero
/// energy deposit.

class SiPMSD : public G4VSensitiveDetector
{
    public:
        SiPMSD(const G4String& name, const G4String& hitsCollectionName, const SiPM_surface surface);
        ~SiPMSD() override;

        // methods from base class
        void   Initialize(G4HCofThisEvent* hitCollection) override;
        G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
        void   EndOfEvent(G4HCofThisEvent* hitCollection) override;

    private:
        SiPMHitsCollection* fHitsCollection = nullptr;
        G4bool IsSelectedSurface(G4Step*, G4Box*); // Used to judge whether a hit should be recorded
        const SiPM_surface fSurface;

};

}

#endif
