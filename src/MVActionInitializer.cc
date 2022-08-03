#include "MVActionInitializer.hh"
#include "MVPrimaryGeneratorAction.hh"
#include "MVRunAction.hh"
#include "MVEventAction.hh"
#include "MVSteppingAction.hh"

MuonVeto::MVActionInitializer::MVActionInitializer(const G4String& outputFilePath, const G4bool useBuiltinAnalysis): 
    fOutputFilePath(outputFilePath), fUseBuiltinAnalysis(useBuiltinAnalysis)
{}

MuonVeto::MVActionInitializer::~MVActionInitializer()
{}

void MuonVeto::MVActionInitializer::BuildForMaster() const
{
    SetUserAction(new MVRunAction(2, fOutputFilePath, fUseBuiltinAnalysis));
}

void MuonVeto::MVActionInitializer::Build() const
{
    SetUserAction(new MVPrimaryGeneratorAction);
    SetUserAction(new MVRunAction(2, fOutputFilePath, fUseBuiltinAnalysis));

    auto eventAction = new MVEventAction();
    SetUserAction(eventAction);
    SetUserAction(new MVSteppingAction(eventAction));
}