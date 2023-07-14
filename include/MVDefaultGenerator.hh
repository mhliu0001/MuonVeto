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
        G4VSolid* fPscint_solid = nullptr;     // pscint solid in detector
        G4double x_bounding, y_bounding, z_bounding; // bounding box
        G4double fPscint_x_translate;          // pscint phys translation(object translation)
        G4double fGroove_z_translate;          // groove phys translation(object translation)
};

}
#endif