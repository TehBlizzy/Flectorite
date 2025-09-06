#pragma once

#include "../Core/SDK.h"
#include "../Menu/Config.h"
#include <Windows.h>
#include <Xinput.h>


#pragma comment(lib, "xinput.lib")



// Note: Do NOT define IsKeyPressed/IsKeyHeld globally as they conflict with ImGui functions
// Use Cheat_IsKeyPressed/Cheat_IsKeyHeld instead, or ImGui::IsKeyPressed for UI code - I tbh lost track of why this was an issue but last I tried it was still an issue, fuck it this works technically

namespace Cheat
{
    // Main-Use Game Variables
    extern TFD::UWorld* GWorld;
    extern TFD::UCanvas* Canvas;
    extern TFD::AM1Player* LocalPlayerCharacter;
    extern TFD::AM1PlayerState* PlayerState;
    extern TFD::AM1PlayerControllerInGame* LocalPlayerController;
    extern TFD::UM1MissionControlComponent* Missions;
    extern TFD::UM1GameSetting* GameSettings;

    // State Variables
    extern bool UpdateCache;
    extern SDK::FVector2D ScreenMiddle;
    
    // Names and Bones ID
    extern std::unordered_map<int, std::string> IDNameMap;
    extern std::unordered_map<int, std::vector<int>> IDBoneMap;
    extern UC::int32 MissionTaskIndex;
    
    // Aimbot Variables
    extern bool Aimbot_KeyHeld;
    extern TFD::AActor* SilentAim_Target;
    extern int SilentAim_BoneIndex;

    // Weapon Variables
    extern TFD::UM1WeaponSlotControlComponent* WeaponControl;

    // Player Variables
    extern TFD::TArray<TFD::FM1CustomizingInfoWrapper> CustomizationItems;
    extern std::unordered_map<int, std::string> PresetsMap;
    extern std::vector<int> HotSwapPreset;
    extern int CurrentPresetIndex;
    extern int HotSwapIndex;

    // Resupply Variables
    extern TFD::ABP_NpcBase_C* ResupplyClass;
    extern TFD::ANPC_MultiSupply_C* ResupplyNPC;


    // Save Point Variables
    extern std::unordered_map<int, CFG::SavePoint> SavePointMap;
    extern std::unordered_map<std::string, std::unordered_map<int, CFG::SavePoint>> WorldSavePointMaps;
    extern std::string CurrentWorldName;
    extern int CurrentSavePointIndex;
    extern int MaxSavePoints;
    std::string GetCurrentWorldName();
    
    // Coloring
    extern SDK::FLinearColor ColorWhite;
    extern SDK::FLinearColor ColorGreen;
    extern SDK::FLinearColor ColorDarkGreen;
    extern SDK::FLinearColor ColorRed;
    extern SDK::FLinearColor ColorGold;
    extern SDK::FLinearColor ColorMana;
    extern SDK::FLinearColor ColorUncommon;
    extern SDK::FLinearColor ColorCommon;
    extern SDK::FLinearColor ColorRare;
    extern SDK::FLinearColor ColorUltimate;
    extern SDK::FLinearColor ColorDarkRed;
    extern SDK::FLinearColor ColorAimbotFOV;
    
    // Utility Function
    bool WorldToScreen(const SDK::FVector& worldLoc, SDK::FVector2D* screenPos);

	// Input Functions - Got Weird Errors with ImGui when using ImGui::IsKeyPressed/IsKeyHeld
    /*bool IsKeyPressedFunc(int key);
    bool IsKeyHeldFunc(int key);*/
}
