#include "MVMuonGenerator.hh"

#include <G4Event.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <Randomize.hh>
#include "G4SystemOfUnits.hh"
#include "G4SolidStore.hh"
#include "G4Box.hh"

using namespace MuonVeto;

MVMuonGenerator::MVMuonGenerator() : gun{new G4ParticleGun}
{
    // http://arxiv.org/abs/1606.06907
    // Energy and angular distributions of atmospheric muons at the Earth
    // constants in Shukla's function
    n = 3.01;
    E0 = 4.29 * GeV;
    epsilon = 854. * GeV;
    Rd = 174.;
    // http://arxiv.org/abs/1005.5332
    // Measurement of the charge ratio of atmospheric muons with the CMS detector
    charge_ratio = 1.2766;
    charge_thres = 1 / (1 + charge_ratio);
    theta_low = 0.;

    // Energy range in simulation
    E_low = 0.5 * GeV;
    E_up = 1000. * GeV;

    // Shielding size, muon will shower cling to the top
    auto solidStore = G4SolidStore::GetInstance();
    G4Box* whole_solid = dynamic_cast<G4Box*>(solidStore->GetSolid("whole_solid"));
    if(!whole_solid)
    {
        throw std::runtime_error("MVMuonGenerator: whole_solid not found, can't generate muons!");
    }
    shielding_x = whole_solid->GetXHalfLength()*2;
    shielding_y = whole_solid->GetYHalfLength()*2;
    shielding_z = whole_solid->GetZHalfLength()*2;
}

G4double MVMuonGenerator::thetaSpectrum(G4double theta)
{
    G4double Dtheta = sqrt(Rd * Rd * cos(theta) * cos(theta) + 2 * Rd + 1) - Rd * cos(theta);
    G4double Dtheta_n = pow(Dtheta, -(n - 1));
    return Dtheta_n;
}

G4double MVMuonGenerator::energySpectrum(G4double energy)
{
    G4double IE = pow(E0 + energy, -n) / (1 + energy / epsilon);
    return IE;
}

G4double MVMuonGenerator::getRandomTheta() {
    G4double theta, rvs=1., upper=0.;
    G4double thetaSpectrum_max = thetaSpectrum(theta_low);
    while (rvs > upper)
    {
        theta = G4UniformRand() * M_PI_2;
        rvs = G4UniformRand();
        upper = thetaSpectrum(theta) / thetaSpectrum_max;
    }
    return theta;
}

G4double MVMuonGenerator::getRandomEnergy() {
    G4double energy, rvs=1., upper=0.;
    G4double energySpectrum_max = energySpectrum(E_low);
    while (rvs > upper)
    {
        energy = G4UniformRand() * (E_up - E_low) + E_low;
        rvs = G4UniformRand();
        upper = energySpectrum(energy) / energySpectrum_max;
    }
    return energy;
}

G4double MVMuonGenerator::getRandomPhi() {
    G4double phi = G4UniformRand() * M_PI * 2;
    return phi;
}

G4ParticleDefinition* MVMuonGenerator::getMuonCharge() {
    G4ParticleDefinition* particle;
    auto particleTable = G4ParticleTable::GetParticleTable();
    if (G4UniformRand() < charge_thres) {
        particle = particleTable->FindParticle("mu-");
    } else {
        particle = particleTable->FindParticle("mu+");
    }
    return particle;
}

void MVMuonGenerator::GeneratePrimaries(G4Event *anEvent) {
    // G4cout << "Cosmic ray muon generator used!" << G4endl;

    G4double posX, posY, posZ;
    G4double dirX, dirY, dirZ;
    G4double theta = getRandomTheta();
    G4double phi = getRandomPhi();
    G4double energy = getRandomEnergy();

    dirZ = sin(theta) * cos(phi);
    dirX = sin(theta) * sin(phi);
    dirY = -cos(theta);
    posX = (G4UniformRand() - 0.5) * shielding_x;
    posY = shielding_y/2;
    posZ = (G4UniformRand() - 0.5) * shielding_z;

    auto particle = getMuonCharge();
    G4double mass = particle->GetPDGMass();

    gun->SetParticleDefinition(particle);
    gun->SetParticleEnergy(energy - mass);
    gun->SetParticlePosition(G4ThreeVector(posX, posY, posZ));
    gun->SetParticleMomentumDirection(G4ThreeVector(dirX, dirY, dirZ));

    gun->GeneratePrimaryVertex(anEvent);
}
