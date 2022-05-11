#ifndef MVActionInitializer_h
#define MVActionInitializer_h 1

#include "G4VUserActionInitialization.hh"

namespace MuonVeto
{

class testActionInitializer : public G4VUserActionInitialization
{
    public:

    testActionInitializer();
    ~testActionInitializer();

    void Build() const;
    
    private:
};

}
#endif