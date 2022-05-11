#include "MVPrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Box.hh"

MuonVeto::PrimaryGeneratorAction::PrimaryGeneratorAction()
{
    G4int nofParticles = 1;
    fParticleGun  = new G4ParticleGun(nofParticles);

    G4ParticleDefinition* particleDefinition = 
        G4ParticleTable::GetParticleTable()->FindParticle("mu-");

    fParticleGun->SetParticleDefinition(particleDefinition);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 1., 0.));
    fParticleGun->SetParticleEnergy(3*GeV);
}

MuonVeto::PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}

void MuonVeto::PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    G4double expHall_y = 0;
    G4LogicalVolume* expHall_log = G4LogicalVolumeStore::GetInstance()->GetVolume("World");
    G4Box* expHall_sold = nullptr;
    if ( expHall_log ) expHall_sold = dynamic_cast<G4Box*>(expHall_log->GetSolid());
    if ( expHall_sold ) expHall_y = expHall_sold->GetYHalfLength();
    else  {
        G4cerr << "World volume of box not found." << G4endl;
        G4cerr << "Perhaps you have changed geometry." << G4endl;
        G4cerr << "The gun will be place in the center." << G4endl;
    }

    // Note that this particular case of starting a primary particle on the world boundary
    // requires shooting in a direction towards inside the world.
    fParticleGun->SetParticlePosition(G4ThreeVector(0., -expHall_y/5, 0.));

    fParticleGun->GeneratePrimaryVertex(anEvent);
}