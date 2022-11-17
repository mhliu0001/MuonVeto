#include "MVSpecialModes.hh"

#include "MVGlobals.hh"
#include <fstream>
#include "G4UImanager.hh"
#include "G4RunManager.hh"

using namespace MuonVeto;

void MuonVeto::RunProbe(Config config)
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
    if(runNumber != 0 && config.runID == -1)
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
    runManager->SetRunIDCounter(config.runID == -1 ? runNumber : config.runID);

    G4int local_runID = 0;

    // Start the probe
    for(; GunXPosition < (GunXMax+GunXStep/10); GunXPosition += GunXStep)
    {
        for(; GunYPosition < (GunYMax+GunYStep/10); GunYPosition += GunYStep)
        {
            for(; GunZPosition < (GunZMax+GunZStep/10); GunZPosition += GunZStep)
            {
                ++local_runID;
                if(config.runID == -1 || (config.runID != -1 && local_runID == config.runID+1))
                {
                    std::ostringstream commandPositionOS;
                    commandPositionOS << "/gun/position " << GunXPosition/cm << " " << GunYPosition/cm << " " << GunZPosition/cm << " " << "cm";
                    UImanager->ApplyCommand(commandPositionOS.str());
                    std::ostringstream commandRunOS;
                    commandRunOS << "/run/beamOn " << runCount;
                    UImanager->ApplyCommand(commandRunOS.str());
                    if(config.runID != -1)  return;
                }
            }
            GunZPosition = GunZMin;
        }
        GunYPosition = GunYMin;
    }
}

void MuonVeto::RunRandom(Config config)
{
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    auto* runManager = G4RunManager::GetRunManager();
    UImanager->ApplyCommand("/run/initialize");
    UImanager->ApplyCommand("/run/verbose 0");
    UImanager->ApplyCommand("/event/verbose 0");
    UImanager->ApplyCommand("/tracking/verbose 0");
    UImanager->ApplyCommand("/gun/particle alpha");
    UImanager->ApplyCommand("/gun/energy 1 keV");
    UImanager->ApplyCommand("/detector/scintYield 40000 /keV");
    runManager->SetRunIDCounter(config.runID == -1 ? 0 : config.runID);

    std::ostringstream commandRunOS;
    commandRunOS << "/run/beamOn " << config.eventPerRun;
    UImanager->ApplyCommand(commandRunOS.str());
}
