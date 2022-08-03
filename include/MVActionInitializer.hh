#ifndef MVActionInitializer_h
#define MVActionInitializer_h 1

#include "G4VUserActionInitialization.hh"
#include "globals.hh"

namespace MuonVeto
{

class MVActionInitializer : public G4VUserActionInitialization
{
    public:

    MVActionInitializer(const G4String& outputFilePath, const G4bool useBuiltinAnalysis);
    ~MVActionInitializer();

    void Build() const override;
    void BuildForMaster() const override;
    
    private:

    const G4String fOutputFilePath;
    const G4bool fUseBuiltinAnalysis;
};

}
#endif