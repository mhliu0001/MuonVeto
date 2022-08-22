#ifndef MVActionInitializer_h
#define MVActionInitializer_h 1

#include "G4VUserActionInitialization.hh"
#include "MVGlobals.hh"

namespace MuonVeto
{

class MVActionInitializer : public G4VUserActionInitialization
{
    public:

    MVActionInitializer(const Config& config);
    ~MVActionInitializer();

    void Build() const override;
    void BuildForMaster() const override;
    
    private:

    const Config fConfig;
};

}
#endif