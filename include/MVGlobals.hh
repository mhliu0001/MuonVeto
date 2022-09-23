#ifndef MVGlobals_h
#define MVGlobals_h 1

#include <map>
#include <vector>
#include <algorithm>
#include "globals.hh"
#include <filesystem>
#include "json.hpp"

namespace MuonVeto
{
/* JSON support 
*/
using nlohmann::json;

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

/* Config
*/
struct Config
{
    G4bool useBuiltinAnalysis;    // Use G4AnalysisManager to create histograms; use "-b" to enable
    G4String macro;               // Path for macro file; use "-m" to specify
    G4String outputFilePath;      // Directory where output files are put; use "-o" to specify
    G4int nThreads;               // Number of threads; use "-t" to specify
    G4int fiberCount;             // Number of fibers; use "-f" to specify
    G4int runID;                  // RunID; use "-n" to specify
    G4String probeConfigFilePath; // Probe mode; a probe_config must be provided after "-p"
    G4bool spectrumAnalysis;      // Spectrum analysis; use "-s" to enable
    G4bool randomPoints;          // Use random points as gun position; use "-r" to enable
    G4int eventPerRun;            // Number of events per run; use "-e" to specify
};

void PrintUsage();

bool isNumber(const char* str);

Config ParseConfig(int argc, char** argv);

/* Probe Mode
*/
void RunProbe(const Config config);

// Find the string's index in a STRLIST
inline G4int GetIndexOfString(const G4String& str, const STRLIST &strList)
{
    return std::distance(strList.begin(), std::find(strList.begin(), strList.end(), str));
}

// Judge whether a string is in the STRLIST
inline bool IsStringInList(const G4String& str, const STRLIST &strList)
{
    //return std::binary_search(strList.begin(), strList.end(), str);
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