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
    G4cerr << " MuonVeto [-m macro] [-t nThreads] [-o output_file_path] [-p probe_config_file] [-b]"
            << G4endl;
    G4cerr << " -m : Specify macro file" << G4endl;
    G4cerr << " -t : Specify number of threads (default: 8)" << G4endl;
    G4cerr << " -o : Specify where the data files are located (default: data)" << G4endl;
    G4cerr << " -p : Use probe mode. A probe config file (.json) must be specified" << G4endl; 
    G4cerr << " -b : Use G4 built-in analysis" << G4endl;
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
    config.useBuiltinAnalysis = false;
    config.outputFilePath = "data";
    config.nThreads = 8;

    // Parse from argument list
    int argN = 1;
    while (argN < argc)
    {
        if(G4String(argv[argN]) == "-m")
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
        else if(G4String(argv[argN]) == "-b")
        {
            config.useBuiltinAnalysis = true;
            argN += 1;
        }
        else
        {
            throw G4String("Unknown argument: " + G4String(argv[argN]));
        }
    }
    return config;
}

void RunProbe(Config config)
{
    // Read probe config file
    std::ifstream probeConfigFS(config.probeConfigFilePath);
    json probeConfig = json::parse(probeConfigFS);
    G4double GunXMin = probeConfig.contains("GunXMin/cm")? (G4double(probeConfig["GunXMin/cm"]) * cm): 0;
    G4double GunXMax = probeConfig.contains("GunXMax/cm")? (G4double(probeConfig["GunXMax/cm"]) * cm): 0;
    G4double GunXStep = probeConfig.contains("GunXStep/cm")? (G4double(probeConfig["GunXStep/cm"]) * cm): 100*cm;

    G4double GunYMin = probeConfig.contains("GunYMin/cm")? (G4double(probeConfig["GunYMin/cm"]) * cm): 0;
    G4double GunYMax = probeConfig.contains("GunYMax/cm")? (G4double(probeConfig["GunYMax/cm"]) * cm): 0;
    G4double GunYStep = probeConfig.contains("GunYStep/cm")? (G4double(probeConfig["GunYStep/cm"]) * cm): 100*cm;

    G4double GunZMin = probeConfig.contains("GunZMin/cm")? (G4double(probeConfig["GunZMin/cm"]) * cm): 0;
    G4double GunZMax = probeConfig.contains("GunZMax/cm")? (G4double(probeConfig["GunZMax/cm"]) * cm): 0;
    G4double GunZStep = probeConfig.contains("GunZStep/cm")? (G4double(probeConfig["GunZStep/cm"]) * cm): 100*cm;

    G4int runCount = probeConfig["Number of Runs"];

    // Search for data
    G4int runNumber = 0;
    while(true)
    {
        std::ostringstream pathOS;
        pathOS << config.outputFilePath << "/" << "run" << runNumber << "/RunConditions.json";
        G4String path = pathOS.str();
        if(!std::filesystem::exists(path.c_str()))  break;
        ++runNumber;
    }

    // Determine initial gun position
    G4double GunXPosition = GunXMin;
    G4double GunYPosition = GunYMin;
    G4double GunZPosition = GunZMin;

    // Read from last RunConditions.json
    if(runNumber != 0)
    {
        std::ostringstream pathOS;
        pathOS << config.outputFilePath << "/" << "run" << runNumber-1 << "/RunConditions.json";
        G4String path = pathOS.str();

        std::ifstream runConditionsFS(path);
        json runConditions = json::parse(runConditionsFS);
        GunXPosition = G4double(runConditions["GunXPosition/cm"]) * cm;
        GunYPosition = G4double(runConditions["GunYPosition/cm"]) * cm;
        GunZPosition = G4double(runConditions["GunZPosition/cm"]) * cm;

        GunZPosition += GunZStep;
        if(GunZPosition > GunZMax + GunZStep/10)
        {
            GunZPosition = GunZMin;
            GunYPosition += GunYStep;
            if(GunYPosition > GunYStep + GunYStep/10)
            {
                GunYPosition = GunYMin;
                GunXPosition += GunXStep;
            }
        }
    }

    // Initialize
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    auto* runManager = G4RunManager::GetRunManager();
    UImanager->ApplyCommand("/run/initialize");
    UImanager->ApplyCommand("/run/verbose 0");
    UImanager->ApplyCommand("/event/verbose 0");
    UImanager->ApplyCommand("/tracking/verbose 0");
    UImanager->ApplyCommand("/gun/particle alpha");
    UImanager->ApplyCommand("/gun/energy 1 keV");
    UImanager->ApplyCommand("/detector/scintYield 40000 /keV");
    runManager->SetRunIDCounter(runNumber);

    // Start the probe
    for(; GunXPosition < (GunXMax+GunXStep/10); GunXPosition += GunXStep)
    {
        for(; GunYPosition < (GunYMax+GunYStep/10); GunYPosition += GunYStep)
        {
            for(; GunZPosition < (GunZMax+GunZStep/10); GunZPosition += GunZStep)
            {
                std::ostringstream commandPositionOS;
                commandPositionOS << "/gun/position " << GunXPosition/cm << " " << GunYPosition/cm << " " << GunZPosition/cm << " " << "cm";
                UImanager->ApplyCommand(commandPositionOS.str());
                std::ostringstream commandRunOS;
                commandRunOS << "/run/beamOn " << runCount;
                UImanager->ApplyCommand(commandRunOS.str());
            }
            GunZPosition = GunZMin;
        }
        GunYPosition = GunYMin;
    }
}

}