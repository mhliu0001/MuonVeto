#ifndef MVDefaultGenerator_h
#define MVDefaultGenerator_h 1

#include "MVGenerator.hh"
#include "MVGlobals.hh"

class G4ParticleGun;
class G4Event;
class G4VSolid;
class G4Box;

namespace MuonVeto
{
class MVDefaultGenerator : public MVGenerator
{
    public:
        MVDefaultGenerator(const Config& config);
        ~MVDefaultGenerator();

        void GeneratePrimaries(G4Event* ) override;

        inline G4ParticleGun* GetParticleGun() const {return fParticleGun;}

    private:
        const Config fConfig;
        G4ParticleGun* fParticleGun = nullptr; // G4 particle gun
        G4VSolid* fGroove_solid = nullptr;     // groove solid in detector
        G4Box* fPscint_solid = nullptr;        // pscint solid in detector
        G4double fPscint_x_translate;          // pscint phys translation(frame translation, not solid transformation)
        G4double fGroove_z_translate;          // groove phys translation(frame translation, not solid transformation)
};

}
#endif