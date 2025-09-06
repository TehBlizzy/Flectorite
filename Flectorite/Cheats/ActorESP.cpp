#include "ActorESP.h"
#include "ESP.h"
#include <algorithm>

namespace Cheat
{
    void PlayerEnemyESP()
    {
        TFD::UM1ActorManagerSubsystem* Actors = LocalPlayerController->ActorManager_Subsystem;
        if (Actors->Characters.IsValid() && Actors->Characters.Num() > 0)
        {
            int StartNumber = Actors->Characters.Num();
            for (int i = 0; i < Actors->Characters.Num(); i++)
            {
                if (!GWorld || !LocalPlayerController || !LocalPlayerController->ActorManager_Subsystem || !Actors || !Actors->Characters)
                    return;
                if (Actors->Characters.Num() != StartNumber)
                    return;
                if (!Actors->Characters.IsValidIndex(i))
                    continue;
                TFD::AM1Character* p = Actors->Characters[i];
                if (p)
                {
                    if (p == LocalPlayerCharacter)
                        continue;
                    if (p->IsDead())
                        continue;
                    if (p->IsA(TFD::AM1Player::StaticClass()))
                    {
                        TFD::AM1Player* player = static_cast<TFD::AM1Player*>(p);
                        SDK::FVector2D ScreenPos = { -1, -1 };
                        if (WorldToScreen(p->K2_GetActorLocation(), &ScreenPos))
                        {
                            ESP::RenderPlayerESP(player, ScreenPos);
                        }
                    }
                    else if (p->IsA(TFD::AM1Monster::StaticClass()) || p->CharacterAttribute->IsA(TFD::UM1MonsterAttribute::StaticClass()))
                    {
                        SDK::FVector2D ScreenPos = { -1, -1 };
                        if (WorldToScreen(p->K2_GetActorLocation(), &ScreenPos))
                        {
                            ESP::RenderEnemyESP(p, ScreenPos);
                        }
                    }
                }
            }
        }
    }
    void DrawTeleportPreviewSpheres()
    {
        bool shouldRenderPreviewSpheres = CFG::cfg_Teleport_ShowTeleportSphere1 || CFG::cfg_Teleport_ShowTeleportSphere2;
        bool shouldRenderTargetMarking = CFG::cfg_Teleport_TeleportTargetMarking;

        if (!shouldRenderPreviewSpheres && !shouldRenderTargetMarking)
            return;

        if (!LocalPlayerCharacter || !LocalPlayerController || !LocalPlayerController->PlayerCameraManager)
            return;

        SDK::FVector playerLocation = LocalPlayerCharacter->K2_GetActorLocation();
        SDK::FRotator cameraRotation = LocalPlayerController->PlayerCameraManager->GetCameraRotation();

        float yawRad = cameraRotation.Yaw * (3.14159f / 180.0f);
        float pitchRad = cameraRotation.Pitch * (3.14159f / 180.0f);

        SDK::FVector playerForward = LocalPlayerController->PlayerCameraManager->GetActorForwardVector();

        if (playerLocation.X == 0.0f && playerLocation.Y == 0.0f && playerLocation.Z == 0.0f)
            return;
        static float globalSpinTime = 0.0f;
        static float slowRotationTime = 0.0f;
        static auto lastUpdateTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastUpdateTime).count();
        lastUpdateTime = currentTime;

        globalSpinTime += deltaTime * 360.0f;
        slowRotationTime += deltaTime * 45.0f;
        if (globalSpinTime > 360.0f) globalSpinTime -= 360.0f;
        if (slowRotationTime > 360.0f) slowRotationTime -= 360.0f;

        try {
            TFD::AActor* teleportTarget = nullptr;
            if (CFG::cfg_Teleport_EnableTeleportToTarget || CFG::cfg_Teleport_TeleportTargetMarking)
            {
                teleportTarget = GetClosestEnemyForTeleport();
            }

            if (CFG::cfg_Teleport_TeleportTargetMarking && teleportTarget && !Menu::ShowMenu)
            {
                SDK::FVector targetLocation = teleportTarget->K2_GetActorLocation();

                if (targetLocation.X != 0.0f || targetLocation.Y != 0.0f || targetLocation.Z != 0.0f)
                {
                    SDK::FLinearColor targetColor = SDK::FLinearColor(CFG::cfg_Teleport_TeleportTargetColorR, CFG::cfg_Teleport_TeleportTargetColorG, CFG::cfg_Teleport_TeleportTargetColorB, 0.8f);
                    float targetSize = CFG::cfg_Teleport_TeleportSphere1Size * 2.0f;

                    SDK::FRotator outerRotation = SDK::FRotator(0, globalSpinTime, 0);
                    SDK::FRotator innerRotation = SDK::FRotator(0, -globalSpinTime * 0.75f, 0);

                    if (CFG::cfg_ESP_GradientCycling) {
                        float gradientTime = Render::GetSharedGradientTime();
                        SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.0f, gradientTime);

                        if (CFG::cfg_Teleport_UseLightBeamSpheres) {
                            Render::R_Draw3DSphereLightBeam(targetLocation, targetSize, outerRotation, 150.0f, 3.0f, &flowingColor);
                            Render::R_Draw3DSphereLightBeam(targetLocation, targetSize * 0.5f, innerRotation, 100.0f, 2.0f, &flowingColor);
                        }
                        else {
                            Render::R_Draw3DSphere(targetLocation, targetSize, &flowingColor);
                            Render::R_Draw3DSphere(targetLocation, targetSize * 0.5f, &flowingColor);
                        }
                    }
                    else {
                        if (CFG::cfg_Teleport_UseLightBeamSpheres) {
                            Render::R_Draw3DSphereLightBeam(targetLocation, targetSize, outerRotation, 150.0f, 3.0f, &targetColor);
                            SDK::FLinearColor innerColor = SDK::FLinearColor(targetColor.R * 0.6f, targetColor.G * 0.8f, targetColor.B * 0.8f, 0.9f);
                            Render::R_Draw3DSphereLightBeam(targetLocation, targetSize * 0.5f, innerRotation, 100.0f, 2.0f, &innerColor);
                        }
                        else {
                            Render::R_Draw3DSphere(targetLocation, targetSize, &targetColor);
                            Render::R_Draw3DSphere(targetLocation, targetSize * 0.5f, &targetColor);
                        }
                    }
                }
            }

            if (CFG::cfg_Teleport_ShowTeleportSphere1 && !Menu::ShowMenu)
            {
                SDK::FVector sphere1Location;
                bool isTargetingEnemy = false;

                if (CFG::cfg_Teleport_EnableTeleportToTarget && teleportTarget)
                {
                    sphere1Location = teleportTarget->K2_GetActorLocation();
                    isTargetingEnemy = true;
                }
                else
                {
                    sphere1Location = playerLocation + (playerForward * CFG::cfg_Teleport_TeleportDistance);
                    if (CFG::cfg_Teleport_GroundedSpheres)
                    {
                        sphere1Location.Z = playerLocation.Z + CFG::cfg_Teleport_GroundOffset;
                    }
                }

                SDK::FLinearColor sphere1Color;
                float sphereSize = CFG::cfg_Teleport_TeleportSphere1Size;
                SDK::FRotator sphereRotation = SDK::FRotator(0, 0, 0);

                if (CFG::cfg_Teleport_UseLightBeamSpheres && !Menu::ShowMenu)
                {
                    sphereRotation = SDK::FRotator(0, slowRotationTime, 0);
                }

                if (isTargetingEnemy && !CFG::cfg_Teleport_TeleportTargetMarking)
                {
                    sphere1Color = SDK::FLinearColor(CFG::cfg_Teleport_TeleportTargetColorR, CFG::cfg_Teleport_TeleportTargetColorG, CFG::cfg_Teleport_TeleportTargetColorB, 0.8f);
                    sphereSize *= 1.25f;
                    sphereRotation = SDK::FRotator(0, globalSpinTime, 0);

                    if (sphere1Location.X != 0.0f || sphere1Location.Y != 0.0f || sphere1Location.Z != 0.0f)
                    {
                        if (CFG::cfg_ESP_GradientCycling) {
                            float gradientTime = Render::GetSharedGradientTime();
                            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f, gradientTime);

                            if (CFG::cfg_Teleport_UseLightBeamSpheres) {
                                Render::R_Draw3DSphereLightBeam(sphere1Location, sphereSize, sphereRotation, 125.0f, 3.0f, &flowingColor);
                            }
                            else {
                                Render::R_Draw3DSphere(sphere1Location, sphereSize, &flowingColor);
                            }
                        }
                        else {
                            if (CFG::cfg_Teleport_UseLightBeamSpheres) {
                                Render::R_Draw3DSphereLightBeam(sphere1Location, sphereSize, sphereRotation, 125.0f, 3.0f, &sphere1Color);
                            }
                            else {
                                Render::R_Draw3DSphere(sphere1Location, sphereSize, &sphere1Color);
                            }
                        }
                    }
                    return;
                }
                else
                {
                    sphere1Color = SDK::FLinearColor(CFG::cfg_Teleport_TeleportSphere1ColorR, CFG::cfg_Teleport_TeleportSphere1ColorG, CFG::cfg_Teleport_TeleportSphere1ColorB, 0.6f);
                }

                if (sphere1Location.X != 0.0f || sphere1Location.Y != 0.0f || sphere1Location.Z != 0.0f)
                {
                    if (CFG::cfg_ESP_GradientCycling) {
                        float gradientTime = Render::GetSharedGradientTime();
                        SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f, gradientTime);

                        if (CFG::cfg_Teleport_UseLightBeamSpheres && !Menu::ShowMenu) {
                            Render::R_Draw3DSphereLightBeam(sphere1Location, sphereSize, sphereRotation, 90.0f, 2.0f, &flowingColor);
                        }
                        else {
                            Render::R_Draw3DSphere(sphere1Location, sphereSize, &flowingColor);
                        }
                    }
                    else {
                        if (CFG::cfg_Teleport_UseLightBeamSpheres && !Menu::ShowMenu) {
                            Render::R_Draw3DSphereLightBeam(sphere1Location, sphereSize, sphereRotation, 90.0f, 2.0f, &sphere1Color);
                        }
                        else {
                            Render::R_Draw3DSphere(sphere1Location, sphereSize, &sphere1Color);
                        }
                    }
                }
            }
            if (CFG::cfg_Teleport_ShowTeleportSphere2 && !Menu::ShowMenu)
            {
                SDK::FVector sphere2Location = playerLocation + (playerForward * CFG::cfg_Teleport_TeleportDistance2);
                if (CFG::cfg_Teleport_GroundedSpheres)
                {
                    sphere2Location.Z = playerLocation.Z + CFG::cfg_Teleport_GroundOffset;
                }

                SDK::FLinearColor sphere2Color = SDK::FLinearColor(CFG::cfg_Teleport_TeleportSphere2ColorR, CFG::cfg_Teleport_TeleportSphere2ColorG, CFG::cfg_Teleport_TeleportSphere2ColorB, 0.75f);

                if (sphere2Location.X != 0.0f || sphere2Location.Y != 0.0f || sphere2Location.Z != 0.0f)
                {
                    if (CFG::cfg_ESP_GradientCycling) {
                        float gradientTime = Render::GetSharedGradientTime();
                        SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.3f, gradientTime);

                        if (CFG::cfg_Teleport_UseLightBeamSpheres) {
                            Render::R_Draw3DSphereLightBeam(sphere2Location, CFG::cfg_Teleport_TeleportSphere2Size, SDK::FRotator(0, 0, 0), 80.0f, 1.25f, &flowingColor);
                        }
                        else {
                            Render::R_Draw3DSphere(sphere2Location, CFG::cfg_Teleport_TeleportSphere2Size, &flowingColor);
                        }
                    }
                    else {
                        if (CFG::cfg_Teleport_UseLightBeamSpheres) {
                            Render::R_Draw3DSphereLightBeam(sphere2Location, CFG::cfg_Teleport_TeleportSphere2Size, SDK::FRotator(0, 0, 0), 80.0f, 1.25f, &sphere2Color);
                        }
                        else {
                            Render::R_Draw3DSphere(sphere2Location, CFG::cfg_Teleport_TeleportSphere2Size, &sphere2Color);
                        }
                    }
                }
            }
        }
        catch (...) {
            return;
        }
    }
    void CollectibleESP()
    {
        if (!CFG::cfg_Actors_EnableCollectibleESP && !Menu::ShowMenu)
            return;

        if (!GWorld || !GWorld->Levels.IsValid())
            return;

        try {
            for (int i = 0; i < GWorld->Levels.Num(); ++i)
            {
                if (!GWorld->Levels.IsValidIndex(i))
                    continue;

                TFD::ULevel* Level = GWorld->Levels[i];
                if (!Level || !Level->Actors.IsValid())
                    continue;

                for (int j = 0; j < Level->Actors.Num(); ++j)
                {
                    if (!Level->Actors.IsValidIndex(j))
                        continue;

                    TFD::AActor* Actor = (TFD::AActor*)Level->Actors[j];
                    if (!Actor)
                        continue;

                    if (!Actor->IsA(TFD::ABP_NpcBase_C::StaticClass()))
                        continue;

                    std::string actorName = Actor->GetFullName();
                    std::string displayName = "";
                    SDK::FLinearColor* color = nullptr;

                    if (actorName.contains("TextJournal"))
                    {
                        displayName = "Journal";
                        color = &ColorGold;
                    }
                    else if (actorName.contains("VoiceJournal"))
                    {
                        displayName = "Echoes";
                        color = &ColorMana;
                    }
                    else if (actorName.contains("ArcheTrace"))
                    {
                        displayName = "Echoes";
                        color = &ColorMana;
                    }
                    else if (actorName.contains("VulgusRecord"))
                    {
                        displayName = "Vulgus Record";
                        color = &ColorGreen;
                    }
                    else if (actorName.contains("AncestorRecord"))
                    {
                        displayName = "Ancestors Record";
                        color = &ColorGold;
                    }

                    if (!displayName.empty() && color)
                    {
                        SDK::FVector2D ScreenPos = { -1, -1 };
                        SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
                        bool OnScreen = WorldToScreen(WorldPosition, &ScreenPos);

                        if (OnScreen)
                        {
                            if (CFG::cfg_ESP_GradientCycling)
                            {
                                float gradientTime = Render::GetSharedGradientTime();
                                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(1.0f, gradientTime);
                                Render::R_DrawText(ScreenPos.X, ScreenPos.Y, displayName, &flowingColor);
                                float distance = Actor->GetDistanceTo(LocalPlayerCharacter) / 100.0f;
                                std::string distanceText = std::to_string((int)distance) + "m";
                                SDK::FLinearColor distanceColor = Render::GetFlowingRainbowColor(1.1f, gradientTime);
                                Render::R_DrawText(ScreenPos.X, ScreenPos.Y + 15, distanceText, &distanceColor);
                            }
                            else
                            {
                                Render::R_DrawText(ScreenPos.X, ScreenPos.Y, displayName, color);
                                float distance = Actor->GetDistanceTo(LocalPlayerCharacter) / 100.0f;
                                std::string distanceText = std::to_string((int)distance) + "m";
                                Render::R_DrawText(ScreenPos.X, ScreenPos.Y + 15, distanceText, color);
                            }
                        }
                    }
                }
            }
        }
        catch (...) {
            return;
        }
        Cheat::RenderTelekinesisMarkers();
    }
}