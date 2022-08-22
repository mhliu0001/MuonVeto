#include "MVActionInitializer.hh"
#include "MVPrimaryGeneratorAction.hh"
#include "MVRunAction.hh"
#include "MVEventAction.hh"
#include "MVSteppingAction.hh"

MuonVeto::MVActionInitializer::MVActionInitializer(const Config& config): 
    fConfig(config)
{}

MuonVeto::MVActionInitializer::~MVActionInitializer()
{}

void MuonVeto::MVActionInitializer::BuildForMaster() const
{
    SetUserAction(new MVRunAction(fConfig));
}

void MuonVeto::MVActionInitializer::Build() const
{
    SetUserAction(new MVPrimaryGeneratorAction);
    SetUserAction(new MVRunAction(fConfig));

    auto eventAction = new MVEventAction();
    SetUserAction(eventAction);
    SetUserAction(new MVSteppingAction(eventAction));
}