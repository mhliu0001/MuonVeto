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

using namespace MuonVeto;

int main(int argc, char** argv)
{
    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

    auto* detector = new MVDetectorConstruction();
    runManager->SetUserInitialization(detector);

    auto* physicsList = new FTFP_BERT;
    auto* opticalPhysics = new G4OpticalPhysics();
    physicsList->RegisterPhysics(opticalPhysics);
    runManager->SetUserInitialization(physicsList);

    runManager->SetUserInitialization(new testActionInitializer);

    runManager->Initialize();

    // Visualization manager construction
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    UImanager->ApplyCommand("/control/execute macro/init_vis.mac");

    ui->SessionStart();
    delete ui;
    
    delete visManager;
    delete runManager;
    return 0;
}