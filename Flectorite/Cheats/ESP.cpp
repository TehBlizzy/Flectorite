#include "ESP.h"



namespace Cheat
{
    void RenderESPDistance(SDK::FVector2D screenPos, float distance, SDK::FLinearColor* staticColor, float gradientOffset, float yOffset)
    {

        std::string distanceText = std::to_string((int)distance) + "m";

        if (CFG::cfg_ESP_GradientCycling)
        {
            float gradientTime = Render::GetSharedGradientTime();
            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(gradientOffset, gradientTime);
            Render::R_DrawText(screenPos.X, screenPos.Y + yOffset, distanceText, &flowingColor);
        }
        else
        {
            Render::R_DrawText(screenPos.X, screenPos.Y + yOffset, distanceText, staticColor);
        }
    }

    void RenderESPName(SDK::FVector2D screenPos, const std::string& name, SDK::FLinearColor* staticColor, float gradientOffset)
    {
        if (name.empty())
            return;

        if (CFG::cfg_ESP_GradientCycling)
        {
            float gradientTime = Render::GetSharedGradientTime();
            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(gradientOffset, gradientTime);
            Render::R_DrawText(screenPos.X, screenPos.Y, name, &flowingColor);
        }
        else
        {
            Render::R_DrawText(screenPos.X, screenPos.Y, name, staticColor);
        }
    }

    void RenderESPBox(SDK::FVector2D screenPos, float boxSize, SDK::FLinearColor* staticColor, float gradientOffset)
    {
        if (CFG::cfg_ESP_GradientCycling)
        {
            float gradientTime = Render::GetSharedGradientTime();
            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(gradientOffset, gradientTime);
            Render::R_DrawRect(
                screenPos.X - boxSize / 1.5f,
                screenPos.Y - boxSize,
                screenPos.X + boxSize / 1.5f,
                screenPos.Y + boxSize, &flowingColor);
        }
        else
        {
            Render::R_DrawRect(
                screenPos.X - boxSize / 1.5f,
                screenPos.Y - boxSize,
                screenPos.X + boxSize / 1.5f,
                screenPos.Y + boxSize, staticColor);
        }
    }

    void RenderESP3DBox(SDK::FVector actorPos, SDK::FVector boxExtent, SDK::FRotator rotation, SDK::FLinearColor* staticColor, float gradientOffset)
    {
        if (CFG::cfg_ESP_GradientCycling)
        {
            float gradientTime = Render::GetSharedGradientTime();
            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(gradientOffset, gradientTime);
            Render::R_Draw3DBox(actorPos, boxExtent, &flowingColor);
        }
        else
        {
            Render::R_Draw3DBoxRotated(actorPos, boxExtent, rotation, staticColor);
        }
    }

    void RenderESPSphere(SDK::FVector actorPos, float radius, SDK::FLinearColor* staticColor, float gradientOffset)
    {
        if (CFG::cfg_ESP_GradientCycling)
        {
            float gradientTime = Render::GetSharedGradientTime();
            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(gradientOffset, gradientTime);
            Render::R_Draw3DSphere(actorPos, radius, &flowingColor);
        }
        else
        {
            Render::R_Draw3DSphere(actorPos, radius, staticColor);
        }
    }

    void RenderESPCircle(SDK::FVector2D screenPos, float radius, SDK::FLinearColor* staticColor, float gradientOffset)
    {
        if (CFG::cfg_ESP_GradientCycling)
        {
            float gradientTime = Render::GetSharedGradientTime();
            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(gradientOffset, gradientTime);
            Render::R_DrawCircle(screenPos.X, screenPos.Y, radius, &flowingColor);
        }
        else
        {
            Render::R_DrawCircle(screenPos.X, screenPos.Y, radius, staticColor);
        }
    }

    void RenderESPLine(SDK::FVector2D startPos, SDK::FVector2D endPos, SDK::FLinearColor* staticColor, float gradientOffset)
    {
        if (CFG::cfg_ESP_GradientCycling)
        {
            float gradientTime = Render::GetSharedGradientTime();
            SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(gradientOffset, gradientTime);
            Render::R_DrawLine(startPos.X, startPos.Y, endPos.X, endPos.Y, &flowingColor);
        }
        else
        {
            {
                Render::R_DrawLine(startPos.X, startPos.Y, endPos.X, endPos.Y, staticColor);
            }
        }
    }


    void RenderPlayerESP(TFD::AM1Player* player, SDK::FVector2D screenPos)
    {
        if (!player || Menu::ShowMenu)
            return;

        float distance = player->GetDistanceTo(LocalPlayerCharacter) / 100.0f;

        if (CFG::cfg_Friendly_DrawNames && player->PlayerName)
        {
            std::string playerNameStr = player->PlayerName.ToString();
            if (!playerNameStr.empty())
            {
                RenderESPName(screenPos, playerNameStr, &ColorGreen, 0.2f);
                RenderESPDistance(screenPos, distance, &ColorGreen, 0.3f, 15.0f);
            }
        }

        if (CFG::cfg_Friendly_DrawBoxes && player->Mesh && player->Mesh->SkeletalMesh)
        {
            float ODistance = player->GetDistanceTo(Cheat::LocalPlayerCharacter) / CFG::cfg_Actors_ShapeDistanceScale;
            if (ODistance > 0)
            {
                float BoxSize = CFG::cfg_Actors_ShapeSize / ODistance;
                BoxSize = std::clamp(BoxSize, 8.0f, 25.0f);
                RenderESPBox(screenPos, BoxSize, &Cheat::ColorGreen, 0.2f);
                if (CFG::cfg_Friendly_ShowDistance)
                {
                    RenderESPDistance(screenPos, distance, &Cheat::ColorGreen, 0.4f, -BoxSize - 15);
                }
            }
        }

        if (CFG::cfg_Friendly_Draw3DBoxes && player->Mesh && player->Mesh->SkeletalMesh)
        {
            SDK::FVector ActorLocation = player->K2_GetActorLocation();
            SDK::FRotator ActorRotation = player->K2_GetActorRotation();
            float Distance = player->GetDistanceTo(LocalPlayerController->Pawn);
            float BoxScale = std::clamp(Distance / 1000.0f, 0.5f, 2.0f);
            SDK::FVector BoxExtent(35.0f * BoxScale, 35.0f * BoxScale, 90.0f * BoxScale);

            RenderESP3DBox(ActorLocation, BoxExtent, ActorRotation, &ColorGreen, 0.4f);

            if (CFG::cfg_Friendly_ShowDistance)
            {
                SDK::FVector BoxTopPoint = ActorLocation + SDK::FVector(0, 0, BoxExtent.Y);
                SDK::FVector2D BoxTopScreenPos;
                if (WorldToScreen(BoxTopPoint, &BoxTopScreenPos))
                {
                    float screenHeight = abs(screenPos.Y - BoxTopScreenPos.Y);
                    RenderESPDistance({ screenPos.X, screenPos.Y - screenHeight - 30 }, distance, &ColorGreen, 0.5f, 0.0f);
                }
                else
                {
                    RenderESPDistance(screenPos, distance, &ColorGreen, 0.5f, -20.0f);
                }
            }
        }

        if (CFG::cfg_Friendly_DrawCircles && player->Mesh && player->Mesh->SkeletalMesh)
        {
            float ODistance = player->GetDistanceTo(Cheat::LocalPlayerCharacter) / CFG::cfg_Actors_ShapeDistanceScale;
            if (ODistance > 0)
            {
                float Radius = CFG::cfg_Actors_ShapeSize / ODistance;
                Radius = std::clamp(Radius, 4.0f, 20.0f);
                RenderESPCircle(screenPos, Radius, &Cheat::ColorGreen, 0.2f);
                if (CFG::cfg_Friendly_ShowDistance)
                {
                    RenderESPDistance(screenPos, distance, &Cheat::ColorGreen, 0.6f, -Radius - 15);
                }
            }
        }

        if (CFG::cfg_Friendly_Draw3DSpheres && player->Mesh && player->Mesh->SkeletalMesh)
        {
            SDK::FVector ActorLocation = player->K2_GetActorLocation();
            float Distance = player->GetDistanceTo(LocalPlayerController->Pawn);
            float SphereScale = std::clamp(Distance / 1000.0f, 0.5f, 2.0f);
            float SphereRadius = 50.0f * SphereScale;

            RenderESPSphere(ActorLocation, SphereRadius, &ColorGreen, 0.7f);

            if (CFG::cfg_Friendly_ShowDistance)
            {
                SDK::FVector2D SphereScreenPos;
                if (WorldToScreen(ActorLocation, &SphereScreenPos))
                {
                    SDK::FVector SphereTopPoint = ActorLocation + SDK::FVector(0, 0, SphereRadius);
                    SDK::FVector2D SphereTopScreenPos;
                    if (WorldToScreen(SphereTopPoint, &SphereTopScreenPos))
                    {
                        float screenRadius = abs(SphereScreenPos.Y - SphereTopScreenPos.Y);
                        RenderESPDistance({ SphereScreenPos.X, SphereScreenPos.Y - screenRadius - 30 }, distance, &ColorGreen, 0.8f, 0.0f);
                    }
                    else
                    {
                        RenderESPDistance({ SphereScreenPos.X, SphereScreenPos.Y - 20 }, distance, &ColorGreen, 0.8f, 0.0f);
                    }
                }
            }
        }

        if (CFG::cfg_Friendly_DrawLines && player->Mesh && player->Mesh->SkeletalMesh)
        {
            SDK::FVector2D LocalScreenPos;
            if (WorldToScreen(LocalPlayerCharacter->K2_GetActorLocation(), &LocalScreenPos))
            {
                RenderESPLine(LocalScreenPos, screenPos, &ColorGreen, 0.3f);

                if (CFG::cfg_Friendly_ShowDistance)
                {
                    float midX = (LocalScreenPos.X + screenPos.X) / 2.0f;
                    float midY = (LocalScreenPos.Y + screenPos.Y) / 2.0f;
                    RenderESPDistance({ midX, midY }, distance, &ColorGreen, 0.9f, 0.0f);
                }
            }
        }
    }

    void RenderEnemyESP(TFD::AM1Character* enemy, SDK::FVector2D screenPos)
    {
        if (!enemy || Menu::ShowMenu)
            return;

        float distance = enemy->GetDistanceTo(LocalPlayerCharacter) / 100.0f;

        if (CFG::cfg_Enemy_DrawNames)
        {
            if (!IDNameMap.contains(enemy->CharacterId.ID))
            {
                static TFD::FString CharacterName;
                CharacterName = *TFD::native_GetCharacterName(enemy, &CharacterName);
                std::string fmtName = CharacterName.ToString();
                fmtName = fmtName.substr(fmtName.find_last_of("_") + 1);
                int id = enemy->CharacterId.ID;
                IDNameMap.insert({ id, fmtName });
                UpdateCache = true;
            }
            else
            {
                RenderESPName(screenPos, IDNameMap[(int)enemy->CharacterId.ID], &ColorRed, 0.6f);
                RenderESPDistance(screenPos, distance, &ColorRed, 0.2f, 15.0f);
            }
        }

        if (CFG::cfg_Enemy_DrawBoxes && enemy->Mesh && enemy->Mesh->SkeletalMesh)
        {
            float ODistance = enemy->GetDistanceTo(Cheat::LocalPlayerCharacter) / CFG::cfg_Actors_ShapeDistanceScale;
            if (ODistance > 0)
            {
                float BoxSize = CFG::cfg_Actors_ShapeSize / ODistance;
                BoxSize = std::clamp(BoxSize, 8.0f, 25.0f);
                RenderESPBox(screenPos, BoxSize, &Cheat::ColorRed, 0.2f);
                if (CFG::cfg_Enemy_ShowDistance)
                {
                    RenderESPDistance(screenPos, distance, &Cheat::ColorRed, 0.4f, -BoxSize - 15);
                }
            }
        }

        if (CFG::cfg_Enemy_Draw3DBoxes && enemy->Mesh && enemy->Mesh->SkeletalMesh)
        {
            SDK::FVector ActorLocation = enemy->K2_GetActorLocation();
            SDK::FRotator ActorRotation = enemy->K2_GetActorRotation();
            float Distance = enemy->GetDistanceTo(LocalPlayerController->Pawn);
            float BoxScale = std::clamp(Distance / 1000.0f, 0.5f, 2.0f);
            SDK::FVector BoxExtent(35.0f * BoxScale, 35.0f * BoxScale, 90.0f * BoxScale);

            RenderESP3DBox(ActorLocation, BoxExtent, ActorRotation, &ColorRed, 0.8f);

            if (CFG::cfg_Enemy_ShowDistance)
            {
                SDK::FVector BoxTopPoint = ActorLocation + SDK::FVector(0, 0, BoxExtent.Y);
                SDK::FVector2D BoxTopScreenPos;
                if (WorldToScreen(BoxTopPoint, &BoxTopScreenPos))
                {
                    float screenHeight = abs(screenPos.Y - BoxTopScreenPos.Y);
                    RenderESPDistance({ screenPos.X, screenPos.Y - screenHeight - 30 }, distance, &ColorRed, 0.9f, 0.0f);
                }
                else
                {
                    RenderESPDistance(screenPos, distance, &ColorRed, 0.9f, -20.0f);
                }
            }
        }

        if (CFG::cfg_Enemy_DrawCircles && enemy->Mesh && enemy->Mesh->SkeletalMesh)
        {
            float ODistance = enemy->GetDistanceTo(Cheat::LocalPlayerCharacter) / CFG::cfg_Actors_ShapeDistanceScale;
            if (ODistance > 0)
            {
                float Radius = CFG::cfg_Actors_ShapeSize / ODistance;
                Radius = std::clamp(Radius, 4.0f, 20.0f);
                RenderESPCircle(screenPos, Radius, &Cheat::ColorRed, 0.2f);
                if (CFG::cfg_Enemy_ShowDistance)
                {
                    RenderESPDistance(screenPos, distance, &Cheat::ColorRed, 0.9f, -Radius - 15);
                }
            }
        }

        if (CFG::cfg_Enemy_Draw3DSpheres && enemy->Mesh && enemy->Mesh->SkeletalMesh)
        {
            SDK::FVector ActorLocation = enemy->K2_GetActorLocation();
            float Distance = enemy->GetDistanceTo(LocalPlayerController->Pawn);
            float SphereScale = std::clamp(Distance / 1000.0f, 0.5f, 2.0f);
            float SphereRadius = 50.0f * SphereScale;

            RenderESPSphere(ActorLocation, SphereRadius, &ColorRed, 0.9f);

            if (CFG::cfg_Enemy_ShowDistance)
            {
                SDK::FVector2D SphereScreenPos;
                if (WorldToScreen(ActorLocation, &SphereScreenPos))
                {
                    SDK::FVector SphereTopPoint = ActorLocation + SDK::FVector(0, 0, SphereRadius);
                    SDK::FVector2D SphereTopScreenPos;
                    if (WorldToScreen(SphereTopPoint, &SphereTopScreenPos))
                    {
                        float screenRadius = abs(SphereScreenPos.Y - SphereTopScreenPos.Y);
                        RenderESPDistance({ SphereScreenPos.X, SphereScreenPos.Y - screenRadius - 30 }, distance, &ColorRed, 1.0f, 0.0f);
                    }
                    else
                    {
                        RenderESPDistance({ SphereScreenPos.X, SphereScreenPos.Y - 20 }, distance, &ColorRed, 1.0f, 0.0f);
                    }
                }
            }
        }

        if (CFG::cfg_Enemy_DrawLines && enemy->Mesh && enemy->Mesh->SkeletalMesh)
        {
            SDK::FVector2D LocalScreenPos;
            if (WorldToScreen(LocalPlayerCharacter->K2_GetActorLocation(), &LocalScreenPos))
            {
                RenderESPLine(LocalScreenPos, screenPos, &ColorRed, 0.8f);

                if (CFG::cfg_Enemy_ShowDistance)
                {
                    float midX = (LocalScreenPos.X + screenPos.X) / 2.0f;
                    float midY = (LocalScreenPos.Y + screenPos.Y) / 2.0f;
                    RenderESPDistance({ midX, midY }, distance, &ColorRed, 1.1f, 0.0f);
                }
            }
        }
    }

    void RenderLootESP(TFD::AActor* lootActor, SDK::FVector2D screenPos, const std::string& itemName, SDK::FLinearColor* itemColor)
    {
        if (!lootActor || Menu::ShowMenu || itemName.empty())
            return;

        float distance = lootActor->GetDistanceTo(LocalPlayerCharacter) / 100.0f;
        SDK::FVector worldPos = lootActor->K2_GetActorLocation();

        if (CFG::cfg_Loot_DrawItemNames)
        {
            RenderESPName(screenPos, itemName, itemColor, 0.5f);

            if (CFG::cfg_Loot_ShowDistance)
            {
                RenderESPDistance(screenPos, distance, itemColor, 0.6f, 15.0f);
            }
        }

        if (CFG::cfg_Loot_DrawItemBoxes)
        {
            float ODistance = lootActor->GetDistanceTo(LocalPlayerController->Pawn) / 100.0f;
            if (ODistance > 0)
            {
                float BoxSize = std::clamp(20.0f / ODistance, 3.0f, 20.0f);
                RenderESPBox(screenPos, BoxSize, itemColor, 0.2f);

                if (CFG::cfg_Loot_ShowDistance)
                {
                    RenderESPDistance(screenPos, distance, itemColor, 0.4f, -BoxSize - 15);
                }
            }
        }

        if (CFG::cfg_Loot_DrawItem3DBoxes)
        {
            float ODistance = lootActor->GetDistanceTo(LocalPlayerController->Pawn) / 100.0f;
            if (ODistance > 0)
            {
                float Size = std::clamp(20.0f / ODistance, 3.0f, 20.0f);
                SDK::FVector extent(Size, Size, Size);
                RenderESP3DBox(worldPos, extent, SDK::FRotator(), itemColor, 0.2f);
            }
        }

        if (CFG::cfg_Loot_DrawItemCircles)
        {
            float ODistance = lootActor->GetDistanceTo(LocalPlayerController->Pawn) / 100.0f;
            if (ODistance > 0)
            {
                float Radius = std::clamp(20.0f / ODistance, 3.0f, 20.0f);
                RenderESPCircle(screenPos, Radius, itemColor, 0.2f);

                if (CFG::cfg_Loot_ShowDistance)
                {
                    RenderESPDistance(screenPos, distance, itemColor, 0.6f, -Radius - 15);
                }
            }
        }

        if (CFG::cfg_Loot_DrawItem3DSpheres)
        {
            float ODistance = lootActor->GetDistanceTo(LocalPlayerController->Pawn) / 100.0f;
            if (ODistance > 0)
            {
                float Radius = std::clamp(20.0f / ODistance, 3.0f, 20.0f);
                RenderESPSphere(worldPos, Radius, itemColor, 0.2f);
            }
        }

    }

    void FriendlyESP()
    {
        if (!CFG::cfg_Friendly_Enable)
            return;

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
                            RenderPlayerESP(player, ScreenPos);
                        }
                    }
                }
            }
        }
    }

    void EnemyESP()
    {
        if (!CFG::cfg_Enemy_Enable)
            return;

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
                    if (p->IsA(TFD::AM1Monster::StaticClass()) || p->CharacterAttribute->IsA(TFD::UM1MonsterAttribute::StaticClass()))
                    {
                        SDK::FVector2D ScreenPos = { -1, -1 };
                        if (WorldToScreen(p->K2_GetActorLocation(), &ScreenPos))
                        {
                            RenderEnemyESP(p, ScreenPos);
                        }
                    }
                }
            }
        }
    }

    void LootESP()
    {
        if (!CFG::cfg_Loot_EnableItemESP || !GWorld || !LocalPlayerController || !LocalPlayerCharacter)
            return;

        TFD::UM1ActorManagerSubsystem* Actors = LocalPlayerController->ActorManager_Subsystem;
        if (!Actors)
            return;

        if (CFG::cfg_Loot_DrawVaults || CFG::cfg_Loot_DrawAxionResources || CFG::cfg_Loot_DrawSupplyResources)
        {
            if (GWorld->Levels.IsValid())
            {
                for (int i = 0; i < GWorld->Levels.Num(); ++i)
                {
                    if (!GWorld->Levels.IsValidIndex(i)) continue;
                    TFD::ULevel* Level = GWorld->Levels[i];
                    if (!Level) continue;

                    for (int j = 0; j < Level->Actors.Num(); ++j)
                    {
                        TFD::AActor* Actor = (TFD::AActor*)Level->Actors[j];
                        if (!Actor) continue;

                        if (CFG::cfg_Loot_DrawVaults && !Menu::ShowMenu)
                        {
                            if (Actor->IsA(TFD::AM1FieldInteractableActorMiniGame::StaticClass()))
                            {
                                SDK::FVector2D ScreenPos = { -1, -1 };
                                SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
                                if (WorldToScreen(WorldPosition, &ScreenPos))
                                {
                                    std::string actorName = Actor->GetName();
                                    std::string VaultText = "Coded Vault";
                                    SDK::FLinearColor* vaultColor = nullptr;

                                    if (actorName.find("_Precise") != std::string::npos)
                                    {
                                        VaultText = "Precise Vault";
                                        vaultColor = &Cheat::ColorMana;
                                    }
                                    else if (actorName.find("_Ultra") != std::string::npos)
                                    {
                                        VaultText = "Ultra Vault";
                                        vaultColor = &Cheat::ColorRare;
                                    }
                                    else
                                    {
                                        static SDK::FLinearColor ColorGrey = { 0.6f, 0.6f, 0.6f, 1.0f };
                                        vaultColor = &ColorGrey;
                                    }

                                    RenderLootESP(Actor, ScreenPos, VaultText, vaultColor);
                                }
                                continue;
                            }
                        }

                        if (CFG::cfg_Loot_DrawAxionResources && !Menu::ShowMenu)
                        {
                            if (Actor->IsA(TFD::AM1FieldInteractableActor::StaticClass()))
                            {
                                std::string name = Actor->GetFullName();
                                if (name.contains("BP_INTER_Axion_HoverBikeBox01_C") || name.contains("BP_INTER_Axion_HoverBikeBox02_C"))
                                {
                                    SDK::FVector2D ScreenPos = { -1, -1 };
                                    SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
                                    if (WorldToScreen(WorldPosition, &ScreenPos))
                                    {
                                        if (name.contains("BP_INTER_Axion_HoverBikeBox01_C"))
                                        {
                                            RenderLootESP(Actor, ScreenPos, "Supply Cache", &Cheat::ColorUncommon);
                                        }
                                        else
                                        {
                                            RenderLootESP(Actor, ScreenPos, "Resource Cache", &Cheat::ColorCommon);
                                        }
                                    }
                                    continue;
                                }
                            }
                        }

                        if (CFG::cfg_Loot_DrawSupplyResources && !Menu::ShowMenu)
                        {
                            if (Actor->IsA(TFD::AM1FieldInteractableActor_Hit::StaticClass()))
                            {
                                if (Actor->GetFullName().contains("VulgusBox"))
                                {
                                    SDK::FVector2D ScreenPos = { -1, -1 };
                                    SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
                                    if (WorldToScreen(WorldPosition, &ScreenPos))
                                    {
                                        RenderLootESP(Actor, ScreenPos, "Resource Box", &Cheat::ColorUncommon);
                                    }
                                    continue;
                                }
                            }
                            else if (Actor->IsA(TFD::AM1FieldInteractableActor_Interaction::StaticClass()))
                            {
                                if (Actor->GetFullName().contains("MilitarySupplies"))
                                {
                                    SDK::FVector2D ScreenPos = { -1, -1 };
                                    SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
                                    if (WorldToScreen(WorldPosition, &ScreenPos))
                                    {
                                        RenderLootESP(Actor, ScreenPos, "Supplies", &Cheat::ColorCommon);
                                    }
                                    continue;
                                }
                            }
                        }
                    }
                }
            }
        }

        for (int i = 0; i < GWorld->Levels.Num(); ++i)
        {
            if (!GWorld->Levels.IsValidIndex(i)) continue;
            TFD::ULevel* Level = GWorld->Levels[i];
            if (!Level) continue;

            for (int j = 0; j < Level->Actors.Num(); ++j)
            {
                TFD::AActor* Actor = (TFD::AActor*)Level->Actors[j];
                if (!Actor) continue;

                if (!Actor->IsA(TFD::AM1DroppedItem::StaticClass()))
                    continue;

                TFD::AM1DroppedItem* Item = static_cast<TFD::AM1DroppedItem*>(Actor);
                if (Item->IsObtained() || Item->bBeingPickedLocally || Item->bTriedSetToObtained || Item->bObtainRequestedOnClient)
                    continue;

                SDK::FVector2D ScreenPos = { -1, -1 };
                SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
                bool OnScreen = WorldToScreen(WorldPosition, &ScreenPos);

                if ((Item->DropItemInfo.ItemBox.Type == TFD::EM1ItemType::Equipment && !CFG::cfg_Loot_FilterEquipment)
                    || Item->DropItemInfo.ItemBox.Type == TFD::EM1ItemType::Consumable
                    || Item->DropItemInfo.ItemBox.Type == TFD::EM1ItemType::Rune)
                {
                    bool isFiltered = true;
                    SDK::FLinearColor color = Cheat::ColorWhite;
                    std::string Text = "Unknown";

                    if (CFG::cfg_Loot_ItemFilterLevel <= 1)
                    {
                        if (Item->IsA(TFD::ABP_EquipTier01DroppedItem_C::StaticClass()))
                        {
                            color = Cheat::ColorUncommon;
                            Text = "T1 Item";
                            isFiltered = false;
                        }
                        else if (Item->IsA(TFD::ABP_ResourceTier01DroppedItem_C::StaticClass()))
                        {
                            color = Cheat::ColorUncommon;
                            Text = "T1 Resource";
                            isFiltered = false;
                        }
                        else if (Item->IsA(TFD::ABP_RuneTier01DroppedItem_C::StaticClass()))
                        {
                            color = Cheat::ColorUncommon;
                            Text = "T1 Rune";
                            isFiltered = false;
                        }
                    }
                    if (CFG::cfg_Loot_ItemFilterLevel <= 2)
                    {
                        if (Item->IsA(TFD::ABP_EquipTier02DroppedItem_C::StaticClass()))
                        {
                            color = Cheat::ColorRare;
                            Text = "T2 Item";
                            isFiltered = false;
                        }
                        else if (Item->IsA(TFD::ABP_ResourceTier02DroppedItem_C::StaticClass()))
                        {
                            color = Cheat::ColorRare;
                            Text = "T2 Resource";
                            isFiltered = false;
                        }
                        else if (Item->IsA(TFD::ABP_RuneTier02DroppedItem_C::StaticClass()))
                        {
                            color = Cheat::ColorRare;
                            Text = "T2 Rune";
                            isFiltered = false;
                        }
                    }
                    if (CFG::cfg_Loot_ItemFilterLevel <= 3)
                    {
                        if (Item->IsA(TFD::ABP_EquipTier03DroppedItem_C::StaticClass()))
                        {
                            color = Cheat::ColorGold;
                            Text = "T3 Item";
                            isFiltered = false;
                        }
                        else if (Item->IsA(TFD::ABP_ResourceTier03DroppedItem_C::StaticClass()))
                        {
                            color = Cheat::ColorGold;
                            Text = "T3 Resource";
                            isFiltered = false;
                        }
                        else if (Item->IsA(TFD::ABP_RuneTier03DroppedItem_C::StaticClass()))
                        {
                            color = Cheat::ColorGold;
                            Text = "T3 Rune";
                            isFiltered = false;
                        }
                        else if (Item->IsA(TFD::ABP_RuneTier04DroppedItem_C::StaticClass()))
                        {
                            color = Cheat::ColorUltimate;
                            Text = "T4 Rune";
                            isFiltered = false;
                        }
                    }

                    if (!isFiltered && OnScreen && CFG::cfg_Loot_DrawItemNames && !Menu::ShowMenu)
                    {
                        RenderLootESP(Actor, ScreenPos, Text, &color);
                    }
                }
            }
        }
    }
}

namespace ESP
{
    void RenderPlayerESP(TFD::AM1Player* player, SDK::FVector2D screenPos)
    {
        Cheat::RenderPlayerESP(player, screenPos);
    }

    void RenderEnemyESP(TFD::AM1Character* enemy, SDK::FVector2D screenPos)
    {
        Cheat::RenderEnemyESP(enemy, screenPos);
    }

    void FriendlyESP()
    {
        Cheat::FriendlyESP();
    }

    void EnemyESP()
    {
        Cheat::EnemyESP();
    }

    void LootESP()
    {
        Cheat::LootESP();
    }
}