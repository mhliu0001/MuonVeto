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
        G4cerr << " MuonVeto [-m macro] [-t nThreads] [-o output_file_path] [-b] [-p]"
               << G4endl;
        G4cerr << " -m : Specify macro file" << G4endl;
        G4cerr << " -t : Specify number of threads (default: 8)" << G4endl;
        G4cerr << " -o : Specify where the data files are located (default: data)" << G4endl;
        G4cerr << " -b : Use G4 built-in analysis" << G4endl;
        G4cerr << " -p : Probe mode (if this is specified, \"-m\" is ignored)" << G4endl; 
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
    if (argc >= 10)
    {
        PrintUsage();
        return 1;
    }
    G4bool useBuiltinAnalysis = false;
    G4String macro;
    G4String outputFilePath = "data";
    G4int nThreads = 8;
    G4bool probe = false;
    int argN = 1;
    while (argN < argc)
    {
        if(G4String(argv[argN]) == "-m")
        {
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
            outputFilePath = argv[argN+1];
            argN += 2;
        }
        else if(G4String(argv[argN]) == "-b")
        {
            useBuiltinAnalysis = true;
            argN += 1;
        }
        else if(G4String(argv[argN]) == "-p")
        {
            probe = true;
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

            // Determine gun position
            G4double GunZPosition = -45*cm;
            G4double GunXPosition = -4.5*cm;
            G4double stepOfZ = 5*cm;
            G4double stepOfX = 0.5*cm;

            // Read from last RunConditions.json
            if(runNumber != 0)
            {
                std::ostringstream pathOS;
                pathOS << outputFilePath << "/" << "run" << runNumber-1 << "/RunConditions.json";
                G4String path = pathOS.str();

                std::ifstream runConditionsFS(path);
                json runConditions = json::parse(runConditionsFS);
                GunXPosition = G4double(runConditions["GunXPosition/cm"]) * cm;
                GunZPosition = G4double(runConditions["GunZPosition/cm"]) * cm + stepOfZ;
            }

            // Initialize
            UImanager->ApplyCommand("/run/initialize");
            UImanager->ApplyCommand("/run/verbose 0");
            UImanager->ApplyCommand("/event/verbose 0");
            UImanager->ApplyCommand("/tracking/verbose 0");
            UImanager->ApplyCommand("/gun/particle alpha");
            UImanager->ApplyCommand("/gun/energy 1 keV");
            runManager->SetRunIDCounter(runNumber);

            if(GunZPosition > 45.1*cm) GunXPosition += stepOfX;
            for(GunXPosition = (GunXPosition < 4.51*cm ? GunXPosition: -4.5*cm); GunXPosition < 4.51*cm; GunXPosition += stepOfX)
            {
                for(GunZPosition = (GunZPosition < 45.1*cm ? GunZPosition: -45*cm); GunZPosition < 45.1*cm; GunZPosition += stepOfZ)
                {
                    std::ostringstream commandOS;
                    commandOS << "/gun/position " << GunXPosition/cm << " " << 0 << " " << GunZPosition/cm << " " << "cm";
                    UImanager->ApplyCommand(commandOS.str());
                    UImanager->ApplyCommand("/run/beamOn 1000");
                }
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