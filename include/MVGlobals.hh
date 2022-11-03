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
template<class datatype>
class MVSingleCounter
{
    public:
        MVSingleCounter(G4String name, G4String description, G4bool hasRecorder=true): fName(name), fDescription(description), fHasRecorder(hasRecorder){}
        ~MVSingleCounter(){}
        void UpdateRecorder(G4int trackID, datatype data)
        {
            if(std::find(fDataList.begin(), fDataList.end(), data) == fDataList.end())
                fDataList.push_back(data);
            G4int dataIndex = std::distance(fDataList.begin(), std::find(fDataList.begin(), fDataList.end(), data));
            fRecorder[trackID] = dataIndex;
        }
        void UpdateCounter()
        {
            assert(fHasRecorder);
            for (datatype data : fDataList)
                fCounter[data] = 0;
            for (std::pair<G4int, G4int> pair : fRecorder)
                ++fCounter[fDataList[pair.second]];
        }
        void UpdateCounter(std::map<datatype, G4int> counter)
        {
            assert(!fHasRecorder);
            fCounter = counter;
        }
        inline G4String GetName() const { return fName; }
        inline G4String GetDescription() const { return fDescription; }
        inline std::map<G4int, G4int> GetRecorder() const { return fRecorder; }
        inline std::map<datatype, G4int> GetCounter() const { return fCounter; }
    private:
        const G4String fName;
        const G4String fDescription;
        const G4bool fHasRecorder;
        std::map<datatype, G4int> fCounter;
        std::map<G4int, G4int> fRecorder;
        std::vector<datatype> fDataList;
};

template<class datatype>
class MVGlobalCounter
{
    public:
        MVGlobalCounter(G4String name, G4String description): fName(name), fDescription(description){}
        MVGlobalCounter(const MVSingleCounter<datatype>& singleCounter)
        {
            fEntries = 1;
            for (std::pair<datatype, G4int> singleCounterItem : singleCounter.GetCounter())
            {
                fGlobalCounter[singleCounterItem.first].push_back(singleCounterItem.second);
            }
        }
        
        ~MVGlobalCounter(){}
        
        inline bool operator== (const MVGlobalCounter<datatype>& gc) const
        {
            return this->fName == gc.fName && this->fDescription == gc.fDescription;
        }

        void Merge(MVGlobalCounter<datatype>& anotherCounter)
        {
            assert(*this == anotherCounter);

            // First, search for all "this" keys in anotherCounter. If exists, push_back a value; if not, push_back 0.
            for(std::pair<datatype, std::vector<G4int>> thisCounterItem : fGlobalCounter)
            {
                if(anotherCounter.fGlobalCounter.find(thisCounterItem.first) != anotherCounter.fGlobalCounter.end())
                    fGlobalCounter[thisCounterItem.first].insert(
                        fGlobalCounter[thisCounterItem.first].end(),
                        anotherCounter.fGlobalCounter[thisCounterItem.first].begin(),
                        anotherCounter.fGlobalCounter[thisCounterItem.first].end()
                    );
                else
                    fGlobalCounter[thisCounterItem.first].insert(
                        fGlobalCounter[thisCounterItem.first].end(),
                        anotherCounter.fEntries, 0
                    );
            }
            // Second, search for all SingleCounter keys in GlobalCounter. If a key doesn't exist, create one and insert lots of 0s.
            for(std::pair<datatype, std::vector<G4int>> anotherCounterItem : anotherCounter.fGlobalCounter)
            {
                if(fGlobalCounter.find(anotherCounterItem.first) == fGlobalCounter.end())
                {
                    fGlobalCounter[anotherCounterItem.first].insert(
                        fGlobalCounter[anotherCounterItem.first].end(),
                        fEntries, 0
                    );
                    fGlobalCounter[anotherCounterItem.first].insert(
                        fGlobalCounter[anotherCounterItem.first].end(),
                        anotherCounter.fGlobalCounter[anotherCounterItem.first].begin(),
                        anotherCounter.fGlobalCounter[anotherCounterItem.first].end()
                    );
                }
            }
            fEntries += anotherCounter.fEntries;
        }
        void AppendSingle(const MVSingleCounter<datatype>& singleCounter)
        {
            assert(singleCounter.GetName() == this->fName && singleCounter.GetDescription() == this->fDescription);
            MVGlobalCounter<datatype> newGlobalCounter(singleCounter);
            Merge(newGlobalCounter);
        }
        
        inline G4String GetName() const { return fName; }
        inline G4String GetDescription() const { return fDescription; }
        inline std::map<datatype, std::vector<G4int>> GetGlobalCounter() const { return fGlobalCounter; }
        inline G4int GetEntries() const { return fEntries; }
    private:
        const G4String fName;
        const G4String fDescription;
        G4int fEntries = 0;
        std::map<datatype, std::vector<G4int>> fGlobalCounter;
};

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

/* Random Mode
*/
void RunRandom(const Config config);

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