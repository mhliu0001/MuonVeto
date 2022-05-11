#ifndef MVPrimaryGeneratorAction_h
#define MVPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

namespace MuonVeto
{

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
    public:
        PrimaryGeneratorAction();
        ~PrimaryGeneratorAction();

        void GeneratePrimaries(G4Event* ) override;

        inline G4ParticleGun* GetParticleGun() {return fParticleGun;}

    private:
        G4ParticleGun* fParticleGun = nullptr; // G4 particle gun
};

}
#endif