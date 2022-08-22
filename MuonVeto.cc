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
#include "MVGlobals.hh"

using namespace MuonVeto;

int main(int argc, char** argv)
{
    G4UIExecutive* ui = nullptr;

    // Parse arguments
    Config config;
    try
    {
        config = ParseConfig(argc, argv);
    }
    catch(const G4String errorMessage)
    {
        G4cerr << errorMessage << G4endl;
        PrintUsage();
        return 1;
    }
    
    // Random seed
    CLHEP::HepRandom::setTheEngine(new CLHEP::MixMaxRng());
    G4long seed = time(NULL);
    CLHEP::HepRandom::setTheSeed(seed);

    // Interactive mode
    if(!config.macro.size() && !config.probeConfigFilePath.size())
        ui = new G4UIExecutive(argc, argv);

    // Run manager
    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::MT);
    runManager->SetNumberOfThreads(config.nThreads);

    // Detector construction
    auto* detector = new MVDetectorConstruction();
    runManager->SetUserInitialization(detector);

    // Physics list
    auto* physicsList = new FTFP_BERT;
    auto* opticalPhysics = new G4OpticalPhysics();
    physicsList->RegisterPhysics(opticalPhysics);
    runManager->SetUserInitialization(physicsList);

    //auto* opticalPara = G4OpticalParameters::Instance();
    //opticalPara->SetProcessActivation("Cerenkov", false);

    // Action initializer
    runManager->SetUserInitialization(new MVActionInitializer(config));

    runManager->Initialize();

    // Visualization manager construction
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // Start processing run
    if ( ! ui ) 
    {
        if(config.probeConfigFilePath.size())
            RunProbe(config);

        else
        {
            // batch mode
            G4String command = "/control/execute ";
            UImanager->ApplyCommand(command+config.macro);
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