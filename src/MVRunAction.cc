#include "MVRunAction.hh"
#include "G4Run.hh"

namespace MuonVeto
{

MVRunAction::MVRunAction(G4int SiPMCount): fSiPMCount(SiPMCount)
{
    std::vector<G4int>* SiPMPhotonCount = new std::vector<G4int>[SiPMCount];
    fSiPMPhotonCount = SiPMPhotonCount;
}

MVRunAction::~MVRunAction()
{
    delete[] fSiPMPhotonCount;
}

void MVRunAction::BeginOfRunAction(const G4Run* aRun)
{
    ClearSiPMPhotonCount();
}

void MVRunAction::EndOfRunAction(const G4Run* aRun)
{
    if(isMaster)
    {
        G4cout << G4endl << "--------------------End of Global Run-----------------------" << G4endl;
    }
    else
    {
        G4cout << G4endl << "--------------------End of Local Run-----------------------" <<  G4endl;
    }
}

void MVRunAction::AddSiPMPhotonCount(const G4int* photonCount)
{
    for(int SiPMNb = 0; SiPMNb < fSiPMCount; SiPMNb++)
    {
        fSiPMPhotonCount[SiPMNb].push_back(photonCount[SiPMNb]);
    }
}

void MVRunAction::ClearSiPMPhotonCount()
{
    for(int index = 0; index < fSiPMCount; index++)
    {
        fSiPMPhotonCount[index].clear();
    }
}

}