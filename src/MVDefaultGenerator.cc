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
#include "BambooUtils.hh"

using namespace MuonVeto;

MVDefaultGenerator::MVDefaultGenerator(const Config &config) : fConfig(config)
{
    G4int nofParticles = 1;
    fParticleGun = new G4ParticleGun(nofParticles);

    G4ParticleDefinition *particleDefinition =
        G4ParticleTable::GetParticleTable()->FindParticle("mu-");

    fParticleGun->SetParticleDefinition(particleDefinition);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., -1.));
    fParticleGun->SetParticleEnergy(3 * GeV);
    fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, 20 * cm));
}

MVDefaultGenerator::~MVDefaultGenerator()
{
    delete fParticleGun;
}

void MVDefaultGenerator::GeneratePrimaries(G4Event *anEvent)
{
    if (fConfig.randomPoints)
    {
        std::ifstream psConfigStream(fConfig.psConfig);
        json psConfig = json::parse(psConfigStream);
        int psType = psConfig["ps_type"];

        if (psType == 1)
        {
            // Generate a uniformly-distributed random point inside PScint but outside Groove.
            if (!fPscint_solid)
            {
                G4LogicalVolume *pscint_log = G4LogicalVolumeStore::GetInstance()->GetVolume("PScintLogicalVolume");
                G4VPhysicalVolume *pscint_phys = G4PhysicalVolumeStore::GetInstance()->GetVolume("PScint");
                if (pscint_phys)
                    fPscint_x_translate = pscint_phys->GetObjectTranslation().x();
                if (pscint_log)
                    fPscint_solid = pscint_log->GetSolid();
            }

            if (!fGroove_solid)
            {
                G4LogicalVolume *groove_log = G4LogicalVolumeStore::GetInstance()->GetVolume("GrooveLogicalVolume");
                G4VPhysicalVolume *groove_phys = G4PhysicalVolumeStore::GetInstance()->GetVolume("Groove");
                if (groove_phys)
                    fGroove_z_translate = groove_phys->GetObjectTranslation().z();
                if (groove_log)
                    fGroove_solid = groove_log->GetSolid();
            }

            if (!fPscint_solid || !fGroove_solid)
            {
                G4ExceptionDescription msg;
                msg << "Either pscint solid or groove solid is not found.\n";
                msg << "Perhaps you have changed geometry.\n";
                msg << "The gun will be place at (0, 0, 20*cm).";
                G4Exception("PrimaryGeneratorAction::GeneratePrimaries()", "VolumeNotFound", JustWarning, msg);
            }

            x_bounding = (dynamic_cast<G4Box*>(fPscint_solid))->GetXHalfLength() * 2;
            y_bounding = (dynamic_cast<G4Box*>(fPscint_solid))->GetYHalfLength() * 2;
            z_bounding = (dynamic_cast<G4Box*>(fPscint_solid))->GetZHalfLength() * 2;

            G4double x0, y0, z0;
            while (true)
            {
                x0 = x_bounding * (G4UniformRand() - 0.5);
                y0 = y_bounding * (G4UniformRand() - 0.5);
                z0 = z_bounding * (G4UniformRand() - 0.5);
                if (fPscint_solid->Inside(G4ThreeVector(x0, y0, z0)) == kInside && fGroove_solid->Inside(G4ThreeVector(x0, y0, z0 - fGroove_z_translate)) == kOutside)
                    break;
            }
            fParticleGun->SetParticlePosition(G4ThreeVector(x0 + fPscint_x_translate, y0, z0));
            fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., -1.));
        }

        else
        {
            if (!fPscint_solid)
            {
                x_bounding = BambooUtils::evaluate(psConfig["whole_x"]);
                y_bounding = BambooUtils::evaluate(psConfig["whole_y"]);
                z_bounding = 20 * mm;

                G4LogicalVolume *pscint_log = G4LogicalVolumeStore::GetInstance()->GetVolume("PScintLogicalVolume");
                G4VPhysicalVolume *pscint_phys = G4PhysicalVolumeStore::GetInstance()->GetVolume("PScint");
                if (pscint_phys)
                    fPscint_x_translate = pscint_phys->GetObjectTranslation().x();
                if (pscint_log)
                    fPscint_solid = pscint_log->GetSolid();
            }
            G4double x0, y0, z0;
            while (true)
            {
                x0 = x_bounding * (G4UniformRand() - 0.5);
                y0 = y_bounding * (G4UniformRand() - 0.5);
                z0 = z_bounding * (G4UniformRand() - 0.5);
                if (fPscint_solid->Inside(G4ThreeVector(x0, y0, z0)) == kInside)
                    break;
            }
            fParticleGun->SetParticlePosition(G4ThreeVector(x0 + fPscint_x_translate, y0, z0));
            fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., -1.));
        }
    }

    fParticleGun->GeneratePrimaryVertex(anEvent);

    // Update information in MVGenerator AFTER a vertex is generated
    G4ThreeVector momentumDirection = fParticleGun->GetParticleMomentumDirection();
    fTheta = acos(momentumDirection.y());
    fPhi = (fTheta > 1e-10 ? atan(momentumDirection.x() / momentumDirection.z()) : 0);
    fEnergy = fParticleGun->GetParticleEnergy();
    fPosition = fParticleGun->GetParticlePosition();
    fParticleName = fParticleGun->GetParticleDefinition()->GetParticleName();
}