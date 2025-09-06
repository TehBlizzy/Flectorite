#include "Mission.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#define IS_DEBUG_VERSION

namespace Cheat
{

	void MissionESPTeleport()
	{
		if (Missions != nullptr && Missions->AvailableMissions)
		{
			if (Missions->AvailableMissions.IsValid())
			{
				TFD::AActor* TPTarget = nullptr;
				float closestDistance = INFINITY;
				for (int i = 0; i < Missions->AvailableMissions.Num(); i++)
				{
					if (Missions->AvailableMissions.IsValidIndex(i))
					{
						TFD::AM1MissionActor* Actor = Missions->AvailableMissions[i];
						if (Actor)
						{
							SDK::FVector2D ScreenPos = { -1, -1 };
							if (WorldToScreen(Actor->K2_GetActorLocation(), &ScreenPos))
							{
								if (CFG::cfg_Mission_EnableMissionESP)
								{
									if (Actor->MissionData)
									{
										std::string MissionName = Actor->MissionData->MissionDataRowName.ToString();
										if (!MissionName.empty())
										{
											//char bufferlol[128];
											//sprintf_s(bufferlol, "Mission %s", Actor->MissionData->MissionDataRowName.ToString().c_str());
											Render::R_DrawText(ScreenPos.X, ScreenPos.Y, MissionName, &ColorGold, true);
										}
									}
								}
								float distance = TFD::UKismetMathLibrary::Distance2D(ScreenMiddle, ScreenPos);
								if (distance > 300)
									continue;
								if (distance < closestDistance)
								{
									closestDistance = distance;
									TPTarget = Actor;
								}
							}
						}
					}
				}
				if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Mission_MissionTeleportKey)) && TPTarget != nullptr)
				{
					LocalPlayerCharacter->TeleportHandler->ServerMoveToTeleportToLocation(TPTarget->K2_GetActorLocation(), TPTarget->K2_GetActorRotation());
					LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
				}
			}
		}
	}

	void AutoRestartMission()
	{
		if (Missions == nullptr)
			return;
		static bool WaitForFirstMission = true;
		static std::chrono::steady_clock::time_point LastMissionEndTime = std::chrono::steady_clock::now();;
		static bool bMissionRecentlyCompleted = false;
		Render::R_DrawText(ScreenMiddle.X, 50, "Mission Auto-Restart Enabled", &ColorWhite, true);

		if (CFG::cfg_Mission_AutoRestartType == 0)
		{	
			// Currently active mission or never started a mission
			if (Missions->ActivatedMissions.IsValid())
			{
				if (Missions->ActivatedMissions[0] && Missions->ActivatedMissions[0]->MissionData)
				{
					Render::R_DrawText(ScreenMiddle.X, 72, "Waiting for the mission to be completed...", &ColorWhite, true);
					WaitForFirstMission = false;
					bMissionRecentlyCompleted = false;
				}
				else
				{
					Render::R_DrawText(ScreenMiddle.X, 72, "Waiting for a mission to be started...", &ColorWhite, true);
				}
				return;
			}

			if (WaitForFirstMission)
				return;

			if (!bMissionRecentlyCompleted && !Missions->ActivatedMissions.IsValid())
			{
				bMissionRecentlyCompleted = true;
				LastMissionEndTime = std::chrono::steady_clock::now();
			}

			if (!bMissionRecentlyCompleted)
				return;

			auto Now = std::chrono::steady_clock::now();
			double ElapsedSeconds = std::chrono::duration<double>(Now - LastMissionEndTime).count();

			char bufferlol[64];
			sprintf_s(bufferlol, "Mission Restart Time: %f - %f", ElapsedSeconds, CFG::cfg_Mission_MissionAutoRestartDelay);
			Render::R_DrawText(ScreenMiddle.X, 96, bufferlol, &ColorGold, true);

			if (ElapsedSeconds >= CFG::cfg_Mission_MissionAutoRestartDelay)
			{

				if (Missions->MissionResult->MissionSubType == TFD::EM1MissionSubType::Explosion || Missions->MissionResult->MissionSubType == TFD::EM1MissionSubType::DestructionVulgusPost)
				{
					for (int i = 0; i < Missions->ActivatedMissions.Num(); i++)
					{
						TFD::AM1MissionActor* LAMA = Missions->ActivatedMissions[i];
						if (LAMA && LAMA->MissionData->MissionSubType == TFD::EM1MissionSubType::DestructionVulgusPost)
						{
							Missions->ServerStartMission(LAMA, true);
							bMissionRecentlyCompleted = false;
						}
					}
				}
				else
				{
					if (Missions->MissionResult->MissionSubType == TFD::EM1MissionSubType::VoidFusion)
					{
						for (int i = 0; i < Missions->AvailableMissions.Num(); i++)
						{
							TFD::AM1MissionActor* LAMA = Missions->AvailableMissions[i];
							if (LAMA && LAMA->MissionData->MissionSubType == TFD::EM1MissionSubType::VoidFusion)
							{
								Missions->ServerStartMission(LAMA, true);
								if (LAMA->TaskLinks[0].InstancedTaskActor)
								{
									LocalPlayerCharacter->TeleportHandler->ServerMoveToTeleportToLocation(LAMA->TaskLinks[0].InstancedTaskActor->K2_GetActorLocation(),
										LAMA->TaskLinks[0].InstancedTaskActor->K2_GetActorRotation());
								}
								LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
								bMissionRecentlyCompleted = false;
								break;
							}
						}
					}
					else
					{
						Missions->ServerRestartLastPlayedMission();
						LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
						bMissionRecentlyCompleted = false;
					}
				}
			}
		}
		else // Restart from AvailableMissions[index]
		{
			if (!Missions->AvailableMissions.IsValid() || CFG::cfg_Mission_AutoRestartMissionIndex > Missions->AvailableMissions.Num())
				return;


			if (Missions->ActivatedMissions.IsValid())
			{
				if (Missions->ActivatedMissions[0] && Missions->ActivatedMissions[0]->MissionData)
				{
					Render::R_DrawText(ScreenMiddle.X, 72, "Waiting for the mission to be completed...", &ColorWhite, true);
					bMissionRecentlyCompleted = false;
					return;
				}
			}

			if (!bMissionRecentlyCompleted && !Missions->ActivatedMissions.IsValid())
			{
				bMissionRecentlyCompleted = true;
				LastMissionEndTime = std::chrono::steady_clock::now();
			}

			if (!bMissionRecentlyCompleted)
				return;

			auto Now = std::chrono::steady_clock::now();
			double ElapsedSeconds = std::chrono::duration<double>(Now - LastMissionEndTime).count();

			char bufferlol[64];
			sprintf_s(bufferlol, "Mission Restart Time: %f - %f", ElapsedSeconds, CFG::cfg_Mission_MissionAutoRestartDelay);
			Render::R_DrawText(ScreenMiddle.X, 96, bufferlol, &ColorGold, true);

			if (ElapsedSeconds >= CFG::cfg_Mission_MissionAutoRestartDelay)
			{
				if (Missions->AvailableMissions.IsValidIndex(CFG::cfg_Mission_AutoRestartMissionIndex))
				{
					Missions->ServerStartMission(Missions->AvailableMissions[CFG::cfg_Mission_AutoRestartMissionIndex], true);
					LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
					bMissionRecentlyCompleted = false;
				}
			}
		}
	}
	std::unordered_map<std::string, bool> MoveMissionTaskExceptionSet =
	{
		{"Kingston_F_Hard_D2|MoveD2-10a",true},
		{"TheFortress_F_Hard_D1|MoveD1_Hard_10",true},
		{"CorrodedLand_Invasion_D1|Move-2",false},
		{"TheFortress_Invasion_D2|Move_4a",true},
		{"ForestDead_Invasion_D2|Move1a",false},
		{"Whitenight_Invasion_D1|MoveD1_I2a",false}
	};

	std::unordered_map<std::string, bool> MissionTaskExceptionSet =
	{
	};

	std::unordered_map<std::string, bool> ForceTeleportMissionTaskExceptionSet =
	{
		{"Kingston_Invasion_D2|Assassination_4",true},
		{"SterileLand_Invasion_D2|CollectionReturnD2-I3",true},
		{"SterileLand_Invasion_D2|CollectionReturnD2-I5",true},
		{"Kingston_Invasion_D1|CollectionReturn_3",true},
		{"VoidVessel_Normal_D1|Explosion01",true},
		{"VoidVessel_Normal_D1|Extermination02",true}
	};
	std::string ToLower(std::string in)
	{
		for_each(in.begin(), in.end(), [](char& c) {
			c = tolower(c);
			});
		return in;
	}
	static std::chrono::steady_clock::time_point AutoTeleportStartTime = std::chrono::steady_clock::now();
	extern UC::int32 MissionTaskIndex;
	void MissionTaskTeleporter()
	{
		static std::chrono::steady_clock::time_point LastTaskAttemptTime = std::chrono::steady_clock::now();
		static bool WaitingForTaskFinish = true;
		static int LastActiveTaskIndex = -1;


		auto Now = std::chrono::steady_clock::now();
		double ElapsedSeconds = std::chrono::duration<double>(Now - LastTaskAttemptTime).count();

		if (ElapsedSeconds < 2)
			return;

		LastTaskAttemptTime = Now;

		if (!LocalPlayerCharacter || !Missions || !Missions->ActivatedMissions.IsValid())
			return;

		if (Missions->ActivatedMissions.Num() == 0)
		{
			WaitingForTaskFinish = true;
			LastActiveTaskIndex = -1;
			return;
		}
		TFD::AM1MissionActor* MissionActor = Missions->ActivatedMissions[0];

		if (!MissionActor || !MissionActor->MissionData)
			return;

		if (!MissionActor->TaskLinks.IsValid() || !MissionActor->TaskLinks.IsValidIndex(MissionActor->ProgressInfo.ActivatedTaskIndex))
			return;

		std::cout << MissionActor->ProgressInfo.ActivatedTaskIndex << " - " << LastActiveTaskIndex << " - " << WaitingForTaskFinish << "\n";

		if (WaitingForTaskFinish)
		{
			if (MissionActor->ProgressInfo.ActivatedTaskIndex != LastActiveTaskIndex)
			{
				//std::cout << "Current task changed, no longer waiting for task to be finished.\n";
				WaitingForTaskFinish = false;
				LastActiveTaskIndex = MissionActor->ProgressInfo.ActivatedTaskIndex;
			}
			else
			{
				//std::cout << "Current task hasn't changed, still waiting for task to be finished.\n";
				return;
			}
		}

		if (!WaitingForTaskFinish)
		{
			if (MissionActor->MissionData->MissionDataRowName.ToString().contains("400"))
			{
				//std::cout << "Mission name has 400, waiting for next task*.\n";
				WaitingForTaskFinish = true;
				return;
			}

			TFD::AM1MissionTaskActor* CurrentTaskActor = MissionActor->TaskLinks[LastActiveTaskIndex].InstancedTaskActor;
			if (CurrentTaskActor->MissionTask)
			{
				std::string CurrentTaskName = ToLower(CurrentTaskActor->MissionTask->TaskName.ToString());
				//std::cout << "Current Task: " << CurrentTaskName.c_str() << "\n";
				if (CurrentTaskName.contains("playerset")
					|| CurrentTaskName.contains("fadein"))
				{
					//std::cout << "Current task is playerset or fadein\n";
					WaitingForTaskFinish = true;
					return;
				}

				if (LastActiveTaskIndex > 0)
				{
					TFD::AM1MissionTaskActor* PreviousTaskActor = MissionActor->TaskLinks[LastActiveTaskIndex - 1].InstancedTaskActor;
					if (PreviousTaskActor)
					{
						std::string PreviousTaskName = ToLower(PreviousTaskActor->MissionTask->TaskName.ToString());
						if (!CurrentTaskName.contains("move")
							&& !CurrentTaskName.contains("interact")
							&& !PreviousTaskName.contains("move")
							&& !PreviousTaskName.contains("interact"))
						{
							//std::cout << "Current task is move or interact, and the previous task was also move or interact.\n";
							WaitingForTaskFinish = true;
							return;
						}
					}
				}

				if (CurrentTaskName.contains("move") || CurrentTaskName.contains("interact"))
				{
					Missions->ServerRunTaskActor(CurrentTaskActor);
					std::string MissionTaskName = MissionActor->MissionData->MissionDataRowName.ToString() + "|" + CurrentTaskActor->MissionTask->TaskName.ToString();
					if (MoveMissionTaskExceptionSet.contains(MissionTaskName))
					{
						bool UseWayPoint = MoveMissionTaskExceptionSet[MissionTaskName];
						if (CurrentTaskActor->WayPoints.Num() > 0 && UseWayPoint)
						{
							int WayPointCount = CurrentTaskActor->WayPoints.Num();
							for (int wpi = 0; wpi < WayPointCount; wpi++)
							{
								if (CurrentTaskActor->WayPoints[wpi]->Index_0 == (WayPointCount - 1))
								{
									//std::cout << "Task is move or interact, teleporting to waypoint instead of task actor.\n";
									WaitingForTaskFinish = true;
									LocalPlayerCharacter->TeleportHandler->ServerMoveToTeleportToLocation(CurrentTaskActor->WayPoints[wpi]->K2_GetActorLocation(), CurrentTaskActor->WayPoints[wpi]->K2_GetActorRotation());
									LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
									return;
								}
							}
						}
						else
						{
							//std::cout << "Task is move or interact, teleporting to task actor.\n";
							WaitingForTaskFinish = true;
							LocalPlayerCharacter->TeleportHandler->ServerMoveToTeleportToLocation(CurrentTaskActor->K2_GetActorLocation(), CurrentTaskActor->K2_GetActorRotation());
							LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
							return;
						}
					}
					WaitingForTaskFinish = true;
					return;
				}

				if (LastActiveTaskIndex > 1)
				{
					std::string MissionTaskName = MissionActor->MissionData->MissionDataRowName.ToString() + "|" + CurrentTaskActor->MissionTask->TaskName.ToString();
					if (ForceTeleportMissionTaskExceptionSet.contains(MissionTaskName))
					{
						bool UseWayPoint = ForceTeleportMissionTaskExceptionSet[MissionTaskName];
						if (CurrentTaskActor->WayPoints.Num() > 0 && UseWayPoint)
						{
							int WayPointCount = CurrentTaskActor->WayPoints.Num();
							for (int wpi = 0; wpi < WayPointCount; wpi++)
							{
								if (CurrentTaskActor->WayPoints[wpi]->Index_0 == (WayPointCount - 1))
								{
									//std::cout << "Teleporting to waypoint instead of task actor.\n";
									WaitingForTaskFinish = true;
									LocalPlayerCharacter->TeleportHandler->ServerMoveToTeleportToLocation(CurrentTaskActor->WayPoints[wpi]->K2_GetActorLocation(), CurrentTaskActor->WayPoints[wpi]->K2_GetActorRotation());
									LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
									return;
								}
							}
						}
						else
						{
							//std::cout << "Teleporting to task actor.\n";
							WaitingForTaskFinish = true;
							LocalPlayerCharacter->TeleportHandler->ServerMoveToTeleportToLocation(CurrentTaskActor->K2_GetActorLocation(), CurrentTaskActor->K2_GetActorRotation());
							LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
							return;
						}
					}
					else
					{
						float ODistanceLocalCharacter = 0.0f;
						float ODistanceLastTask = 0.0f;
						float ODistanceBetweenLastTaskLocalChar = 0.0f;
						if (CurrentTaskActor->WayPoints.Num() > 0)
						{
							int WayPointCount = CurrentTaskActor->WayPoints.Num();
							for (int wpi = 0; wpi < WayPointCount; wpi++)
							{
								if (CurrentTaskActor->WayPoints[wpi]->Index_0 == (WayPointCount - 1))
								{
									ODistanceLocalCharacter = CurrentTaskActor->K2_GetActorLocation().GetDistanceTo(LocalPlayerCharacter->K2_GetActorLocation());
									ODistanceLastTask = CurrentTaskActor->K2_GetActorLocation().GetDistanceTo(CurrentTaskActor->WayPoints[wpi]->K2_GetActorLocation());
									ODistanceBetweenLastTaskLocalChar = LocalPlayerCharacter->K2_GetActorLocation().GetDistanceTo(CurrentTaskActor->WayPoints[wpi]->K2_GetActorLocation());
									if (ODistanceBetweenLastTaskLocalChar <= 1500.0f)
										return;
									if (ODistanceLastTask < ODistanceLocalCharacter)
									{
										//std::cout << "Teleporting to waypoint instead of task actor.\n";
										WaitingForTaskFinish = true;
										LocalPlayerCharacter->TeleportHandler->ServerMoveToTeleportToLocation(CurrentTaskActor->WayPoints[wpi]->K2_GetActorLocation(), CurrentTaskActor->WayPoints[wpi]->K2_GetActorRotation());
										LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
									}

								}
							}
						}
						else
						{
							ODistanceLocalCharacter = CurrentTaskActor->K2_GetActorLocation().GetDistanceTo(LocalPlayerCharacter->K2_GetActorLocation());
							ODistanceLastTask = CurrentTaskActor->K2_GetActorLocation().GetDistanceTo(CurrentTaskActor->K2_GetActorLocation());
							ODistanceBetweenLastTaskLocalChar = LocalPlayerCharacter->K2_GetActorLocation().GetDistanceTo(CurrentTaskActor->K2_GetActorLocation());
							if (ODistanceBetweenLastTaskLocalChar <= 1500.0f)
								return;
							if (ODistanceLastTask < ODistanceLocalCharacter)
							{
								//std::cout << "Teleporting to task actor.\n";
								WaitingForTaskFinish = true;
								LocalPlayerCharacter->TeleportHandler->ServerMoveToTeleportToLocation(CurrentTaskActor->K2_GetActorLocation(), CurrentTaskActor->K2_GetActorRotation());
								LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
							}

						}
					}
				}
				WaitingForTaskFinish = true;
			}
		}
	}
	static std::chrono::steady_clock::time_point AutoInstantInfilStartTime = std::chrono::steady_clock::now();
	void InstantInfiltration()
	{
		if (!LocalPlayerController)
			return;

		auto* PlayerState = static_cast<TFD::AM1PlayerState*>(LocalPlayerController->PlayerState);
		if (!PlayerState || !PlayerState->MissionControlComponent)
			return;

		TFD::UM1MissionControlComponent* MCC = PlayerState->MissionControlComponent;
		if (!MCC || MCC->ActivatedMissions.Num() == 0 || MCC->SubServices.Num() == 0)
			return;

		for (TFD::AM1MissionActor* MissionActor : MCC->ActivatedMissions)
		{
			if (!MissionActor)
				continue;

			//int CurrentTask = MissionActor->ProgressInfo.ActivatedTaskIndex;
			if (MissionActor->TaskLinks.IsValidIndex(0))
			{
				TFD::FM1MissionTaskLink TaskLink = MissionActor->TaskLinks[0];
				TFD::AM1MissionTaskActor* TaskActor = TaskLink.InstancedTaskActor;
				if (TaskActor)
				{
					if (!TaskActor->IsA(TFD::AM1MissionTaskActorDestructionVulgusPost::StaticClass()))
						continue;

					auto* VPost = static_cast<TFD::AM1MissionTaskActorDestructionVulgusPost*>(TaskActor);
					if (!VPost)
						continue;
					for (TFD::UM1MissionTaskService* MCCSub : MCC->SubServices)
					{
						if (!MCCSub || !MCCSub->bJoined)
							continue;

						auto* MCCInt = static_cast<TFD::UM1MissionTaskServiceInteraction*>(MCCSub);
						if (!MCCInt)
							continue;
						if (!VPost->MissionTargets.IsValid())
							continue;

						for (TFD::AM1MissionTargetInteraction* Target : VPost->MissionTargets)
						{
							if (!Target ||
								Target->CurrentState == TFD::EM1MissionTargetState::Destructed ||
								Target->CurrentState == TFD::EM1MissionTargetState::Deactivated)
								continue;
							//std::cout << "target is not destructed or deactivated\n";
							auto now = std::chrono::steady_clock::now();
							if (std::chrono::duration_cast<std::chrono::milliseconds>(now - AutoInstantInfilStartTime).count() > 200)
							{
								AutoInstantInfilStartTime = now;
								//std::cout << "target class: " << Target->Class->GetFullName().c_str() << "\n";
								//std::cout << "target state: " << (int)Target->CurrentState << "\n";
								MCCInt->ServerRequestMissionTargetBeginInteraction(Target, LocalPlayerController);
								return;
							}
						}
					}
				}
			}
		}
	}
}
