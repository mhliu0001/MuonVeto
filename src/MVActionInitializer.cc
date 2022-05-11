#include "MVActionInitializer.hh"
#include "MVPrimaryGeneratorAction.hh"
#include "MVEventAction.hh"

MuonVeto::MVActionInitializer::MVActionInitializer()
{}

MuonVeto::MVActionInitializer::~MVActionInitializer()
{}

void MuonVeto::MVActionInitializer::Build() const
{
    SetUserAction(new PrimaryGeneratorAction);
    SetUserAction(new MVEventAction);
}