#include "MVActionInitializer.hh"
#include "MVPrimaryGeneratorAction.hh"
#include "MVRunAction.hh"
#include "MVEventAction.hh"
#include "MVSteppingAction.hh"
#include "MVMuonGenerator.hh"
#include "MVGammaGenerator.hh"

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
    switch (fConfig.generator)
    {
    case DEFAULT:
        SetUserAction(new MVPrimaryGeneratorAction(fConfig));
        break;
    case MUON:
        SetUserAction(new MVMuonGenerator);
        break;
    case GAMMA:
        SetUserAction(new MVGammaGenerator);
    default:
        break;
    }
    
    SetUserAction(new MVRunAction(fConfig));

    auto eventAction = new MVEventAction(fConfig);
    SetUserAction(eventAction);
    SetUserAction(new MVSteppingAction(eventAction));
}