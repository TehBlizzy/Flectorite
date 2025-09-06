#include "SavePointConfig.h"
#include "../Menu/Config.h"
#include "../Menu/Render.h"
#include "../Core/Cheat.h"
#include <iostream>
#include <format>

namespace SavePointCFG
{
    CSimpleIniA savePointIni;

    void InitializeSavePointConfig()
    {
        savePointIni.SetUnicode();
        if (savePointIni.LoadFile("savepointconfig.cfg") < 0)
        {
            std::cout << "No save point config found, creating new savepointconfig.cfg\n";
            // Create empty file
            savePointIni.SaveFile("savepointconfig.cfg");
        }
    }

    void LoadWorldSavePoints(std::unordered_map<std::string, std::unordered_map<int, CFG::SavePoint>>& worldSavePointMaps)
    {
        worldSavePointMaps.clear();

        // Get all sections (world names)
        CSimpleIniA::TNamesDepend sections;
        savePointIni.GetAllSections(sections);

        for (const auto& section : sections)
        {
            std::string worldName = section.pItem;
            std::unordered_map<int, CFG::SavePoint> savePointMap;

            // Load save points for this world
            for (int i = 0; i < 10; i++)
            {
                std::string pointKey = "Point" + std::to_string(i) + "_IsValid";
                bool isValid = savePointIni.GetBoolValue(worldName.c_str(), pointKey.c_str(), false);

                if (isValid)
                {
                    CFG::SavePoint point;
                    point.IsValid = true;

                    std::string xKey = "Point" + std::to_string(i) + "_X";
                    std::string yKey = "Point" + std::to_string(i) + "_Y";
                    std::string zKey = "Point" + std::to_string(i) + "_Z";
                    std::string nameKey = "Point" + std::to_string(i) + "_Name";

                    point.X = (float)savePointIni.GetDoubleValue(worldName.c_str(), xKey.c_str(), 0.0);
                    point.Y = (float)savePointIni.GetDoubleValue(worldName.c_str(), yKey.c_str(), 0.0);
                    point.Z = (float)savePointIni.GetDoubleValue(worldName.c_str(), zKey.c_str(), 0.0);
                    point.Name = savePointIni.GetValue(worldName.c_str(), nameKey.c_str(), "");

                    savePointMap[i] = point;
                }
            }

            if (!savePointMap.empty())
            {
                worldSavePointMaps[worldName] = savePointMap;
            }
        }

        std::cout << "Loaded save points for " << worldSavePointMaps.size() << " worlds\n";
    }

    void SaveWorldSavePoints(const std::unordered_map<std::string, std::unordered_map<int, CFG::SavePoint>>& worldSavePointMaps)
    {
        // Clear existing data
        savePointIni.Reset();

        for (const auto& worldEntry : worldSavePointMaps)
        {
            const std::string& worldName = worldEntry.first;
            const std::unordered_map<int, CFG::SavePoint>& savePointMap = worldEntry.second;

            for (const auto& pointEntry : savePointMap)
            {
                int index = pointEntry.first;
                const CFG::SavePoint& point = pointEntry.second;

                if (point.IsValid)
                {
                    std::string pointPrefix = "Point" + std::to_string(index) + "_";

                    savePointIni.SetBoolValue(worldName.c_str(), (pointPrefix + "IsValid").c_str(), true, nullptr);
                    savePointIni.SetDoubleValue(worldName.c_str(), (pointPrefix + "X").c_str(), point.X, nullptr);
                    savePointIni.SetDoubleValue(worldName.c_str(), (pointPrefix + "Y").c_str(), point.Y, nullptr);
                    savePointIni.SetDoubleValue(worldName.c_str(), (pointPrefix + "Z").c_str(), point.Z, nullptr);
                    savePointIni.SetValue(worldName.c_str(), (pointPrefix + "Name").c_str(), point.Name.c_str(), nullptr);
                }
            }
        }

        if (savePointIni.SaveFile("savepointconfig.cfg") < 0)
        {
            std::cout << "Failed to save savepointconfig.cfg\n";
        }
        else
        {
            std::cout << "Saved save points for " << worldSavePointMaps.size() << " worlds to savepointconfig.cfg\n";
        }
    }

    void LoadWorldSavePoints(const std::string& worldName, std::unordered_map<int, CFG::SavePoint>& savePointMap)
    {
        savePointMap.clear();

        for (int i = 0; i < 10; i++)
        {
            std::string pointKey = "Point" + std::to_string(i) + "_IsValid";
            bool isValid = savePointIni.GetBoolValue(worldName.c_str(), pointKey.c_str(), false);

            if (isValid)
            {
                CFG::SavePoint point;
                point.IsValid = true;

                std::string xKey = "Point" + std::to_string(i) + "_X";
                std::string yKey = "Point" + std::to_string(i) + "_Y";
                std::string zKey = "Point" + std::to_string(i) + "_Z";
                std::string nameKey = "Point" + std::to_string(i) + "_Name";

                point.X = (float)savePointIni.GetDoubleValue(worldName.c_str(), xKey.c_str(), 0.0);
                point.Y = (float)savePointIni.GetDoubleValue(worldName.c_str(), yKey.c_str(), 0.0);
                point.Z = (float)savePointIni.GetDoubleValue(worldName.c_str(), zKey.c_str(), 0.0);
                point.Name = savePointIni.GetValue(worldName.c_str(), nameKey.c_str(), "");

                savePointMap[i] = point;
            }
        }
    }

    void SaveWorldSavePoints(const std::string& worldName, const std::unordered_map<int, CFG::SavePoint>& savePointMap)
    {
        // Remove existing entries for this world
        savePointIni.Delete(worldName.c_str(), NULL, true);

        for (const auto& pointEntry : savePointMap)
        {
            int index = pointEntry.first;
            const CFG::SavePoint& point = pointEntry.second;

            if (point.IsValid)
            {
                std::string pointPrefix = "Point" + std::to_string(index) + "_";

                savePointIni.SetBoolValue(worldName.c_str(), (pointPrefix + "IsValid").c_str(), true, nullptr);
                savePointIni.SetDoubleValue(worldName.c_str(), (pointPrefix + "X").c_str(), point.X, nullptr);
                savePointIni.SetDoubleValue(worldName.c_str(), (pointPrefix + "Y").c_str(), point.Y, nullptr);
                savePointIni.SetDoubleValue(worldName.c_str(), (pointPrefix + "Z").c_str(), point.Z, nullptr);
                savePointIni.SetValue(worldName.c_str(), (pointPrefix + "Name").c_str(), point.Name.c_str(), nullptr);
            }
        }

        if (savePointIni.SaveFile("savepointconfig.cfg") < 0)
        {
            std::cout << "Failed to save world " << worldName << " to savepointconfig.cfg\n";
        }
    }

    void SavePointOverlay()
    {
        if (!Cheat::SavePointMap.empty() || Cheat::MaxSavePoints > 0)
        {
            int YPos = 40;
            // Closer to second overlay for even spacing
            int XPos = 580;

            // RGB support for header
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.0f, gradientTime);
                Render::R_DrawText(XPos, YPos - 20, "Saved Points:", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos - 20, "Saved Points:", &Cheat::ColorGreen, true);
            }

            for (int i = 0; i < Cheat::MaxSavePoints; i++)
            {
                std::string buffer;

                if (Cheat::SavePointMap.contains(i))
                {
                    auto& point = Cheat::SavePointMap[i];

                    float distance = 0.0f;
                    if (Cheat::LocalPlayerCharacter)
                    {
                        SDK::FVector playerPos = Cheat::LocalPlayerCharacter->K2_GetActorLocation();
                        SDK::FVector savePos = { point.X, point.Y, point.Z };
                        distance = sqrtf(powf(savePos.X - playerPos.X, 2) + powf(savePos.Y - playerPos.Y, 2) + powf(savePos.Z - playerPos.Z, 2)) / 100.0f; // Convert to meters
                    }

                    buffer = std::format("{} ({}m)", point.Name, (int)distance);
                    if (i == Cheat::CurrentSavePointIndex)
                    {
                        if (CFG::cfg_ESP_GradientCycling) {
                            float gradientTime = Render::GetSharedGradientTime();
                            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.2f, gradientTime);
                            Render::R_DrawText(XPos, YPos, buffer, &flowingColor, true);
                        }
                        else {
                            Render::R_DrawText(XPos, YPos, buffer, &Cheat::ColorGreen, true);
                        }
                    }
                    else
                    {
                        if (CFG::cfg_ESP_GradientCycling) {
                            float gradientTime = Render::GetSharedGradientTime();
                            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.4f, gradientTime);
                            Render::R_DrawText(XPos, YPos, buffer, &flowingColor, true);
                        }
                        else {
                            Render::R_DrawText(XPos, YPos, buffer, &Cheat::ColorMana, true);
                        }
                    }
                }
                else
                {
                    buffer = std::format("Empty {}", i + 1);
                    if (i == Cheat::CurrentSavePointIndex)
                    {
                        if (CFG::cfg_ESP_GradientCycling) {
                            float gradientTime = Render::GetSharedGradientTime();
                            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.6f, gradientTime);
                            Render::R_DrawText(XPos, YPos, buffer, &flowingColor, true);
                        }
                        else {
                            Render::R_DrawText(XPos, YPos, buffer, &Cheat::ColorGreen, true);
                        }
                    }
                    else
                    {
                        if (CFG::cfg_ESP_GradientCycling) {
                            float gradientTime = Render::GetSharedGradientTime();
                            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.8f, gradientTime);
                            Render::R_DrawText(XPos, YPos, buffer, &flowingColor, true);
                        }
                        else {
                            Render::R_DrawText(XPos, YPos, buffer, &Cheat::ColorDarkRed, true);
                        }
                    }
                }
                YPos += 20;
            }
        }
    }

    void SaveCurrentPoint()
    {
        if (!Cheat::LocalPlayerCharacter)
            return;

        SDK::FVector currentLocation = Cheat::LocalPlayerCharacter->K2_GetActorLocation();
        std::string pointName = CFG::cfg_SavePoint_CustomName;
        if (pointName.empty()) {
            pointName = std::format("Point {}", Cheat::CurrentSavePointIndex + 1);
        }
        CFG::cfg_SavePoint_CustomName = "";

        CFG::SavePoint newPoint;
        newPoint.IsValid = true;
        newPoint.X = currentLocation.X;
        newPoint.Y = currentLocation.Y;
        newPoint.Z = currentLocation.Z;
        newPoint.Name = pointName;

        // Update map and the new save point system
        Cheat::SavePointMap[Cheat::CurrentSavePointIndex] = newPoint;

        // Reset UI to defaults for next save point
        CFG::cfg_SavePoint_CustomName = "";

        // DEBUG: Get and display current world/area name
        std::string detectedWorldName = Cheat::GetCurrentWorldName();

        // Update CurrentWorldName if detection found something
        if (!detectedWorldName.empty() && detectedWorldName != "Unknown") {
            Cheat::CurrentWorldName = detectedWorldName;
        }

        // Save to current world's config
        if (!Cheat::CurrentWorldName.empty()) {
            SaveWorldSavePoints(Cheat::CurrentWorldName, Cheat::SavePointMap);
            printf("[DEBUG] Saved save point %d with name '%s'\n",
                Cheat::CurrentSavePointIndex, newPoint.Name.c_str());
        }
    }

    void TeleportToSavePoint()
    {
        if (!Cheat::LocalPlayerCharacter || !Cheat::LocalPlayerCharacter->TeleportHandler)
            return;
        if (!Cheat::SavePointMap.contains(Cheat::CurrentSavePointIndex))
            return;

        auto& savePoint = Cheat::SavePointMap[Cheat::CurrentSavePointIndex];
        SDK::FVector targetLocation;
        targetLocation.X = savePoint.X;
        targetLocation.Y = savePoint.Y;
        targetLocation.Z = savePoint.Z;

        SDK::FRotator currentRotation = Cheat::LocalPlayerController->PlayerCameraManager->GetCameraRotation();
        Cheat::LocalPlayerCharacter->TeleportHandler->ServerMoveToTeleportToLocation(targetLocation, currentRotation);
        Cheat::LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
    }

    SDK::FVector* GetClosestSavePoint()
    {
        if (!Cheat::LocalPlayerCharacter || Cheat::SavePointMap.empty())
            return nullptr;

        SDK::FVector* closestSavePoint = nullptr;
        float closestDistance = INFINITY;
        int closestSavePointIndex = -1;

        for (auto& pair : Cheat::SavePointMap)
        {
            SDK::FVector savePointLocation = { pair.second.X, pair.second.Y, pair.second.Z };
            SDK::FVector2D ScreenPos = { -1, -1 };
            if (Cheat::WorldToScreen(savePointLocation, &ScreenPos))
            {
                float distance = TFD::UKismetMathLibrary::Distance2D(Cheat::ScreenMiddle, ScreenPos);
                if (distance <= CFG::cfg_Teleport_FOV && distance < closestDistance)
                {
                    closestDistance = distance;
                    closestSavePointIndex = pair.first;  // Store the save point index
                    static SDK::FVector tempSavePoint;
                    tempSavePoint = { pair.second.X, pair.second.Y, pair.second.Z };
                    closestSavePoint = &tempSavePoint;
                }
            }
        }

        // Update CurrentSavePointIndex if we found a target save point
        if (closestSavePoint && closestSavePointIndex != -1)
        {
            Cheat::CurrentSavePointIndex = closestSavePointIndex;
        }

        return closestSavePoint;
    }

    void UpdateWorldSavePoints()
    {
        std::string newWorldName = Cheat::GetCurrentWorldName();

        // Check if we've changed worlds
        if (newWorldName != Cheat::CurrentWorldName)
        {
            // Save current world's save points before switching
            if (!Cheat::CurrentWorldName.empty())
            {
                SaveWorldSavePoints(Cheat::CurrentWorldName, Cheat::SavePointMap);
            }

            // Update current world name
            Cheat::CurrentWorldName = newWorldName;

            // Load save points for the new world
            LoadWorldSavePoints(Cheat::CurrentWorldName, Cheat::SavePointMap);

            // Also update the in-memory world map
            if (!Cheat::SavePointMap.empty())
            {
                Cheat::WorldSavePointMaps[Cheat::CurrentWorldName] = Cheat::SavePointMap;
            }

            // Reset current save point index to first valid save point in new world
            Cheat::CurrentSavePointIndex = 0;

            // If index 0 doesn't exist, find the first valid save point
            if (!Cheat::SavePointMap.empty() && !Cheat::SavePointMap.contains(0))
            {
                Cheat::CurrentSavePointIndex = Cheat::SavePointMap.begin()->first;
            }
        }
    }

    void LoadSavePointsFromConfig()
    {
        // This function is now handled by the world-based system
        // LoadWorldSavePoints() is called automatically when switching worlds
        // For backward compatibility, we can load from current world
        if (!Cheat::CurrentWorldName.empty()) {
            LoadWorldSavePoints(Cheat::CurrentWorldName, Cheat::SavePointMap);
            printf("[DEBUG] Loaded save points for world '%s', found %d points\n",
                Cheat::CurrentWorldName.c_str(), (int)Cheat::SavePointMap.size());
            for (const auto& pair : Cheat::SavePointMap) {
                const auto& point = pair.second;
                printf("[DEBUG] Point %d: name='%s'\n",
                    pair.first, point.Name.c_str());
            }
        }
    }
    void InitializeSavePointSystem()
    {
        // Initialize the separate save point config system
        InitializeSavePointConfig();

        // Load all existing world save points from the separate config file
        LoadWorldSavePoints(Cheat::WorldSavePointMaps);
    }

    void SaveAllWorldSavePoints()
    {
        // Save all world save points to the separate config file
        SaveWorldSavePoints(Cheat::WorldSavePointMaps);
    }
}
