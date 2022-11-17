#include "MVGlobals.hh"
#include "G4UImanager.hh"
#include "G4UIcommand.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

namespace MuonVeto
{

void PrintUsage()
{
    G4cerr << " Usage: " << G4endl;
    G4cerr << " MuonVeto [--gen generator][-m macro] [-t nThreads] [-o output_file_path] [-p probe_config_file] [-f fiber_count] [-n runID] [-e events_per_run] [-b] [-s] [-r]"
            << G4endl;
    G4cerr << " --gen : Specify generator. Can be \"default\", \"muon\" or \"gamma\". " << G4endl;
    G4cerr << " -m : Specify macro file" << G4endl;
    G4cerr << " -t : Specify number of threads (default: 8)" << G4endl;
    G4cerr << " -o : Specify where the data files are located (default: data)" << G4endl;
    G4cerr << " -p : Use probe mode. A probe config file (.json) must be specified" << G4endl;
    G4cerr << " -f : Specify fiber count (default: 6; can be 4 or 6)" << G4endl;
    G4cerr << " -n : Specify runID (useful in probe mode, if this is specified in probe mode, only ONE run is generated)" << G4endl;
    G4cerr << " -e : Specify events per run (default: 1000), used in random points (-r)" << G4endl;
    G4cerr << " -b : Use G4 built-in analysis" << G4endl;
    G4cerr << " -s : Enable spectrum analysis" << G4endl;
    G4cerr << " -r : Use random points as gun position" << G4endl;
}

bool isNumber(const char* str)
{
    std::istringstream sin(str);
    int test;
    return sin >> test && sin.eof();
}

Config ParseConfig(int argc, char** argv)
{
    Config config;

    // Default values
    config.generator = DEFAULT;
    config.useBuiltinAnalysis = false;
    config.outputFilePath = "data";
    config.nThreads = 8;
    config.fiberCount = 6;
    config.runID = -1;
    config.spectrumAnalysis = false;
    config.eventPerRun = 1000;
    config.randomPoints = false;

    // Parse from argument list
    int argN = 1;
    while (argN < argc)
    {
        if(G4String(argv[argN]) == "--gen")
        {
            if(argN+1 >= argc)
            {
                throw G4String("A generator must be specified after \"--gen\"!");
            }
            else if(G4String(argv[argN+1]) == "default")
            {
                config.generator = DEFAULT;
            }
            else if(G4String(argv[argN+1]) == "muon")
            {
                config.generator = MUON;
            }
            else if(G4String(argv[argN+1]) == "gamma")
            {
                config.generator = GAMMA;
            }
            else
            {
                throw G4String("Unrecognized generator " + G4String(argv[argN]));
            }
            argN += 2;
        }
        else if(G4String(argv[argN]) == "-m")
        {
            if(argN+1 >= argc)
            {
                throw G4String("A macro file must be specified after \"-m\"!");
            }
            config.macro = argv[argN+1];
            if(!std::filesystem::is_regular_file(config.macro.c_str()))
            {
                throw G4String("Macro file not found! Given path: " + config.macro);
            }
            argN += 2;
        }
        else if(G4String(argv[argN]) == "-t")
        {
            if(argN+1 >= argc)
            {
                throw G4String("nThreads must be specified after \"-t\"!");
            }
            if(!isNumber(argv[argN+1]))
            {
                throw G4String("nThreads must be an integer, but " + G4String(argv[argN+1]) + G4String(" is given"));
            }
            config.nThreads = G4UIcommand::ConvertToInt(argv[argN+1]);
            argN += 2;
        }
        else if(G4String(argv[argN]) == "-o")
        {
            if(argN+1 >= argc)
            {
                throw G4String("An output file path must be specified after \"-o\"!");
            }
            config.outputFilePath = argv[argN+1];
            argN += 2;
        }
        else if(G4String(argv[argN]) == "-p")
        {
            if(argN+1 >= argc)
            {
                throw G4String("A probe config file must be specified after \"-p\"!");
            }
            config.probeConfigFilePath = argv[argN+1];
            if(!std::filesystem::is_regular_file(config.probeConfigFilePath.c_str()))
            {
                throw G4String("Probe config file not found! Given path: " + config.probeConfigFilePath);
            }
            argN += 2;
        }
        else if(G4String(argv[argN]) == "-f")
        {
            if(argN+1 >= argc)
            {
                throw G4String("Fiber count must be specified after \"-f\"!");
            }
            if(G4String(argv[argN+1]) != std::to_string(4) && G4String(argv[argN+1]) != std::to_string(6))
            {
                throw G4String(G4String("Fiber count must be 4 or 6, but ") + G4String(argv[argN+1]) + G4String(" is specified!"));
            }
            config.fiberCount = G4UIcommand::ConvertToInt(argv[argN+1]);
            argN += 2;
        }
        else if(G4String(argv[argN]) == "-n")
        {
            if(argN+1 >= argc)
            {
                throw G4String("RunID must be specified after \"-n\"!");
            }
            if(!isNumber(argv[argN+1]))
            {
                throw G4String(G4String("RunID must be an integer, but ") + G4String(argv[argN+1]) + G4String(" is specified!"));
            }
            config.runID = G4UIcommand::ConvertToInt(argv[argN+1]);
            argN += 2;
        }
        else if(G4String(argv[argN]) == "-e")
        {
            if(argN+1 >= argc)
            {
                throw G4String("EventPerRun must be specified after \"-e\"!");
            }
            if(!isNumber(argv[argN+1]))
            {
                throw G4String(G4String("EventPerRun must be an positive integer, but ") + G4String(argv[argN+1]) + G4String(" is specified!"));
            }
            config.eventPerRun = G4UIcommand::ConvertToInt(argv[argN+1]);
            if(config.eventPerRun <= 0)
            {
                throw G4String(G4String("EventPerRun must be an positive integer, but ") + G4String(argv[argN+1]) + G4String(" is specified!"));
            }
            argN += 2;
        }
        else if(G4String(argv[argN]) == "-b")
        {
            config.useBuiltinAnalysis = true;
            argN += 1;
        }
        else if(G4String(argv[argN]) == "-s")
        {
            config.spectrumAnalysis = true;
            argN += 1;
        }
        else if(G4String(argv[argN]) == "-r")
        {
            config.randomPoints = true;
            argN += 1;
        }
        else
        {
            throw G4String("Unknown argument: " + G4String(argv[argN]));
        }
    }
    return config;
}

}