#include "Player.h"
#include "../Core/Cheat.h"
#include "../Core/SDK.h"
#include "../Menu/Config.h"
#include "References.h"
#include "../Cheats/SavePointConfig.h"
#include "../Minhook/include/MinHook.h"
#include <format>
#include <random>

//#define IS_DEBUG_VERSION

namespace Cheat
{

    void PresetOverlay()
    {
        if (!PresetsMap.empty())
        {
            int YPos = 20;
            for (int i = 0; i < PresetsMap.size(); i++)
            {
                std::string buffer;

                if (PresetsMap.contains(i))
                    buffer = std::format("Preset {}: {}", i + 1, PresetsMap[i]);
                else
                    buffer = std::format("Preset {}: None", i + 1);

                if (i == CurrentPresetIndex)
                {
                    if (CFG::cfg_ESP_GradientCycling) {
                        float gradientTime = Render::GetSharedGradientTime();
                        SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f, gradientTime);
                        Render::R_DrawText(270, YPos, buffer, &flowingColor, true);
                    }
                    else {
                        Render::R_DrawText(270, YPos, buffer, &ColorGold, true);
                    }
                }
                else
                {
                    if (CFG::cfg_ESP_GradientCycling) {
                        float gradientTime = Render::GetSharedGradientTime();
                        SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.3f, gradientTime);
                        Render::R_DrawText(270, YPos, buffer, &flowingColor);
                    }
                    else {
                        Render::R_DrawText(270, YPos, buffer, &ColorWhite);
                    }
                }

                YPos += 18;
            }
        }
    }

    void PresetRefresh()
    {
        if (!LocalPlayerController || !LocalPlayerController->PrivateOnlineServiceComponent)
            return;

        TFD::UM1Account* Account = static_cast<TFD::UM1Account*>(TFD::native_GetUM1Account(LocalPlayerController->PrivateOnlineServiceComponent));
        if (!Account)
            return;
        if (Account->Preset)
        {
            for (const auto& Pair : Account->Preset->PresetSlotByIndex)
            {
                TFD::FM1PresetSlot Value = Pair.Value();
                std::string PresetName = Value.PresetName.ToString();
                if (PresetName.empty())
                {
                    PresetName = std::format("Preset {}", Value.PresetIndex);
                }
                PresetsMap.insert({ Value.PresetIndex, PresetName });
            }
        }
    }

    void PresetActivate()
    {
        if (!LocalPlayerController || !LocalPlayerController->PrivateOnlineServiceComponent)
            return;

        if (!PresetsMap.empty() && CurrentPresetIndex < PresetsMap.size())
        {
            for (TFD::UM1PrivateOnlineSubService* Subserv : LocalPlayerController->PrivateOnlineServiceComponent->SubServices)
            {
                if (!Subserv)
                    continue;
                if (Subserv->IsA(TFD::UM1PrivateOnlineServicePreset::StaticClass()) && Subserv->bIsReady == true)
                {
                    static_cast<TFD::UM1PrivateOnlineServicePreset*>(Subserv)->ServerRequestApplyPreset(CurrentPresetIndex);
                    break;
                }
            }
        }
    }

    void ResearchBookmarkedItems()
    {
        if (!Cheat::LocalPlayerController || !Cheat::LocalPlayerController->PrivateOnlineServiceComponent)
            return;

        TFD::UM1LocalGameInstanceSubsystem* LS = TFD::UM1LocalGameInstanceSubsystem::GetSystem(Cheat::GWorld);
        if (!LS || !LS->ResearchSystem)
            return;

        TFD::UM1ResearchSystem* RS = LS->ResearchSystem;

        if (Cheat::LocalPlayerController->PrivateOnlineServiceComponent->IsA(TFD::UM1PrivateOnlineServiceComponent::StaticClass()))
        {
            for (TFD::UM1PrivateOnlineSubService* Subserv : Cheat::LocalPlayerController->PrivateOnlineServiceComponent->SubServices)
            {
                if (!Subserv)
                    continue;


                if (Subserv->IsA(TFD::UM1PrivateOnlineServiceResearch::StaticClass()) && Subserv->bIsReady)
                {
                    for (const auto& RDTID : RS->BookmarkResearchTids)
                    {
                        static_cast<TFD::UM1PrivateOnlineServiceResearch*>(Subserv)->ServerRequestStartResearch(RDTID, CFG::cfg_Loot_ResearchQuantity);
                    }
                }
            }
        }
    }

    void AutoResupply()
    {
        if (!LocalPlayerController
            || !LocalPlayerCharacter
            || !LocalPlayerCharacter->StatComponent)
            //|| !LocalPlayerController->MultiSupplierObtainComponent)
            return;

        bool ShouldResupply = false;
        static std::chrono::steady_clock::time_point LastResupplyTime = std::chrono::steady_clock::now();

        if (CFG::cfg_Abilities_AutomaticResupplyTimed)
        {
            auto now = std::chrono::steady_clock::now();
            float elapsed = std::chrono::duration<float>(now - LastResupplyTime).count();

            if (elapsed >= CFG::cfg_Abilities_AutomaticResupplyTimeDelay)
            {
                LastResupplyTime = now;
                ShouldResupply = true;
            }
            else
                return;
        }

        //if (!ShouldResupply)// && TryResetAbilities)
        //{
        //    ShouldResupply = true;
        //    //TryResetAbilities = false;
        //}

        if (!ShouldResupply)
        {
            float CurrentHP = (float)LocalPlayerCharacter->StatComponent->GetStatValue(TFD::EM1StatType::Stat_CurrentHp).Value / 10000.0f;
            float MaxHP = (float)LocalPlayerCharacter->StatComponent->GetStatValue(TFD::EM1StatType::Stat_MaxHp).Value / 10000.0f;
            if (CurrentHP <= (MaxHP * (CFG::cfg_Abilities_AutomaticResupplyHealth / 100.0f)))
                ShouldResupply = true;
        }


        //if (!ShouldResupply)
        //{
        //    if (LocalPlayerCharacter->WeaponSlotControl
        //        && LocalPlayerCharacter->WeaponSlotControl->ActivatedWeaponSlot.WeaponSlot.Weapon)
        //        //&& LocalPlayerCharacter->WeaponSlotControl->ActivatedWeaponSlot.WeaponSlot.Weapon->RoundsComponent)
        //    {
        //        auto* WeaponBase = LocalPlayerCharacter->WeaponSlotControl->ActivatedWeaponSlot.WeaponSlot.Weapon;
        //        if (!WeaponBase || !WeaponBase->IsA(TFD::AM1RangedWeapon::StaticClass()))
        //            return;

        //        TFD::AM1RangedWeapon* Weapon = static_cast<TFD::AM1RangedWeapon*>(WeaponBase);
        //        if (Weapon->RoundsComponent->CurrentRounds < CFG::cfg_Abilities_AutomaticResupplyAmmo)
        //            ShouldResupply = true;
        //    }
        //}

        if (ShouldResupply)
        {
            //static std::random_device rd;
            //static std::mt19937 gen(rd());
            //static std::uniform_int_distribution<int> dis(100000, 999999);
            //int ObjectUniqueID = dis(gen);

            //std::cout << "resupply......\n";
            //if (ResupplyNPC)
                //LocalPlayerController->MultiSupplierObtainComponent->ServerRequestProcessInteraction({ 363100004 }, ObjectUniqueID, ResupplyNPC);
            PresetActivate();
        }
    }

    void TeleportWithDistance(float distance, int teleportIndex)
    {
        if (!LocalPlayerCharacter)
            return;

        SDK::FVector CurrentLocation = LocalPlayerCharacter->K2_GetActorLocation();
        static SDK::FRotator StoredCameraRotations[2] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
        static bool bFirstTeleports[2] = { true, true };
        SDK::FRotator CameraRotation = LocalPlayerController->PlayerCameraManager->GetCameraRotation();
        SDK::FRotator TeleportRotation = bFirstTeleports[teleportIndex] ? CameraRotation : StoredCameraRotations[teleportIndex];
        if (!bFirstTeleports[teleportIndex]) {
            TeleportRotation.Pitch = std::clamp(TeleportRotation.Pitch, -45.0f, 45.0f);
            TeleportRotation.Roll = 0.0f;
        }
        StoredCameraRotations[teleportIndex] = CameraRotation;
        bFirstTeleports[teleportIndex] = false;
        float YawRad = CameraRotation.Yaw * (3.14159f / 180.0f);
        float PitchRad = CameraRotation.Pitch * (3.14159f / 180.0f);

        SDK::FVector ForwardDirection;
        ForwardDirection.X = cosf(PitchRad) * cosf(YawRad);
        ForwardDirection.Y = cosf(PitchRad) * sinf(YawRad);
        ForwardDirection.Z = sinf(PitchRad);

        SDK::FVector NewLocation;
        NewLocation.X = CurrentLocation.X + (ForwardDirection.X * distance);
        NewLocation.Y = CurrentLocation.Y + (ForwardDirection.Y * distance);
        NewLocation.Z = CurrentLocation.Z + (ForwardDirection.Z * distance);

        if (LocalPlayerCharacter->TeleportHandler)
        {
            LocalPlayerCharacter->TeleportHandler->ServerMoveToTeleportToLocation(NewLocation, TeleportRotation);
            LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
        }
    }

    void TeleportForward()
    {
        TeleportWithDistance(CFG::cfg_Teleport_TeleportDistance, 0);
    }

    void TeleportForward2()
    {
        TeleportWithDistance(CFG::cfg_Teleport_TeleportDistance2, 1);
    }

    void TeleportToTarget()
    {
        if (!CFG::cfg_Teleport_EnableTeleportToTarget)
        {
            TeleportForward();
            return;
        }

        SDK::FVector* savePointTarget = nullptr;
        TFD::AActor* enemyTarget = nullptr;
        if (CFG::cfg_SavePoint_RenderSavePoints)
        {
            savePointTarget = SavePointCFG::GetClosestSavePoint();
        }

        if (!savePointTarget)
        {
            enemyTarget = GetClosestEnemyForTeleport();
        }

        if (!savePointTarget && !enemyTarget)
        {
            TeleportForward();
            return;
        }

        if (!LocalPlayerCharacter || !LocalPlayerCharacter->TeleportHandler)
            return;

        SDK::FVector targetLocation;
        SDK::FVector playerLocation = LocalPlayerCharacter->K2_GetActorLocation();

        if (savePointTarget)
        {
            targetLocation = *savePointTarget;
        }
        else
        {
            targetLocation = enemyTarget->K2_GetActorLocation();
        }

        // Apply configurable offset - move back towards player position by offset amount
        if (CFG::cfg_Teleport_TargetOffset > 0.0f)
        {
            SDK::FVector directionToTarget = targetLocation - playerLocation;
            float distance = sqrtf(directionToTarget.X * directionToTarget.X + directionToTarget.Y * directionToTarget.Y + directionToTarget.Z * directionToTarget.Z);
            if (distance > 0.0001f)
            {
                SDK::FVector normalizedDirection = { directionToTarget.X / distance, directionToTarget.Y / distance, directionToTarget.Z / distance };
                targetLocation = targetLocation - (normalizedDirection * CFG::cfg_Teleport_TargetOffset);
            }
        }

        SDK::FRotator currentRotation = LocalPlayerController->PlayerCameraManager->GetCameraRotation();

        LocalPlayerCharacter->TeleportHandler->ServerMoveToTeleportToLocation(targetLocation, currentRotation);
        LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
    }

    void SavedPointsESP()
    {
        if (!SavePointMap.empty() && CFG::cfg_SavePoint_RenderSavePoints)
        {
            static float rotationTime = 0.0f;
            rotationTime += 0.15f;
            if (rotationTime > 360.0f) rotationTime = 0.0f;

            SDK::FRotator rotation;
            rotation.Pitch = 0.0f;
            rotation.Yaw = rotationTime * 2.0f;
            rotation.Roll = 0.0f;

            for (auto& pair : SavePointMap)
            {
                int index = pair.first;
                SDK::FVector spherePos = { pair.second.X, pair.second.Y, pair.second.Z };

                SDK::FVector2D ScreenPos;
                bool OnScreen = Cheat::WorldToScreen(spherePos, &ScreenPos);
                if (!OnScreen)
                    continue;

                // Calculate distance for LOD system
                float distance = 0.0f;
                if (LocalPlayerCharacter)
                {
                    SDK::FVector playerPos = LocalPlayerCharacter->K2_GetActorLocation();
                    distance = spherePos.GetDistanceTo(playerPos) / 100.0f; // Convert to meters
                }

                SDK::FLinearColor configColor;
                float beamIntensity;

                if (index == CurrentSavePointIndex)
                {
                    configColor =
                    {
                        CFG::cfg_SavePoint_SphereColorR,
                        CFG::cfg_SavePoint_SphereColorG,
                        CFG::cfg_SavePoint_SphereColorB,
                        CFG::cfg_SavePoint_SphereColorA
                    };
                    beamIntensity = 1.0f;
                }
                else
                {
                    configColor =
                    {
                        CFG::cfg_SavePoint_InactiveSphereColorR,
                        CFG::cfg_SavePoint_InactiveSphereColorG,
                        CFG::cfg_SavePoint_InactiveSphereColorB,
                        CFG::cfg_SavePoint_InactiveSphereColorA
                    };
                    beamIntensity = 0.6f;
                }

                // Distance-based LOD rendering
                if (distance > 150.0f)
                {
                    // Far distance: Low quality - simple sphere, no rotation, no beams, RGB only
                    if (CFG::cfg_ESP_GradientCycling) {
                        float gradientTime = Render::GetSharedGradientTime();
                        SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.0f, gradientTime);
                        Render::R_Draw3DSphere(spherePos, 25.0f, &flowingColor);
                    }
                    else {
                        Render::R_Draw3DSphere(spherePos, 25.0f, &configColor);
                    }
                }
                else if (distance > 100.0f)
                {
                    // Medium distance: Gradient circle only, no rotation, no beams
                    if (CFG::cfg_ESP_GradientCycling) {
                        float gradientTime = Render::GetSharedGradientTime();
                        SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.0f, gradientTime);
                        Render::R_Draw3DSphere(spherePos, 25.0f, &flowingColor);
                    }
                    else {
                        Render::R_Draw3DSphere(spherePos, 25.0f, &configColor);
                    }
                }
                else
                {
                    // Close distance: Full quality - rotating spheres with light beams
                    if (CFG::cfg_ESP_GradientCycling) {
                        float gradientTime = Render::GetSharedGradientTime();
                        SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.0f, gradientTime);
                        Render::R_Draw3DSphereLightBeam(spherePos, 30.0f, rotation, 150.0f, beamIntensity, &flowingColor);
                    }
                    else {
                        Render::R_Draw3DSphereLightBeam(spherePos, 30.0f, rotation, 150.0f, beamIntensity, &configColor);
                    }
                }
                SDK::FVector textPos = { spherePos.X, spherePos.Y, spherePos.Z + 60.0f };
                SDK::FVector2D textScreenPos;
                if (Cheat::WorldToScreen(textPos, &textScreenPos))
                {
                    CFG::SavePoint& savePoint = const_cast<CFG::SavePoint&>(pair.second);
                }
                if (CFG::cfg_Friendly_ShowDistance)
                {
                    float distance = 0.0f;
                    if (LocalPlayerCharacter)
                    {
                        SDK::FVector playerPos = LocalPlayerCharacter->K2_GetActorLocation();
                        distance = spherePos.GetDistanceTo(playerPos) / 100.0f;
                    }
                    SDK::FVector textPos = { spherePos.X, spherePos.Y, spherePos.Z + 60.0f };
                    SDK::FVector2D textScreenPos;
                    if (Cheat::WorldToScreen(textPos, &textScreenPos))
                    {
                        std::string nameAndDistance = std::format("{} ({}m)", pair.second.Name, (int)distance);
                        Render::R_DrawText(textScreenPos.X, textScreenPos.Y, nameAndDistance, &configColor, true);
                    }
                }
                if (CFG::cfg_Teleport_EnableTeleportToTarget && CFG::cfg_Teleport_TeleportTargetMarking)
                {
                    SDK::FVector* currentTarget = SavePointCFG::GetClosestSavePoint();
                    SDK::FVector savePointPos = { pair.second.X, pair.second.Y, pair.second.Z };
                    if (currentTarget && currentTarget == &savePointPos)
                    {
                        SDK::FRotator innerRotation;
                        innerRotation.Pitch = rotationTime * 5.0f;
                        innerRotation.Yaw = -rotationTime * 4.0f;
                        innerRotation.Roll = rotationTime * 2.0f;

                        SDK::FLinearColor teleportColor =
                        {
                            CFG::cfg_Teleport_TeleportTargetColorR,
                            CFG::cfg_Teleport_TeleportTargetColorG,
                            CFG::cfg_Teleport_TeleportTargetColorB,
                            CFG::cfg_Teleport_TeleportTargetColorA
                        };
                        if (CFG::cfg_ESP_GradientCycling) {
                            float gradientTime = Render::GetSharedGradientTime();
                            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.5f, gradientTime); // Different position offset for variety
                            Render::R_Draw3DSphereLightBeam(spherePos, 50.0f, innerRotation, 200.0f, 1.2f, &flowingColor);
                        }
                        else {
                            Render::R_Draw3DSphereLightBeam(spherePos, 50.0f, innerRotation, 200.0f, 1.2f, &teleportColor);
                        }
                    }
                }
            }
        }
    }





    void ForceSpawnVehicle()
    {
        if (!LocalPlayerCharacter || !LocalPlayerCharacter->VehicleHandlerComponent)
            return;
        LocalPlayerCharacter->VehicleHandlerComponent->ServerSpawnEquippedVehicle(true, true);
    }

    void ModifyVehicleStats()
    {
        if (!LocalPlayerCharacter || !LocalPlayerCharacter->VehicleHandlerComponent || !LocalPlayerCharacter->VehicleHandlerComponent->MountedVehicle)
            return;
        TFD::AM1Vehicle* MV = LocalPlayerCharacter->VehicleHandlerComponent->MountedVehicle;
        if (MV->CurrentMovementData.MaxAcceleration != CFG::cfg_Extra_MaxAcceleration)
            MV->CurrentMovementData.MaxAcceleration = CFG::cfg_Extra_MaxAcceleration;

        if (MV->CurrentMovementData.MaxSpeed != CFG::cfg_Extra_MaxSpeed)
            MV->CurrentMovementData.MaxSpeed = CFG::cfg_Extra_MaxSpeed;

        if (MV->CurrentMovementData.MaxTurnSpeed != CFG::cfg_Extra_MaxTurnSpeed)
            MV->CurrentMovementData.MaxTurnSpeed = CFG::cfg_Extra_MaxTurnSpeed;
    }




    void EnabledFeaturesOverlay()
    {
        if (!CFG::cfg_Menu_ShowEnabledFeatures)
            return;

        int YPos = 40;
        int XPos = 430;  // Moved right for better spacing

        // RGB support for header
        if (CFG::cfg_ESP_GradientCycling) {
            float gradientTime = Render::GetSharedGradientTime();
            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.0f, gradientTime);
            Render::R_DrawText(XPos, YPos - 20, "Enabled Features:", &flowingColor, true);
        }
        else {
            Render::R_DrawText(XPos, YPos - 20, "Enabled Features:", &ColorGreen, true);
        }

        // Feature counter for RGB offset
        int featureIndex = 0;

        // Check and display enabled features (sorted by display name length, longest first)

        // 30 chars: "Teleport To Targets FoV Circle"
        if (CFG::cfg_Teleport_DrawTeleportFOVCircle)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Teleport To Targets FoV Circle", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Teleport To Targets FoV Circle", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }

        // 26 chars: "Tokyo Drift (Vehicle Mods)"
        if (CFG::cfg_Extra_EnableVehicleMods)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Tokyo Drift (Vehicle Mods)", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Tokyo Drift (Vehicle Mods)", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }

        // 25 chars: "Container Drop Multiplier"
        if (CFG::cfg_Loot_MultiplyDrops)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Container Drop Multiplier", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Container Drop Multiplier", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }

        // 24 chars: "Teleport Target Marking"
        if (CFG::cfg_Teleport_TeleportTargetMarking)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Teleport Target Marking", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Teleport Target Marking", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }

        // 23 chars: "Sphere Teleport Spheres"
        if (CFG::cfg_Teleport_ShowTeleportSphere1 || CFG::cfg_Teleport_ShowTeleportSphere2)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Preview Sphere 1", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Sphere Teleport Spheres", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }

        // 20 chars: "Mission Item Vacuum"
        if (CFG::cfg_Loot_VacuumMissionItems)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Mission Item Vacuum", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Mission Item Vacuum", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Abilities_EnableAutomaticResupply)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Automatic Resupply", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Automatic Resupply", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Abilities_AutomaticResupplyHealth)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Low Health Resupply", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Low Health Resupply", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Abilities_AutomaticResupplyTimed)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Timed Resupply", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Timed Resupply", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Mission_EnableInstantInfiltration)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Instant Infiltration", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Instant Infiltration", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Mission_EnableMissionAutoRestart)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Mission Auto Restart", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Mission Auto Restart", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Teleport_EnableTeleportToTarget)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Teleport to Target", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Teleport to Target", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Abilities_EnableModifyGrapple)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Grapple Distance", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Grapple Distance", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Actors_EnableCollectibleESP)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Collectible ESP", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Collectible ESP", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Aim_DrawFOVCircle)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Aimbot FoV Circle", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Aimbot FoV Circle", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_SavePoint_RenderSavePoints)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Save Point Render", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Save Point Render", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Aim_NoRecoilAndSpread)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "No Recoil/Spread", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "No Recoil/Spread", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Loot_ShowDistance)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Show Actor Distance", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Show Actor Distance", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Friendly_ShowDistance)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Friendly Distance", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Friendly Distance", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Enemy_ShowDistance)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Enemy Distance", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Enemy Distance", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Loot_EnableLootVacuum)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Loot Vacuum", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Loot Vacuum", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Telekinesis_Enable)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Telekinesis", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Telekinesis", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Loot_EnableItemESP)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Item ESP", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Item ESP", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Aim_SilentAim)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "Silent Aim", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "Silent Aim", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
        if (CFG::cfg_Aim_NoReload)
        {
            if (CFG::cfg_ESP_GradientCycling) {
                float gradientTime = Render::GetSharedGradientTime();
                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f + (featureIndex * 0.05f), gradientTime);
                Render::R_DrawText(XPos, YPos, "No Reload", &flowingColor, true);
            }
            else {
                Render::R_DrawText(XPos, YPos, "No Reload", &ColorGreen, true);
            }
            YPos += 20;
            featureIndex++;
        }
    }
}