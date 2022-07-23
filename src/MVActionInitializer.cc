#include "MVActionInitializer.hh"
#include "MVPrimaryGeneratorAction.hh"
#include "MVRunAction.hh"
#include "MVEventAction.hh"
#include "MVSteppingAction.hh"

MuonVeto::MVActionInitializer::MVActionInitializer()
{}

MuonVeto::MVActionInitializer::~MVActionInitializer()
{}

void MuonVeto::MVActionInitializer::BuildForMaster() const
{
    SetUserAction(new MVRunAction(2));
}

void MuonVeto::MVActionInitializer::Build() const
{
    SetUserAction(new MVPrimaryGeneratorAction);
    SetUserAction(new MVRunAction(2));

    auto eventAction = new MVEventAction();
    SetUserAction(eventAction);
    SetUserAction(new MVSteppingAction(eventAction));
}