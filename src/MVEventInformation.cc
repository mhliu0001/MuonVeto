#include "MVEventInformation.hh"

using namespace MuonVeto;

MVEventInformation::MVEventInformation()
{
}

MVEventInformation::~MVEventInformation()
{
}

void MVEventInformation::Print() const
{
    G4cout << ">>> Creator Process Name list: " << G4endl;
    for (auto it : fCPNCounter)
    {
        G4cout << "    " << fStrList[it.first] << ": " << it.second << G4endl;
    }
    G4cout << ">>> Final Volume Path list: " << G4endl;
    for (auto it : fFVPathCounter)
    {
        G4cout << "    " << fStrList[it.first] << ": " << it.second << G4endl;
    }
    G4cout << ">>> Ending Process Name list: " << G4endl;
    for (auto it : fEPNCounter)
    {
        G4cout << "    " << fStrList[it.first] << ": " << it.second << G4endl;
    }
    G4cout << ">>> SiPM Photon Count: " << G4endl;
    for (auto it : fSiPMPhotonCounter)
    {
        G4cout << "    " << fStrList[it.first] << ": " << it.second << G4endl;
    }
}