#include "MVGammaGenerator.hh"

#include <G4Event.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <Randomize.hh>
#include "G4SystemOfUnits.hh"
#include "G4SolidStore.hh"
#include "G4Box.hh"

using namespace MuonVeto;

MVGammaGenerator::MVGammaGenerator() : gun{new G4ParticleGun}
{
    // Energy range in simulation
    E_gamma = 2.6 * MeV;

    // Shielding size, gamma will appear on the surfaces
    auto solidStore = G4SolidStore::GetInstance();
    G4Box* whole_solid = dynamic_cast<G4Box*>(solidStore->GetSolid("whole_solid"));
    if(!whole_solid)
    {
        throw std::runtime_error("MVGammaGenerator: whole_solid not found, can't generate gammas!");
    }
    shielding_x = whole_solid->GetXHalfLength()*2;
    shielding_y = whole_solid->GetYHalfLength()*2;
    shielding_z = whole_solid->GetZHalfLength()*2;
}

G4double MVGammaGenerator::getRandomTheta() {
    G4double theta, rvs=1., upper=0.;
    while (rvs > upper)
    {
        theta = G4UniformRand() * M_PI_2;
        rvs = G4UniformRand();
        upper = sin(theta);
    }
    return theta;
}

G4double MVGammaGenerator::getRandomPhi() {
    G4double phi = G4UniformRand() * M_PI * 2;
    return phi;
}

void MVGammaGenerator::GeneratePrimaries(G4Event *anEvent) {
    G4double posX, posY, posZ;
    G4double dirX, dirY, dirZ;
    G4double theta = getRandomTheta();
    G4double phi = getRandomPhi();
    G4double energy = E_gamma;

    // Determine which surface, according to the area
    G4double areaXY = shielding_x*shielding_y;
    G4double areaYZ = shielding_y*shielding_z;
    G4double areaXZ = shielding_x*shielding_z;
    G4double totalArea = areaXY + areaYZ + areaXZ;
    G4double rvs = G4UniformRand();
    
    if(rvs < areaXY/totalArea)
    {
        // +XY surface
        dirX = sin(theta) * cos(phi);
        dirY = sin(theta) * sin(phi);
        dirZ = -cos(theta);
        posX = (G4UniformRand() - 0.5) * shielding_x;
        posY = (G4UniformRand() - 0.5) * shielding_y;
        posZ = shielding_z/2;
    }
    else if(rvs < (areaXY+areaXZ)/totalArea)
    {
        // +XZ surface
        dirX = sin(theta) * cos(phi);
        dirZ = sin(theta) * sin(phi);
        dirY = -cos(theta);
        posX = (G4UniformRand() - 0.5) * shielding_x;
        posY = shielding_y/2;
        posZ = (G4UniformRand() - 0.5) * shielding_z;
    }
    else
    {
        // +YZ surface
        dirY = sin(theta) * cos(phi);
        dirZ = sin(theta) * sin(phi);
        dirX = -cos(theta);
        posX = shielding_x/2;
        posY = (G4UniformRand() - 0.5) * shielding_y;
        posZ = (G4UniformRand() - 0.5) * shielding_z;
    }

    auto particle = G4ParticleTable::GetParticleTable()->FindParticle("gamma");

    fTheta = theta;
    fPhi = phi;
    fEnergy = energy;
    fPosition = G4ThreeVector(posX, posY, posZ);
    fParticleName = particle->GetParticleName();
    fMomentumDir = G4ThreeVector(dirX, dirY, dirZ);

    gun->SetParticleDefinition(particle);
    gun->SetParticleEnergy(energy);
    gun->SetParticlePosition(fPosition);
    gun->SetParticleMomentumDirection(fMomentumDir);

    gun->GeneratePrimaryVertex(anEvent);
}
