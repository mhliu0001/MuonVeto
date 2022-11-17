#include "MVDefaultGenerator.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Box.hh"
#include "G4MultiUnion.hh"
#include "Randomize.hh"

using namespace MuonVeto;

MVDefaultGenerator::MVDefaultGenerator(const Config& config): fConfig(config)
{
    G4int nofParticles = 1;
    fParticleGun = new G4ParticleGun(nofParticles);

    G4ParticleDefinition *particleDefinition =
        G4ParticleTable::GetParticleTable()->FindParticle("mu-");

    fParticleGun->SetParticleDefinition(particleDefinition);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 1., 0.));
    fParticleGun->SetParticleEnergy(3 * GeV);
    fParticleGun->SetParticlePosition(G4ThreeVector(0, -20 * cm, 0));
}

MVDefaultGenerator::~MVDefaultGenerator()
{
    delete fParticleGun;
}

void MVDefaultGenerator::GeneratePrimaries(G4Event *anEvent)
{
    if (fConfig.randomPoints)
    {
        // Generate a uniformly-distributed random point inside pscint_solid but outside groove_solid.
        if (!fPscint_solid)
        {
            G4LogicalVolume *pscint_log = G4LogicalVolumeStore::GetInstance()->GetVolume("pscint_log");
            if(pscint_log)
                fPscint_solid = dynamic_cast<G4Box *>(pscint_log->GetSolid());
        }

        if (!fGroove_solid)
        {
            G4LogicalVolume *groove_log = G4LogicalVolumeStore::GetInstance()->GetVolume("groove_log");
            G4VPhysicalVolume *groove_phys = G4PhysicalVolumeStore::GetInstance()->GetVolume("groove_phys");
            if(groove_phys)
                fGroove_y_translate = groove_phys->GetFrameTranslation().y();
            if(groove_log)
                fGroove_solid = dynamic_cast<G4MultiUnion *>(groove_log->GetSolid());
        }

        if (!fPscint_solid || !fGroove_solid)
        {
            G4ExceptionDescription msg;
            msg << "Either pscint solid or groove solid is not found.\n";
            msg << "Perhaps you have changed geometry.\n";
            msg << "The gun will be place at (0, -20 * cm, 0).";
            G4Exception("PrimaryGeneratorAction::GeneratePrimaries()", "VolumeNotFound", JustWarning, msg);
        }

        G4double pscint_x = fPscint_solid->GetXHalfLength() * 2;
        G4double pscint_y = fPscint_solid->GetYHalfLength() * 2;
        G4double pscint_z = fPscint_solid->GetZHalfLength() * 2;

        G4double x0, y0, z0;
        while (true)
        {
            x0 = pscint_x * (G4UniformRand() - 0.5);
            y0 = pscint_y * (G4UniformRand() - 0.5);
            z0 = pscint_z * (G4UniformRand() - 0.5);

            if (fPscint_solid->Inside(G4ThreeVector(x0, y0, z0)) == kInside && fGroove_solid->Inside(G4ThreeVector(x0, y0+fGroove_y_translate, z0)) == kOutside)
                break;
        }
        //G4cout << "The point generated is " << G4ThreeVector(x0, y0, z0) << G4endl;
        fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
    }

    fParticleGun->GeneratePrimaryVertex(anEvent);

    // Update information in MVGenerator AFTER a vertex is generated
    G4ThreeVector momentumDirection = fParticleGun->GetParticleMomentumDirection();
    fTheta = acos(momentumDirection.y());
    fPhi = (fTheta > 1e-10 ? atan(momentumDirection.x()/momentumDirection.z()) : 0);
    fEnergy = fParticleGun->GetParticleEnergy();
    fPosition = fParticleGun->GetParticlePosition();
    fParticleName = fParticleGun->GetParticleDefinition()->GetParticleName();

}