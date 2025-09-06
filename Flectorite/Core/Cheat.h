#pragma once

#include "SDK.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include <TlHelp32.h>
#include "SimpleIni.h"
#include <bitset>
#include <XInput.h>
#pragma comment(lib,"xinput.lib")
#include "../Menu/Dx12.h"
#include "../Menu/Render.h"
#include "../Menu/Config.h"
#include "../Menu/Menu.h"
#include "../Cheats/WeaponModifications.h"
#include "../Cheats/Aimbot.h"
#include "../Cheats/ActorESP.h"
#include "../Cheats/Mission.h"
#include "../Cheats/ItemDrops.h"
#include "../Cheats/Player.h"
#include "../Cheats/References.h"
#include "../Cheats/Cosmetics.h"
#include <chrono>


namespace Cheat
{
	extern __int64 (*oPostRender)(void* ViewportClient, void* DebugCanvas);
	__int64 PostRender(void* self, void* canvas);
	// Both of these must return true before any other cheat functions are called
	bool IsValidUWorld();
	bool IsCheatReady();
	// Saving and Loading of Cache Data
	void SaveCacheData();
	void LoadCacheData();
	// Extra
	void CreateConsole();
	void CreateCheatManager();
	void ToggleCheatUI();

	extern bool ModifyViewMode;

	//extern float BoundsScale; I aint touchin this shit fuck that noise -Tivmo
	extern float g_SavePointIconColor[4];

	void __fastcall hkSpeedHackDetecting(void* This, float InDeltaTime);
	void __fastcall hkBP_FireInternal_Implementation(TFD::UM1WeaponInstantHitComponent* This, float FireTime, SDK::FVector* FireLoc, float WeaponRange, TFD::FM1ScaledInteger PenetrationStat);
	void __fastcall hkHandleInstantHitResult(TFD::UM1WeaponInstantHitComponent* This, TFD::FM1WeaponInstantHitParams* Params, TFD::FM1WeaponInstantHitTraceResult_Multi_Penetration* Result);
	bool __fastcall hkIsValidHit(TFD::UM1WeaponInstantHitComponent* This, float FireTime, SDK::FVector* FireLoc, SDK::FVector* FireDir, TFD::FHitResult* HitResult);
	bool __fastcall hkLineTraceMulti(SDK::TArray<TFD::FHitResult>* OutHits, TFD::AActor* InSrcActor, SDK::FVector* StartPos, SDK::FVector* EndPos, TFD::EM1RelationsCheckType InRelationsCheckType, void* InQueryTag, bool InbCheckValidTarget, bool bTraceComplex, bool bReturnBlockedTargetIfNoTarget);
	bool __fastcall hkTestBeamHits(void* This, void* Start, void* Dir, void* Results, float Size);

	bool hkRequestProcessInteractionCheckA(void* This, int ID);
	bool hkRequestProcessInteractionCheckB(void* This, TFD::AActor* NPC);
	void hkServerRequestProcessInteraction(void* This, TFD::FM1TemplateId* ID, int ActorUniqueID, TFD::AActor* InNpc);
}
