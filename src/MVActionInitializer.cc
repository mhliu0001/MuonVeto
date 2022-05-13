#include "MVActionInitializer.hh"
#include "MVPrimaryGeneratorAction.hh"
#include "MVRunAction.hh"
#include "MVEventAction.hh"

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
    SetUserAction(new PrimaryGeneratorAction);
    SetUserAction(new MVRunAction(2));
    SetUserAction(new MVEventAction);
}