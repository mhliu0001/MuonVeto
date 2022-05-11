#include "MVActionInitializer.hh"
#include "MVPrimaryGeneratorAction.hh"
#include "MVEventAction.hh"

MuonVeto::testActionInitializer::testActionInitializer()
{}

MuonVeto::testActionInitializer::~testActionInitializer()
{}

void MuonVeto::testActionInitializer::Build() const
{
    SetUserAction(new PrimaryGeneratorAction);
    SetUserAction(new EventAction);
}