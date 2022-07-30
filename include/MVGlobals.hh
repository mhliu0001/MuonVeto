#ifndef MVGlobals_h
#define MVGlobals_h 1

#include <map>
#include <vector>
#include <algorithm>
#include "globals.hh"

namespace MuonVeto
{

/* COUNTERS are used to count how many photons are created in some process(CPNCounter),
** ended in some volume(FVPathCounter), ended with some process(EPNCounter), or detected
** by a SiPM(SiPMPhotonCounter).
** A SINGLE_COUNTER is for a single event, while a COUNTER is for multiple events.
** A SINGLE_COUNTER is a map with the key to be the index in STRLIST, and value to be the count.
** A COUNTER is just a vector of SINGLE_COUNTERs.
*/
using SINGLE_COUNTER = std::map<G4int, G4int>;
using COUNTER = std::vector<SINGLE_COUNTER>;

/* To save memory, STRLISTs are used. It is a vector of G4String.
** Using this can avoid G4Strings to be created multiple times.
*/
using STRLIST = std::vector<G4String>;

/* A RECORDER is used in MVSteppingAction. The key is the trackID, and the value is the
** key of COUNTERS (Scintillation, SiPM_0, etc.)
** It is only for a single event, and will be transformed to COUNTERS in MVEventAction.
*/
using RECORDER = std::map<G4int, G4String>;

// Find the string's index in a STRLIST
inline G4int GetIndexOfString(const G4String& str, const STRLIST &strList)
{
    return std::distance(strList.begin(), std::find(strList.begin(), strList.end(), str));
}

// Judge whether a string is in the STRLIST
inline bool IsStringInList(const G4String& str, const STRLIST &strList)
{
    // return std::binary_search(strList.begin(), strList.end(), str);
    return std::find(strList.begin(), strList.end(), str) != strList.end();
}

// This class is used by MapFindValue only, and should not be used elsewhere.
class int_map_finder
{
    public:
        int_map_finder(const G4int value) : fValue(value) {}
        inline bool operator() (const std::map<G4int, G4int>::value_type &pair) { return pair.second == fValue; }

    private:
        G4int fValue;
};

// Find according to the value, not the key.
inline auto MapFindValue(G4int value, std::map<G4int, G4int> &map)
{
    return std::find_if(map.begin(), map.end(), int_map_finder(value));
}

}

#endif