#ifndef MVPrimaryGeneratorAction_h
#define MVPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "MVGlobals.hh"

class G4ParticleGun;
class G4Event;
class G4MultiUnion;
class G4Box;

namespace MuonVeto
{
class MVPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
    public:
        MVPrimaryGeneratorAction(const Config& config);
        ~MVPrimaryGeneratorAction();

        void GeneratePrimaries(G4Event* ) override;

        inline G4ParticleGun* GetParticleGun() const {return fParticleGun;}

    private:
        const Config fConfig;
        G4ParticleGun* fParticleGun = nullptr; // G4 particle gun
        G4MultiUnion* fGroove_solid = nullptr; // groove solid in detector
        G4Box* fPscint_solid = nullptr;        // pscint solid in detector
};

}
#endif