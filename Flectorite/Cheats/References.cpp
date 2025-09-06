#include "References.h"
#include <vector>
#include <string>
#include <unordered_map>



using SavePointMapType = std::unordered_map<int, CFG::SavePoint>;
using WorldSavePointMapsType = std::unordered_map<std::string, std::unordered_map<int, CFG::SavePoint>>;

namespace Cheat
{
    // Main-Use Game Variables
    TFD::UWorld* GWorld = nullptr;
    TFD::UCanvas* Canvas = nullptr;
    TFD::AM1PlayerControllerInGame* LocalPlayerController = nullptr;
    TFD::AM1Player* LocalPlayerCharacter = nullptr;
    TFD::AM1PlayerState* PlayerState = nullptr;
    TFD::UM1MissionControlComponent* Missions = nullptr;
    TFD::UM1GameSetting* GameSettings = nullptr;
    
    // State Variables
    bool UpdateCache = false;
    SDK::FVector2D ScreenMiddle = { 0, 0 };
    
    // Data Structures
    std::unordered_map<int, std::string> IDNameMap = { };
    std::unordered_map<int, std::vector<int>> IDBoneMap = { };
    
    // Aimbot Variables
    TFD::AActor* SilentAim_Target = nullptr;
    int SilentAim_BoneIndex = -1;
    
    // Weapon Variables
    TFD::UM1WeaponSlotControlComponent* WeaponControl = nullptr;

    // Player Variables
    std::unordered_map<int, std::string> PresetsMap = { };
    std::vector<int> HotSwapPreset = { -1, -1, -1, -1, -1, -1 };
    UC::int32 MissionTaskIndex = 0;
    int CurrentPresetIndex = 0;
    int HotSwapIndex = 0;

    // Resupply Variables
    TFD::ABP_NpcBase_C* ResupplyClass = nullptr;
    TFD::ANPC_MultiSupply_C* ResupplyNPC = nullptr;

    // Save Point Variables
    std::unordered_map<int, CFG::SavePoint> SavePointMap;
    std::unordered_map<std::string, std::unordered_map<int, CFG::SavePoint>> WorldSavePointMaps;
    std::string CurrentWorldName = "";
    int CurrentSavePointIndex = 0;
    int MaxSavePoints = 10;
    
    // Coloring
    SDK::FLinearColor ColorWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
    SDK::FLinearColor ColorGreen = { 0, 1.0f, 0, 1.0f };
    SDK::FLinearColor ColorDarkGreen = { 0, 0.75f, 0, 0.75f };
    SDK::FLinearColor ColorRed = { 1.0f, 0, 0, 1.0f };
    SDK::FLinearColor ColorGold = { 1.0f, 0.882f, 0, 1.0f };
    SDK::FLinearColor ColorMana = { 0.184f, 0.702f, 1.0f, 1.0f };
    SDK::FLinearColor ColorUncommon = { 0.2f, 0.29f, 1.0f, 1.0f };
    SDK::FLinearColor ColorCommon = { 0, 0.922f, 0.29f, 1.0f };
    SDK::FLinearColor ColorRare = { 0.761f, 0.063f, 0.961, 1.0f };
    SDK::FLinearColor ColorUltimate = { 1.0f, 0.247f, 0.247f, 1.0f };
    SDK::FLinearColor ColorDarkRed = { 0.5f, 0.0f, 0.0f, 0.8f };
    SDK::FLinearColor ColorAimbotFOV = { 0.9f, 0.2f, 0.85f, 0.35f };

    // Utility Function
    bool WorldToScreen(const SDK::FVector& worldLoc, SDK::FVector2D* screenPos)
    {
        if (!LocalPlayerController || !screenPos || !Canvas)
            return false;

        bool inView = TFD::UGameplayStatics::ProjectWorldToScreen(LocalPlayerController, worldLoc, screenPos, false);
        if (inView && screenPos->X > 0 && screenPos->X < Canvas->SizeX && 
            screenPos->Y > 0 && screenPos->Y < Canvas->SizeY)
            return true;
            
        return false;
    }

    TFD::FVector RotatorToVector(const TFD::FRotator& R)
    {
        float CP = std::cos(R.Pitch * 3.14159265f / 180.f);
        float SP = std::sin(R.Pitch * 3.14159265f / 180.f);
        float CY = std::cos(R.Yaw * 3.14159265f / 180.f);
        float SY = std::sin(R.Yaw * 3.14159265f / 180.f);

        return TFD::FVector{ CP * CY, CP * SY, SP };
    }

    // Input helper functions (if needed as functions instead of macros)
    /*bool IsKeyPressedFunc(int key)
    {
        return GetAsyncKeyState(key) & 0x8000;
    }
    bool IsKeyHeldFunc(int key)
    {
        return GetAsyncKeyState(key) & 0x8000;
    }*/

    // World Detection Function
    std::string GetCurrentWorldName()
    {
        if (!GWorld)
            return "Unknown";

        std::string gWorldName = GWorld->GetName();

        if (gWorldName == "Map_2000_P")
            return "Albion";
        else if (gWorldName == "Map_1002_P")
            return "Kingston";
        else if (gWorldName == "Map_1006_P")
            return "Sterile Land";
        else if (gWorldName == "Map_1003_P")
            return "Agna Desert";
        else if (gWorldName == "Map_1008_P")
            return "White-Night Gulch";
        else if (gWorldName == "Map_1001_P")
            return "Vespers";
        else if (gWorldName == "Map_1004_P")
            return "Echo Swamp";
        else if (gWorldName == "Map_1007_P")
            return "Hagios";
        else if (gWorldName == "Map_1005_P")
            return "Fortress";
        else if (gWorldName == "Map_1011_P")
            return "Sigma Sector Desert";
        else if (gWorldName == "Map_1010_P")
            return "Sigma Sector Boundary";
        else if (gWorldName == "Map_1015_P")
            return "Acion Plains";
        else if (gWorldName == "Map_1009_P")
            return "Void Vessel";
        else if (gWorldName == "Map_VoidLevel0_P")
            return "Void Abyss";
        else if (gWorldName == "Map_1023_P")
            return "Void Erosion";
    }
}
