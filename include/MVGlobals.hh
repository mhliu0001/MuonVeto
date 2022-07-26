#ifndef MVGlobals_h
#define MVGlobals_h 1

#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include "globals.hh"

using SINGLE_COUNTER = std::map<G4int, G4int>;
using COUNTER = std::vector<SINGLE_COUNTER>;
using STRLIST = std::vector<G4String>;
using RECORDER = std::map<G4int, G4int>; // Key is trackID; Value is index of string

inline G4int GetIndexOfString(const G4String& str, const STRLIST strList)
{
    return std::distance(strList.begin(), std::find(strList.begin(), strList.end(), str));
}

inline bool IsStringInList(const G4String& str, const STRLIST strList)
{
    return std::find(strList.begin(), strList.end(), str) != strList.end();
}

#endif