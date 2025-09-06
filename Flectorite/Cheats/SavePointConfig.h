#pragma once
#include <map>
#include <string>
#include <vector>
#include "../Core/SDK.h"
#include "../Core/SimpleIni.h"
#include "../Menu/Config.h"

namespace SavePointCFG
{
    extern CSimpleIniA savePointIni;

    // Load all world save points from savepointconfig.cfg
    void LoadWorldSavePoints(std::unordered_map<std::string, std::unordered_map<int, CFG::SavePoint>>& worldMaps);

    // Save all world save points to savepointconfig.cfg
    void SaveWorldSavePoints(const std::unordered_map<std::string, std::unordered_map<int, CFG::SavePoint>>& worldMaps);

    // Load save points for a specific world
    void LoadWorldSavePoints(const std::string& worldName, std::unordered_map<int, CFG::SavePoint>& savePointMap);

    // Save save points for a specific world
    void SaveWorldSavePoints(const std::string& worldName, const std::unordered_map<int, CFG::SavePoint>& savePoints);

    // Initialize the save point config system
    void InitializeSavePointConfig();

    // Save point overlay rendering
    void SavePointOverlay();

    // Save current player position as save point
    void SaveCurrentPoint();

    // Teleport to current save point
    void TeleportToSavePoint();

    // Get closest save point to crosshair
    SDK::FVector* GetClosestSavePoint();

    // Update world save points when world changes
    void UpdateWorldSavePoints();

    // Load save points from config (compatibility wrapper)
    void LoadSavePointsFromConfig();

    // Initialize the save point system
    void InitializeSavePointSystem();

    // Save all world save points
    void SaveAllWorldSavePoints();
}
