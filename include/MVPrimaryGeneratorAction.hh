#ifndef MVPrimaryGeneratorAction_h
#define MVPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

namespace MuonVeto
{

class MVPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
    public:
        MVPrimaryGeneratorAction();
        ~MVPrimaryGeneratorAction();

        void GeneratePrimaries(G4Event* ) override;

        inline G4ParticleGun* GetParticleGun() const {return fParticleGun;}

    private:
        G4ParticleGun* fParticleGun = nullptr; // G4 particle gun
};

}
#endif