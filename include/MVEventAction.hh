#ifndef MVEventAction_h
#define MVEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

namespace MuonVeto
{

class EventAction : public G4UserEventAction
{
  public:
    EventAction();
    ~EventAction() override;

    void  BeginOfEventAction(const G4Event* ) override;
    void    EndOfEventAction(const G4Event* ) override;
};

}
#endif
