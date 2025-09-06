#include "ItemDrops.h"
#include "ESP.h"

namespace Cheat
{
    bool TrySpawnGenericLoot = false;
    bool TrySpawnVaultLoot = false;
    bool RestartDecoding = false;

    bool GetSpareRounds(TFD::EM1RoundsType RoundsType, int RoundsPerLoot)
    {
        if (!LocalPlayerCharacter || !LocalPlayerCharacter->RoundsComponent)
            return false;

        int CurrentSpareRounds = 0;
        int MaxRoundCapacity = 0;

        CurrentSpareRounds = LocalPlayerCharacter->RoundsComponent->CurrentSpareRounds[(int)RoundsType];

        for (const auto& Capacities : LocalPlayerCharacter->RoundsComponent->CachedMaxCapacities)
        {
            if (Capacities.Key() == RoundsType)
            {
                MaxRoundCapacity = Capacities.Value().CachedCapacity;
                break;
            }
        }

        if (CurrentSpareRounds <= (MaxRoundCapacity - RoundsPerLoot))
            return true;
        else
            return false;
    }

    void MultiplyDrops(TFD::AActor* Actor, int Amount, bool isVault)
    {
        if (!Actor || !LocalPlayerController || Amount <= 0)
            return;

        TFD::FVector ActorPos = Actor->K2_GetActorLocation();
        TFD::FVector PlayerPos = LocalPlayerCharacter->K2_GetActorLocation();
        float Distance = ActorPos.GetDistanceTo(PlayerPos);

        if (Distance > CFG::cfg_Loot_ContainerDropRange)
            return;

        for (int i = 0; i < Amount; ++i)
        {
            if (isVault)
            {
                auto* Vault = static_cast<TFD::AM1MiniGameActor*>(Actor);
                Vault->ServerDropItems(LocalPlayerController);
            }
            else
            {
                auto* Generic = static_cast<TFD::AM1FieldInteractableActor*>(Actor);
                LocalPlayerController->ServerRequestFieldObjectDropItems(Generic);
            }
        }
    }

    void Loot()
    {
        if (GWorld->Levels.IsValid())
        {
            static TFD::UM1LocalGameInstanceSubsystem* GISystem = nullptr;
            for (int i = 0; i < GWorld->Levels.Num(); ++i)
            {
                if (GWorld->Levels.IsValidIndex(i))
                {
                    TFD::ULevel* Level = GWorld->Levels[i];
                    if (!Level) continue;

                    for (int j = 0; j < Level->Actors.Num(); ++j)
                    {
                        TFD::AActor* Actor = (TFD::AActor*)Level->Actors[j];
                        if (!Actor)
                            continue;

                        if (Actor->IsA(TFD::AM1MiniGameActor::StaticClass()))
                        {
                            GISystem = TFD::UM1LocalGameInstanceSubsystem::GetSystem(GWorld);
                            if (GISystem && GISystem->MiniGamePlayer && GISystem->MiniGamePlayer->CurrentMiniGame)
                            {
                                auto* Vault = static_cast<TFD::AM1MiniGameActor*>(Actor);
                                TFD::AM1Character* BaseCharacter = static_cast<TFD::AM1Character*>(LocalPlayerCharacter);
                                int PlayerTid = BaseCharacter->CharacterId.ID;
                                if (RestartDecoding)
                                {
                                    Vault->ClientStartMiniGame(Vault->MiniGameTid, { PlayerTid }, Vault->FieldDifficultyTid, static_cast<TFD::EM1MiniGameDifficulty>(1));
                                }
                                if (TrySpawnVaultLoot)
                                {
                                    int Amount = CFG::cfg_Loot_MultiplyDrops ? CFG::cfg_Loot_SpawnCount : 1;
                                    MultiplyDrops(Vault, Amount, true);
                                }
                            }
                            continue;
                        }
                        if (Actor->IsA(TFD::AM1FieldInteractableActor::StaticClass()))
                        {
                            std::string name = Actor->GetFullName();
                            if (name.contains("BP_INTER_Axion_HoverBikeBox01_C") || name.contains("BP_INTER_Axion_HoverBikeBox02_C"))
                            {
                                if (TrySpawnGenericLoot)
                                {
                                    int Amount = CFG::cfg_Loot_MultiplyDrops ? CFG::cfg_Loot_SpawnCount : 1;
                                    MultiplyDrops(Actor, Amount, false);
                                }
                                continue;
                            }
                        }
                        if (Actor->IsA(TFD::AM1FieldInteractableActor_Hit::StaticClass()))
                        {
                            if (Actor->GetFullName().contains("VulgusBox"))
                            {
                                if (TrySpawnGenericLoot)
                                {
                                    int Amount = CFG::cfg_Loot_MultiplyDrops ? CFG::cfg_Loot_SpawnCount : 1;
                                    MultiplyDrops(Actor, Amount, false);
                                }
                                continue;
                            }
                        }
                        else if (Actor->IsA(TFD::AM1FieldInteractableActor_Interaction::StaticClass()))
                        {
                            if (Actor->GetFullName().contains("MilitarySupplies"))
                            {
                                if (TrySpawnGenericLoot)
                                {
                                    int Amount = CFG::cfg_Loot_MultiplyDrops ? CFG::cfg_Loot_SpawnCount : 1;
                                    MultiplyDrops(Actor, Amount, false);
                                }
                                continue;
                            }
                        }

                        if (Actor->IsA(TFD::AM1FieldInteractableActor_Interaction::StaticClass()))
                        {
                            std::string aName = Actor->GetFullName();
                            if (aName.contains("CompanionVault_TypeB_Item_C") || aName.contains("CompanionVault_TypeA_Broken_C") ||
                                aName.contains("CompanionVault_TypeA_C") || aName.contains("CompanionVault_TypeB_Data_C"))
                            {
                                if (TrySpawnGenericLoot)
                                {
                                    int Amount = CFG::cfg_Loot_MultiplyDrops ? CFG::cfg_Loot_SpawnCount : 1;
                                    MultiplyDrops(Actor, Amount, false);
                                }
                                continue;
                            }
                        }

                        if (CFG::cfg_Loot_VacuumMissionItems)
                        {
                            if (Actor->IsA(TFD::AM1DropMissionSupplies::StaticClass()) || Actor->IsA(TFD::AM1DropMissionCollectibles::StaticClass()) || Actor->IsA(TFD::AM1DropContainerPickupWeapon::StaticClass())
                                || Actor->IsA(TFD::AM1DropContainerByMissionInstant_UnDisturbed::StaticClass()) || Actor->IsA(TFD::AM1DropContainerByMissionInstant::StaticClass()))
                            {
                                SDK::FVector WorldPosition = Actor->K2_GetActorLocation();


                                if (CFG::cfg_Loot_EnableLootVacuum && CFG::cfg_Loot_VacuumMissionItems)
                                {
                                    SDK::FVector player = LocalPlayerCharacter->K2_GetActorLocation();
                                    float Distance = WorldPosition.GetDistanceTo(player);
                                    if (Distance > 150 && Distance < CFG::cfg_Loot_LootVacuumRange)
                                        Actor->K2_SetActorLocation(player, false, nullptr, true);
                                }
                                continue;
                            }
                        }
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
                            if (CFG::cfg_Loot_EnableLootVacuum)
                            {
                                SDK::FVector player = LocalPlayerCharacter->K2_GetActorLocation();
                                float Distance = WorldPosition.GetDistanceTo(player);
                                if (Distance > 150 && Distance < CFG::cfg_Loot_LootVacuumRange)
                                    Item->K2_SetActorLocation(player, false, nullptr, true);
                            }
                        }
                        else if (Item->DropItemInfo.ItemBox.Type == TFD::EM1ItemType::InstantUse)
                        {
                            float hp = (float)Cheat::LocalPlayerCharacter->StatComponent->GetStatValue(TFD::EM1StatType::Stat_CurrentHp).Value / 10000.0f;
                            float maxhp = (float)Cheat::LocalPlayerCharacter->StatComponent->GetStatValue(TFD::EM1StatType::Stat_MaxHp).Value / 10000.0f;
                            float mana = (float)Cheat::LocalPlayerCharacter->StatComponent->GetStatValue(TFD::EM1StatType::Stat_CurrentMp).Value / 10000.0f;
                            float maxmana = (float)Cheat::LocalPlayerCharacter->StatComponent->GetStatValue(TFD::EM1StatType::Stat_MaxMp).Value / 10000.0f;
                            bool shouldVacuum = false;

                            if (Item->IsA(TFD::ABP_HealthOrbDroppedItem_C::StaticClass()))
                            {
                                if (hp < (maxhp * (CFG::cfg_Loot_HPThreshold / 100.0f)))
                                    shouldVacuum = true;
                            }
                            else if (Item->IsA(TFD::ABP_ManaOrbDroppedItem_C::StaticClass()))
                            {
                                if (mana < (maxmana * (CFG::cfg_Loot_MPThreshold / 100.0f)))
                                    shouldVacuum = true;
                            }
                            else if (Item->IsA(TFD::ABP_AmmoEnhancedDroppedItem_C::StaticClass()))
                            {
                                if (GetSpareRounds(TFD::EM1RoundsType::EnhancedRounds, 120))
                                    shouldVacuum = true;
                            }
                            else if (Item->IsA(TFD::ABP_AmmoGeneralDroppedItem_C::StaticClass()))
                            {
                                if (GetSpareRounds(TFD::EM1RoundsType::GeneralRounds, 200))
                                    shouldVacuum = true;
                            }
                            else if (Item->IsA(TFD::ABP_AmmoHighpowerDroppedItem_C::StaticClass()))
                            {
                                if (GetSpareRounds(TFD::EM1RoundsType::HighpowerRounds, 2))
                                    shouldVacuum = true;
                            }
                            else if (Item->IsA(TFD::ABP_AmmoImpactDroppedItem_C::StaticClass()))
                            {
                                if (GetSpareRounds(TFD::EM1RoundsType::ImpactRounds, 25))
                                    shouldVacuum = true;
                            }

                            if (shouldVacuum && CFG::cfg_Loot_EnableLootVacuum)
                            {
                                SDK::FVector player = LocalPlayerCharacter->K2_GetActorLocation();
                                float Distance = WorldPosition.GetDistanceTo(player);
                                if (Distance > 150 && Distance < CFG::cfg_Loot_LootVacuumRange)
                                    Item->K2_SetActorLocation(player, false, nullptr, true);
                            }
                        }
                        else if (Item->DropItemInfo.ItemBox.Type == TFD::EM1ItemType::Currency)
                        {
                            if (CFG::cfg_Loot_EnableLootVacuum)
                            {
                                SDK::FVector player = LocalPlayerCharacter->K2_GetActorLocation();
                                float Distance = WorldPosition.GetDistanceTo(player);
                                if (Distance > 150 && Distance < CFG::cfg_Loot_LootVacuumRange)
                                    Item->K2_SetActorLocation(player, false, nullptr, true);
                            }
                        }
                    }
                }
            }
        }
    }
    void GrabLoot()
    {
        if (!LocalPlayerCharacter || !LocalPlayerController)
            return;

        if (GWorld->Levels.IsValid())
        {
            for (int i = 0; i < GWorld->Levels.Num(); ++i)
            {
                if (GWorld->Levels.IsValidIndex(i))
                {
                    TFD::ULevel* Level = GWorld->Levels[i];
                    if (!Level) continue;

                    for (int j = 0; j < Level->Actors.Num(); ++j)
                    {
                        TFD::AActor* Actor = (TFD::AActor*)Level->Actors[j];
                        if (!Actor) continue;
                        if (Actor->IsA(TFD::AM1DroppedItem::StaticClass()))
                        {
                            SDK::FVector player = LocalPlayerCharacter->K2_GetActorLocation();
                            SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
                            float Distance = WorldPosition.GetDistanceTo(player);

                            if (Distance > 150 && Distance < CFG::cfg_Loot_LootVacuumRange)
                                Actor->K2_SetActorLocation(player, false, nullptr, true);
                        }
                        else if (Actor->IsA(TFD::AM1DropMissionSupplies::StaticClass()))
                        {
                            if (Actor->GetFullName().contains("SupplyObject") || Actor->GetFullName().contains("Explosive"))
                            {
                                SDK::FVector player = LocalPlayerCharacter->K2_GetActorLocation();
                                SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
                                float Distance = WorldPosition.GetDistanceTo(player);

                                if (Distance > 150 && Distance < CFG::cfg_Loot_LootVacuumRange)
                                    Actor->K2_SetActorLocation(player, false, nullptr, true);
                            }
                        }
                        else if (Actor->IsA(TFD::AM1DropMissionCollectibles::StaticClass()))
                        {
                            if (Actor->GetFullName().contains("CollectionObject"))
                            {
                                SDK::FVector player = LocalPlayerCharacter->K2_GetActorLocation();
                                SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
                                float Distance = WorldPosition.GetDistanceTo(player);

                                if (Distance > 150 && Distance < CFG::cfg_Loot_LootVacuumRange)
                                    Actor->K2_SetActorLocation(player, false, nullptr, true);
                            }
                        }
                        else if (Actor->IsA(TFD::AM1DropContainerPickupWeapon::StaticClass()) || Actor->IsA(TFD::AM1DropContainerByMissionInstant::StaticClass()) || Actor->IsA(TFD::AM1DropContainerByMissionInstant_UnDisturbed::StaticClass()))
                        {
                            SDK::FVector player = LocalPlayerCharacter->K2_GetActorLocation();
                            SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
                            float Distance = WorldPosition.GetDistanceTo(player);

                            if (Distance > 150 && Distance < CFG::cfg_Loot_LootVacuumRange)
                                Actor->K2_SetActorLocation(player, false, nullptr, true);
                        }
                    }
                }
            }
        }
    }

    void GrabContainers()
    {
        if (!LocalPlayerCharacter || !LocalPlayerController)
            return;

        if (GWorld->Levels.IsValid())
        {
            for (int i = 0; i < GWorld->Levels.Num(); ++i)
            {
                if (!GWorld->Levels.IsValidIndex(i))
                    continue;

                TFD::ULevel* Level = GWorld->Levels[i];
                if (!Level) continue;

                for (int j = 0; j < Level->Actors.Num(); ++j)
                {
                    TFD::AActor* Actor = Level->Actors[j];
                    if (!Actor) continue;

                    if (Actor->IsA(TFD::AM1FieldInteractableActor::StaticClass()) || Actor->IsA(TFD::AM1FieldInteractableActor_Hit::StaticClass()) || Actor->IsA(TFD::AM1FieldInteractableActor_Interaction::StaticClass()) || Actor->IsA(TFD::AM1FieldInteractableActor_UnsyncedInteraction::StaticClass()))
                    {
                        SDK::FVector playerPos = LocalPlayerCharacter->K2_GetActorLocation();
                        SDK::FVector worldPos = Actor->K2_GetActorLocation();
                        float distance = worldPos.GetDistanceTo(playerPos);

                        if (distance > 150 && distance < CFG::cfg_Loot_LootVacuumRange)
                        {
                            Actor->K2_SetActorLocation(playerPos, false, nullptr, true);
                        }
                    }
                }
            }
        }
    }

    static TFD::AActor* telekinesisTarget = nullptr;
    static TFD::AActor* grabbedObject = nullptr;
    static float objectRotationYaw = 0.0f;
    static float objectRotationPitch = 0.0f;
    static bool wasScrollingUp = false;
    static bool wasScrollingDown = false;
    TFD::AActor* FindTelekinesisTarget()
    {
        if (!LocalPlayerCharacter || !LocalPlayerController || !LocalPlayerController->PlayerCameraManager || !GWorld)
            return nullptr;

        SDK::FVector playerPos = LocalPlayerCharacter->K2_GetActorLocation();
        SDK::FVector playerForward = LocalPlayerController->PlayerCameraManager->GetActorForwardVector();

        TFD::AActor* closestTarget = nullptr;
        float closestDistance = FLT_MAX;
        float bestFOVScore = FLT_MAX;

        if (GWorld->Levels.IsValid())
        {
            for (int i = 0; i < GWorld->Levels.Num(); ++i)
            {
                if (!GWorld->Levels.IsValidIndex(i))
                    continue;

                TFD::ULevel* Level = GWorld->Levels[i];
                if (!Level) continue;

                for (int j = 0; j < Level->Actors.Num(); ++j)
                {
                    TFD::AActor* Actor = Level->Actors[j];
                    if (!Actor) continue;

                    if (Actor == LocalPlayerCharacter)
                        continue;

                    if (Actor->IsA(TFD::AM1FieldInteractableActor::StaticClass()) ||
                        Actor->IsA(TFD::AM1FieldInteractableActor_Hit::StaticClass()) ||
                        Actor->IsA(TFD::AM1FieldInteractableActor_Interaction::StaticClass()) ||
                        Actor->IsA(TFD::AM1FieldInteractableActor_UnsyncedInteraction::StaticClass()))
                    {
                        SDK::FVector actorPos = Actor->K2_GetActorLocation();
                        float distance = actorPos.GetDistanceTo(playerPos);

                        if (distance > 50.0f && distance < 5000.0f)
                        {
                            SDK::FVector directionToActor = (actorPos - playerPos).GetNormalized();

                            float dotProduct = playerForward.Dot(directionToActor);
                            float angle = acosf(dotProduct) * (180.0f / 3.14159f);

                            if (angle <= CFG::cfg_Telekinesis_FOV)
                            {
                                float fovScore = angle + (distance * 0.001f);
                                if (fovScore < bestFOVScore)
                                {
                                    bestFOVScore = fovScore;
                                    closestTarget = Actor;
                                    closestDistance = distance;
                                }
                            }
                        }
                    }
                }
            }
        }
        return closestTarget;
    }

    void UpdateTelekinesis(bool isGrabbing)
    {
        if (!CFG::cfg_Telekinesis_Enable || !LocalPlayerCharacter)
            return;

        if (isGrabbing && grabbedObject)
        {
            ImGuiIO& io = ImGui::GetIO();
            bool ctrlHeld = io.KeyCtrl;

            if (ctrlHeld)
            {
                io.WantCaptureKeyboard = true;

                if (ImGui::IsKeyDown(ImGuiKey_W))
                {
                    objectRotationPitch += 1.0f;
                }

                if (ImGui::IsKeyDown(ImGuiKey_S))
                {
                    objectRotationPitch -= 1.0f;
                }

                if (ImGui::IsKeyDown(ImGuiKey_A))
                {
                    objectRotationYaw += 1.0f;
                }

                if (ImGui::IsKeyDown(ImGuiKey_D))
                {
                    objectRotationYaw -= 1.0f;
                }

                if (ImGui::IsKeyDown(ImGuiKey_Q) && !wasScrollingUp)
                {
                    CFG::cfg_Telekinesis_Distance += 50.0f;
                    if (CFG::cfg_Telekinesis_Distance > 2000.0f) CFG::cfg_Telekinesis_Distance = 2000.0f;
                    wasScrollingUp = true;
                }
                else if (!ImGui::IsKeyDown(ImGuiKey_Q))
                {
                    wasScrollingUp = false;
                }

                if (ImGui::IsKeyDown(ImGuiKey_E) && !wasScrollingDown)
                {
                    CFG::cfg_Telekinesis_Distance -= 50.0f;
                    if (CFG::cfg_Telekinesis_Distance < 100.0f) CFG::cfg_Telekinesis_Distance = 100.0f;
                    wasScrollingDown = true;
                }
                else if (!ImGui::IsKeyDown(ImGuiKey_E))
                {
                    wasScrollingDown = false;
                }
            }
        }

        if (isGrabbing)
        {
            if (!grabbedObject)
            {
                telekinesisTarget = FindTelekinesisTarget();
                if (telekinesisTarget)
                {
                    grabbedObject = telekinesisTarget;
                    objectRotationYaw = 0.0f;   
                    objectRotationPitch = 0.0f;
                }
            }

            if (grabbedObject)
            {
                SDK::FVector playerPos = LocalPlayerCharacter->K2_GetActorLocation();
                SDK::FVector playerForward = LocalPlayerController->PlayerCameraManager->GetActorForwardVector();

                SDK::FVector objectLocation = playerPos + (playerForward * CFG::cfg_Telekinesis_Distance);

                if (objectRotationYaw != 0.0f || objectRotationPitch != 0.0f)
                {
                    SDK::FRotator currentRotation = grabbedObject->K2_GetActorRotation();
                    SDK::FRotator newRotation = SDK::FRotator(
                        currentRotation.Pitch + objectRotationPitch,
                        currentRotation.Yaw + objectRotationYaw,
                        currentRotation.Roll
                    );
                    grabbedObject->K2_SetActorLocationAndRotation(objectLocation, newRotation, false, nullptr, true);
                    objectRotationYaw = 0.0f;    
                    objectRotationPitch = 0.0f;
                }
                else
                {
                    grabbedObject->K2_SetActorLocation(objectLocation, false, nullptr, true);
                }
            }
        }
        else
        {
            grabbedObject = nullptr;
            objectRotationYaw = 0.0f;
            objectRotationPitch = 0.0f;
        }

        if (!grabbedObject)
        {
            telekinesisTarget = FindTelekinesisTarget();
        }
    }

    void RenderTelekinesisMarkers()
    {
        if (!CFG::cfg_Telekinesis_Enable || !LocalPlayerCharacter)
            return;

        if (telekinesisTarget && !grabbedObject)
        {
            SDK::FVector targetPos = telekinesisTarget->K2_GetActorLocation();
            SDK::FLinearColor markerColor = SDK::FLinearColor(
                CFG::cfg_Telekinesis_MarkerColorR,
                CFG::cfg_Telekinesis_MarkerColorG,
                CFG::cfg_Telekinesis_MarkerColorB,
                CFG::cfg_Telekinesis_MarkerColorA
            );

            static float spinTime = 0.0f;
            static auto lastTime = std::chrono::high_resolution_clock::now();
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            spinTime += deltaTime * 180.0f;     

            SDK::FRotator spinRotation = SDK::FRotator(0, spinTime, 0);
            Render::R_Draw3DSphereRotated(targetPos, CFG::cfg_Telekinesis_MarkerSize, spinRotation, &markerColor);
        }

        if (grabbedObject)
        {
            SDK::FVector grabbedPos = grabbedObject->K2_GetActorLocation();
            SDK::FLinearColor grabbedColor = SDK::FLinearColor(
                CFG::cfg_Telekinesis_GrabbedColorR,
                CFG::cfg_Telekinesis_GrabbedColorG,
                CFG::cfg_Telekinesis_GrabbedColorB,
                CFG::cfg_Telekinesis_GrabbedColorA
            );

            static float grabbedSpinTime = 0.0f;
            static auto lastGrabbedTime = std::chrono::high_resolution_clock::now();
            auto currentGrabbedTime = std::chrono::high_resolution_clock::now();
            float grabbedDeltaTime = std::chrono::duration<float>(currentGrabbedTime - lastGrabbedTime).count();
            lastGrabbedTime = currentGrabbedTime;
            grabbedSpinTime += grabbedDeltaTime * 360.0f;      

            SDK::FRotator grabbedSpinRotation = SDK::FRotator(0, grabbedSpinTime, 0);
            Render::R_Draw3DSphereRotated(grabbedPos, CFG::cfg_Telekinesis_MarkerSize * 1.2f, grabbedSpinRotation, &grabbedColor);
        }
    }

    std::string GetActorProperties(TFD::AActor* Actor)
    {
        std::string properties = "";

        return properties;
    }
    std::string ExtractClassName(const std::string& fullClassName)
    {
        size_t lastSlash = fullClassName.find_last_of('/');
        size_t lastDot = fullClassName.find_last_of('.');

        if (lastSlash != std::string::npos && lastDot != std::string::npos && lastDot > lastSlash)
        {
            return fullClassName.substr(lastSlash + 1, lastDot - lastSlash - 1);
        }
        else if (lastSlash != std::string::npos)
        {
            return fullClassName.substr(lastSlash + 1);
        }
        else if (lastDot != std::string::npos)
        {
            return fullClassName.substr(0, lastDot);
        }

        return fullClassName;
    }

    std::string ExtractObjectName(const std::string& fullObjectName)
    {
        size_t lastDot = fullObjectName.find_last_of('.');
        if (lastDot != std::string::npos)
        {
            return fullObjectName.substr(lastDot + 1);
        }

        size_t firstSpace = fullObjectName.find(' ');
        if (firstSpace != std::string::npos)
        {
            return fullObjectName.substr(0, firstSpace);
        }

        return fullObjectName;
    }

    void DebugActorsInRange()
    {
        if (!LocalPlayerCharacter || !GWorld || !GWorld->Levels.IsValid())
            return;

        TFD::FVector PlayerPosition = LocalPlayerCharacter->K2_GetActorLocation();
        float DebugRange = CFG::cfg_Loot_ContainerDropRange;

        int displayIndex = 0;
        const int maxDisplayItems = 15;        
        float yPos = 90.0f;       

        for (int i = 0; i < GWorld->Levels.Num(); ++i)
        {
            if (!GWorld->Levels.IsValidIndex(i))
                continue;

            TFD::ULevel* Level = GWorld->Levels[i];
            if (!Level)
                continue;

            for (int j = 0; j < Level->Actors.Num(); ++j)
            {
                TFD::AActor* Actor = (TFD::AActor*)Level->Actors[j];
                if (!Actor)
                    continue;

                TFD::FVector ActorPosition = Actor->K2_GetActorLocation();
                float Distance = ActorPosition.GetDistanceTo(PlayerPosition);

                if (Distance > DebugRange)
                    continue;

                if (Actor == LocalPlayerCharacter)
                    continue;

                bool hasWidgetComponent = false;
                std::string actorType = "Unknown";

                std::string fullClassName = "Unknown";
                if (Actor->Class)
                {
                    fullClassName = Actor->Class->GetFullName();
                    actorType = ExtractClassName(fullClassName);

                    if (fullClassName.find("Light") != std::string::npos ||
                        fullClassName.find("light") != std::string::npos ||
                        fullClassName.find("Lighting") != std::string::npos ||
                        fullClassName.find("lighting") != std::string::npos ||
                        fullClassName.find("Lamp") != std::string::npos ||
                        fullClassName.find("lamp") != std::string::npos)
                    {
                        continue;
                    }
                }

                bool isInteresting = false;
                TFD::UM1ActorWidgetComponent* WidgetComponent = nullptr;

                isInteresting = true;

                if (actorType.find("MissionTask") != std::string::npos)
                {
                    "[MISSION:" + actorType + "]";

                    if (Actor->IsA(TFD::AM1MissionTaskActor::StaticClass()))
                    {
                        TFD::AM1MissionTaskActor* MissionTaskActor = static_cast<TFD::AM1MissionTaskActor*>(Actor);

                        if (MissionTaskActor->ActorWidgetsOfWayPointsAndTask.Num() > 0)
                        {
                            "[WIDGET_ARRAY:" + std::to_string(MissionTaskActor->ActorWidgetsOfWayPointsAndTask.Num()) + "]";

                            if (MissionTaskActor->ActorWidgetsOfWayPointsAndTask.IsValidIndex(0))
                            {
                                WidgetComponent = MissionTaskActor->ActorWidgetsOfWayPointsAndTask[0];
                            }
                            if (WidgetComponent && WidgetComponent->GetActorWidget())
                            {
                                "[ACTIVE_WIDGET]";
                            }
                        }
                    }
                    else if (Actor->IsA(TFD::AM1MissionTaskMoveWayPoint::StaticClass()))
                    {
                        TFD::AM1MissionTaskMoveWayPoint* WaypointActor = static_cast<TFD::AM1MissionTaskMoveWayPoint*>(Actor);
                        WidgetComponent = WaypointActor->WidgetComponent;
                        if (WidgetComponent)
                        {
                            "[HAS_WIDGET]";
                            if (WidgetComponent->GetActorWidget())
                            {
                                "[ACTIVE_WIDGET]";
                            }
                        }
                    }
                }
                else
                {
                    "[CLASS:" + fullClassName + "]";
                }

                if (actorType.find("Widget") != std::string::npos ||
                    actorType.find("UI") != std::string::npos ||
                    actorType.find("HUD") != std::string::npos ||
                    actorType.find("Marker") != std::string::npos ||
                    actorType.find("Waypoint") != std::string::npos ||
                    actorType.find("Objective") != std::string::npos ||
                    actorType.find("Icon") != std::string::npos ||
                    actorType.find("Sprite") != std::string::npos ||
                    actorType.find("Canvas") != std::string::npos ||
                    actorType.find("Overlay") != std::string::npos ||
                    actorType.find("Display") != std::string::npos ||
                    actorType.find("Render") != std::string::npos ||
                    actorType.find("Visual") != std::string::npos ||
                    actorType.find("Component") != std::string::npos ||
                    actorType.find("Billboard") != std::string::npos ||
                    actorType.find("Decal") != std::string::npos ||
                    actorType.find("Mesh") != std::string::npos ||
                    actorType.find("Static") != std::string::npos ||
                    actorType.find("Skeletal") != std::string::npos ||
                    actorType.find("Particle") != std::string::npos ||
                    actorType.find("Effect") != std::string::npos ||
                    actorType.find("Light") != std::string::npos ||
                    actorType.find("Camera") != std::string::npos ||
                    actorType.find("Volume") != std::string::npos ||
                    actorType.find("Trigger") != std::string::npos ||
                    actorType.find("Blueprint") != std::string::npos ||
                    actorType.find("Generated") != std::string::npos ||
                    fullClassName.find("BP_") != std::string::npos ||
                    fullClassName.find("WBP_") != std::string::npos ||
                    fullClassName.find("UMG_") != std::string::npos)
                {
                    "[INTERESTING]";
                }

                if (fullClassName.find("Waypoint") != std::string::npos ||
                    fullClassName.find("Objective") != std::string::npos ||
                    fullClassName.find("Quest") != std::string::npos ||
                    fullClassName.find("Mission") != std::string::npos ||
                    fullClassName.find("Target") != std::string::npos ||
                    fullClassName.find("Marker") != std::string::npos ||
                    fullClassName.find("Indicator") != std::string::npos ||
                    fullClassName.find("Pointer") != std::string::npos ||
                    fullClassName.find("Arrow") != std::string::npos ||
                    fullClassName.find("Diamond") != std::string::npos ||
                    fullClassName.find("Navigation") != std::string::npos)
                {
                    "[WAYPOINT_CANDIDATE]";
                }

                hasWidgetComponent = isInteresting;

                if (hasWidgetComponent)
                {
                    SDK::FVector2D ScreenPos = { -1, -1 };
                    if (WorldToScreen(ActorPosition, &ScreenPos))
                    {
                        bool hasActiveWidget = false;
                        if (WidgetComponent)
                        {
                            hasActiveWidget = WidgetComponent->GetActorWidget() != nullptr;
                        }
                    }
                    else if (Actor->IsA(TFD::AM1MissionTaskMoveWayPoint::StaticClass()))
                    {
                        TFD::AM1MissionTaskMoveWayPoint* WaypointActor = static_cast<TFD::AM1MissionTaskMoveWayPoint*>(Actor);
                        WidgetComponent = WaypointActor->WidgetComponent;
                        if (WidgetComponent)
                        {
                            "[HAS_WIDGET]";
                            if (WidgetComponent->GetActorWidget())
                            {
                                "[ACTIVE_WIDGET]";
                            }
                        }
                    }
                }
                else
                {
                    "[CLASS:" + fullClassName + "]";
                }

                hasWidgetComponent = isInteresting;

                if (hasWidgetComponent)
                {
                    SDK::FVector2D ScreenPos = { -1, -1 };
                    if (WorldToScreen(ActorPosition, &ScreenPos))
                    {
                        bool hasActiveWidget = false;
                        if (WidgetComponent)
                        {
                            hasActiveWidget = WidgetComponent->GetActorWidget() != nullptr;
                        }

                        if (hasActiveWidget)
                        {
                            if (CFG::cfg_ESP_GradientCycling) {
                                float gradientTime = Render::GetSharedGradientTime();
                                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.1f, gradientTime);
                                Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 15.0f, &flowingColor);
                                Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 10.0f, &flowingColor);
                            }
                            else {
                                Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 15.0f, &ColorGold);
                                Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 10.0f, &ColorMana);
                            }
                            Render::R_DrawText(ScreenPos.X, ScreenPos.Y - 40, "ACTIVE WIDGET", &ColorGold, true);
                            Render::R_DrawText(ScreenPos.X, ScreenPos.Y + 40, "Has Active Widget", &ColorGreen, true);
                        }
                        else if (WidgetComponent)
                        {
                            if (CFG::cfg_ESP_GradientCycling) {
                                float gradientTime = Render::GetSharedGradientTime();
                                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.2f, gradientTime);
                                Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 10.0f, &flowingColor);
                            }
                            else {
                                Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 10.0f, &ColorUncommon);
                            }
                            Render::R_DrawText(ScreenPos.X, ScreenPos.Y - 40, "INACTIVE WIDGET", &ColorUncommon, true);
                            Render::R_DrawText(ScreenPos.X, ScreenPos.Y + 40, "No Active Widget", &ColorRed, true);
                        }
                        else
                        {
                            if (CFG::cfg_ESP_GradientCycling) {
                                float gradientTime = Render::GetSharedGradientTime();
                                SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.3f, gradientTime);
                                Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 8.0f, &flowingColor);
                            }
                            else {
                                Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 8.0f, &ColorMana);
                            }
                            Render::R_DrawText(ScreenPos.X, ScreenPos.Y - 55, "GENERAL ACTOR", &ColorMana, true);
                            Render::R_DrawText(ScreenPos.X, ScreenPos.Y + 55, "No Widget Component", &ColorRed, true);
                        }

                        std::string fullClassName = Actor->Class->GetFullName();
                        Render::R_DrawText(ScreenPos.X, ScreenPos.Y - 40, fullClassName.c_str(), &ColorWhite, true);

                        std::string actorName = "No Name";
                        if (Actor)
                        {
                            actorName = Actor->GetName();
                        }
                        Render::R_DrawText(ScreenPos.X, ScreenPos.Y - 25, actorName.c_str(), &ColorMana, true);

                        Render::R_DrawText(ScreenPos.X, ScreenPos.Y - 10, actorType, &ColorGold, true);

                        std::string distanceText = std::to_string(static_cast<int>(Distance)) + "m";
                        Render::R_DrawText(ScreenPos.X, ScreenPos.Y + 25, distanceText, &ColorMana, true);
                    }
                }

                if (!hasWidgetComponent && (Actor->IsA(TFD::AM1DroppedItem::StaticClass()) ||
                    Actor->IsA(TFD::AM1MiniGameActor::StaticClass()) ||
                    Actor->IsA(TFD::AM1FieldInteractableActorMiniGame::StaticClass()) ||
                    Actor->IsA(TFD::AM1FieldInteractableActor::StaticClass()) ||
                    Actor->IsA(TFD::AM1FieldInteractableActor_Hit::StaticClass()) ||
                    Actor->IsA(TFD::AM1FieldInteractableActor_Interaction::StaticClass())))
                    continue;

                if (hasWidgetComponent)
                    continue;

                std::string fullName = Actor->GetFullName();
                std::string actorFullClassName = Actor->Class ? Actor->Class->GetFullName() : "Unknown Class";

                std::string cleanClassName = ExtractClassName(actorFullClassName);
                std::string cleanObjectName = ExtractObjectName(fullName);
                std::string actorProperties = GetActorProperties(Actor);

                if (fullName.contains("PlayerStart") ||
                    fullName.contains("LightmassImportanceVolume") ||
                    fullName.contains("PostProcessVolume") ||
                    fullName.contains("SkyLight") ||
                    fullName.contains("DirectionalLight") ||
                    fullName.contains("StaticMeshActor") ||
                    fullName.contains("Landscape") ||
                    fullName.contains("CameraActor") ||
                    fullName.contains("TriggerVolume") ||
                    fullName.contains("BlockingVolume"))
                    continue;

                TFD::FVector ActorLocation = Actor->K2_GetActorLocation();
                TFD::FRotator ActorRotation = Actor->K2_GetActorRotation();

                SDK::FVector2D ScreenPos = { -1, -1 };
                if (WorldToScreen(ActorPosition, &ScreenPos))
                {
                    std::string coordText = cleanClassName + " [" + std::to_string(static_cast<int>(Distance)) + "m]";
                    Render::R_DrawText(ScreenPos.X, ScreenPos.Y + 20, coordText, &ColorUncommon, true);

                    if (CFG::cfg_ESP_GradientCycling) {
                        float gradientTime = Render::GetSharedGradientTime();
                        SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.4f, gradientTime);
                        Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 5.0f, &flowingColor);
                    }
                    else {
                        Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 5.0f, &ColorRed);
                    }

                    std::string listText = "[" + std::to_string(displayIndex + 1) + "] " + cleanClassName;
                    Render::R_DrawText(10.0f, yPos, listText, &ColorWhite, false);

                    std::string listName = "  Name: " + cleanObjectName;
                    Render::R_DrawText(10.0f, yPos + 15.0f, listName, &ColorGreen, false);

                    std::string listDist = "  Dist: " + std::to_string(static_cast<int>(Distance)) + "m" + actorProperties;
                    Render::R_DrawText(10.0f, yPos + 30.0f, listDist, &ColorMana, false);

                    std::string listCoords = "  Pos: (" + std::to_string(static_cast<int>(ActorLocation.X)) + ", " +
                        std::to_string(static_cast<int>(ActorLocation.Y)) + ", " +
                        std::to_string(static_cast<int>(ActorLocation.Z)) + ")";
                    Render::R_DrawText(10.0f, yPos + 45.0f, listCoords, &ColorUncommon, false);

                    yPos += 65.0f;     
                    displayIndex++;
                }
            }
        }

        std::string headerText = "Enhanced Debug: Actors in Range (" + std::to_string(static_cast<int>(DebugRange)) + "m)";
        Render::R_DrawText(10.0f, 50.0f, headerText, &ColorGold, false);

        std::string countText = "Found: " + std::to_string(displayIndex) + " interesting actors";
        Render::R_DrawText(10.0f, 70.0f, countText, &ColorUncommon, false);
    }
}
