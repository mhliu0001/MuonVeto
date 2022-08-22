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

        // Run Conditions
        MVRunMT *MTRun = (MVRunMT *)aRun;

        G4double particleEnergy = MTRun->GetParticleEnergy();
        G4ThreeVector particlePosition = MTRun->GetParticlePosition();
        G4String particleName = MTRun->GetParticleName();
        auto SiPMPhotonCounter = MTRun->GetSiPMPhotonCounter();
        auto CPNCounter = MTRun->GetCPNCounter();
        auto FVPathCounter = MTRun->GetFVPathCounter();
        auto EPNCounter = MTRun->GetEPNCounter();
        auto strList = MTRun->GetStrList();

        G4int eventCount = SiPMPhotonCounter.size();

        G4cout << "This run consists of " << eventCount << " events of " << particleName << " with energy " << particleEnergy / GeV << " GeV"
                << " located at " << particlePosition << G4endl;

        if (CPNCounter.size() != 0)
        {
            auto CPNMeanAndRMS = GetMeanAndRMSOfCounter(CPNCounter, eventCount);
            G4cout << ">>> Creator Process List: " << G4endl;
            for (auto item : CPNMeanAndRMS[0])
                G4cout << "    " << strList[item.first] << ": " << item.second << " +- " << (CPNMeanAndRMS[1].find(item.first) != CPNMeanAndRMS[1].end() ? (*CPNMeanAndRMS[1].find(item.first)).second : -1) << G4endl;
            
            delete[] CPNMeanAndRMS;
        }

        if (FVPathCounter.size() != 0)
        {
            auto FVPathMeanAndRMS = GetMeanAndRMSOfCounter(FVPathCounter, eventCount);
            G4cout << ">>> Final Volume Path List: " << G4endl;
            for (auto item : FVPathMeanAndRMS[0])
                G4cout << "    " << strList[item.first] << ": " << item.second << " +- " << (FVPathMeanAndRMS[1].find(item.first) != FVPathMeanAndRMS[1].end() ? (*FVPathMeanAndRMS[1].find(item.first)).second : -1) << G4endl;

            delete[] FVPathMeanAndRMS;
        }

        if (EPNCounter.size() != 0)
        {
            auto EPNMeanAndRMS = GetMeanAndRMSOfCounter(EPNCounter, eventCount);
            G4cout << ">>> Ending Process List: " << G4endl;
            for (auto item : EPNMeanAndRMS[0])
                G4cout << "    " << strList[item.first] << ": " << item.second << " +- " << (EPNMeanAndRMS[1].find(item.first) != EPNMeanAndRMS[1].end() ? (*EPNMeanAndRMS[1].find(item.first)).second : -1) << G4endl;

            delete[] EPNMeanAndRMS;
        }

        if (SiPMPhotonCounter.size() != 0)
        {
            auto EPNMeanAndRMS = GetMeanAndRMSOfCounter(SiPMPhotonCounter, eventCount);
            G4cout << ">>> SiPM Photon Count: " << G4endl;
            for (auto item : EPNMeanAndRMS[0])
                G4cout << "    " << strList[item.first] << ": " << item.second << " +- " << (EPNMeanAndRMS[1].find(item.first) != EPNMeanAndRMS[1].end() ? (*EPNMeanAndRMS[1].find(item.first)).second : -1) << G4endl;

            delete[] EPNMeanAndRMS;
        }

        // Analysis
        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->SetVerboseLevel(1);

        // A map whose aim is to create different histograms of all counters.
        // It should handle the names and titles properly, and store the H1IDs.
        // The key is a counter name;
        // The value is a map with the key to be H1ID, and the value to be the index in strList.
        std::map<G4String, std::map<G4int, G4int>> histMap;
        
        // These are maps aiming to deal with xmax and xmin properly.
        // The key is the H1ID, and the value is the maximum/minimum
        std::map<G4int, G4int> histXMax, histXMin;

        G4int H1ID = 0;

        if (SiPMPhotonCounter.size() != 0)
        {
            G4String counterName = "PEs of SiPM";
            for(auto singleCounter : SiPMPhotonCounter)
            {
                for(auto it : singleCounter)
                {
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

        if (CPNCounter.size() != 0)
        {
            G4String counterName = "Creator Process Name";
            for(auto singleCounter : CPNCounter)
            {
                for(auto it : singleCounter)
                {
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

        if (FVPathCounter.size() != 0)
        {
            G4String counterName = "Final Volume Path";
            for(auto singleCounter : FVPathCounter)
            {
                for(auto it : singleCounter)
                {
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
        if (EPNCounter.size() != 0)
        {
            G4String counterName = "Ending Process Name";
            for(auto singleCounter : EPNCounter)
            {
                for(auto it : singleCounter)
                {
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

        G4cout << "H1ID is " << H1ID << G4endl;
        // Create histograms
        for(G4int local_h1id = 0; local_h1id < H1ID; ++local_h1id)
        {
            G4String histTitle = "Not Found";
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
                        analysisManager->SetH1Plotting(local_h1id,true);
                        // G4cout << "Creating histogram with title " << histTitle;
                        break;
                    }
                }
                if(histTitle != "Not Found")   break;
            }
        }
        /*
        for(auto singleType : histMap)
        {
            for(auto histIDAndStrIndex : singleType.second)
            {
                analysisManager->CreateH1(
                    std::to_string(histIDAndStrIndex.first),
                    singleType.first + ": " + strList[histIDAndStrIndex.second],
                    50,
                    histXMin[histIDAndStrIndex.first]/50*50,
                    (histXMax[histIDAndStrIndex.first]/50+1)*50
                );
                G4cout << "Creating histogram with name " << singleType.first + ": " + strList[histIDAndStrIndex.second] << G4endl;
            }
        }
        */

        // Filling histograms
        for(auto singleType : histMap)
        {
            for(auto histIDAndStrIndex : singleType.second)
            {
                if(singleType.first == "PEs of SiPM")
                {
                    for(auto it : SiPMPhotonCounter)
                    {
                        analysisManager->FillH1(
                            histIDAndStrIndex.first,
                            (double)it[histIDAndStrIndex.second]
                        );
                    }
                }
                else if(singleType.first == "Creator Process Name")
                {
                    for(auto it : CPNCounter)
                    {
                        analysisManager->FillH1(
                            histIDAndStrIndex.first,
                            (double)it[histIDAndStrIndex.second]
                        );
                    }
                }
                else if(singleType.first == "Final Volume Path")
                {
                    for(auto it : FVPathCounter)
                    {
                        analysisManager->FillH1(
                            histIDAndStrIndex.first,
                            (double)it[histIDAndStrIndex.second]
                        );
                    }
                }
                else if(singleType.first == "Ending Process Name")
                {
                    for(auto it : EPNCounter)
                    {
                        analysisManager->FillH1(
                            histIDAndStrIndex.first,
                            (double)it[histIDAndStrIndex.second]
                        );
                        //G4cout << "Filling histogram " << histIDAndStrIndex.first << " with " << it[histIDAndStrIndex.second] << G4endl;
                    }
                }
            }
        }
        
        // Data output
        const char* optDirName = fConfig.outputFilePath.c_str();
        G4int runID = aRun->GetRunID();
        char dirName[100];
        sprintf(dirName, "%s/run%d", optDirName, runID);
        std::filesystem::create_directories(dirName);

        // Run Conditions Output
        char RCFileName[120];
        sprintf(RCFileName, "%s/%s", dirName, "RunConditions.json");
        auto RCFP = fopen(RCFileName, "w");
        json runConditions;
        runConditions["RunID"] = runID;
        runConditions["ParticleName"] = (const char*)particleName;
        runConditions["KineticEnergy/MeV"] = particleEnergy/MeV;
        runConditions["GunXPosition/cm"] = particlePosition.x()/cm;
        runConditions["GunYPosition/cm"] = particlePosition.y()/cm;
        runConditions["GunZPosition/cm"] = particlePosition.z()/cm;
        fprintf(RCFP, "%s", runConditions.dump(4).c_str());
        fclose(RCFP);

        // Analysis Manager Output
        G4bool builtinAnalysis = fConfig.useBuiltinAnalysis;
        const char* builtinAnalysisFileName = "data.csv";
        if(builtinAnalysis)
        {
            char dirNameForBA[200];
            sprintf(dirNameForBA, "%s/%s", dirName, "built-in");
            std::filesystem::create_directory(dirNameForBA);

            char fileName[300];
            sprintf(fileName, "%s/%s", dirNameForBA, builtinAnalysisFileName);
            analysisManager->OpenFile(fileName);
            analysisManager->Write();
            analysisManager->CloseFile();
        }

        // csv output
        for(auto singleType : histMap)
        {
            G4String dir = G4String(dirName) + G4String("/") + singleType.first;
            std::filesystem::create_directory((const char*)dir);
            
            COUNTER counter;
            if(singleType.first == "Creator Process Name") counter = CPNCounter;
            else if(singleType.first == "Final Volume Path") counter = FVPathCounter;
            else if(singleType.first == "Ending Process Name") counter = EPNCounter;
            else if(singleType.first == "PEs of SiPM") counter = SiPMPhotonCounter;
            else throw "Name Error";

            for(auto histIDAndStrIndex : singleType.second)
            {
                char fileName[400];
                sprintf(fileName, "%s/%s%s", (const char*)dir, (const char*)(strList[histIDAndStrIndex.second]), ".csv");
                auto fp = fopen(fileName, "w");
                if(!fp)
                {   
                    G4cerr << "Open File Failed: " << fileName << G4endl;
                    continue;
                }
                fprintf(fp, "# %s: %s\n", (const char*) (singleType.first), (const char*) (strList[histIDAndStrIndex.second]));
                for(auto it : counter)
                    fprintf(fp, "%d\n", it[histIDAndStrIndex.second]);
                fclose(fp);
            }
        }

        // Spectrum output
        G4String dir = G4String(dirName) + "/Spectrum";
        std::filesystem::create_directory(dir.c_str());
        for(auto singleProcess : MTRun->GetProcessSpectrum())
        {
            char fileName[400];
            sprintf(fileName, "%s/%s%s", dir.c_str(), strList[singleProcess.first].c_str(), ".csv");
            auto fp = fopen(fileName, "w");
            if(!fp)
            {   
                G4cerr << "Open File Failed: " << fileName << G4endl;
                continue;
            }
            fprintf(fp, "# %s: %s\n", "Spectrum", (const char*) (strList[singleProcess.first]));
            for(auto it : singleProcess.second)
                fprintf(fp, "%f\n", it/eV);
            fclose(fp);
        }

        // Output for .csv
        /*
        G4int particleNb = -1;
        if (particleName == "mu-")
            particleNb = 0;
        if (particleName == "gamma")
            particleNb = 1;

        G4cout << ">>>> " << particlePosition.z() << "," << particlePosition.x() << "," << particleNb << "," << particleEnergy / GeV << ",";
        for (int SiPMNb = 0; SiPMNb < fSiPMCount; SiPMNb++)
        {
            G4cout << mean[SiPMNb] << "," << rms[SiPMNb] << (SiPMNb == fSiPMCount - 1 ? "" : ",");
        }
        G4cout << G4endl;
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