#include "MVEventInformation.hh"

using namespace MuonVeto;

MVEventInformation::MVEventInformation(): genInfo("MVGeneratorInformation"), trackLength("trackLength"), Edep("Edep")
{
    counters.push_back(MVSingleCounter<G4String>("CPNCounter", "Creator Process Name list", true));
    counters.push_back(MVSingleCounter<G4String>("FVPathCounter", "Final Volume Path list", true));
    counters.push_back(MVSingleCounter<G4String>("EPNCounter", "Ending Process Name list", true));
    counters.push_back(MVSingleCounter<G4String>("SiPMPhotonCounter", "SiPM Photon Count", false));
    trackLength.constant = 0;
    Edep.constant = 0;
}

MVEventInformation::~MVEventInformation()
{
}

void MVEventInformation::Print() const
{
    for (auto counter : counters)
    {
        G4cout << ">>> " << counter.GetDescription() << ": " << G4endl;
        for (auto it : counter.GetCounter())
        {
            G4cout << "    " << it.first << ": " << it.second << G4endl;
        }
    }
    G4cout << ">>> Track Length: " << trackLength.constant/cm << "cm" << G4endl;
    G4cout << ">>> Energy Deposition: " << Edep.constant/MeV << "MeV" << G4endl; 
}