#ifndef MVGenerator_h
#define MVGenerator_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

namespace MuonVeto
{

class MVGenerator : public G4VUserPrimaryGeneratorAction
{
    public:
        MVGenerator() {}
        virtual ~MVGenerator() {}
        inline G4String GetCurrentParticleName() const { return fParticleName; }
        inline G4double GetCurrentTheta() const { return fTheta; }
        inline G4double GetCurrentPhi() const { return fPhi; }
        inline G4double GetCurrentEnergy() const { return fEnergy; }
        inline G4ThreeVector GetCurrentParticlePosition() const { return fPosition; }
    protected:
        G4String fParticleName;
        G4double fTheta;
        G4double fPhi;
        G4double fEnergy;
        G4ThreeVector fPosition;
};

}
#endif