#pragma once

#include "References.h"
#include "../Core/SDK.h"
#include "../Menu/Config.h"
#include "../Menu/Render.h"
#include "../ImGui/Fonts/IconsFontAwesome6.h"

namespace Cheat
{
    void PresetOverlay();
    void PresetRefresh();
    void PresetActivate();
    void ResearchBookmarkedItems();
    void AutoResupply();

    void ForceSpawnVehicle();
    void ModifyVehicleStats();
    void EnabledFeaturesOverlay();

    void TeleportForward();
    void TeleportForward2();
    void TeleportToTarget();
    void SavedPointsESP();

    void UpdateTelekinesis(bool isGrabbing);
    void RenderTelekinesisMarkers();
    TFD::AActor* FindTelekinesisTarget();
}
