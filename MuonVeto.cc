#include "G4RunManager.hh"
#include "G4RunManagerFactory.hh"
#include "MVDetectorConstruction.hh"
#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "MVActionInitializer.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "time.h"

using namespace MuonVeto;

int main(int argc, char** argv)
{
    G4UIExecutive* ui = nullptr;
    if ( argc == 1 ) { ui = new G4UIExecutive(argc, argv); }

    // Random seed
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
    G4long seed = time(NULL);
    CLHEP::HepRandom::setTheSeed(seed);

    // Run manager
    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

    // Detector construction
    auto* detector = new MVDetectorConstruction();
    runManager->SetUserInitialization(detector);

    // Physics list
    auto* physicsList = new FTFP_BERT;
    auto* opticalPhysics = new G4OpticalPhysics();
    physicsList->RegisterPhysics(opticalPhysics);
    runManager->SetUserInitialization(physicsList);

    // Action initializer
    runManager->SetUserInitialization(new MVActionInitializer);

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
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command+fileName);
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