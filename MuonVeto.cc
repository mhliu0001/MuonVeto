#include "G4RunManager.hh"
#include "G4RunManagerFactory.hh"
#include "MVDetectorConstruction.hh"
#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalParametersMessenger.hh"
#include "MVActionInitializer.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4SystemOfUnits.hh"
#include "json.hpp"
#include "time.h"
#include <filesystem>
#include <sstream>
#include <fstream>

using json = nlohmann::json;

namespace {
    void PrintUsage() {
        G4cerr << " Usage: " << G4endl;
        G4cerr << " MuonVeto [-m macro] [-t nThreads] [-o output_file_path] [-p probe_config_file][-b]"
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
}

using namespace MuonVeto;

int main(int argc, char** argv)
{
    G4UIExecutive* ui = nullptr;

    // Parse arguments
    if (argc >= 11)
    {
        PrintUsage();
        return 1;
    }
    G4bool useBuiltinAnalysis = false;
    G4String macro;
    G4String outputFilePath = "data";
    G4int nThreads = 8;
    G4bool probe = false;
    G4String probeConfigFilePath;
    int argN = 1;
    while (argN < argc)
    {
        if(G4String(argv[argN]) == "-m")
        {
            if(argN+1 >= argc)
            {
                G4cerr << "A macro file must be specified after \"-m\"!" << G4endl;
                return 1;
            }
            macro = argv[argN+1];
            if(!std::filesystem::is_regular_file(macro.c_str()))
            {
                G4cerr << "Macro file not found! Given path: " << macro << G4endl;
                return 1;
            }
            argN += 2;
        }
        else if(G4String(argv[argN]) == "-t")
        {
            if(argN+1 >= argc)
            {
                G4cerr << "nThreads must be specified after \"-t\"!" << G4endl;
                return 1;
            }
            if(!isNumber(argv[argN+1]))
            {
                G4cerr << "nThreads must be an integer, but " << argv[argN+1] << " is given" << G4endl;
                return 1;
            }
            nThreads = G4UIcommand::ConvertToInt(argv[argN+1]);
            argN += 2;
        }
        else if(G4String(argv[argN]) == "-o")
        {
            if(argN+1 >= argc)
            {
                G4cerr << "An output file path must be specified after \"-o\"!" << G4endl;
                return 1;
            }
            outputFilePath = argv[argN+1];
            argN += 2;
        }
        else if(G4String(argv[argN]) == "-p")
        {
            if(argN+1 >= argc)
            {
                G4cerr << "A probe config file must be specified after \"-p\"!" << G4endl;
                return 1;
            }
            probe = true;
            probeConfigFilePath = argv[argN+1];
            if(!std::filesystem::is_regular_file(probeConfigFilePath.c_str()))
            {
                G4cerr << "Probe config file not found! Given path: " << probeConfigFilePath << G4endl;
                return 1;
            }
            argN += 2;
        }
        else if(G4String(argv[argN]) == "-b")
        {
            useBuiltinAnalysis = true;
            argN += 1;
        }
        else
        {
            PrintUsage();
            return 1;
        }
    }

    // Random seed
    CLHEP::HepRandom::setTheEngine(new CLHEP::MixMaxRng());
    G4long seed = time(NULL);
    CLHEP::HepRandom::setTheSeed(seed);

    // Interactive mode
    if(!macro.size() && !probe)
        ui = new G4UIExecutive(argc, argv);

    // Run manager
    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::MT);
    runManager->SetNumberOfThreads(nThreads);

    // Detector construction
    auto* detector = new MVDetectorConstruction();
    runManager->SetUserInitialization(detector);

    // Physics list
    auto* physicsList = new FTFP_BERT;
    auto* opticalPhysics = new G4OpticalPhysics();
    physicsList->RegisterPhysics(opticalPhysics);
    runManager->SetUserInitialization(physicsList);

    auto* opticalPara = G4OpticalParameters::Instance();
    opticalPara->SetProcessActivation("Cerenkov", false);

    // Action initializer
    runManager->SetUserInitialization(new MVActionInitializer(outputFilePath, useBuiltinAnalysis));

    runManager->Initialize();

    // Visualization manager construction
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // Start processing run
    if ( ! ui ) 
    {
        if(probe)
        {
            // Read probe config file
            std::ifstream probeConfigFS(probeConfigFilePath);
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
                pathOS << outputFilePath << "/" << "run" << runNumber << "/RunConditions.json";
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
                pathOS << outputFilePath << "/" << "run" << runNumber-1 << "/RunConditions.json";
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
            UImanager->ApplyCommand("/run/initialize");
            UImanager->ApplyCommand("/run/verbose 0");
            UImanager->ApplyCommand("/event/verbose 0");
            UImanager->ApplyCommand("/tracking/verbose 0");
            UImanager->ApplyCommand("/gun/particle alpha");
            UImanager->ApplyCommand("/gun/energy 1 keV");
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

        else
        {
            // batch mode
            G4String command = "/control/execute ";
            UImanager->ApplyCommand(command+macro);
        }
    }
    else
    {
        // interactive mode
        UImanager->ApplyCommand("/control/execute ./macro/init_vis.mac");
        if (ui->IsGUI())
        {
            UImanager->ApplyCommand("/control/execute ./macro/gui.mac");
        }
        ui->SessionStart();
        delete ui;
    }
    
    delete visManager;
    delete runManager;
    return 0;
}