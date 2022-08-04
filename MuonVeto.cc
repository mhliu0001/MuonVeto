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
#include "time.h"
#include <filesystem>
#include <sstream>

namespace {
    void PrintUsage() {
        G4cerr << " Usage: " << G4endl;
        G4cerr << " MuonVeto [-m macro] [-t nThreads] [-o output_file_path] [-b]"
               << G4endl;
        G4cerr << " -m : Specify macro file" << G4endl;
        G4cerr << " -t : Specify number of threads (default: 8)" << G4endl;
        G4cerr << " -o : Specify where the data files are located (default: data)" << G4endl;
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
    if (argc >= 8)
    {
        PrintUsage();
        return 1;
    }
    G4bool useBuiltinAnalysis = false;
    G4String macro;
    G4String outputFilePath = "data";
    G4int nThreads = 8;
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
    if(!macro.size())
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

    /*
    auto* opticalPara = G4OpticalParameters::Instance();
    opticalPara->SetProcessActivation("Scintillation", false);
    */

    // Action initializer
    runManager->SetUserInitialization(new MVActionInitializer(outputFilePath, useBuiltinAnalysis));

    runManager->Initialize();

    // Visualization manager construction
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // Process macro or start UI session
    //
    if ( ! ui ) 
    {
        // batch mode
        G4String command = "/control/execute ";
        UImanager->ApplyCommand(command+macro);
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