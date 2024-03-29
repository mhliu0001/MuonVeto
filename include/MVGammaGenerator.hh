#ifndef MVGammaGenerator_h
#define MVGammaGenerator_h 1

#include "MVGenerator.hh"
#include <G4ParticleGun.hh>

class G4ParticleGun;
class G4Event;

namespace MuonVeto
{
class MVGammaGenerator : public MVGenerator {
  public:
    MVGammaGenerator();

    ~MVGammaGenerator() = default;

    virtual void GeneratePrimaries(G4Event *anEvent) override;

  private:
    G4double E_gamma;
    G4double shielding_x;
    G4double shielding_y;
    G4double shielding_z;

    G4double getRandomTheta();
    G4double getRandomPhi();

    std::unique_ptr<G4ParticleGun> gun;
};
}
#endif