#ifndef MVActionInitializer_h
#define MVActionInitializer_h 1

#include "G4VUserActionInitialization.hh"

namespace MuonVeto
{

class MVActionInitializer : public G4VUserActionInitialization
{
    public:

    MVActionInitializer();
    ~MVActionInitializer();

    void Build() const;
    
    private:
};

}
#endif