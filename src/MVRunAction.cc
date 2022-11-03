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
        G4double particleEnergy = MTRun->GetParticleEnergy();
        std::vector<G4ThreeVector> particlePosition = MTRun->GetParticlePosition();
        G4String particleName = MTRun->GetParticleName();
        G4int eventCount = MTRun->runCounters[0].GetEntries();
        
        if(!fConfig.randomPoints)
        {
            G4cout << "This run consists of " << eventCount << " events of " << particleName << " with energy " << particleEnergy / GeV << " GeV"
                << " located at " << particlePosition[0] << G4endl;
        }
        else
        {
            G4cout << "This run consists of " << eventCount << " events of " << particleName << " with energy " << particleEnergy / GeV << " GeV"
                << " located randomly inside pscint " << G4endl;
        }

        // TODO: FIX HISTOGRAMS
        /*
        // Analysis
        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->SetVerboseLevel(1);

        auto SiPMPhotonCounter = MTRun->GetSiPMPhotonCounter();
        auto CPNCounter = MTRun->GetCPNCounter();
        auto FVPathCounter = MTRun->GetFVPathCounter();
        auto EPNCounter = MTRun->GetEPNCounter();
        auto strList = MTRun->GetStrList();

        // A map whose aim is to create different histograms of all counters.
        // It should handle the names and titles properly, and store the H1IDs.
        // The key is a counter name;
        // The value is a map with the key to be H1ID, and the value to be the index in strList.
        std::map<G4String, std::map<G4int, G4int>> histMap;
        
        // These are maps aiming to deal with xmax and xmin properly.
        // The key is the H1ID, and the value is the maximum/minimum
        std::map<G4int, G4int> histXMax, histXMin;

        // The map that stores the name and the counter
        // The key is a counter name;
        // The value is a pointer to the counter.
        std::map<G4String, COUNTER*> nameAndCounter;
        nameAndCounter.insert(std::make_pair(G4String("PEs of SiPM"), &SiPMPhotonCounter));
        nameAndCounter.insert(std::make_pair(G4String("Creator Process Name"), &CPNCounter));
        nameAndCounter.insert(std::make_pair(G4String("Final Volume Path"), &FVPathCounter));
        nameAndCounter.insert(std::make_pair(G4String("Ending Process Name"), &EPNCounter));

        // Print mean and rms information for each counter;
        // Register H1ID, histmap, histXMax and histXMin
        G4int H1ID = 0;
        for (auto singleNameAndCounter : nameAndCounter)
        {
            if(singleNameAndCounter.second->size() != 0)
            {
                auto meanAndRMS = GetMeanAndRMSOfCounter(*(singleNameAndCounter.second), eventCount);
                G4cout << ">>> " << singleNameAndCounter.first << ":" << G4endl;
                for (auto item : meanAndRMS[0])
                    G4cout << "    " << strList[item.first] << ": " << item.second << " +- " << (meanAndRMS[1].find(item.first) != meanAndRMS[1].end() ? (*meanAndRMS[1].find(item.first)).second : -1) << G4endl;
                
                delete[] meanAndRMS;
        
                for(auto singleCounter : *(singleNameAndCounter.second))
                {
                    for(auto it : singleCounter)
                    {
                        G4String counterName = singleNameAndCounter.first;
                        if(MapFindValue(it.first, histMap[counterName]) == histMap[counterName].end())
                        {
                            histMap[counterName][H1ID] = it.first;
                            ++H1ID;
                        }
                        G4int local_h1id = MapFindValue(it.first, histMap[counterName])->first;
                        if(histXMax.find(local_h1id) == histXMax.end())
                        {
                            histXMax[local_h1id] = it.second;
                            histXMin[local_h1id] = it.second;
                        }
                        else
                        {
                            if(it.second > histXMax[local_h1id])    histXMax[local_h1id] = it.second;
                            if(it.second < histXMin[local_h1id])    histXMin[local_h1id] = it.second;
                        }
                    }
                }
            }
        }

        // Create histograms
        for(G4int local_h1id = 0; local_h1id < H1ID; ++local_h1id)
        {
            G4String histTitle;
            for(auto singleType : histMap)
            {
                for(auto histIDAndStrIndex : singleType.second)
                {
                    if(histIDAndStrIndex.first == local_h1id)
                    {
                        histTitle = singleType.first + ": " + strList[histIDAndStrIndex.second];
                        analysisManager->CreateH1(
                            std::to_string(local_h1id),
                            histTitle,
                            50,
                            histXMin[histIDAndStrIndex.first]/50*50,
                            (histXMax[histIDAndStrIndex.first]/50+1)*50
                        );
                        analysisManager->SetH1Plotting(local_h1id, true);
                        break;
                    }
                }
                if(histTitle.size())   break;
            }
        }

        // Fill histograms
        for(auto singleType : histMap)
        {
            for(auto histIDAndStrIndex : singleType.second)
            {
                for(auto it : *(nameAndCounter[singleType.first]))
                {
                    analysisManager->FillH1(
                        histIDAndStrIndex.first,
                        (double)it[histIDAndStrIndex.second]
                    );
                }
            }
        }
        */
        
        // Data output
        std::stringstream optDirStream;
        optDirStream << fConfig.outputFilePath << "/run" << aRun->GetRunID();
        std::filesystem::create_directories(optDirStream.str());

        // Run Conditions Output  
        json runConditions;
        runConditions["RunID"] = aRun->GetRunID();
        runConditions["ParticleName"] = (const char*)particleName;
        runConditions["KineticEnergy/MeV"] = particleEnergy/MeV;
        runConditions["RandomPoints"] = fConfig.randomPoints;

        if(!fConfig.randomPoints)
        {
            runConditions["GunXPosition/cm"] = particlePosition[0].x()/cm;
            runConditions["GunYPosition/cm"] = particlePosition[0].y()/cm;
            runConditions["GunZPosition/cm"] = particlePosition[0].z()/cm;
        }

        std::stringstream RCFileNameStream;
        RCFileNameStream << optDirStream.str() << "/RunConditions.json";
        std::ofstream RCFileStream(RCFileNameStream.str());
        if(RCFileStream.is_open())
            RCFileStream << runConditions.dump(4);
        else
            G4cerr << "Open File \"" << RCFileNameStream.str() << "\" Failed!";
        RCFileStream.close();

        /* TODO: Fix built-in analysis
        // Analysis Manager Output
        G4bool builtinAnalysis = fConfig.useBuiltinAnalysis;
        if(builtinAnalysis)
        {
            std::stringstream dirNameForBAStream;
            dirNameForBAStream << optDirStream.str() << "/" << "built-in";
            std::filesystem::create_directory(dirNameForBAStream.str());

            std::stringstream BAFileNameStream;
            BAFileNameStream << dirNameForBAStream.str() << "/data.csv";
            analysisManager->OpenFile(BAFileNameStream.str());
            analysisManager->Write();
            analysisManager->CloseFile();
        }
        */

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

        // If random points, gun position output
        if(fConfig.randomPoints)
        {
            std::stringstream GPDirStream;
            GPDirStream << optDirStream.str() << "/" << "Gun Position";
            std::filesystem::create_directory(GPDirStream.str());

            std::stringstream GPFileNameStream;
            GPFileNameStream << GPDirStream.str() << "/" << "Gun_Position.csv";
            std::ofstream GPFileStream(GPFileNameStream.str());
            if(!GPFileStream.is_open())
            {   
                G4cerr << "Open File \"" << GPFileNameStream.str() << "\" Failed!" << G4endl;
            }
            else
            {
                GPFileStream << "# " << GPDirStream.str() << ": " << "Gun Position" << std::endl;
                for(auto singlePosition : particlePosition)
                    GPFileStream << singlePosition.x() << "," << singlePosition.y() << "," << singlePosition.z() << std::endl;
                GPFileStream.close();
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

std::map<G4int, G4double>* MVRunAction::GetMeanAndRMSOfCounter(COUNTER counter, G4int eventCount) const
{
    auto meanAndRMS = new std::map<G4int, G4double>[2];
    std::map<G4int, long long> sum;
    std::map<G4int, long long> squaredSum;
    for (auto singleCounter : counter)
    {
        for (auto item : singleCounter)
        {
            if (sum.find(item.first) != sum.end())
            {
                sum[item.first] += (long long)item.second;
                squaredSum[item.first] += (long long)item.second * (long long)item.second;
            }
            else
            {
                sum[item.first] = (long long)item.second;
                squaredSum[item.first] = (long long)item.second * (long long)item.second;
            }
        }
    }
    for (auto item : sum)
    {
        meanAndRMS[0][item.first] = (double)item.second / eventCount;
    }
    for (auto item : squaredSum)
    {
        meanAndRMS[1][item.first] = std::sqrt(-meanAndRMS[0][item.first] * meanAndRMS[0][item.first] + (double)item.second / eventCount);
    }

    return meanAndRMS;
}

}