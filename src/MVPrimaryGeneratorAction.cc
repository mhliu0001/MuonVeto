#include "MVPrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Box.hh"

MuonVeto::MVPrimaryGeneratorAction::MVPrimaryGeneratorAction()
{
    G4int nofParticles = 1;
    fParticleGun  = new G4ParticleGun(nofParticles);

    G4ParticleDefinition* particleDefinition = 
        G4ParticleTable::GetParticleTable()->FindParticle("mu-");

    fParticleGun->SetParticleDefinition(particleDefinition);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 1., 0.));
    fParticleGun->SetParticleEnergy(3*GeV);
    fParticleGun->SetParticlePosition(G4ThreeVector(0, -20*cm, 0));
}

MuonVeto::MVPrimaryGeneratorAction::~MVPrimaryGeneratorAction()
{
    delete fParticleGun;
}

void MuonVeto::MVPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    fParticleGun->GeneratePrimaryVertex(anEvent);
}