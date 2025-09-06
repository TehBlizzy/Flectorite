#include "Aimbot.h"
#include "../Menu/Render.h"

namespace Cheat
{
	CONTROLLER_STATE g_Controllers[4];

	bool Aimbot_KeyHeld = false;

	bool ValidateAimTarget(TFD::AActor* Actor)
	{
		if (!LocalPlayerController || !LocalPlayerController->ActorManager_Subsystem)
			return false;

		if (!LocalPlayerController->ActorManager_Subsystem->Characters.IsValid() ||
			LocalPlayerController->ActorManager_Subsystem->Characters.Num() <= 0)
			return false;

		bool Exists = false;
		for (int i = 0; i < LocalPlayerController->ActorManager_Subsystem->Characters.Num(); i++)
		{
			if (!LocalPlayerController->ActorManager_Subsystem->Characters.IsValidIndex(i))
				continue;
			if (!LocalPlayerController->ActorManager_Subsystem->Characters[i])
				continue;

			if (LocalPlayerController->ActorManager_Subsystem->Characters[i] == Actor)
			{
				Exists = true;
				break;
			}
			if (LocalPlayerController->ActorManager_Subsystem->Characters[i]->Children.Num() > 0)
			{
				for (int j = 0; j < LocalPlayerController->ActorManager_Subsystem->Characters[i]->Children.Num(); j++)
				{
					if (LocalPlayerController->ActorManager_Subsystem->Characters[i]->Children.IsValidIndex(j))
					{
						if (LocalPlayerController->ActorManager_Subsystem->Characters[i]->Children[j] == Actor)
						{
							Exists = true;
							break;
						}
					}
				}
			}
		}
		if (Exists)
		{
			if (Actor->IsA(TFD::AM1Character::StaticClass()))
			{
				TFD::AM1Character* AimTarget = static_cast<TFD::AM1Character*>(Actor);
				if (AimTarget && !AimTarget->IsDead() && AimTarget->Mesh &&
					LocalPlayerController->LineOfSightTo(AimTarget, SDK::FVector{ 0, 0, 0 }, false))
				{
					return true;
				}
			}
			else if (Actor->IsA(TFD::AM1AbilityActor::StaticClass()))
			{
				if (LocalPlayerController->LineOfSightTo(Actor, SDK::FVector{ 0, 0, 0 }, false))
					return true;
			}
		}
		return false;
	}

	void SilentAim()
	{
		FindAimTarget(SilentAim_Target, SilentAim_BoneIndex);

		if(SilentAim_Target != nullptr && ValidateAimTarget(SilentAim_Target))
		{
			if (SilentAim_Target->IsA(TFD::AM1Character::StaticClass()))
			{
				TFD::AM1Character* AimTarget = static_cast<TFD::AM1Character*>(SilentAim_Target);
				if (AimTarget->Mesh->BoneArray.IsValid() && AimTarget->Mesh->BoneArray.IsValidIndex(SilentAim_BoneIndex))
				{
					SDK::FMatrix ComponentMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(AimTarget->Mesh->K2_GetComponentToWorld());
					SDK::FTransform bone = AimTarget->Mesh->BoneArray[SilentAim_BoneIndex];
					SDK::FMatrix BoneMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(bone);
					SDK::FMatrix WorldMatrix = TFD::UKismetMathLibrary::Multiply_MatrixMatrix(BoneMatrix, ComponentMatrix);
					SDK::FTransform WorldPosition = TFD::UKismetMathLibrary::Conv_MatrixToTransform(WorldMatrix);

					SDK::FVector2D BoneScreenPos = { -1, -1 };
					if (WorldToScreen(WorldPosition.Translation, &BoneScreenPos))
					{
						Render::R_DrawCircle(BoneScreenPos.X, BoneScreenPos.Y, 6.0f, &ColorGold);
					}
				}
			}
			else if (SilentAim_Target->IsA(TFD::AM1AbilityActor::StaticClass()))
			{
				SDK::FVector2D ScreenPos = { -1, -1 };
				if (WorldToScreen(SilentAim_Target->K2_GetActorLocation(), &ScreenPos))
				{
					Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 6.0f, &ColorGold);
				}
			}
		}
	}


	void Aimbot()
	{
		static TFD::AActor* AimbotTarget = nullptr;
		static int AimbotTargetBone = -1;

		if (CFG::cfg_Aim_DrawFOVCircle && !Menu::ShowMenu)
		{
			float r = (CFG::cfg_Colors_AccentR < 0.0f) ? 0.0f : (CFG::cfg_Colors_AccentR > 1.0f) ? 1.0f : CFG::cfg_Colors_AccentR;
			float g = (CFG::cfg_Colors_AccentG < 0.0f) ? 0.0f : (CFG::cfg_Colors_AccentG > 1.0f) ? 1.0f : CFG::cfg_Colors_AccentG;
			float b = (CFG::cfg_Colors_AccentB < 0.0f) ? 0.0f : (CFG::cfg_Colors_AccentB > 1.0f) ? 1.0f : CFG::cfg_Colors_AccentB;

			SDK::FLinearColor ColorAimbotFOV(r, g, b, 0.8f);
			Render::R_DrawCircle(ScreenMiddle.X, ScreenMiddle.Y, CFG::cfg_Aim_AimbotFOV, &ColorAimbotFOV);
		}
		if (CFG::cfg_Teleport_DrawTeleportFOVCircle && !Menu::ShowMenu)
		{
			float r = (CFG::cfg_Teleport_FOVColorR < 0.0f) ? 0.0f : (CFG::cfg_Teleport_FOVColorR > 1.0f) ? 1.0f : CFG::cfg_Teleport_FOVColorR;
			float g = (CFG::cfg_Teleport_FOVColorG < 0.0f) ? 0.0f : (CFG::cfg_Teleport_FOVColorG > 1.0f) ? 1.0f : CFG::cfg_Teleport_FOVColorG;
			float b = (CFG::cfg_Teleport_FOVColorB < 0.0f) ? 0.0f : (CFG::cfg_Teleport_FOVColorB > 1.0f) ? 1.0f : CFG::cfg_Teleport_FOVColorB;

			SDK::FLinearColor ColorTeleportFOV(r, g, b, 0.7f);
			Render::R_DrawCircle(ScreenMiddle.X, ScreenMiddle.Y, CFG::cfg_Teleport_FOV, &ColorTeleportFOV);
		}

		if (CFG::cfg_Aim_AimbotMode == 0)
		{
			if (!CFG::cfg_Aim_AimbotUseController && !Aimbot_KeyHeld)
				return;
			if (CFG::cfg_Aim_AimbotUseController)
			{
				bool isAiming = false;
				for (DWORD i = 0; i < 4; i++)
				{
					if (g_Controllers[i].bConnected)
					{
						if (g_Controllers[i].state.Gamepad.bLeftTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
						{
							isAiming = true;
						}
					}
				}
				if (!isAiming)
					return;
			}
		}

		if (!AimbotTarget || !ValidateAimTarget(AimbotTarget))
		{
			FindAimTarget(AimbotTarget, AimbotTargetBone);
			return;
		}

		if (AimbotTarget)
		{
			if (AimbotTarget->IsA(TFD::AM1Character::StaticClass()))
			{
				TFD::AM1Character* AimTarget = static_cast<TFD::AM1Character*>(AimbotTarget);
				if (AimTarget->Mesh->BoneArray.IsValid() && AimTarget->Mesh->BoneArray.IsValidIndex(AimbotTargetBone))
				{
					SDK::FMatrix ComponentMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(AimTarget->Mesh->K2_GetComponentToWorld());
					SDK::FTransform bone = AimTarget->Mesh->BoneArray[AimbotTargetBone];
					SDK::FMatrix BoneMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(bone);
					SDK::FMatrix WorldMatrix = TFD::UKismetMathLibrary::Multiply_MatrixMatrix(BoneMatrix, ComponentMatrix);
					SDK::FTransform WorldPosition = TFD::UKismetMathLibrary::Conv_MatrixToTransform(WorldMatrix);

					SDK::FVector2D BoneScreenPos = { -1, -1 };
					if (WorldToScreen(WorldPosition.Translation, &BoneScreenPos))
					{
						float distance = TFD::UKismetMathLibrary::Distance2D(ScreenMiddle, BoneScreenPos);
						if (distance <= CFG::cfg_Aim_AimbotFOV)
						{
							SDK::FRotator Angles = TFD::UKismetMathLibrary::FindLookAtRotation(LocalPlayerController->PlayerCameraManager->GetCameraLocation(), WorldPosition.Translation);
							LocalPlayerController->SetControlRotation(
								TFD::UKismetMathLibrary::RInterpTo(LocalPlayerController->PlayerCameraManager->GetCameraRotation(), Angles, ((TFD::UGameplayStatics*)TFD::UGameplayStatics::StaticClass())->GetWorldDeltaSeconds(GWorld), CFG::cfg_Aim_AimbotSpeed));
						}
					}
				}
			}
			else if (AimbotTarget->IsA(TFD::AM1AbilityActor::StaticClass()))
			{
				SDK::FVector2D ScreenPos = { -1, -1 };
				if (WorldToScreen(AimbotTarget->K2_GetActorLocation(), &ScreenPos))
				{
					float distance = TFD::UKismetMathLibrary::Distance2D(ScreenMiddle, ScreenPos);
					if (distance <= CFG::cfg_Aim_AimbotFOV)
					{
						SDK::FRotator Angles = TFD::UKismetMathLibrary::FindLookAtRotation(LocalPlayerController->PlayerCameraManager->GetCameraLocation(), AimbotTarget->K2_GetActorLocation());
						LocalPlayerController->SetControlRotation(
							TFD::UKismetMathLibrary::RInterpTo(LocalPlayerController->PlayerCameraManager->GetCameraRotation(), Angles, ((TFD::UGameplayStatics*)TFD::UGameplayStatics::StaticClass())->GetWorldDeltaSeconds(GWorld), CFG::cfg_Aim_AimbotSpeed));
					}
				}

			}
		}
	}

	void FindAimTarget(TFD::AActor* &ActorOut, int& BoneIndexOut)
	{
		if (!LocalPlayerController || !LocalPlayerController->ActorManager_Subsystem)
		{
			ActorOut = nullptr;
			BoneIndexOut = -1;
			return;
		}
		if (!LocalPlayerController->ActorManager_Subsystem->Characters.IsValid() ||
			LocalPlayerController->ActorManager_Subsystem->Characters.Num() <= 0)
		{
			ActorOut = nullptr;
			BoneIndexOut = -1;
			return;
		}

		TFD::AActor* BestTarget = nullptr;
		int BestTargetBoneIndex = -1;
		float closestDistance = INFINITY;

		for (int i = 0; i < LocalPlayerController->ActorManager_Subsystem->Characters.Num(); i++)
		{
			if (!LocalPlayerController->ActorManager_Subsystem->Characters.IsValidIndex(i))
				continue;

			TFD::AM1Character* p = LocalPlayerController->ActorManager_Subsystem->Characters[i];
			if (!p)
				continue;

			if (p->IsDead())
				continue;

			if (!p->IsA(TFD::AM1Monster::StaticClass()) &&
				(!p->CharacterAttribute || !p->CharacterAttribute->IsA(TFD::UM1MonsterAttribute::StaticClass())))
				continue;


			// Checking main actor first
			if (LocalPlayerController->LineOfSightTo(p, SDK::FVector{ 0, 0, 0 }, false))
			{
				if (!IDBoneMap.contains(p->CharacterId.ID))
				{
					if (p->Mesh && p->Mesh->BoneArray.IsValid() && p->Mesh->BoneArray.Num() > 0)
					{
						std::vector<int> bones = {};
						int boneCount = p->Mesh->BoneArray.Num();

						for (int j = 0; j < boneCount; j++)
						{
							if (p->Mesh->BoneArray.IsValidIndex(j))
							{
								std::string boneName = p->Mesh->GetBoneName(j).ToString();
								if (boneName.find("Weakness") != std::string::npos ||
									boneName.find("-Head") != std::string::npos ||
									boneName.find("_head") != std::string::npos ||
									boneName.find("Spine1") != std::string::npos)
								{
									bones.push_back(j);
								}
								if (boneName.find("LHandWeaponSocket") != std::string::npos)
								{
									if (IDNameMap.contains(p->CharacterId.ID) && IDNameMap[p->CharacterId.ID].find("ShieldBearer") != std::string::npos)
									{
										bones.push_back(j);
									}
								}
							}
						}
						if (bones.size() > 0)
						{
							IDBoneMap.insert({ p->CharacterId.ID, bones });
							UpdateCache = true;
						}
					}
					continue;
				}

				if (p->Mesh && p->Mesh->BoneArray.IsValid() && p->Mesh->BoneArray.Num() > 0)
				{
					SDK::FMatrix ComponentMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(p->Mesh->K2_GetComponentToWorld());
					std::vector<int> indexes = IDBoneMap[p->CharacterId.ID];

					for (size_t j = 0; j < indexes.size(); j++)
					{
						if (p->Mesh->BoneArray.IsValidIndex(indexes[j]))
						{
							SDK::FTransform bone = p->Mesh->BoneArray[indexes[j]];
							SDK::FMatrix BoneMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(bone);
							SDK::FMatrix WorldMatrix = TFD::UKismetMathLibrary::Multiply_MatrixMatrix(BoneMatrix, ComponentMatrix);
							SDK::FTransform WorldPosition = TFD::UKismetMathLibrary::Conv_MatrixToTransform(WorldMatrix);

							SDK::FVector2D BoneScreenPos = { -1, -1 };
							if (WorldToScreen(WorldPosition.Translation, &BoneScreenPos))
							{
								float distance = TFD::UKismetMathLibrary::Distance2D(ScreenMiddle, BoneScreenPos);
								if (distance <= CFG::cfg_Aim_AimbotFOV && distance < closestDistance)
								{
									closestDistance = distance;
									BestTarget = p;
									BestTargetBoneIndex = indexes[j];
								}
							}
						}
					}
				}
			}

			if (p->Children.IsValid() && p->Children.Num() > 0)
			{
				int childCount = p->Children.Num();
				for (int a = 0; a < childCount; a++)
				{
					if (!p->Children.IsValidIndex(a) || !p->Children[a])
						continue;

					if (p->Children[a]->IsA(TFD::AM1AbilityActor::StaticClass()))
					{
						std::string className = p->Children[a]->Class->GetFullName();
						if (className.find("Immun") != std::string::npos ||
							className.find("JudgementEye") != std::string::npos)
						{
							SDK::FVector2D ScreenPos = { -1, -1 };
							if (WorldToScreen(p->Children[a]->K2_GetActorLocation(), &ScreenPos))
							{
								float distance = TFD::UKismetMathLibrary::Distance2D(ScreenMiddle, ScreenPos);
								if (distance <= CFG::cfg_Aim_AimbotFOV && distance < closestDistance)
								{
									closestDistance = distance;
									BestTarget = p->Children[a];
									BestTargetBoneIndex = -1;
								}
							}
						}
					}
				}
			}
		}

		ActorOut = BestTarget;
		BoneIndexOut = BestTargetBoneIndex;
	}

	TFD::AActor* GetClosestEnemyForTeleport()
	{
		TFD::AActor* ret = nullptr;
		float closestDistance = INFINITY;

		if (!LocalPlayerController || !LocalPlayerController->ActorManager_Subsystem)
			return nullptr;

		if (!LocalPlayerController->ActorManager_Subsystem->Characters.IsValid() ||
			LocalPlayerController->ActorManager_Subsystem->Characters.Num() <= 0)
			return nullptr;
		int characterCount = LocalPlayerController->ActorManager_Subsystem->Characters.Num();

		for (int i = 0; i < characterCount; i++)
		{
			if (!LocalPlayerController->ActorManager_Subsystem->Characters.IsValidIndex(i))
				continue;

			TFD::AM1Character* p = LocalPlayerController->ActorManager_Subsystem->Characters[i];
			if (!p)
				continue;

			if (p == LocalPlayerCharacter)
				continue;

			if (p->IsDead())
				continue;

			if (!p->IsA(TFD::AM1Monster::StaticClass()) &&
				(!p->CharacterAttribute || !p->CharacterAttribute->IsA(TFD::UM1MonsterAttribute::StaticClass())))
				continue;

			SDK::FVector2D ScreenPos = { -1, -1 };
			if (WorldToScreen(p->K2_GetActorLocation(), &ScreenPos))
			{
				float distance = TFD::UKismetMathLibrary::Distance2D(ScreenMiddle, ScreenPos);
				if (distance <= CFG::cfg_Teleport_FOV && distance < closestDistance)
				{
					closestDistance = distance;
					ret = p;
				}
			}
		}
		return ret;
	}

	HRESULT UpdateControllerState()
	{
		DWORD dwResult;
		for (DWORD i = 0; i < 4; i++)
		{
			dwResult = XInputGetState(i, &g_Controllers[i].state);

			if (dwResult == ERROR_SUCCESS)
				g_Controllers[i].bConnected = true;
			else
				g_Controllers[i].bConnected = false;
		}

		return S_OK;
	}
}
