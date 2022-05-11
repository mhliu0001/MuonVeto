#ifndef MVEventAction_h
#define MVEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

namespace MuonVeto
{

class MVEventAction : public G4UserEventAction
{
  public:
    MVEventAction();
    ~MVEventAction() override;

    void  BeginOfEventAction(const G4Event* ) override;
    void    EndOfEventAction(const G4Event* ) override;
};

}
#endif
