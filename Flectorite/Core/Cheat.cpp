#include "Cheat.h"
#include "../Cheats/SavePointConfig.h"
#include "../Cheats/Mission.h"
#include "../Cheats/ESP.h"
#include "../magic_enum/magic_enum.hpp"
#include <random>

//#define IS_DEBUG_VERSION

namespace Cheat
{
	__int64 (*oPostRender)(void* ViewportClient, void* DebugCanvas) = nullptr;

	//float BoundsScale = 1.0f; if someone else wants to fix this go for it but fuck messing with that after all this

	CSimpleIniA ini;
	bool ModifyViewMode = true;

	__int64 PostRender(void* self, void* canvas)
	{
		//std::cout << "PostRender called" << std::endl;
		//return oPostRender(self, canvas);

		static bool Init = false;
		if (!Init)
		{
			//std::cout << "PostRender called once." << std::endl;
			Canvas = static_cast<TFD::UCanvas*>(canvas);
			LoadCacheData();
			Init = true;
		}

		if (!IsValidUWorld())
		{
			Render::R_End();
			return oPostRender(self, canvas);
		}

		if (!IsCheatReady())
		{
			Render::R_End();
			return oPostRender(self, canvas);
		}

		static bool TryEquipCustomization = true;
		if ((int)(static_cast<TFD::UM1GameInstance*>(GWorld->OwningGameInstance)->ConnectionState) != 10)
		{
			Render::R_End();
			WeaponControl = nullptr;
			if (CFG::cfg_Customize_EnableAutoApplyCustomization && TryEquipCustomization == false)
				TryEquipCustomization = true;
			return oPostRender(self, canvas);
		}

		if (ModifyViewMode)
		{
			TFD::UEngine* Engine = TFD::UEngine::GetEngine();
			if (Engine && Engine->GameViewport)
			{
				if (CFG::cfg_Extra_EnableFullBright)
					Engine->GameViewport->ViewModeIndex = 0x1;
				else
					Engine->GameViewport->ViewModeIndex = 0x3;
				ModifyViewMode = false;
			}
		}

		static bool InitCustomization = true;
		if (InitCustomization)
		{
			InitCustomizationData();
			InitCustomization = false;
		}

		if (CFG::cfg_Customize_EnableAutoApplyCustomization && TryEquipCustomization)
		{

			if (TryEquipSavedCustomization())
				TryEquipCustomization = false;
		}

		static bool RunOnce = true;
		if (RunOnce)
		{
			GameSettings = TFD::UM1GameSetting::GetDefaultObj();
			RunOnce = false;
		}

		if (TryAddAllItems)
		{
			TryAddAllItems = false;
			AddAllCustomizationItems();
		}

		//ScreenMiddle.X = (Canvas->SizeX / 2.0f) / (Canvas->SizeX / Canvas->ClipX);
		//ScreenMiddle.Y = (Canvas->SizeY / 2.0f) / (Canvas->SizeY / Canvas->ClipY);


		static bool PresetFirstCheck = true;
		if (PresetFirstCheck && PresetsMap.empty())
		{
			PresetFirstCheck = false;
		}
		UpdateControllerState();

		Menu::HandleKeybinds();

		if (UpdateCache)
		{
			SaveCacheData();
			UpdateCache = false;
		}

		if (CFG::cfg_Friendly_Enable)
			ESP::FriendlyESP();

		if (CFG::cfg_Enemy_Enable)
			ESP::EnemyESP();

		if (CFG::cfg_Actors_EnableCollectibleESP)
			CollectibleESP();

		if (CFG::cfg_Loot_EnableItemESP)
			ESP::LootESP();

		if (CFG::cfg_Loot_EnableLootVacuum || TrySpawnGenericLoot || TrySpawnVaultLoot || RestartDecoding)
			Loot();

		if (CFG::cfg_Mission_EnableMissionESP)
			MissionESPTeleport();

		if (CFG::cfg_Mission_EnableMissionAutoRestart)
			AutoRestartMission();

		if (CFG::cfg_Mission_EnableMissionTaskTeleport)
			MissionTaskTeleporter();

		if (CFG::cfg_Aim_EnableAimbot)
			Aimbot();

		if (CFG::cfg_Aim_SilentAim)
			SilentAim();

		if (CFG::cfg_Abilities_EnableModifyGrapple)
			ModifyGrapple();

		if (CFG::cfg_Aim_NoRecoilAndSpread || CFG::cfg_Aim_NoReload || CFG::cfg_Aim_RapidFire)
			WeaponModifications();

		if (PresetsMap.empty())
			PresetRefresh();

		if (CFG::cfg_Hotswap_EnableOverlay)
			PresetOverlay();

		if (CFG::cfg_SavePoint_EnableSavePointOverlay)
		{
			// Update world-based save points (handles world changes automatically)
			SavePointCFG::UpdateWorldSavePoints();

			// Load saved points from config on first run
			static bool savedPointsLoaded = false;
			if (!savedPointsLoaded) {
				SavePointCFG::LoadSavePointsFromConfig();
				savedPointsLoaded = true;
			}
			SavePointCFG::SavePointOverlay();
		}

		if (CFG::cfg_SavePoint_RenderSavePoints)
			SavedPointsESP();

		if (CFG::cfg_Mission_EnableInstantInfiltration)
			InstantInfiltration();

		if (CFG::cfg_Abilities_EnableAutomaticResupply)
			AutoResupply();

		if (CFG::cfg_Teleport_ShowTeleportSphere1 || CFG::cfg_Teleport_ShowTeleportSphere2 || CFG::cfg_Teleport_TeleportTargetMarking)
			DrawTeleportPreviewSpheres();

		//if (CFG::cfg_Extra_EnableVehicleMods)
		//	ModifyVehicleStats();

		if (CFG::cfg_Menu_ShowEnabledFeatures)
			EnabledFeaturesOverlay();

		if (CFG::cfg_Loot_DebugActorsInRange)
			DebugActorsInRange();

		Render::R_End();
		return oPostRender(self, canvas);
	}

	bool IsValidUWorld()
	{
		if (GWorld == nullptr)
		{
#ifdef IS_DEBUG_VERSION
			std::cout << "[Cheat] World is null.\n";
#endif
			GWorld = *reinterpret_cast<TFD::UWorld**>(TFD::BASE + SDK::Offsets::GWorld);
			if (GWorld && GWorld->IsA(TFD::UWorld::StaticClass()))
			{
#ifdef IS_DEBUG_VERSION
				std::cout << "[Cheat] Found World\n";
#endif
				return true;
			}
			else
			{
#ifdef IS_DEBUG_VERSION
				std::cout << "[Cheat] World is null and GWorld not pointing to valid UWorld.\n";
#endif
				GWorld = nullptr;
				return false;
			}
		}
		else
		{
			TFD::UWorld* CheckWorld = *reinterpret_cast<TFD::UWorld**>(TFD::BASE + SDK::Offsets::GWorld);
			if (!CheckWorld)
			{
#ifdef IS_DEBUG_VERSION
				std::cout << "[Cheat] World was not null but GWorld not currently pointing to valid UWorld.\n";
#endif
				GWorld = nullptr;
				return false;
			}
			if (CheckWorld->IsA(TFD::UWorld::StaticClass()))
			{
				if (GWorld != CheckWorld)
				{
#ifdef IS_DEBUG_VERSION
					std::cout << "[Cheat] World is not null and GWorld is pointing to different UWorld.\n";
#endif
					GWorld = CheckWorld;
				}
				return true;
			}
		}
		return false;
	}

	bool IsCheatReady()
	{
		std::string Name = GWorld->Name.ToString();
		if (Name != "" && Name != "None")
		{
			if (Name != "Lobby_P" && Name != "Level_Transition")
			{
				if (GWorld->OwningGameInstance && GWorld->OwningGameInstance->IsA(TFD::UM1GameInstance::StaticClass()))
				{
					if (GWorld->OwningGameInstance->LocalPlayers && GWorld->OwningGameInstance->LocalPlayers[0] && GWorld->OwningGameInstance->LocalPlayers[0]->PlayerController)
					{
						if (GWorld->OwningGameInstance->LocalPlayers[0]->PlayerController->IsA(TFD::AM1PlayerControllerInGame::StaticClass()))
						{
							TFD::AM1PlayerControllerInGame* PC = static_cast<TFD::AM1PlayerControllerInGame*>(GWorld->OwningGameInstance->LocalPlayers[0]->PlayerController);
							if (PC->Character && PC->Character->IsA(TFD::AM1Player::StaticClass()) && PC->ActorManager_Subsystem && PC->ActorManager_Subsystem->IsA(TFD::UM1ActorManagerSubsystem::StaticClass()))
							{
								LocalPlayerController = PC;
								LocalPlayerCharacter = static_cast<TFD::AM1Player*>(PC->Character);
								if (PC->PlayerState && PC->PlayerState->IsA(TFD::AM1PlayerState::StaticClass()))
								{
									PlayerState = static_cast<TFD::AM1PlayerState*>(PC->PlayerState);
									Missions = PlayerState->MissionControlComponent;
								}
								else
								{
									PlayerState = nullptr;
									Missions = nullptr;
								}
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}

	void SaveCacheData()
	{
		std::ofstream outFile("cache.dat", std::ios::binary);

		if (!outFile) {
			return;
		}
		outFile.clear();
		// Write the size of the map first
		size_t mapSize = IDBoneMap.size();
		outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));

		// Write each key-vector pair
		for (const auto& pair : IDBoneMap) {
			// Write the key
			outFile.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));

			// Write the size of the vector
			size_t vectorSize = pair.second.size();
			outFile.write(reinterpret_cast<const char*>(&vectorSize), sizeof(vectorSize));

			// Write the vector elements
			outFile.write(reinterpret_cast<const char*>(pair.second.data()), vectorSize * sizeof(int));
		}

		size_t size = IDNameMap.size();
		outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));

		// Write each key-value pair
		for (const auto& pair : IDNameMap) {
			// Write the key
			outFile.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));

			// Write the length of the string value
			size_t strLength = pair.second.size();
			outFile.write(reinterpret_cast<const char*>(&strLength), sizeof(strLength));

			// Write the string value
			outFile.write(pair.second.data(), strLength);
		}

		outFile.close();

	}
	void LoadCacheData()
	{
		std::unordered_map<int, std::vector<int>> bonemap;
		std::unordered_map<int, std::string> map;
		std::ifstream inFile("cache.dat", std::ios::binary);

		if (!inFile) {
			return;
		}

		size_t mapSize = 0;
		inFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

		for (size_t i = 0; i < mapSize; ++i) {
			int key;
			size_t vectorSize;
			std::vector<int> value;

			// Read the key
			inFile.read(reinterpret_cast<char*>(&key), sizeof(key));

			// Read the size of the vector
			inFile.read(reinterpret_cast<char*>(&vectorSize), sizeof(vectorSize));

			// Resize the vector to hold the elements
			value.resize(vectorSize);

			// Read the vector elements
			inFile.read(reinterpret_cast<char*>(value.data()), vectorSize * sizeof(int));

			// Insert into the map
			bonemap[key] = value;
		}

		// Read the size of the map
		size_t size = 0;
		inFile.read(reinterpret_cast<char*>(&size), sizeof(size));

		for (size_t i = 0; i < size; ++i) {
			int key;
			size_t strLength;
			std::string value;

			// Read the key
			inFile.read(reinterpret_cast<char*>(&key), sizeof(key));

			// Read the length of the string value
			inFile.read(reinterpret_cast<char*>(&strLength), sizeof(strLength));

			// Read the string value
			value.resize(strLength);
			inFile.read(&value[0], strLength);

			// Insert into the map
			map[key] = value;
		}
		inFile.close();

		IDBoneMap = bonemap;
		IDNameMap = map;
	}


	void CreateConsole()
	{
		TFD::UEngine* Engine = TFD::UEngine::GetEngine();
		if (!Engine->GameViewport->ViewportConsole)
		{
			TFD::UInputSettings::GetDefaultObj()->ConsoleKeys[0].KeyName = TFD::UKismetStringLibrary::Conv_StringToName(L"F1");
			Engine->GameViewport->ViewportConsole = static_cast<TFD::UConsole*>(TFD::UGameplayStatics::SpawnObject(Engine->ConsoleClass, Engine->GameViewport));
		}
	}

	void CreateCheatManager()
	{
		LocalPlayerController->CheatManager = static_cast<TFD::UCheatManager*>(TFD::UGameplayStatics::SpawnObject(LocalPlayerController->CheatClass, LocalPlayerController));
	}

	void ToggleCheatUI()
	{
		LocalPlayerController->EnableCheats();
		//static_cast<TFD::UM1Cheat*>(LocalPlayerController->CheatManager)->ToggleUI();
	}

	void __fastcall hkSpeedHackDetecting(void* This, float InDeltaTime)
	{
#ifdef IS_DEBUG_VERSION
		std::cout << "UM1SpeedHackDetectorSubSystem::SpeedHackDetecting was triggered.\n";
#endif
	}

	void __fastcall hkBP_FireInternal_Implementation(TFD::UM1WeaponInstantHitComponent* This, float FireTime, SDK::FVector* FireLoc, float WeaponRange, TFD::FM1ScaledInteger PenetrationStat)
	{
		//std::cout << std::endl;
		//std::cout << std::endl;
		//std::cout << std::endl;
		//std::cout << "FireLoc Before: (" << FireLoc->X << ", " << FireLoc->Y << ", " << FireLoc->Z << ")\n";
		//if (Aimbot_Target)
		//{
		//	SDK::FVector FirePos = This->Weapon_Owner->WeaponMesh->GetSocketTransform(This->Weapon_Owner->VFXComponent->FireBone, false).Translation;
		//	std::cout << "FirePos Pos: (" << FirePos.X << ", " << FirePos.Y << ", " << FirePos.Z << ")\n";
			//FireLoc->X = FirePos.X;
			//FireLoc->Y = FirePos.Y;
			//FireLoc->Z = FirePos.Z;
		//}

		TFD::native_BP_FireInternal_Implementation(This, FireTime, FireLoc, WeaponRange, PenetrationStat);
	}
	void __fastcall hkHandleInstantHitResult(TFD::UM1WeaponInstantHitComponent* This, TFD::FM1WeaponInstantHitParams* Params, TFD::FM1WeaponInstantHitTraceResult_Multi_Penetration* Result)
	{
		//if (Aimbot_Target)
		//{
		//	TFD::AM1Character* Targ = static_cast<TFD::AM1Character*>(Aimbot_Target);
		//	if (Targ->Mesh && Targ->Mesh->GetNumBones() > 0 && Targ->Mesh->BoneArray.IsValidIndex(Aimbot_BoneIndex))
		//	{
		//		SDK::FMatrix ComponentMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(Targ->Mesh->K2_GetComponentToWorld());
		//		SDK::FTransform bone = Targ->Mesh->BoneArray[Aimbot_BoneIndex];
		//		SDK::FMatrix BoneMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(bone);
		//		SDK::FMatrix WorldMatrix = TFD::UKismetMathLibrary::Multiply_MatrixMatrix(BoneMatrix, ComponentMatrix);
		//		SDK::FTransform WorldPosition = TFD::UKismetMathLibrary::Conv_MatrixToTransform(WorldMatrix);
		//		SDK::FVector Target = WorldPosition.Translation;
		//		for (int i = 0; i < Result->Traces.Num(); i++)
		//		{
		//			Result->Traces[i].FireEnd = Target;
		//			Result->Traces[i].bBlocked = false;
		//			for (int j = 0; j < Result->Traces[i].HitResults.Num(); j++)
		//			{
		//				Result->Traces[i].HitResults[j].ImpactPoint = Target;
		//				Result->Traces[i].HitResults[j].ImpactNormal = (Target - Params->FireLoc).Normalize();
		//				Result->Traces[i].HitResults[j].bBlockingHit = false;
		//				Result->Traces[i].HitResults[j].HitObjectHandle.Actor.ObjectIndex = Targ->Index;
		//				Result->Traces[i].HitResults[j].HitObjectHandle.Actor.ObjectSerialNumber = SDK::UObject::GObjects->GetItemByIndex(Targ->Index)->SerialNumber;
		//				Result->Traces[i].HitResults[j].Component.ObjectIndex = Targ->Mesh->Index;
		//				Result->Traces[i].HitResults[j].Component.ObjectSerialNumber = SDK::UObject::GObjects->GetItemByIndex(Targ->Mesh->Index)->SerialNumber;
		//				Result->Traces[i].HitResults[j].BoneName = Targ->Mesh->GetBoneName(Aimbot_BoneIndex);
		//			}
		//		}
		//	}
		//}
		TFD::native_HandleInstantHitResult(This, Params, Result);
	}

	bool __fastcall hkIsValidHit(TFD::UM1WeaponInstantHitComponent* This, float FireTime, SDK::FVector* FireLoc, SDK::FVector* FireDir, TFD::FHitResult* HitResult)
	{
		bool IsValid = TFD::native_IsValidHit(This, FireTime, FireLoc, FireDir, HitResult);
		std::cout << "IsValidHit Return: (" << IsValid << ")\n";
		std::cout << "IsValidHit FireLoc: (" << FireLoc->X << ", " << FireLoc->Y << ", " << FireLoc->Z << ")\n";
		std::cout << "IsValidHit FireDir: (" << FireDir->X << ", " << FireDir->Y << ", " << FireDir->Z << ")\n";
		std::cout << "IsValidHit HitResult ImpactPoint: (" << HitResult->ImpactPoint.X << ", " << HitResult->ImpactPoint.Y << ", " << HitResult->ImpactPoint.Z << ")\n";
		return IsValid;
	}


	float DistX = 0.0f;
	float DistY = 0.0f;
	float DistZ = 0.0f;

	bool __fastcall hkLineTraceMulti(SDK::TArray<TFD::FHitResult>* OutHits, TFD::AActor* InSrcActor, SDK::FVector* StartPos, SDK::FVector* EndPos, TFD::EM1RelationsCheckType InRelationsCheckType, void* InQueryTag, bool InbCheckValidTarget, bool bTraceComplex, bool bReturnBlockedTargetIfNoTarget)
	{
		if (CFG::cfg_Aim_SilentAim && SilentAim_Target != nullptr)
		{
			//std::cout << "[Silent Aim] Redirecting trace to target\n";
			if (SilentAim_Target->IsA(TFD::AM1Character::StaticClass()))
			{
				TFD::AM1Character* Targ = static_cast<TFD::AM1Character*>(SilentAim_Target);
				if (Targ && !Targ->IsDead() && Targ->Mesh && Targ->Mesh->GetNumBones() > 0 && Targ->Mesh->BoneArray.IsValidIndex(SilentAim_BoneIndex))
				{
					
					SDK::FMatrix ComponentMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(Targ->Mesh->K2_GetComponentToWorld());
					SDK::FTransform bone = Targ->Mesh->BoneArray[SilentAim_BoneIndex];
					SDK::FMatrix BoneMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(bone);
					SDK::FMatrix WorldMatrix = TFD::UKismetMathLibrary::Multiply_MatrixMatrix(BoneMatrix, ComponentMatrix);
					SDK::FTransform WorldPosition = TFD::UKismetMathLibrary::Conv_MatrixToTransform(WorldMatrix);
					SDK::FVector Target = WorldPosition.Translation;

					*EndPos = Target;
					InbCheckValidTarget = false;
					bTraceComplex = false;
					bReturnBlockedTargetIfNoTarget = false;
					//std::cout << "[Silent Aim] Trace redirected to: (" << Target.X << ", " << Target.Y << ", " << Target.Z << ")\n";
				}
			}
			else if (SilentAim_Target->IsA(TFD::AM1AbilityActor::StaticClass()))
			{
				SDK::FVector Target = SilentAim_Target->K2_GetActorLocation();
				*EndPos = Target;
				InbCheckValidTarget = false;
				bTraceComplex = false;
				bReturnBlockedTargetIfNoTarget = false;
				//std::cout << "[Silent Aim] Ability actor trace redirected to: (" << Target.X << ", " << Target.Y << ", " << Target.Z << ")\n";
			}
		}
		bool result = TFD::native_LineTraceMulti(OutHits, InSrcActor, StartPos, EndPos, InRelationsCheckType, InQueryTag, InbCheckValidTarget, bTraceComplex, bReturnBlockedTargetIfNoTarget);
		return result;
	}

	bool __fastcall hkTestBeamHits(void* This, void* Start, void* Dir, void* Results, float Size)
	{
		bool Pass = TFD::nativeTestBeamHits(This, Start, Dir, Results, Size);
		std::cout << "Test beam: " << (int)Pass << "\n";
		return Pass;
	}

	void hkServerRequestProcessInteraction(void* This, TFD::FM1TemplateId* ID, int ActorUniqueID, TFD::AActor* InNpc)
	{
		std::cout << "interact: " << ID->ID << " - " << ActorUniqueID << " - " << std::hex << static_cast<void*>(InNpc) << std::dec << "\n";

		return TFD::ServerRequestProcessInteraction(This, ID, ActorUniqueID, InNpc);
	}

#include <dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
	void PrintStackTrace()
	{
		void* stack[64];
		USHORT frames = CaptureStackBackTrace(0, 64, stack, nullptr);

		HANDLE process = GetCurrentProcess();

		for (USHORT i = 0; i < frames; i++)
		{
			DWORD64 addr = (DWORD64)stack[i];

			HMODULE hModule = NULL;
			if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)addr, &hModule))
			{
				char moduleName[MAX_PATH];
				if (GetModuleFileNameA(hModule, moduleName, MAX_PATH))
				{
					// Get offset from module base
					DWORD64 base = (DWORD64)hModule;
					DWORD64 offset = addr - base;

					// Extract just the file name (optional)
					const char* fileName = strrchr(moduleName, '\\');
					if (!fileName) fileName = moduleName;
					else fileName++;

					std::cout << i << ": " << fileName
						<< "!0x" << std::hex << offset << std::dec << "\n";
				}
				else
				{
					std::cout << i << ": [Unknown Module]!0x" << std::hex << addr << std::dec << "\n";
				}
			}
			else
			{
				std::cout << i << ": [No Module]!0x" << std::hex << addr << std::dec << "\n";
			}
		}
	}

	bool hkRequestProcessInteractionCheckA(void* This, int ID)
	{
		//PrintStackTrace();
		//if (ResupplyNPC != nullptr)
		//	std::cout << "got npc\n";
		//std::cout << "hkRequestProcessInteractionCheckA\n";
		return true;
	}
	bool hkRequestProcessInteractionCheckB(void* This, TFD::AActor* NPC)
	{
		//PrintStackTrace();
		//if (ResupplyNPC != nullptr)
		//	std::cout << "got npc\n";
		//std::cout << "hkRequestProcessInteractionCheckB\n";
		return true;
	}

}
