#include "MVRunAction.hh"
#include "G4Run.hh"
#include "MVRunMT.hh"
#include <numeric>
#include <filesystem>
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"

namespace MuonVeto
{

MVRunAction::MVRunAction(const Config& config): 
    fConfig(config)
{
}

MVRunAction::~MVRunAction()
{
}

G4Run *MVRunAction::GenerateRun()
{
    return new MVRunMT();
}

void MVRunAction::BeginOfRunAction(const G4Run *)
{
}

void MVRunAction::EndOfRunAction(const G4Run *aRun)
{
    if (isMaster)
    {
        G4cout << G4endl << "--------------------End of Global Run-----------------------" << G4endl;

        MVRunMT *MTRun = (MVRunMT *)aRun;

        // Run Conditions
        G4int eventCount = MTRun->runCounters[0].GetEntries();
        G4cout << "This run consists of " << eventCount << " events." << G4endl;
        switch (fConfig.generator)
        {
        case DEFAULT:
            G4cout << "Generator: Default" << G4endl;
            if(fConfig.randomPoints)
            {
                G4cout << "Particle: " << MTRun->genInfos.GetVector()[0].name << G4endl;
                G4cout << "Energy: " << MTRun->genInfos.GetVector()[0].energy/MeV << "MeV" << G4endl;
                G4cout << "Position: Random" << G4endl;
            }
            else
            {
                G4cout << "Particle: " << MTRun->genInfos.GetVector()[0].name << G4endl;
                G4cout << "Energy: " << MTRun->genInfos.GetVector()[0].energy/MeV << "MeV" << G4endl;
                G4cout << "Position:" << MTRun->genInfos.GetVector()[0].position/mm << "mm" << G4endl;
            }
            break;
        
        case GAMMA:
            G4cout << "Generator: gamma" << G4endl;
            break;
        case MUON:
            G4cout << "Generator: muon" << G4endl;
            break;
        }
        
        // Data output
        std::stringstream optDirStream;
        optDirStream << fConfig.outputFilePath << "/run" << aRun->GetRunID();
        std::filesystem::create_directories(optDirStream.str());

        // Analysis
        if(fConfig.useBuiltinAnalysis)
            CreateHists(MTRun);

        // Run Conditions Output  
        json runConditions;
        runConditions["RunID"] = aRun->GetRunID();
        runConditions["NumberOfEvents"] = eventCount;
        runConditions["Generator"] = int(fConfig.generator);
        runConditions["RandomPoints"] = fConfig.randomPoints;

        std::stringstream RCFileNameStream;
        RCFileNameStream << optDirStream.str() << "/RunConditions.json";
        std::ofstream RCFileStream(RCFileNameStream.str());
        if(RCFileStream.is_open())
            RCFileStream << runConditions.dump(4);
        else
            G4cerr << "Open File \"" << RCFileNameStream.str() << "\" Failed!";
        RCFileStream.close();

        // csv output
        for(auto runCounter : MTRun->runCounters)
        {
            std::stringstream counterDirStream;
            counterDirStream << optDirStream.str() << "/" << runCounter.GetName();
            std::filesystem::create_directory(counterDirStream.str());

            for(auto nameAndValue: runCounter.GetGlobalCounter())
            {
                std::stringstream counterFileNameStream;
                counterFileNameStream << counterDirStream.str() << "/" << nameAndValue.first << ".csv";
                std::ofstream counterFileStream(counterFileNameStream.str());
                if(!counterFileStream.is_open())
                {   
                    G4cerr << "Open File \"" << counterFileNameStream.str() << "\" Failed!" << G4endl;
                    continue;
                }
                counterFileStream << "# " << counterDirStream.str() << ": " << nameAndValue.first <<std::endl;
                for(auto value : nameAndValue.second)
                    counterFileStream << value << std::endl;
                counterFileStream.close();
            }
        }

        // GenInfo csv output
        auto genInfos = MTRun->genInfos;
        std::stringstream GenDirStream;
        GenDirStream << optDirStream.str() << "/" << genInfos.GetName();
        std::filesystem::create_directory(GenDirStream.str());

        {
            std::stringstream GenFileNameStream;
            GenFileNameStream << GenDirStream.str() << "/" << "phi.csv";
            std::ofstream GenFileStream(GenFileNameStream.str());
            if(!GenFileStream.is_open())
            {   
                G4cerr << "Open File \"" << GenFileNameStream.str() << "\" Failed!" << G4endl;
            }
            else
            {
                GenFileStream << "# " << GenDirStream.str() << ": " << "phi" << std::endl;
                for(auto genInfo : genInfos.GetVector())
                    GenFileStream << genInfo.phi << std::endl;
                GenFileStream.close();
            }
        }
        {
            std::stringstream GenFileNameStream;
            GenFileNameStream << GenDirStream.str() << "/" << "theta.csv";
            std::ofstream GenFileStream(GenFileNameStream.str());
            if(!GenFileStream.is_open())
            {   
                G4cerr << "Open File \"" << GenFileNameStream.str() << "\" Failed!" << G4endl;
            }
            else
            {
                GenFileStream << "# " << GenDirStream.str() << ": " << "theta" << std::endl;
                for(auto genInfo : genInfos.GetVector())
                    GenFileStream << genInfo.theta << std::endl;
                GenFileStream.close();
            }
        }
        {
            std::stringstream GenFileNameStream;
            GenFileNameStream << GenDirStream.str() << "/" << "energy.csv";
            std::ofstream GenFileStream(GenFileNameStream.str());
            if(!GenFileStream.is_open())
            {   
                G4cerr << "Open File \"" << GenFileNameStream.str() << "\" Failed!" << G4endl;
            }
            else
            {
                GenFileStream << "# " << GenDirStream.str() << ": " << "energy" << std::endl;
                for(auto genInfo : genInfos.GetVector())
                    GenFileStream << genInfo.energy << std::endl;
                GenFileStream.close();
            }
        }
        {
            std::stringstream GenFileNameStream;
            GenFileNameStream << GenDirStream.str() << "/" << "name.csv";
            std::ofstream GenFileStream(GenFileNameStream.str());
            if(!GenFileStream.is_open())
            {   
                G4cerr << "Open File \"" << GenFileNameStream.str() << "\" Failed!" << G4endl;
            }
            else
            {
                GenFileStream << "# " << GenDirStream.str() << ": " << "name" << std::endl;
                for(auto genInfo : genInfos.GetVector())
                    GenFileStream << genInfo.name << std::endl;
                GenFileStream.close();
            }
        }
        {
            std::stringstream GenFileNameStream;
            GenFileNameStream << GenDirStream.str() << "/" << "position.csv";
            std::ofstream GenFileStream(GenFileNameStream.str());
            if(!GenFileStream.is_open())
            {   
                G4cerr << "Open File \"" << GenFileNameStream.str() << "\" Failed!" << G4endl;
            }
            else
            {
                GenFileStream << "# " << GenDirStream.str() << ": " << "position" << std::endl;
                for(auto genInfo : genInfos.GetVector())
                    GenFileStream << genInfo.position.x() << "," << genInfo.position.y() << "," << genInfo.position.z() << std::endl;
                GenFileStream.close();
            }
        }
        

        // TODO: Fix spectrum
        /*
        // Spectrum output
        if(fConfig.spectrumAnalysis)
        {
            std::stringstream spectrumDirStream;
            spectrumDirStream << optDirStream.str() << "/Spectrum";
            std::filesystem::create_directory(spectrumDirStream.str());
    
            for(auto singleProcess : MTRun->GetProcessSpectrum())
            {
                std::stringstream spectrumFileNameStream;
                spectrumFileNameStream << spectrumDirStream.str() << "/" << strList[singleProcess.first] << ".csv";
                std::ofstream spectrumFileStream(spectrumFileNameStream.str());
                if(!spectrumFileStream.is_open())
                {   
                    G4cerr << "Open File \"" << spectrumFileNameStream.str() << "\" Failed!" << G4endl;
                    continue;
                }
                spectrumFileStream << "# " << "Spectrum: " << strList[singleProcess.first] << std::endl;
                for(auto it : singleProcess.second)
                    spectrumFileStream << it/eV << std::endl;
                spectrumFileStream.close();
            }
        }
        */
    }
    else
    {
        G4cout << G4endl << "--------------------End of Local Run-----------------------" << G4endl;
    }
}

void MVRunAction::CreateHists(const MVRunMT* MTRun) const
{
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(1);

    // titleMap maps H1ID to title
    std::map<G4int, G4String> titleMap;
    
    // These are maps aiming to deal with xmax and xmin properly.
    // The key is the H1ID, and the value is the maximum/minimum
    std::map<G4int, G4int> histXMax, histXMin;

    // Get all maps
    int H1ID = 0;
    for(int counter_index = 0; counter_index < 4; ++counter_index)
    {
        auto globalCounter = MTRun->runCounters[counter_index];
        for(auto nameAndCounter : globalCounter.GetGlobalCounter())
        {
            titleMap[H1ID] = G4String(globalCounter.GetDescription() + G4String(":") + G4String(nameAndCounter.first));
            for(auto count : nameAndCounter.second)
            {
                if(histXMax.find(H1ID) == histXMax.end())
                {
                    histXMax[H1ID] = count;
                    histXMin[H1ID] = count;
                }
                else
                {
                    if(histXMax[H1ID] < count)
                        histXMax[H1ID] = count;
                    if(histXMin[H1ID] > count)
                        histXMin[H1ID] = count;
                }
            }
            ++H1ID;
        }
    }

    // Create histograms
    for(G4int local_h1id = 0; local_h1id < H1ID; ++local_h1id)
    {
        analysisManager->CreateH1(
            std::to_string(local_h1id),
            titleMap[local_h1id],
            50,
            histXMin[local_h1id]/50*50,
            (histXMax[local_h1id]/50+1)*50
        );
        analysisManager->SetH1Plotting(local_h1id, true);
    }

    // Fill histograms
    H1ID = 0;
    for(int counter_index = 0; counter_index < 4; ++counter_index)
    {
        auto globalCounter = MTRun->runCounters[counter_index];
        for(auto nameAndCounter : globalCounter.GetGlobalCounter())
        {
            for(auto count : nameAndCounter.second)
            {
                analysisManager->FillH1(
                    H1ID,
                    (double)count
                );
            }
            ++H1ID;
        }
    }

    // Analysis Manager Output
    std::stringstream optDirStream;
    optDirStream << fConfig.outputFilePath << "/run" << MTRun->GetRunID();
    std::stringstream dirNameForBAStream;
    dirNameForBAStream << optDirStream.str() << "/" << "built-in";
    std::filesystem::create_directory(dirNameForBAStream.str());

    std::stringstream BAFileNameStream;
    BAFileNameStream << dirNameForBAStream.str() << "/data.csv";
    analysisManager->OpenFile(BAFileNameStream.str());
    analysisManager->Write();
    analysisManager->CloseFile();
}

}