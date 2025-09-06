#include "Menu.h"
#include "Effects/Effects.h"
#include "../Menu/Browser/Browser.h"
#include "../Cheats/Mission.h"
#include "../Cheats/SavePointConfig.h"
#include "imgui_custom.h"

//#define IS_DEBUG_VERSION
namespace Cheat
{
	// Global variable for save point icon color (accessible from other files)
	float g_SavePointIconColor[4] = { CFG::cfg_Colors_AccentR, CFG::cfg_Colors_AccentG, CFG::cfg_Colors_AccentB, 1.0f }; // Default to menu accent color
}
namespace Menu
{

	bool ShowMenu = false;

	int CurrentStyleIndex = 0;
	bool ShowEnabledFeatures = false;

	static int iTabs = 0;
	static bool effectsInitialized = false;
	static bool browserInitialized = false;



	void HandleKeybinds()
	{
		if (!ImGui::GetCurrentContext() || !ImGui::GetIO().BackendPlatformUserData)
			return;

		ImGuiIO& IO = ImGui::GetIO();
		if (ImGui::IsBindingKey
			|| IO.WantCaptureKeyboard)
			return;

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Loot_LootVacuumKey)))
			CFG::cfg_Loot_EnableLootVacuum = !CFG::cfg_Loot_EnableLootVacuum;

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Loot_GrabLootKey)))
			Cheat::GrabLoot();

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Loot_GrabContainersKey)))
			Cheat::GrabContainers();

		if (CFG::cfg_Aim_AimbotMode == 0)
			Cheat::Aimbot_KeyHeld = ImGui::IsKeyDown(ImGuiKey(CFG::cfg_Aim_AimbotHoldKey));

		if (CFG::cfg_Aim_AimbotMode == 1 && ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Aim_AimbotToggleKey)))
			CFG::cfg_Aim_EnableAimbot = !CFG::cfg_Aim_EnableAimbot;

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Aim_SilentAimKey)))
			CFG::cfg_Aim_SilentAim = !CFG::cfg_Aim_SilentAim;

		static bool timeEnable = false;
		static bool isTimeHeld = false;
		if (!timeEnable && ImGui::IsKeyDown(ImGuiKey(CFG::cfg_Extra_TimeScaleHoldKey)))
		{
			if (!isTimeHeld)
			{
				Cheat::GWorld->PersistentLevel->WorldSettings->TimeDilation = CFG::cfg_Extra_TimeScale;
				isTimeHeld = true;
			}
		}
		if (isTimeHeld && !ImGui::IsKeyDown(ImGuiKey(CFG::cfg_Extra_TimeScaleHoldKey)))
		{
			Cheat::GWorld->PersistentLevel->WorldSettings->TimeDilation = 1.0f;
			isTimeHeld = false;
		}
		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Extra_TimeScaleKey)))
		{
			timeEnable = !timeEnable;
			if (timeEnable)
				Cheat::GWorld->PersistentLevel->WorldSettings->TimeDilation = CFG::cfg_Extra_TimeScale;
			else
				Cheat::GWorld->PersistentLevel->WorldSettings->TimeDilation = 1.0f;
		}

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Mission_MissionAutoRestartKey)))
			CFG::cfg_Mission_EnableMissionAutoRestart = !CFG::cfg_Mission_EnableMissionAutoRestart;

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Mission_InstantInfiltrationKey)))
			CFG::cfg_Mission_EnableInstantInfiltration = !CFG::cfg_Mission_EnableInstantInfiltration;


		if (ImGui::IsKeyDown(ImGuiKey(CFG::cfg_Loot_SpawnLootKey)))
			Cheat::TrySpawnGenericLoot = true;
		else
			Cheat::TrySpawnGenericLoot = false;


		if (ImGui::IsKeyDown(ImGuiKey(CFG::cfg_Loot_SpawnVaultRewardKey)))
			Cheat::TrySpawnVaultLoot = true;
		else
			Cheat::TrySpawnVaultLoot = false;


		if (ImGui::IsKeyDown(ImGuiKey(CFG::cfg_Loot_RestartDecodingKey)))
			Cheat::RestartDecoding = true;
		else
			Cheat::RestartDecoding = false;

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Hotswap_PresetSelectKey)))
			Cheat::PresetActivate();

		if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
			Cheat::CurrentPresetIndex = Cheat::CurrentPresetIndex - 1 < 0 ? (int)(Cheat::PresetsMap.size() - 1) : Cheat::CurrentPresetIndex - 1;

		if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
			Cheat::CurrentPresetIndex = Cheat::CurrentPresetIndex + 1 > (int)(Cheat::PresetsMap.size() - 1) ? 0 : Cheat::CurrentPresetIndex + 1;

		if (ImGui::IsKeyPressed(ImGuiKey_PageUp))
			Cheat::CurrentSavePointIndex = Cheat::CurrentSavePointIndex - 1 < 0 ? (Cheat::MaxSavePoints - 1) : Cheat::CurrentSavePointIndex - 1;

		if (ImGui::IsKeyPressed(ImGuiKey_PageDown))
			Cheat::CurrentSavePointIndex = Cheat::CurrentSavePointIndex + 1 > (Cheat::MaxSavePoints - 1) ? 0 : Cheat::CurrentSavePointIndex + 1;

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Abilities_AutomaticResupplyToggleKey)))
			CFG::cfg_Abilities_EnableAutomaticResupply = !CFG::cfg_Abilities_EnableAutomaticResupply;

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Teleport_TeleportKey)))
		{
			if (CFG::cfg_Teleport_EnableTeleportToTarget)
				Cheat::TeleportToTarget();
			else
				Cheat::TeleportForward();
		}

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Teleport_TeleportKey2)))
			Cheat::TeleportForward2();

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Extra_ForceSpawnVehicleKey)))
			Cheat::ForceSpawnVehicle();

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Mission_MissionTaskTeleportKey)))
			Cheat::MissionTaskTeleporter();

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_SavePoint_SaveCurrentPointKey)))
			SavePointCFG::SaveCurrentPoint();

		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_SavePoint_SelectedSaveTeleportKey)))
			SavePointCFG::TeleportToSavePoint();

		if (CFG::cfg_Telekinesis_Enable)
		{
			if (ImGui::IsKeyDown(ImGuiKey(CFG::cfg_Telekinesis_GrabKey)))
				Cheat::UpdateTelekinesis(true);
			else
				Cheat::UpdateTelekinesis(false);
		}

		//#ifdef IS_DEBUG_VERSION
		//
		//		static bool debugShown = false;
		//		if (!debugShown)
		//		{
		//			std::cout << "[HandleKeybinds] Mission Task Teleport Key: " << CFG::cfg_Mission_MissionTaskTeleportKey << "\n";
		//			std::cout << "[HandleKeybinds] Mission Task Teleport Toggle: " << CFG::cfg_Mission_EnableMissionTaskTeleport << "\n";
		//			debugShown = true;
		//		}
		//#endif
		//
		//		if (CFG::cfg_Mission_MissionTaskTeleportKey != ImGuiKey_None && CFG::cfg_Mission_MissionTaskTeleportKey >= 0 && CFG::cfg_Mission_MissionTaskTeleportKey < ImGuiKey_COUNT && ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_Mission_MissionTaskTeleportKey)))
		//		{
		//#ifdef IS_DEBUG_VERSION
		//			std::cout << "[HandleKeybinds] Mission Task Teleport keybind pressed!\n";
		//#endif
		//			Cheat::MissionTaskTeleporter();
		//		}
	}

	void RenderMenu()
	{
		void UpdatePlaybackProgress();
		if (ShowEnabledFeatures)
		{


		}

		if (ShowMenu)
		{

			if (!effectsInitialized)
			{
				main_color = ImColor(CFG::cfg_Colors_AccentR, CFG::cfg_Colors_AccentG, CFG::cfg_Colors_AccentB, 1.0f);
				Effects::g_EffectsManager.Initialize();
				Effects::EffectColors colors;
				colors.accent = ImVec4(CFG::cfg_Colors_AccentR, CFG::cfg_Colors_AccentG, CFG::cfg_Colors_AccentB, 1.0f);
				colors.glow = ImVec4(CFG::cfg_Colors_AccentR, CFG::cfg_Colors_AccentG, CFG::cfg_Colors_AccentB, 0.6f);
				colors.background = ImVec4(CFG::cfg_Colors_BackgroundR, CFG::cfg_Colors_BackgroundG, CFG::cfg_Colors_BackgroundB, 1.0f);
				colors.text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
				colors.particleColor = ImVec4(CFG::cfg_Colors_ParticleR, CFG::cfg_Colors_ParticleG, CFG::cfg_Colors_ParticleB, 1.0f);
				colors.rippleColor = ImVec4(CFG::cfg_Colors_RippleR, CFG::cfg_Colors_RippleG, CFG::cfg_Colors_RippleB, 1.0f);
				colors.galaxyCenter = ImVec4(CFG::cfg_Colors_GalaxyCenterR, CFG::cfg_Colors_GalaxyCenterG, CFG::cfg_Colors_GalaxyCenterB, 1.0f);
				colors.galaxyArms = ImVec4(CFG::cfg_Colors_GalaxyArmsR, CFG::cfg_Colors_GalaxyArmsG, CFG::cfg_Colors_GalaxyArmsB, 1.0f);
				colors.galaxySparkles = ImVec4(CFG::cfg_Colors_GalaxySparklesR, CFG::cfg_Colors_GalaxySparklesG, CFG::cfg_Colors_GalaxySparklesB, 1.0f);
				Effects::g_EffectsManager.SetColors(colors);


				Effects::g_EffectsManager.EnableParticles(CFG::cfg_Effects_EnableParticles);
				Effects::g_EffectsManager.EnableRipples(CFG::cfg_Effects_EnableRipples);
				Effects::g_EffectsManager.EnableGlow(CFG::cfg_Effects_EnableGlow);
				Effects::g_EffectsManager.EnableSparkles(CFG::cfg_Effects_EnableSparkles);
				Effects::g_EffectsManager.EnableGalaxy(CFG::cfg_Effects_EnableGalaxy);
				Effects::g_EffectsManager.SetParticleCount(CFG::cfg_Effects_ParticleCount);
				Effects::g_EffectsManager.SetParticleSpeed(CFG::cfg_Effects_ParticleSpeed);
				Effects::g_EffectsManager.SetParticleSize(CFG::cfg_Effects_ParticleSize);
				Effects::g_EffectsManager.SetParticleStyle((Effects::ParticleStyle)CFG::cfg_Effects_ParticleStyle);
				Effects::g_EffectsManager.SetGlowIntensity(CFG::cfg_Effects_GlowIntensity);
				Effects::g_EffectsManager.SetRippleSpeed(CFG::cfg_Effects_RippleSpeed);
				Effects::g_EffectsManager.SetRippleMaxRadius(CFG::cfg_Effects_RippleMaxRadius);
				Effects::g_EffectsManager.SetRippleLifetime(CFG::cfg_Effects_RippleLifetime);
				Effects::g_EffectsManager.SetGalaxyRotationSpeed(CFG::cfg_Effects_GalaxyRotationSpeed);
				Effects::g_EffectsManager.SetGalaxyArmCount(CFG::cfg_Effects_GalaxyArmCount);
				Effects::g_EffectsManager.SetGalaxyRadius(CFG::cfg_Effects_GalaxyRadius);
				Effects::g_EffectsManager.SetGalaxySparkleCount(CFG::cfg_Effects_GalaxySparkleCount);
				Effects::g_EffectsManager.SetGalaxySparkleBrightness(CFG::cfg_Effects_GalaxySparkleBrightness);
				Effects::g_EffectsManager.SetGalaxySparkleSize(CFG::cfg_Effects_GalaxySparkleSize);
				Effects::g_EffectsManager.SetGalaxyPosition(CFG::cfg_Effects_GalaxyPositionX, CFG::cfg_Effects_GalaxyPositionY);

				Effects::g_EffectsManager.EnableCustomCursor(CFG::cfg_Cursor_UseCustomCursor);
				Effects::g_EffectsManager.SetCursorStyle((Effects::CursorStyle)CFG::cfg_Cursor_Style);
				Effects::g_EffectsManager.SetCursorSize(CFG::cfg_Cursor_Size);
				Effects::g_EffectsManager.SetCursorGlowIntensity(CFG::cfg_Cursor_GlowIntensity);
				Effects::g_EffectsManager.EnableCursorTrail(CFG::cfg_Cursor_EnableTrail);
				Effects::g_EffectsManager.SetCursorTrailLength(CFG::cfg_Cursor_TrailLength);
				Effects::g_EffectsManager.SetCursorSparkleCount(CFG::cfg_Cursor_SparkleCount);
				Effects::g_EffectsManager.SetCursorSparkleSize(CFG::cfg_Cursor_SparkleSize);
				Effects::g_EffectsManager.SetCursorColor(ImVec4(CFG::cfg_Cursor_ColorR, CFG::cfg_Cursor_ColorG, CFG::cfg_Cursor_ColorB, CFG::cfg_Cursor_ColorA));

				Effects::g_EffectsManager.SetCursorGalaxyArmCount(CFG::cfg_Cursor_GalaxyArmCount);
				Effects::g_EffectsManager.SetCursorGalaxyRadius(CFG::cfg_Cursor_GalaxyRadius);
				Effects::g_EffectsManager.SetCursorGalaxyRotationSpeed(CFG::cfg_Cursor_GalaxyRotationSpeed);
				Effects::g_EffectsManager.SetCursorGalaxyCenterPulseIntensity(CFG::cfg_Cursor_GalaxyCenterPulseIntensity);
				Effects::g_EffectsManager.SetCursorGalaxyTwinkleSpeed(CFG::cfg_Cursor_GalaxyTwinkleSpeed);
				Effects::g_EffectsManager.SetCursorEnableGalaxySparkles(CFG::cfg_Cursor_EnableGalaxySparkles);
				Effects::g_EffectsManager.SetCursorGalaxySparkleCount(CFG::cfg_Cursor_GalaxySparkleCount);
				Effects::g_EffectsManager.SetCursorGalaxySparkleBrightness(CFG::cfg_Cursor_GalaxySparkleBrightness);
				Effects::g_EffectsManager.SetCursorGalaxySparkleSize(CFG::cfg_Cursor_GalaxySparkleSize);
				Effects::g_EffectsManager.SetCursorGalaxyCenterColor(ImVec4(CFG::cfg_Cursor_GalaxyCenterColorR, CFG::cfg_Cursor_GalaxyCenterColorG, CFG::cfg_Cursor_GalaxyCenterColorB, CFG::cfg_Cursor_GalaxyCenterColorA));
				Effects::g_EffectsManager.SetCursorGalaxyArmsColor(ImVec4(CFG::cfg_Cursor_GalaxyArmsColorR, CFG::cfg_Cursor_GalaxyArmsColorG, CFG::cfg_Cursor_GalaxyArmsColorB, CFG::cfg_Cursor_GalaxyArmsColorA));
				Effects::g_EffectsManager.SetCursorGalaxySparklesColor(ImVec4(CFG::cfg_Cursor_GalaxySparklesColorR, CFG::cfg_Cursor_GalaxySparklesColorG, CFG::cfg_Cursor_GalaxySparklesColorB, CFG::cfg_Cursor_GalaxySparklesColorA));

				effectsInitialized = true;
			}

			if (!browserInitialized)
			{
				Browser::Initialize();
				browserInitialized = true;
			}

#ifdef IMGUI_BLUR_TEST
			ImGui::Begin("Blurred", nullptr);
			{
				ImGui::Image((ImTextureID)ImGui::BLUR.srvTempOriginal.ptr, ImVec2(512, 512));

				ImGui::End();
			}
#endif


			ImGui::SetNextWindowSize(ImVec2(827, 604));
			ImGui::Begin("General", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);
			{
				auto draw = ImGui::GetWindowDrawList();
				const auto& p = ImGui::GetWindowPos();
				const ImGuiStyle& s = ImGui::GetStyle();

				ImGuiIO& io = ImGui::GetIO();
				if (draw && io.DeltaTime > 0.0f && io.DeltaTime < 1.0f)
				{
					Effects::g_EffectsManager.Update(io.DeltaTime);
				}

				if (CFG::cfg_ESP_GradientCycling)
				{
					// Use the shared gradient time system for synchronization
					float gradientTime = Render::GetSharedGradientTime();

					// Use the same flowing rainbow system as ESP shapes
					SDK::FLinearColor flowingColor = Render::GetFlowingRainbowColor(0.0f, gradientTime);
					main_color = ImColor(flowingColor.R, flowingColor.G, flowingColor.B, 1.0f);
				}
				else
				{
					// Use static accent color when gradient cycling is disabled
					main_color = ImColor(CFG::cfg_Colors_AccentR, CFG::cfg_Colors_AccentG, CFG::cfg_Colors_AccentB, 1.0f);
				}

				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					ImVec2 mousePos = ImGui::GetMousePos();
					ImVec2 windowPos = ImGui::GetWindowPos();
					ImVec2 windowSize = ImGui::GetWindowSize();

					if (mousePos.x >= windowPos.x && mousePos.x <= windowPos.x + windowSize.x &&
						mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + windowSize.y)
					{
						Effects::g_EffectsManager.CreateRipple(mousePos, CFG::cfg_Effects_RippleMaxRadius);
					}
				}

				draw->AddRectFilled(p + ImVec2(0, 55), p + ImVec2(827, 604), winbg_color, s.WindowRounding, ImDrawFlags_RoundCornersBottom);

				Effects::g_EffectsManager.UpdateColors(main_color, main_color);
				Effects::g_EffectsManager.Render(draw, p, ImVec2(827, 604));
				draw->AddRectFilled(p + ImVec2(0, 55), p + ImVec2(187, 604), background_color, s.WindowRounding, ImDrawFlags_RoundCornersBottomLeft);
				draw->AddRectFilled(p, p + ImVec2(827, 55), background_color/*ImColor(5, 5, 5, 45)*/, s.WindowRounding, ImDrawFlags_RoundCornersTop);
				draw->AddRectFilled(p + ImVec2(0, 52), p + ImVec2(827, 55), main_color, s.WindowRounding);

				ImGui::PushFont(logo_font);
				draw->AddText(GetTextCenterPosition(p, p + ImVec2(827, 52), "Flectorite v2.41"), ImColor(1.f, 1.f, 1.f, 1.f), "Flectorite v2.41");
				ImGui::PopFont();

				ImGui::SetCursorPos(ImVec2(10, 70));

				ImGui::BeginGroup();

				ImGui::PushFont(medium_font);
				ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 0.8f), "Combat");
				ImGui::PopFont();

				ImGui::VerticalTab("Actor ESP", "8", &iTabs, 0);
				ImGui::VerticalTab("Aimbot", "9", &iTabs, 1);
				ImGui::VerticalTab("Features ", "2", &iTabs, 2);

				ImGui::PushFont(medium_font);
				ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 0.8f), "Loot");
				ImGui::PopFont();

				ImGui::VerticalTab("Loot ESP", "8", &iTabs, 3);
				ImGui::VerticalTab("Missions", "5", &iTabs, 4);
				ImGui::VerticalTab("Features", "2", &iTabs, 5);

				ImGui::PushFont(medium_font);
				ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 0.8f), "Misc");
				ImGui::PopFont();

				ImGui::VerticalTab("Teleporting", "1", &iTabs, 6);
				ImGui::VerticalTab("Customization", "3", &iTabs, 7);
				ImGui::VerticalTab("Features  ", "2", &iTabs, 8);
				ImGui::VerticalTab("Menu", "6", &iTabs, 9);

				ImGui::EndGroup();

				ImGui::SetCursorPos(ImVec2(197, 72));
				ImGui::BeginGroup();
				{
					if (iTabs == 0)
					{
						ImGui::BeginChild("FriendlyESP", ImVec2(frame_size.x, 532));
						{
							ImGui::CustomCheckbox("Enable Friendly ESP", &CFG::cfg_Friendly_Enable);
							ImGui::QuickTooltip("ESP for Players and Allies.");

							if (CFG::cfg_Friendly_Enable)
							{
								ImGui::CustomCheckbox("Show Distance", &CFG::cfg_Friendly_ShowDistance);
								ImGui::CustomCheckbox("Player Names", &CFG::cfg_Friendly_DrawNames);
								ImGui::CustomCheckbox("Player Boxes", &CFG::cfg_Friendly_DrawBoxes);
								ImGui::CustomCheckbox("Player 3D Boxes", &CFG::cfg_Friendly_Draw3DBoxes);
								//ImGui::QuickTooltip("May SLIGHTLY Hurt Performance If The Screen Is Filled With Them. (Even more a Risk with RGB Shapes and stuff.)");
								ImGui::CustomCheckbox("Player Circles", &CFG::cfg_Friendly_DrawCircles);
								ImGui::CustomCheckbox("Player 3D Spheres", &CFG::cfg_Friendly_Draw3DSpheres);
								//ImGui::QuickTooltip("May SLIGHTLY Hurt Performance If The Screen Is Filled With Them. (Even more a Risk with RGB Shapes and stuff.)");
								ImGui::CustomCheckbox("Player Lines", &CFG::cfg_Friendly_DrawLines);
							}

							ImGui::Spacing();
							ImGui::CustomCheckbox("Enable Collectible ESP", &CFG::cfg_Actors_EnableCollectibleESP);
							ImGui::QuickTooltip("Arche Echoes and Journals Name ESP + Distance.");

							ImGui::EndChild();
						}
						ImGui::SetCursorPos(ImVec2(207 + frame_size.x, 72));
						ImGui::BeginChild("EnemyESP", ImVec2(frame_size.x, 532));
						{
							ImGui::CustomCheckbox("Enable Enemy ESP", &CFG::cfg_Enemy_Enable);
							ImGui::QuickTooltip("ESP for Enemies");

							if (CFG::cfg_Enemy_Enable)
							{
								ImGui::CustomCheckbox("Show Distance", &CFG::cfg_Enemy_ShowDistance);
								ImGui::CustomCheckbox("Enemy Names", &CFG::cfg_Enemy_DrawNames);
								ImGui::CustomCheckbox("Enemy Boxes", &CFG::cfg_Enemy_DrawBoxes);
								ImGui::CustomCheckbox("Enemy 3D Boxes", &CFG::cfg_Enemy_Draw3DBoxes);
								//ImGui::QuickTooltip("May SLIGHTLY Hurt Performance If The Screen Is Filled With Them. (Even more a Risk with RGB Shapes and stuff.)");
								ImGui::CustomCheckbox("Enemy Circles", &CFG::cfg_Enemy_DrawCircles);
								ImGui::CustomCheckbox("Enemy 3D Spheres", &CFG::cfg_Enemy_Draw3DSpheres);
								//ImGui::QuickTooltip("May SLIGHTLY Hurt Performance If The Screen Is Filled With Them. (Even more a Risk with RGB Shapes and stuff.)");
								ImGui::CustomCheckbox("Enemy Lines", &CFG::cfg_Enemy_DrawLines);
							}

							ImGui::EndChild();
						}
					}
					else if (iTabs == 1)
					{
						ImGui::BeginChild("Aimbot", ImVec2(frame_size.x, 532));
						{
							ImGui::CustomCheckbox("Enable Aimbot", &CFG::cfg_Aim_EnableAimbot);
							const char* AimbotModes[] = {
										"Hold",
										"Toggle",
							};
							ImGui::CustomCombo("Aimbot Mode", &CFG::cfg_Aim_AimbotMode, AimbotModes, IM_ARRAYSIZE(AimbotModes));
							ImGui::Keybind("Aimbot Hold Key", &CFG::cfg_Aim_AimbotHoldKey);
							ImGui::Keybind("Aimbot Toggle Key", &CFG::cfg_Aim_AimbotToggleKey);
							ImGui::CustomCheckbox("Enable Controller Support", &CFG::cfg_Aim_AimbotUseController);
							ImGui::CustomSliderFloat("Aimbot FOV", &CFG::cfg_Aim_AimbotFOV, 1, 2160);
							ImGui::CustomCheckbox("Draw FOV Circle", &CFG::cfg_Aim_DrawFOVCircle);
							ImGui::CustomSliderFloat("Aimbot Speed", &CFG::cfg_Aim_AimbotSpeed, 1, 100);
							//if (Cheat::GameSettings != nullptr)
							//{
							//	ImGui::CustomSliderFloat("FireLocationAllowDistance", &Cheat::GameSettings->FireLocationAllowDistance, 0.01, 10);
							//	ImGui::CustomSliderFloat("FireDirectionAllowRatio", &Cheat::GameSettings->FireDirectionAllowRatio, 0.01, 10);
							//	ImGui::CustomSliderFloat("HitValidationPrecision", &Cheat::GameSettings->HitValidationPrecision, 0.01, 10);
							//	ImGui::CustomSliderFloat("TraceStartLocDiffTolerance", &Cheat::GameSettings->TraceStartLocDiffTolerance, 0.01, 10);
							//	ImGui::CustomSliderFloat("WeaponRangeTolerance", &Cheat::GameSettings->WeaponRangeTolerance, 0.01, 10);
							//}
							ImGui::EndChild();
						}
					}
					else if (iTabs == 2)
					{
						ImGui::BeginChild("CombatFeatures", ImVec2(frame_size.x, 532));
						{
							ImGui::KeybindWithToggle("Enable Silent Aim", &CFG::cfg_Aim_SilentAim, &CFG::cfg_Aim_SilentAimKey);
							ImGui::QuickTooltip("You still need to look in the direction of the Gold-colored target circle.");
							ImGui::CustomCheckbox("Enable No-Recoil and No-Spread", &CFG::cfg_Aim_NoRecoilAndSpread);
							ImGui::CustomCheckbox("Enable Auto-Reload", &CFG::cfg_Aim_NoReload);
							ImGui::CustomCheckbox("Enable Rapid Fire", &CFG::cfg_Aim_RapidFire);
							ImGui::QuickTooltip("Doesnt Work With Semi-Auto, Auto/Burst Work a Little at Low Values.");
							ImGui::CustomSliderFloat("Rapid Fire Rate", &CFG::cfg_Aim_FireRate, 1.0f, 100.0f);
							ImGui::CustomCheckbox("Enable Extended Grapple", &CFG::cfg_Abilities_EnableModifyGrapple);
							ImGui::QuickTooltip("Only works without a module or with Air Maneuvering.");
							ImGui::CustomSliderFloat("Extended Grapple Distance", &CFG::cfg_Abilities_GrappleRange, 1000.0f, 50000.0f);

							ImGui::EndChild();
						}
						ImGui::SetCursorPos(ImVec2(207 + frame_size.x, 72));
						ImGui::BeginChild("CombatFeatures2", ImVec2(frame_size.x, 532));
						{
							ImGui::CustomCheckbox("Enable Preset Overlay", &CFG::cfg_Hotswap_EnableOverlay);
							ImGui::QuickTooltip("You need to have a preset saved and highlighted in the Overlay for Resupply.");
							ImGui::Keybind("Preset Swap Key", &CFG::cfg_Hotswap_PresetSelectKey);
							ImGui::KeybindWithToggle("Enable Automatic Resupply", &CFG::cfg_Abilities_EnableAutomaticResupply, &CFG::cfg_Abilities_AutomaticResupplyToggleKey);
							ImGui::QuickTooltip("Automatically Swap Presets for Health, Ammo, Mana, and to Reset Cooldowns.");
							ImGui::CustomCheckbox("Resupply Using Timer", &CFG::cfg_Abilities_AutomaticResupplyTimed);
							ImGui::CustomSliderFloat("Automatic Resupply Timer (Seconds)", &CFG::cfg_Abilities_AutomaticResupplyTimeDelay, 0.1f, 10.0f);
							ImGui::CustomCheckbox("Resupply On Low Health", &CFG::cfg_Abilities_AutomaticResupplyHealth);
							ImGui::CustomSliderFloat("Resupply Health Percent", &CFG::cfg_Abilities_AutomaticResupplyHealthPercent, 5.0f, 95.0f);
							ImGui::EndChild();
						}

					}
					else if (iTabs == 3)
					{
						ImGui::BeginChild("LootESP", ImVec2(frame_size.x, 532));
						{
							ImGui::CustomCheckbox("Enable Loot ESP", &CFG::cfg_Loot_EnableItemESP);
							ImGui::QuickTooltip("ESPs No Longer Show in Menu.");
							ImGui::CustomCheckbox("Show Distance", &CFG::cfg_Loot_ShowDistance);
							ImGui::CustomCheckbox("Item Boxes", &CFG::cfg_Loot_DrawItemBoxes);
							ImGui::CustomCheckbox("Item 3D Boxes", &CFG::cfg_Loot_DrawItem3DBoxes);
							//ImGui::QuickTooltip("May SLIGHTLY Hurt Performance If The Screen Is Filled With Them. (Even more a Risk with RGB Shapes and stuff.)");
							ImGui::CustomCheckbox("Item Circles", &CFG::cfg_Loot_DrawItemCircles);
							ImGui::CustomCheckbox("Item Spheres", &CFG::cfg_Loot_DrawItem3DSpheres);
							//ImGui::QuickTooltip("May SLIGHTLY Hurt Performance If The Screen Is Filled With Them. (Even more a Risk with RGB Shapes and stuff.)");
							ImGui::CustomCheckbox("Item Names", &CFG::cfg_Loot_DrawItemNames);
							ImGui::EndChild();
						}
						ImGui::SetCursorPos(ImVec2(207 + frame_size.x, 72));
						ImGui::BeginChild("ContainerESP", ImVec2(frame_size.x, 532));
						{
							ImGui::CustomCheckbox("Mission Item ESP", &CFG::cfg_Loot_MissionItemESP);
							ImGui::CustomCheckbox("Encrypted Vaults", &CFG::cfg_Loot_DrawVaults);
							ImGui::CustomCheckbox("Axion Resources", &CFG::cfg_Loot_DrawAxionResources);
							ImGui::CustomCheckbox("Resource Boxes", &CFG::cfg_Loot_DrawSupplyResources);
							ImGui::CustomCheckbox("Void Vessels", &CFG::cfg_Loot_DrawVoidVesselBox);
							//ImGui::CustomCheckbox("Debug Actors in Range", &CFG::cfg_Loot_DebugActorsInRange);
							//ImGui::QuickTooltip("DEBUG TOOL I LEFT IN FOR CURIOUS PEOPLE, CHECK ACTORS WITHIN 'Container Loot Spawn Range'");
							ImGui::EndChild();
						}
					}
					else if (iTabs == 4)
					{
						ImGui::BeginChild("Missions", ImVec2(frame_size.x, 532));
						{
							ImGui::CustomCheckbox("Enable Mission ESP", &CFG::cfg_Mission_EnableMissionESP);
							ImGui::KeybindWithToggle("Enable Mission Auto-Restart", &CFG::cfg_Mission_EnableMissionAutoRestart, &CFG::cfg_Mission_MissionAutoRestartKey);
							const char* MissionMode[] = {
								"Last Active",
								"Specific",
							};
							ImGui::CustomCombo("Mission Auto-Restart Mode", &CFG::cfg_Mission_AutoRestartType, MissionMode, IM_ARRAYSIZE(MissionMode));
							ImGui::CustomSliderFloat("Mission Restart Delay", &CFG::cfg_Mission_MissionAutoRestartDelay, 0.25f, 30.0f);
							ImGui::Keybind("Mission Teleport Key", &CFG::cfg_Mission_MissionTeleportKey);
							ImGui::KeybindWithToggle("Enable Automatic Infiltration", &CFG::cfg_Mission_EnableInstantInfiltration, &CFG::cfg_Mission_InstantInfiltrationKey);
							ImGui::CustomCheckbox("Auto Mission Task Teleport", &CFG::cfg_Mission_EnableMissionTaskTeleport);
							ImGui::Keybind("Mission Task Teleport Key", &CFG::cfg_Mission_MissionTaskTeleportKey);
							ImGui::EndChild();
						}
						ImGui::SetCursorPos(ImVec2(207 + frame_size.x, 72));
						ImGui::BeginChild("MissionsList", ImVec2(frame_size.x, 532));
						{
							if (Cheat::Missions != nullptr)
							{
								if (Cheat::Missions->AvailableMissions.IsValid())
								{
									if (Cheat::Missions->AvailableMissions.Num() > 0)
										if (CFG::cfg_Mission_AutoRestartMissionIndex > Cheat::Missions->AvailableMissions.Num() - 1)
											CFG::cfg_Mission_AutoRestartMissionIndex = 0;
								}
								if (Cheat::Missions->AvailableMissions.IsValid())
								{
									char countbuffer[16];
									sprintf_s(countbuffer, "Missions: %i", Cheat::Missions->AvailableMissions.Num());
									ImGui::Text(countbuffer);
									for (int i = 0; i < Cheat::Missions->AvailableMissions.Num(); i++)
									{
										//TFD::AM1MissionActor* Actor = Cheat::Missions->AvailableMissions[i];
										if (Cheat::Missions->AvailableMissions[i])
										{
											char bufferlol[128];
											auto enumtostr = magic_enum::enum_name(Cheat::Missions->AvailableMissions[i]->MissionData->MissionSubType);
											sprintf_s(bufferlol, "Mission %s, Type: %s", Cheat::Missions->AvailableMissions[i]->MissionData->MissionDataRowName.ToString().c_str(), enumtostr.data());
											if (CFG::cfg_Mission_AutoRestartMissionIndex == i)
												ImGui::TextColored(main_color, bufferlol);
											else
												ImGui::Text(bufferlol);
											ImGui::PushID(i);
											if (ImGui::CustomButton("Set As Auto-Restart Target"))
											{
												CFG::cfg_Mission_AutoRestartMissionIndex = i;
											}
											ImGui::PopID();
											ImGui::PushID(i + Cheat::Missions->AvailableMissions.Num());
											if (ImGui::CustomButton("Start Mission"))
											{
												Cheat::Missions->ServerStartMission(Cheat::Missions->AvailableMissions[i], true);
											}
											ImGui::PopID();
											ImGui::PushID(i + (Cheat::Missions->AvailableMissions.Num() * 2));
											if (ImGui::CustomButton("Teleport To Mission"))
											{
												if (Cheat::LocalPlayerCharacter && Cheat::LocalPlayerCharacter->TeleportHandler && Cheat::Missions)
													Cheat::LocalPlayerCharacter->TeleportHandler->ServerMoveToTeleportToLocation(Cheat::Missions->AvailableMissions[i]->K2_GetActorLocation(), Cheat::Missions->AvailableMissions[i]->K2_GetActorRotation());
												Cheat::LocalPlayerCharacter->TeleportHandler->ServerFinishTeleportProcess();
											}
											ImGui::PopID();
										}
									}
								}
							}
							ImGui::EndChild();
						}
					}
					else if (iTabs == 5)
					{
						ImGui::BeginChild("LootFeatures", ImVec2(frame_size.x, 532));
						{
							ImGui::KeybindWithToggle("Enable Loot Vacuum", &CFG::cfg_Loot_EnableLootVacuum, &CFG::cfg_Loot_LootVacuumKey);
							ImGui::QuickTooltip("Isnt Really 'Visible' to Other Players, They Dont See Your Loot.");
							ImGui::CustomCheckbox("Vacuum Mission Items", &CFG::cfg_Loot_VacuumMissionItems);
							ImGui::QuickTooltip("Grabs Mission Items Like Vulgus Biometrics and Mission Weapons");
							ImGui::CustomSliderFloat("Loot Vacuum Range", &CFG::cfg_Loot_LootVacuumRange, 151, 80000);
							ImGui::QuickTooltip("This is still the Range used by Loot Vacuum AND Grab Loot.");
							const char* FilterTxt[] = {
								"None",
								"Tier 1",
								"Tier 2",
								"Tier 3",
							};
							ImGui::CustomCombo("Loot Filter Rarity", &CFG::cfg_Loot_ItemFilterLevel, FilterTxt, IM_ARRAYSIZE(FilterTxt));
							//ImGui::CustomCheckbox("Filter All Weapons", &CFG::cfg_Loot_FilterWeapons);
							ImGui::CustomCheckbox("Filter All Equipment", &CFG::cfg_Loot_FilterEquipment);
							ImGui::Keybind("Spawn Nearby Container Loot", &CFG::cfg_Loot_SpawnLootKey);
							ImGui::QuickTooltip("You need to have the Containers ESP on. Turning on Multiplier will mark containers in range.");
							ImGui::CustomSliderFloat("Container Loot Spawn Range", &CFG::cfg_Loot_ContainerDropRange, 151, 80000);
							ImGui::QuickTooltip("Enabling Multiplier will Create Spinning Spheres on Containers in this Range");
							ImGui::Keybind("Spawn Active Vault Loot", &CFG::cfg_Loot_SpawnVaultRewardKey);
							ImGui::QuickTooltip("THESE CAN BE SEEN BY OTHERS IN PUBLIC, DO ALONE OR WITH FRIENDS");
							ImGui::Keybind("Refresh Active Vault Timer", &CFG::cfg_Loot_RestartDecodingKey);
							ImGui::QuickTooltip("Pressing This Hotkey Will Reset a Vault Without Using Another Key");

							ImGui::EndChild();
						}
						ImGui::SetCursorPos(ImVec2(207 + frame_size.x, 72));
						ImGui::BeginChild("LootFeatures2", ImVec2(frame_size.x, 532));
						{
							ImGui::Keybind("Grab Loot", &CFG::cfg_Loot_GrabLootKey);
							ImGui::QuickTooltip("Acts as a Key-Press Vacuum, Only Grabbing Items and Teleporting Them When Pressed. Ignores Filters.");
							ImGui::Keybind("Grab Field Containers", &CFG::cfg_Loot_GrabContainersKey);
							ImGui::QuickTooltip("Teleports Field Containers to the Player");
							//ImGui::QuickTooltip("Vulgus Resource Crates/Munitions, Void Vessel Chests, Explosive Tanks, Etc. Grabs Within Vacuum Range.");
							ImGui::CustomCheckbox("Enable Loot Spawn Multiplier", &CFG::cfg_Loot_MultiplyDrops);
							ImGui::QuickTooltip("This will Also Mark any Containers Within Range.");
							ImGui::CustomSliderInt("Loot Spawn Multiplier", &CFG::cfg_Loot_SpawnCount, 1, 100);
							ImGui::QuickTooltip("Any is Safe as Long as You Dont do Billions in an Hour");
							ImGui::CustomSliderInt("Research Item Quantity", &CFG::cfg_Loot_ResearchQuantity, 1, 2000);
							if (ImGui::CustomButton("Start Bookmarked Research"))
							{
								Cheat::ResearchBookmarkedItems();
							}
							ImGui::QuickTooltip("Bookmark the item you want to research in game.\n"
								"You Need the Resources Still, Only Allows More Specific Amounts.");
							ImGui::CustomSliderFloat("Vacuum HP if below %%: ", &CFG::cfg_Loot_HPThreshold, 1, 100);
							ImGui::QuickTooltip("This is Ignored by 'Loot All' and 'Grab Loot'");
							ImGui::CustomSliderFloat("Vacuum MP if below %%: ", &CFG::cfg_Loot_MPThreshold, 1, 100);
							ImGui::QuickTooltip("This is Ignored by 'Loot All' and 'Grab Loot'");
							ImGui::EndChild();
						}
					}
					else if (iTabs == 6)
					{
						ImGui::BeginChild("TeleportFeatures", ImVec2(frame_size.x, 532));
						{
							ImGui::Text("Enabling Options Will Show More.");
							ImGui::Keybind("Teleport Forward Key", &CFG::cfg_Teleport_TeleportKey);
							ImGui::QuickTooltip("This is also the Key used for 'Teleport To Target'");
							char teleportLabel[64];
							sprintf_s(teleportLabel, "Teleport Distance (%.1fm)", CFG::cfg_Teleport_TeleportDistance / 100.0f);
							ImGui::CustomSliderFloat(teleportLabel, &CFG::cfg_Teleport_TeleportDistance, 100.0f, 50000.0f);
							ImGui::CustomCheckbox("Teleport Preview Sphere", &CFG::cfg_Teleport_ShowTeleportSphere1);
							ImGui::QuickTooltip("An Attempt to help 'Visualize' the Distance.\n"
								"If Enabled with Target Marking this Sphere will be moved to be the Inner Sphere");
							if (CFG::cfg_Teleport_ShowTeleportSphere1)
							{
								ImGui::CustomCheckbox("Preview Spheres Stay Grounded", &CFG::cfg_Teleport_GroundedSpheres);
								ImGui::QuickTooltip("When Enabled, a Slider will show Below, Letting you set a Static Height for them.");
								if (CFG::cfg_Teleport_GroundedSpheres)
								{
									ImGui::CustomSliderFloat("Grounded Sphere Offset", &CFG::cfg_Teleport_GroundOffset, -200.0f, 200.0f);
								}
								ImGui::CustomSliderFloat("Sphere 1 Size", &CFG::cfg_Teleport_TeleportSphere1Size, 5.0f, 200.0f);
								float sphere1Col[4] = { CFG::cfg_Teleport_TeleportSphere1ColorR, CFG::cfg_Teleport_TeleportSphere1ColorG, CFG::cfg_Teleport_TeleportSphere1ColorB, CFG::cfg_Teleport_TeleportSphere1ColorA };
								if (ImGui::RGBColorEdit4("Sphere 1 Color", sphere1Col))
								{
									CFG::cfg_Teleport_TeleportSphere1ColorR = sphere1Col[0];
									CFG::cfg_Teleport_TeleportSphere1ColorG = sphere1Col[1];
									CFG::cfg_Teleport_TeleportSphere1ColorB = sphere1Col[2];
									CFG::cfg_Teleport_TeleportSphere1ColorA = sphere1Col[3];
									CFG::SaveCFG();
								}
							}
							ImGui::CustomCheckbox("Use Light Beam Spheres", &CFG::cfg_Teleport_UseLightBeamSpheres);
							ImGui::QuickTooltip("Use 3D Light Beam Spheres instead of regular rotating spheres for teleport preview and targeting");
							ImGui::Keybind("Second Teleport Forward Key", &CFG::cfg_Teleport_TeleportKey2);
							char teleportLabel2[64];
							sprintf_s(teleportLabel2, "Second Teleport Distance (%.1fm)", CFG::cfg_Teleport_TeleportDistance2 / 100.0f);
							ImGui::CustomSliderFloat(teleportLabel2, &CFG::cfg_Teleport_TeleportDistance2, 1000.0f, 50000.0f);
							ImGui::CustomCheckbox("Show Teleport Preview Sphere 2", &CFG::cfg_Teleport_ShowTeleportSphere2);
							if (CFG::cfg_Teleport_ShowTeleportSphere2)
							{
								ImGui::CustomSliderFloat("Sphere 2 Size", &CFG::cfg_Teleport_TeleportSphere2Size, 10.0f, 200.0f);

								float sphere2Col[4] = { CFG::cfg_Teleport_TeleportSphere2ColorR, CFG::cfg_Teleport_TeleportSphere2ColorG, CFG::cfg_Teleport_TeleportSphere2ColorB, CFG::cfg_Teleport_TeleportSphere2ColorA };
								if (ImGui::RGBColorEdit4("Sphere 2 Color", sphere2Col))
								{
									CFG::cfg_Teleport_TeleportSphere2ColorR = sphere2Col[0];
									CFG::cfg_Teleport_TeleportSphere2ColorG = sphere2Col[1];
									CFG::cfg_Teleport_TeleportSphere2ColorB = sphere2Col[2];
									CFG::cfg_Teleport_TeleportSphere2ColorA = sphere2Col[3];
									CFG::SaveCFG();
								}
							}
							ImGui::CustomCheckbox("Enable Teleport to Target", &CFG::cfg_Teleport_EnableTeleportToTarget);
							ImGui::QuickTooltip("Uses a Field of View Like Aimbot to Find an Enemy to Teleport to\n"
								"This Also Allows You to Target a Save Point on Screen to Teleport to");
							if (CFG::cfg_Teleport_EnableTeleportToTarget)
							{
								ImGui::CustomCheckbox("Teleport Target Marking", &CFG::cfg_Teleport_TeleportTargetMarking);
								ImGui::QuickTooltip("Makes a Larger, Spinning Sphere on its Target");
								if (CFG::cfg_Teleport_EnableTeleportToTarget && CFG::cfg_Teleport_TeleportTargetMarking)
								{
									char offsetLabel[64];
									sprintf_s(offsetLabel, "Target Offset Distance (%.1fm)", CFG::cfg_Teleport_TargetOffset / 100.0f);
									ImGui::CustomSliderFloat(offsetLabel, &CFG::cfg_Teleport_TargetOffset, 0.0f, 1000.0f);
									float targetCol[4] = { CFG::cfg_Teleport_TeleportTargetColorR, CFG::cfg_Teleport_TeleportTargetColorG, CFG::cfg_Teleport_TeleportTargetColorB, CFG::cfg_Teleport_TeleportTargetColorA };
									if (ImGui::RGBColorEdit4("Target Sphere Color ", targetCol))
									{
										CFG::cfg_Teleport_TeleportTargetColorR = targetCol[0];
										CFG::cfg_Teleport_TeleportTargetColorG = targetCol[1];
										CFG::cfg_Teleport_TeleportTargetColorB = targetCol[2];
										CFG::cfg_Teleport_TeleportTargetColorA = targetCol[3];
										CFG::SaveCFG();
									}
								}
								ImGui::CustomSliderFloat("Teleport FOV", &CFG::cfg_Teleport_FOV, 10.0f, 500.0f);
								ImGui::QuickTooltip("The Field of View Teleport to Target checks for a Target");
								ImGui::CustomCheckbox("Draw Teleport FOV Circle", &CFG::cfg_Teleport_DrawTeleportFOVCircle);
								if (CFG::cfg_Teleport_DrawTeleportFOVCircle)
								{
									float fovCol[4] = { CFG::cfg_Teleport_FOVColorR, CFG::cfg_Teleport_FOVColorG, CFG::cfg_Teleport_FOVColorB, 1.0f };
									if (ImGui::RGBColorEdit4("Teleport FoV Circle Color", fovCol))
									{
										CFG::cfg_Teleport_FOVColorR = fovCol[0];
										CFG::cfg_Teleport_FOVColorG = fovCol[1];
										CFG::cfg_Teleport_FOVColorB = fovCol[2];
										CFG::SaveCFG();
									}
								}
							}
							ImGui::EndChild();
						}
						ImGui::SetCursorPos(ImVec2(207 + frame_size.x, 72));
						ImGui::BeginChild("SavePointFeatures", ImVec2(frame_size.x, 532));
						{
							ImGui::CustomCheckbox("Enable Save Point Overlay", &CFG::cfg_SavePoint_EnableSavePointOverlay);
							if (CFG::cfg_SavePoint_EnableSavePointOverlay)
							{
								ImGui::Keybind("Save Current Point Key", &CFG::cfg_SavePoint_SaveCurrentPointKey);
								if (ImGui::CustomButton("Save Current Point"))
								{
									SavePointCFG::SaveCurrentPoint();
								}
								ImGui::Keybind("Teleport To Highlighted Save Point Key", &CFG::cfg_SavePoint_SelectedSaveTeleportKey);
								if (ImGui::CustomButton("Teleport To Selected Save Point"))
								{
									SavePointCFG::TeleportToSavePoint();
								}
								ImGui::CustomCheckbox("Render Save Points", &CFG::cfg_SavePoint_RenderSavePoints);
								ImGui::QuickTooltip("When Enabled, Teleport To Target Will Work With Save Points");
								if (CFG::cfg_SavePoint_RenderSavePoints)
								{
									float sphereColor[4] = { CFG::cfg_SavePoint_SphereColorR, CFG::cfg_SavePoint_SphereColorG, CFG::cfg_SavePoint_SphereColorB, CFG::cfg_SavePoint_SphereColorA };
									if (ImGui::RGBColorEdit4("Selected Sphere Color", sphereColor)) {
										CFG::cfg_SavePoint_SphereColorR = sphereColor[0];
										CFG::cfg_SavePoint_SphereColorG = sphereColor[1];
										CFG::cfg_SavePoint_SphereColorB = sphereColor[2];
										CFG::cfg_SavePoint_SphereColorA = sphereColor[3];
									}
									float inactiveSphereColor[4] = { CFG::cfg_SavePoint_InactiveSphereColorR, CFG::cfg_SavePoint_InactiveSphereColorG, CFG::cfg_SavePoint_InactiveSphereColorB, CFG::cfg_SavePoint_InactiveSphereColorA };
									if (ImGui::RGBColorEdit4("Inactive Sphere Color", inactiveSphereColor)) {
										CFG::cfg_SavePoint_InactiveSphereColorR = inactiveSphereColor[0];
										CFG::cfg_SavePoint_InactiveSphereColorG = inactiveSphereColor[1];
										CFG::cfg_SavePoint_InactiveSphereColorB = inactiveSphereColor[2];
										CFG::cfg_SavePoint_InactiveSphereColorA = inactiveSphereColor[3];
									}
								}
								static char nameBuffer[64] = "";
								static bool bufferNeedsUpdate = true;
								if (CFG::cfg_SavePoint_CustomName.empty())
								{
									strcpy_s(nameBuffer, "");
									bufferNeedsUpdate = false;
								}
								else
								{
									strcpy_s(nameBuffer, CFG::cfg_SavePoint_CustomName.c_str());
								}
								ImGui::Text("Next Point Name:");
								ImGui::PushStyleColor(ImGuiCol_FrameBg, ImU32(second_color));
								ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImU32(GetColorWithAlpha(second_color, 0.8f)));
								ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImU32(GetColorWithAlpha(second_color, 0.9f)));
								ImGui::PushStyleColor(ImGuiCol_Text, ImU32(text_color[1]));
								ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
								ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
								if (ImGui::InputText("##SavePointName", nameBuffer, sizeof(nameBuffer)))
								{
									CFG::cfg_SavePoint_CustomName = std::string(nameBuffer);
									CFG::SaveCFG();
								}

								ImGui::PopStyleVar(2);
								ImGui::PopStyleColor(4);

								if (ImGui::CustomButton("Save Current Position"))
								{
									SavePointCFG::SaveCurrentPoint();
								}
							}
							ImGui::EndChild();
						}
					}
					else if (iTabs == 7)
					{
						ImGui::BeginChild("Customization", ImVec2(frame_size.x, 532));
						{
							if (ImGui::CustomButton("'Unlock' All Customization Items"))
							{
								Cheat::TryAddAllItems = true;
							}
							ImGui::CustomCheckbox("Enable Customization Bypass", &CFG::cfg_Customize_EnableCustomizationBypass);
							ImGui::CustomCheckbox("Enable Customization Auto-Equip", &CFG::cfg_Customize_EnableAutoApplyCustomization);
							static bool UpdateSlotTxt = true;
							static std::vector<std::string> SlotTxt = {};
							static const char* SlotTxtArray[10] = {};
							if (UpdateSlotTxt)
							{
								SlotTxt.clear();
								SlotTxt.push_back(std::format("Slot 1: {}", CFG::cfg_Customize_SaveSlots[0].CharacterName));
								SlotTxt.push_back(std::format("Slot 2: {}", CFG::cfg_Customize_SaveSlots[1].CharacterName));
								SlotTxt.push_back(std::format("Slot 3: {}", CFG::cfg_Customize_SaveSlots[2].CharacterName));
								SlotTxt.push_back(std::format("Slot 4: {}", CFG::cfg_Customize_SaveSlots[3].CharacterName));
								SlotTxt.push_back(std::format("Slot 5: {}", CFG::cfg_Customize_SaveSlots[4].CharacterName));
								SlotTxt.push_back(std::format("Slot 6: {}", CFG::cfg_Customize_SaveSlots[5].CharacterName));
								SlotTxt.push_back(std::format("Slot 7: {}", CFG::cfg_Customize_SaveSlots[6].CharacterName));
								SlotTxt.push_back(std::format("Slot 8: {}", CFG::cfg_Customize_SaveSlots[7].CharacterName));
								SlotTxt.push_back(std::format("Slot 9: {}", CFG::cfg_Customize_SaveSlots[8].CharacterName));
								SlotTxt.push_back(std::format("Slot 10: {}", CFG::cfg_Customize_SaveSlots[9].CharacterName));

								SlotTxtArray[0] = SlotTxt[0].c_str();
								SlotTxtArray[1] = SlotTxt[1].c_str();
								SlotTxtArray[2] = SlotTxt[2].c_str();
								SlotTxtArray[3] = SlotTxt[3].c_str();
								SlotTxtArray[4] = SlotTxt[4].c_str();
								SlotTxtArray[5] = SlotTxt[5].c_str();
								SlotTxtArray[6] = SlotTxt[6].c_str();
								SlotTxtArray[7] = SlotTxt[7].c_str();
								SlotTxtArray[8] = SlotTxt[8].c_str();
								SlotTxtArray[9] = SlotTxt[9].c_str();
								UpdateSlotTxt = false;
							}
							static int CustomizeSaveSlot = 0;
							ImGui::CustomCombo("Save Slot", &CustomizeSaveSlot, SlotTxtArray, IM_ARRAYSIZE(SlotTxtArray));
							if (ImGui::CustomButton("Save to Current Slot"))
							{
								// 282000003 - 282100000 = Head		// Slot 0
								// 282100001 - 282200000 = Body		// Slot 1
								// 282400001 - 282500000 = Back		// Slot 2
								// 282500001 - 282600000 = Front	// Slot 3
								// 282600002 - 282700000 = Makeup	// Slot 5
								// 283000001 - 283000090 = Character Paint
								// 283100001 - 283100060 = Hair Paint
								// 284100001 - 284100029 = Spawn	// Slot 4
								// 282009001 - 282109000 = Default Head
								// 282109001 - 282109110 = Default Body
								CFG::cfg_Customize_SaveSlots[CustomizeSaveSlot].CharacterID = Cheat::LocalPlayerCharacter->CharacterId.ID;
								static TFD::FString CharacterName;
								CharacterName = *TFD::native_GetCharacterName(Cheat::LocalPlayerCharacter, &CharacterName);
								std::string fmtName = CharacterName.ToString();
								fmtName = fmtName.substr(fmtName.find_last_of("_") + 1);
								CFG::cfg_Customize_SaveSlots[CustomizeSaveSlot].CharacterName = fmtName;
								CFG::cfg_Customize_SaveSlots[CustomizeSaveSlot].Head = Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[0].SkinTid.ID;
								CFG::cfg_Customize_SaveSlots[CustomizeSaveSlot].Body = Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[1].SkinTid.ID;
								CFG::cfg_Customize_SaveSlots[CustomizeSaveSlot].Back = Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[2].SkinTid.ID;
								CFG::cfg_Customize_SaveSlots[CustomizeSaveSlot].Chest = Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[3].SkinTid.ID;
								CFG::cfg_Customize_SaveSlots[CustomizeSaveSlot].Spawn = Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[4].SkinTid.ID;
								CFG::cfg_Customize_SaveSlots[CustomizeSaveSlot].Makeup = Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[5].SkinTid.ID;
								UpdateSlotTxt = true;
							}
							if (ImGui::CustomButton("Load from Current Slot"))
							{
								Cheat::TryEquipCustomization = true;
							}
							ImGui::EndChild();
						}
					}
					else if (iTabs == 8)
					{
						ImGui::BeginChild("MiscFeatures", ImVec2(frame_size.x, 532));
						{
							ImGui::Keybind("Timescale Toggle Key", &CFG::cfg_Extra_TimeScaleKey);
							ImGui::QuickTooltip("Increases the Clients Speed, Too High and You'll Experience Rubber Banding");
							ImGui::Keybind("Timescale Hold Key", &CFG::cfg_Extra_TimeScaleHoldKey);
							ImGui::QuickTooltip("Better for Quickly Emptying the Loot Pickup Queue/Sounds");
							ImGui::CustomSliderFloat("Timescale", &CFG::cfg_Extra_TimeScale, 0.1f, 10.0f);
							//ImGui::KeybindWithToggle("Enable Freecam", &CFG::cfg_Extra_EnableFreeCam, &CFG::cfg_Extra_FreeCamKey);
							ImGui::Keybind("Force Spawn Vehicle Key", &CFG::cfg_Extra_ForceSpawnVehicleKey);
							/*ImGui::QuickTooltip("THIS CAN BE DONE IN ALBION/BOSS FIGHTS/MISIONS, DONT USE IN PUBLIC DUH");
							ImGui::CustomCheckbox("Enable Vehicle Stat Override", &CFG::cfg_Extra_EnableVehicleMods);
							ImGui::QuickTooltip("Very Easy to be Reported for, but I have Drove well past any Speed Hack Checks and have been fine.");
							ImGui::CustomSliderFloat("Vehicle Acceleration", &CFG::cfg_Extra_MaxAcceleration, 1800.0f, 50000.0f);
							ImGui::CustomSliderFloat("Vehicle Speed", &CFG::cfg_Extra_MaxSpeed, 1700.0f, 50000.0f);
							ImGui::CustomSliderFloat("Vehicle Turn Speed", &CFG::cfg_Extra_MaxTurnSpeed, 190.0f, 50000.0f);*/
							if (ImGui::CustomCheckbox("Enable FullBright Render Mode", &CFG::cfg_Extra_EnableFullBright))
							{
								Cheat::ModifyViewMode = true;
							}
							if (ImGui::CustomButton("Enable UE Console (F1 Key)"))
							{
								Cheat::CreateConsole();
							}
							ImGui::EndChild();
							ImGui::SetCursorPos(ImVec2(207 + frame_size.x, 72));
							ImGui::BeginChild("Telekinesis", ImVec2(frame_size.x, 532));
							{
								ImGui::CustomCheckbox("Enable Telekinesis", &CFG::cfg_Telekinesis_Enable);
								ImGui::QuickTooltip("Physgun-like (Garry's Mod / Halo Forge, System to grab and move Interactable objects");

								if (CFG::cfg_Telekinesis_Enable)
								{
									ImGui::Keybind("Telekinesis Grab Key", &CFG::cfg_Telekinesis_GrabKey);
									ImGui::QuickTooltip("Hold this key to grab and move the targeted object, Use CTRL+WASD to Rotate grabbed objects.\n"
										"Need to hold CTRL to rotate it as well as push away and pull back, Q Pushes, E Pulls.");
									ImGui::CustomSliderFloat("Telekinesis FOV", &CFG::cfg_Telekinesis_FOV, 5.0f, 50.0f);
									ImGui::QuickTooltip("Field of view for targeting objects (smaller = more precise)");
									ImGui::CustomSliderFloat("Marker Size", &CFG::cfg_Telekinesis_MarkerSize, 5.0f, 50.0f);
									char distanceLabel[64];
									sprintf_s(distanceLabel, "Object Distance (%.1fm)", CFG::cfg_Telekinesis_Distance / 100.0f);
									if (ImGui::CustomSliderFloat(distanceLabel, &CFG::cfg_Telekinesis_Distance, 100.0f, 20000.0f))
									{
										CFG::SaveCFG();
									}
									ImGui::QuickTooltip("Distance for how far/close bjects are held from player . Hold CTRL + Q/E to Move away or bring toward to adjust while grabbing.");

									float markerCol[4] = { CFG::cfg_Telekinesis_MarkerColorR, CFG::cfg_Telekinesis_MarkerColorG, CFG::cfg_Telekinesis_MarkerColorB, CFG::cfg_Telekinesis_MarkerColorA };
									if (ImGui::RGBColorEdit4("Target Marker Color", markerCol))
									{
										CFG::cfg_Telekinesis_MarkerColorR = markerCol[0];
										CFG::cfg_Telekinesis_MarkerColorG = markerCol[1];
										CFG::cfg_Telekinesis_MarkerColorB = markerCol[2];
										CFG::cfg_Telekinesis_MarkerColorA = markerCol[3];
										CFG::SaveCFG();
									}

									float grabbedCol[4] = { CFG::cfg_Telekinesis_GrabbedColorR, CFG::cfg_Telekinesis_GrabbedColorG, CFG::cfg_Telekinesis_GrabbedColorB, CFG::cfg_Telekinesis_GrabbedColorA };
									if (ImGui::RGBColorEdit4("Grabbed Object Color", grabbedCol))
									{
										CFG::cfg_Telekinesis_GrabbedColorR = grabbedCol[0];
										CFG::cfg_Telekinesis_GrabbedColorG = grabbedCol[1];
										CFG::cfg_Telekinesis_GrabbedColorB = grabbedCol[2];
										CFG::cfg_Telekinesis_GrabbedColorA = grabbedCol[3];
										CFG::SaveCFG();
									}
								}
								ImGui::EndChild();
							}
						}
					}
					else if (iTabs == 9)
					{
						ImGui::BeginChild("Menu", ImVec2(frame_size.x, 532));
						{
							ImGui::Keybind("Menu Key", &CFG::cfg_ShowMenuKey);

							// RGB Gradient Toggle
							ImGui::CustomCheckbox("Enable RGB Gradient Cycling", &CFG::cfg_ESP_GradientCycling);
							ImGui::QuickTooltip("Makes the menu accent color and ESP elements cycle through RGB gradients");

							if (CFG::cfg_ESP_GradientCycling)
							{
								ImGui::CustomSliderFloat("Rainbow Flow Speed", &CFG::cfg_ESP_GradientSpeed, 0.01f, 2.0f);
								ImGui::QuickTooltip("Controls the speed of the flowing rainbow effect for menu accent and all ESP elements");
							}
							else
							{
								// Show normal accent color picker when RGB mode is disabled
								if (ImGui::CustomColorEdit("Accent Color", &main_color.Value.x, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip))
								{
									CFG::cfg_Colors_AccentR = main_color.Value.x;
									CFG::cfg_Colors_AccentG = main_color.Value.y;
									CFG::cfg_Colors_AccentB = main_color.Value.z;
									CFG::SaveCFG();
								}
							}
							/*
							ImGui::CustomColorEdit("Text 1 Color", &text_color[0].Value.x, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip);
							ImGui::CustomColorEdit("Text 2 Color", &text_color[1].Value.x, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip);
							ImGui::CustomColorEdit("Text 3 Color", &text_color[2].Value.x, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip);
							ImGui::CustomColorEdit("Foreground Color", &foreground_color.Value.x, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip);
							*/
							if (ImGui::CustomColorEdit("Widget BG Color", &background_color.Value.x, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip))
							{
								CFG::cfg_Colors_BackgroundR = background_color.Value.x;
								CFG::cfg_Colors_BackgroundG = background_color.Value.y;
								CFG::cfg_Colors_BackgroundB = background_color.Value.z;
								CFG::SaveCFG();
							}
							if (ImGui::CustomColorEdit("Window BG Color", &winbg_color.Value.x, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip))
							{
								CFG::cfg_Colors_WindowBGR = winbg_color.Value.x;
								CFG::cfg_Colors_WindowBGG = winbg_color.Value.y;
								CFG::cfg_Colors_WindowBGB = winbg_color.Value.z;
								CFG::SaveCFG();
							}

							ImGui::Separator();

							ImGui::PushStyleColor(ImGuiCol_Header, background_color.Value);
							ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(background_color.Value.x + 0.1f, background_color.Value.y + 0.1f, background_color.Value.z + 0.1f, background_color.Value.w));
							ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(background_color.Value.x + 0.2f, background_color.Value.y + 0.2f, background_color.Value.z + 0.2f, background_color.Value.w));
							if (ImGui::CollapsingHeader("Effect Colors"))
							{
								ImGui::PushStyleColor(ImGuiCol_FrameBg, child_color.Value);
								ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(child_color.Value.x + 0.1f, child_color.Value.y + 0.1f, child_color.Value.z + 0.1f, child_color.Value.w));
								ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(child_color.Value.x + 0.2f, child_color.Value.y + 0.2f, child_color.Value.z + 0.2f, child_color.Value.w));
								ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color.Value);
								ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(main_color.Value.x + 0.2f, main_color.Value.y + 0.2f, main_color.Value.z + 0.2f, main_color.Value.w));

								float particleCol[4] = { CFG::cfg_Colors_ParticleR, CFG::cfg_Colors_ParticleG, CFG::cfg_Colors_ParticleB, CFG::cfg_Colors_ParticleA };
								if (ImGui::RGBColorEdit4("Particle Color", particleCol))
								{
									CFG::cfg_Colors_ParticleR = particleCol[0];
									CFG::cfg_Colors_ParticleG = particleCol[1];
									CFG::cfg_Colors_ParticleB = particleCol[2];
									CFG::cfg_Colors_ParticleA = particleCol[3];
									Effects::g_EffectsManager.SetParticleColor(ImVec4(particleCol[0], particleCol[1], particleCol[2], particleCol[3]));
									CFG::SaveCFG();
								}

								float rippleCol[4] = { CFG::cfg_Colors_RippleR, CFG::cfg_Colors_RippleG, CFG::cfg_Colors_RippleB, CFG::cfg_Colors_RippleA };
								if (ImGui::RGBColorEdit4("Ripple Color", rippleCol))
								{
									CFG::cfg_Colors_RippleR = rippleCol[0];
									CFG::cfg_Colors_RippleG = rippleCol[1];
									CFG::cfg_Colors_RippleB = rippleCol[2];
									CFG::cfg_Colors_RippleA = rippleCol[3];
									Effects::g_EffectsManager.SetRippleColor(ImVec4(rippleCol[0], rippleCol[1], rippleCol[2], rippleCol[3]));
									CFG::SaveCFG();
								}

								float centerCol[4] = { CFG::cfg_Colors_GalaxyCenterR, CFG::cfg_Colors_GalaxyCenterG, CFG::cfg_Colors_GalaxyCenterB, CFG::cfg_Colors_GalaxyCenterA };
								if (ImGui::RGBColorEdit4("Galaxy Center", centerCol))
								{
									CFG::cfg_Colors_GalaxyCenterR = centerCol[0];
									CFG::cfg_Colors_GalaxyCenterG = centerCol[1];
									CFG::cfg_Colors_GalaxyCenterB = centerCol[2];
									CFG::cfg_Colors_GalaxyCenterA = centerCol[3];
									Effects::g_EffectsManager.SetGalaxyCenterColor(ImVec4(centerCol[0], centerCol[1], centerCol[2], centerCol[3]));
									CFG::SaveCFG();
								}

								float armsCol[4] = { CFG::cfg_Colors_GalaxyArmsR, CFG::cfg_Colors_GalaxyArmsG, CFG::cfg_Colors_GalaxyArmsB, CFG::cfg_Colors_GalaxyArmsA };
								if (ImGui::RGBColorEdit4("Galaxy Arms", armsCol))
								{
									CFG::cfg_Colors_GalaxyArmsR = armsCol[0];
									CFG::cfg_Colors_GalaxyArmsG = armsCol[1];
									CFG::cfg_Colors_GalaxyArmsB = armsCol[2];
									CFG::cfg_Colors_GalaxyArmsA = armsCol[3];
									Effects::g_EffectsManager.SetGalaxyArmsColor(ImVec4(armsCol[0], armsCol[1], armsCol[2], armsCol[3]));
									CFG::SaveCFG();
								}

								float sparklesCol[4] = { CFG::cfg_Colors_GalaxySparklesR, CFG::cfg_Colors_GalaxySparklesG, CFG::cfg_Colors_GalaxySparklesB, CFG::cfg_Colors_GalaxySparklesA };
								if (ImGui::RGBColorEdit4("Galaxy Sparkles", sparklesCol))
								{
									CFG::cfg_Colors_GalaxySparklesR = sparklesCol[0];
									CFG::cfg_Colors_GalaxySparklesG = sparklesCol[1];
									CFG::cfg_Colors_GalaxySparklesB = sparklesCol[2];
									CFG::cfg_Colors_GalaxySparklesA = sparklesCol[3];
									Effects::g_EffectsManager.SetGalaxySparklesColor(ImVec4(sparklesCol[0], sparklesCol[1], sparklesCol[2], sparklesCol[3]));
									CFG::SaveCFG();
								}
								ImGui::PopStyleColor(5);
							}
							ImGui::PopStyleColor(3);

							ImGui::Separator();

							ImGui::PushStyleColor(ImGuiCol_Header, background_color.Value);
							ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(background_color.Value.x + 0.1f, background_color.Value.y + 0.1f, background_color.Value.z + 0.1f, background_color.Value.w));
							ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(background_color.Value.x + 0.2f, background_color.Value.y + 0.2f, background_color.Value.z + 0.2f, background_color.Value.w));
							if (ImGui::CollapsingHeader("Background Effects"))
							{
								if (ImGui::CustomCheckbox("Enable Particles", &CFG::cfg_Effects_EnableParticles))
								{
									Effects::g_EffectsManager.EnableParticles(CFG::cfg_Effects_EnableParticles);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemClicked())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomCheckbox("Enable Ripples", &CFG::cfg_Effects_EnableRipples))
								{
									Effects::g_EffectsManager.EnableRipples(CFG::cfg_Effects_EnableRipples);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemClicked())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomCheckbox("Enable Glow", &CFG::cfg_Effects_EnableGlow))
								{
									Effects::g_EffectsManager.EnableGlow(CFG::cfg_Effects_EnableGlow);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemClicked())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomCheckbox("Enable Sparkles", &CFG::cfg_Effects_EnableSparkles))
								{
									Effects::g_EffectsManager.EnableSparkles(CFG::cfg_Effects_EnableSparkles);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemClicked())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderInt("Particle Count", &CFG::cfg_Effects_ParticleCount, 10, 100))
								{
									Effects::g_EffectsManager.SetParticleCount(CFG::cfg_Effects_ParticleCount);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Particle Speed", &CFG::cfg_Effects_ParticleSpeed, 50.0f, 500.0f))
								{
									Effects::g_EffectsManager.SetParticleSpeed(CFG::cfg_Effects_ParticleSpeed);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Particle Size", &CFG::cfg_Effects_ParticleSize, 10.0f, 50.0f))
								{
									Effects::g_EffectsManager.SetParticleSize(CFG::cfg_Effects_ParticleSize);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								const char* particleStyles[] = { "Square", "Circle", "Triangle", "Star", "Diamond" };
								if (ImGui::CustomCombo("Particle Style", &CFG::cfg_Effects_ParticleStyle, particleStyles, IM_ARRAYSIZE(particleStyles)))
								{
									Effects::g_EffectsManager.SetParticleStyle((Effects::ParticleStyle)CFG::cfg_Effects_ParticleStyle);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemClicked())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Glow Intensity", &CFG::cfg_Effects_GlowIntensity, 0.1f, 5.0f))
								{
									Effects::g_EffectsManager.SetGlowIntensity(CFG::cfg_Effects_GlowIntensity);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								ImGui::Separator();
								ImGui::Text("Ripple Effects");

								if (ImGui::CustomSliderFloat("Ripple Speed", &CFG::cfg_Effects_RippleSpeed, 0.1f, 5.0f))
								{
									Effects::g_EffectsManager.SetRippleSpeed(CFG::cfg_Effects_RippleSpeed);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Ripple Max Radius", &CFG::cfg_Effects_RippleMaxRadius, 25.0f, 300.0f))
								{
									Effects::g_EffectsManager.SetRippleMaxRadius(CFG::cfg_Effects_RippleMaxRadius);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), CFG::cfg_Effects_RippleMaxRadius);

								if (ImGui::CustomSliderFloat("Ripple Lifetime", &CFG::cfg_Effects_RippleLifetime, 0.1f, 5.0f))
								{
									Effects::g_EffectsManager.SetRippleLifetime(CFG::cfg_Effects_RippleLifetime);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderInt("Sparkle Count", &CFG::cfg_Effects_RippleSparkleCount, 0, 20))
								{
									Effects::g_EffectsManager.SetRippleSparkleCount(CFG::cfg_Effects_RippleSparkleCount);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Sparkle Brightness", &CFG::cfg_Effects_RippleSparkleBrightness, 0.1f, 5.0f))
								{
									Effects::g_EffectsManager.SetRippleSparkleBrightness(CFG::cfg_Effects_RippleSparkleBrightness);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Sparkle Size", &CFG::cfg_Effects_RippleSparkleSize, 0.1f, 5.0f))
								{
									Effects::g_EffectsManager.SetRippleSparkleSize(CFG::cfg_Effects_RippleSparkleSize);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Sparkle Rotation Speed", &CFG::cfg_Effects_RippleSparkleRotationSpeed, 0.1f, 5.0f))
								{
									Effects::g_EffectsManager.SetRippleSparkleRotationSpeed(CFG::cfg_Effects_RippleSparkleRotationSpeed);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								ImGui::Separator();
								ImGui::Text("Visual Polish");

								if (ImGui::CustomSliderFloat("Particle Pulse Intensity", &CFG::cfg_Effects_ParticlePulseIntensity, 0.0f, 2.0f))
								{
									Effects::g_EffectsManager.SetParticlePulseIntensity(CFG::cfg_Effects_ParticlePulseIntensity);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Particle Drift Strength", &CFG::cfg_Effects_ParticleDriftStrength, 0.0f, 10.0f))
								{
									Effects::g_EffectsManager.SetParticleDriftStrength(CFG::cfg_Effects_ParticleDriftStrength);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Galaxy Center Pulse", &CFG::cfg_Effects_GalaxyCenterPulseIntensity, 0.0f, 1.0f))
								{
									Effects::g_EffectsManager.SetGalaxyCenterPulseIntensity(CFG::cfg_Effects_GalaxyCenterPulseIntensity);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Galaxy Twinkle Speed", &CFG::cfg_Effects_GalaxyTwinkleSpeed, 0.1f, 10.0f))
								{
									Effects::g_EffectsManager.SetGalaxyTwinkleSpeed(CFG::cfg_Effects_GalaxyTwinkleSpeed);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomCheckbox("Enable Galaxy", &CFG::cfg_Effects_EnableGalaxy))
								{
									Effects::g_EffectsManager.SetEnableGalaxy(CFG::cfg_Effects_EnableGalaxy);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemClicked())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Galaxy Rotation Speed", &CFG::cfg_Effects_GalaxyRotationSpeed, 0.01f, 5.0f))
								{
									Effects::g_EffectsManager.SetGalaxyRotationSpeed(CFG::cfg_Effects_GalaxyRotationSpeed);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Galaxy Arm Count", &CFG::cfg_Effects_GalaxyArmCount, 1.0f, 15.0f))
								{
									Effects::g_EffectsManager.SetGalaxyArmCount(CFG::cfg_Effects_GalaxyArmCount);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Galaxy Radius", &CFG::cfg_Effects_GalaxyRadius, 50.0f, 500.0f))
								{
									Effects::g_EffectsManager.SetGalaxyRadius(CFG::cfg_Effects_GalaxyRadius);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderInt("Galaxy Sparkle Count", &CFG::cfg_Effects_GalaxySparkleCount, 0, 20))
								{
									Effects::g_EffectsManager.SetGalaxySparkleCount(CFG::cfg_Effects_GalaxySparkleCount);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Galaxy Sparkle Brightness", &CFG::cfg_Effects_GalaxySparkleBrightness, 0.01f, 5.0f))
								{
									Effects::g_EffectsManager.SetGalaxySparkleBrightness(CFG::cfg_Effects_GalaxySparkleBrightness);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Galaxy Sparkle Size", &CFG::cfg_Effects_GalaxySparkleSize, 0.1f, 5.0f))
								{
									Effects::g_EffectsManager.SetGalaxySparkleSize(CFG::cfg_Effects_GalaxySparkleSize);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Galaxy Position X", &CFG::cfg_Effects_GalaxyPositionX, -400.0f, 400.0f))
								{
									Effects::g_EffectsManager.SetGalaxyPosition(CFG::cfg_Effects_GalaxyPositionX, CFG::cfg_Effects_GalaxyPositionY);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Galaxy Position Y", &CFG::cfg_Effects_GalaxyPositionY, -400.0f, 400.0f))
								{
									Effects::g_EffectsManager.SetGalaxyPosition(CFG::cfg_Effects_GalaxyPositionX, CFG::cfg_Effects_GalaxyPositionY);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);
							}
							ImGui::PopStyleColor(3);

							ImGui::Separator();

							ImGui::PushStyleColor(ImGuiCol_Header, background_color.Value);
							ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(background_color.Value.x + 0.1f, background_color.Value.y + 0.1f, background_color.Value.z + 0.1f, background_color.Value.w));
							ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(background_color.Value.x + 0.2f, background_color.Value.y + 0.2f, background_color.Value.z + 0.2f, background_color.Value.w));
							if (ImGui::CollapsingHeader("Cursor Settings"))
							{
								if (ImGui::CustomCheckbox("Use Custom Cursor", &CFG::cfg_Cursor_UseCustomCursor))
								{
									Effects::g_EffectsManager.EnableCustomCursor(CFG::cfg_Cursor_UseCustomCursor);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemClicked())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								const char* cursorStyles[] = { "Arrow", "Hand", "Starry Arrow", "Starry Hand", "Galaxy", "Galaxy Star" };
								if (ImGui::CustomCombo("Cursor Style", &CFG::cfg_Cursor_Style, cursorStyles, IM_ARRAYSIZE(cursorStyles)))
								{
									Effects::g_EffectsManager.SetCursorStyle((Effects::CursorStyle)CFG::cfg_Cursor_Style);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemClicked())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Cursor Size", &CFG::cfg_Cursor_Size, 0.25f, 5.0f))
								{
									Effects::g_EffectsManager.SetCursorSize(CFG::cfg_Cursor_Size);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Cursor Glow", &CFG::cfg_Cursor_GlowIntensity, 0.0f, 3.0f))
								{
									Effects::g_EffectsManager.SetCursorGlowIntensity(CFG::cfg_Cursor_GlowIntensity);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomCheckbox("Enable Cursor Trail", &CFG::cfg_Cursor_EnableTrail))
								{
									Effects::g_EffectsManager.EnableCursorTrail(CFG::cfg_Cursor_EnableTrail);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemClicked())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Cursor Trail Length", &CFG::cfg_Cursor_TrailLength, 1.0f, 50.0f))
								{
									Effects::g_EffectsManager.SetCursorTrailLength(CFG::cfg_Cursor_TrailLength);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderInt("Cursor Sparkle Count", &CFG::cfg_Cursor_SparkleCount, 0, 16))
								{
									Effects::g_EffectsManager.SetCursorSparkleCount(CFG::cfg_Cursor_SparkleCount);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								if (ImGui::CustomSliderFloat("Cursor Sparkle Size", &CFG::cfg_Cursor_SparkleSize, 0.01f, 8.0f))
								{
									Effects::g_EffectsManager.SetCursorSparkleSize(CFG::cfg_Cursor_SparkleSize);
									CFG::SaveCFG();
								}
								if (ImGui::IsItemActive())
									Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

								ImGui::PushStyleColor(ImGuiCol_FrameBg, child_color.Value);
								ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(child_color.Value.x + 0.1f, child_color.Value.y + 0.1f, child_color.Value.z + 0.1f, child_color.Value.w));
								ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(child_color.Value.x + 0.2f, child_color.Value.y + 0.2f, child_color.Value.z + 0.2f, child_color.Value.w));
								ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color.Value);
								ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(main_color.Value.x + 0.2f, main_color.Value.y + 0.2f, main_color.Value.z + 0.2f, main_color.Value.w));

								float cursorCol[4] = { CFG::cfg_Cursor_ColorR, CFG::cfg_Cursor_ColorG, CFG::cfg_Cursor_ColorB, CFG::cfg_Cursor_ColorA };
								if (ImGui::RGBColorEdit4("Cursor Color", cursorCol))
								{
									CFG::cfg_Cursor_ColorR = cursorCol[0];
									CFG::cfg_Cursor_ColorG = cursorCol[1];
									CFG::cfg_Cursor_ColorB = cursorCol[2];
									CFG::cfg_Cursor_ColorA = cursorCol[3];
									Effects::g_EffectsManager.SetCursorColor(ImVec4(cursorCol[0], cursorCol[1], cursorCol[2], cursorCol[3]));
									CFG::SaveCFG();
								}

								ImGui::PopStyleColor(5);

								if (CFG::cfg_Cursor_Style == 4 || CFG::cfg_Cursor_Style == 5) // Galaxy or Galaxy Star
								{
									ImGui::Separator();
									ImGui::Text("Cursor Galaxy Settings");

									ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(background_color.Value.x + 0.1f, background_color.Value.y + 0.1f, background_color.Value.z + 0.1f, background_color.Value.w));
									ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(background_color.Value.x + 0.15f, background_color.Value.y + 0.15f, background_color.Value.z + 0.15f, background_color.Value.w));
									ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(background_color.Value.x + 0.2f, background_color.Value.y + 0.2f, background_color.Value.z + 0.2f, background_color.Value.w));
									ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color.Value);
									ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(main_color.Value.x + 0.2f, main_color.Value.y + 0.2f, main_color.Value.z + 0.2f, main_color.Value.w));

									float galaxyCenterCol[4] = { CFG::cfg_Cursor_GalaxyCenterColorR, CFG::cfg_Cursor_GalaxyCenterColorG, CFG::cfg_Cursor_GalaxyCenterColorB, CFG::cfg_Cursor_GalaxyCenterColorA };
									if (ImGui::RGBColorEdit4("Cursor Galaxy Center Color", galaxyCenterCol))
									{
										CFG::cfg_Cursor_GalaxyCenterColorR = galaxyCenterCol[0];
										CFG::cfg_Cursor_GalaxyCenterColorG = galaxyCenterCol[1];
										CFG::cfg_Cursor_GalaxyCenterColorB = galaxyCenterCol[2];
										CFG::cfg_Cursor_GalaxyCenterColorA = galaxyCenterCol[3];
										Effects::g_EffectsManager.SetCursorGalaxyCenterColor(ImVec4(galaxyCenterCol[0], galaxyCenterCol[1], galaxyCenterCol[2], galaxyCenterCol[3]));
										CFG::SaveCFG();
									}

									float galaxyArmsCol[4] = { CFG::cfg_Cursor_GalaxyArmsColorR, CFG::cfg_Cursor_GalaxyArmsColorG, CFG::cfg_Cursor_GalaxyArmsColorB, CFG::cfg_Cursor_GalaxyArmsColorA };
									if (ImGui::RGBColorEdit4("Cursor Galaxy Arms Color", galaxyArmsCol))
									{
										CFG::cfg_Cursor_GalaxyArmsColorR = galaxyArmsCol[0];
										CFG::cfg_Cursor_GalaxyArmsColorG = galaxyArmsCol[1];
										CFG::cfg_Cursor_GalaxyArmsColorB = galaxyArmsCol[2];
										CFG::cfg_Cursor_GalaxyArmsColorA = galaxyArmsCol[3];
										Effects::g_EffectsManager.SetCursorGalaxyArmsColor(ImVec4(galaxyArmsCol[0], galaxyArmsCol[1], galaxyArmsCol[2], galaxyArmsCol[3]));
										CFG::SaveCFG();
									}

									if (ImGui::CustomCheckbox("Enable Cursor Galaxy Sparkles", &CFG::cfg_Cursor_EnableGalaxySparkles))
									{
										Effects::g_EffectsManager.SetCursorEnableGalaxySparkles(CFG::cfg_Cursor_EnableGalaxySparkles);
										CFG::SaveCFG();
									}
									if (ImGui::IsItemClicked())
										Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

									if (ImGui::CustomSliderFloat("Cursor Galaxy Arm Count", &CFG::cfg_Cursor_GalaxyArmCount, 1.0f, 16.0f, "%.0f"))
									{
										Effects::g_EffectsManager.SetCursorGalaxyArmCount(CFG::cfg_Cursor_GalaxyArmCount);
										CFG::SaveCFG();
									}
									if (ImGui::IsItemClicked())
										Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

									if (ImGui::CustomSliderFloat("Cursor Galaxy Galaxy Radius", &CFG::cfg_Cursor_GalaxyRadius, 10.0f, 100.0f, "%.0f"))
									{
										Effects::g_EffectsManager.SetCursorGalaxyRadius(CFG::cfg_Cursor_GalaxyRadius);
										CFG::SaveCFG();
									}
									if (ImGui::IsItemClicked())
										Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

									if (ImGui::CustomSliderFloat("Cursor Galaxy Rotation Speed", &CFG::cfg_Cursor_GalaxyRotationSpeed, 0.01f, 5.0f, "%.1f"))
									{
										Effects::g_EffectsManager.SetCursorGalaxyRotationSpeed(CFG::cfg_Cursor_GalaxyRotationSpeed);
										CFG::SaveCFG();
									}
									if (ImGui::IsItemClicked())
										Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

									if (ImGui::CustomSliderFloat("Cursor Galaxy Center Pulse", &CFG::cfg_Cursor_GalaxyCenterPulseIntensity, 0.0f, 2.0f, "%.2f"))
									{
										Effects::g_EffectsManager.SetCursorGalaxyCenterPulseIntensity(CFG::cfg_Cursor_GalaxyCenterPulseIntensity);
										CFG::SaveCFG();
									}
									if (ImGui::IsItemClicked())
										Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

									if (CFG::cfg_Cursor_EnableGalaxySparkles)
									{
										float sparkleCol[4] = { CFG::cfg_Cursor_GalaxySparklesColorR, CFG::cfg_Cursor_GalaxySparklesColorG, CFG::cfg_Cursor_GalaxySparklesColorB, CFG::cfg_Cursor_GalaxySparklesColorA };
										if (ImGui::RGBColorEdit4("Cursor Galaxy Sparkle Color", sparkleCol))
										{
											CFG::cfg_Cursor_GalaxySparklesColorR = sparkleCol[0];
											CFG::cfg_Cursor_GalaxySparklesColorG = sparkleCol[1];
											CFG::cfg_Cursor_GalaxySparklesColorB = sparkleCol[2];
											CFG::cfg_Cursor_GalaxySparklesColorA = sparkleCol[3];
											Effects::g_EffectsManager.SetCursorGalaxySparklesColor(ImVec4(sparkleCol[0], sparkleCol[1], sparkleCol[2], sparkleCol[3]));
											CFG::SaveCFG();
										}

										if (ImGui::CustomSliderInt("Cursor Galaxy Sparkle Count", &CFG::cfg_Cursor_GalaxySparkleCount, 1, 16))
										{
											Effects::g_EffectsManager.SetCursorGalaxySparkleCount(CFG::cfg_Cursor_GalaxySparkleCount);
											CFG::SaveCFG();
										}
										if (ImGui::IsItemClicked())
											Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

										if (ImGui::CustomSliderFloat("Cursor Galaxy Sparkle Brightness", &CFG::cfg_Cursor_GalaxySparkleBrightness, 0.01f, 5.0f, "%.1f"))
										{
											Effects::g_EffectsManager.SetCursorGalaxySparkleBrightness(CFG::cfg_Cursor_GalaxySparkleBrightness);
											CFG::SaveCFG();
										}
										if (ImGui::IsItemClicked())
											Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

										if (ImGui::CustomSliderFloat("Cursor Galaxy Sparkle Size", &CFG::cfg_Cursor_GalaxySparkleSize, 0.1f, 5.0f, "%.1f"))
										{
											Effects::g_EffectsManager.SetCursorGalaxySparkleSize(CFG::cfg_Cursor_GalaxySparkleSize);
											CFG::SaveCFG();
										}
										if (ImGui::IsItemClicked())
											Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);

										if (ImGui::CustomSliderFloat("Cursor Galaxy Twinkle Speed", &CFG::cfg_Cursor_GalaxyTwinkleSpeed, 0.1f, 5.0f, "%.1f"))
										{
											Effects::g_EffectsManager.SetCursorGalaxyTwinkleSpeed(CFG::cfg_Cursor_GalaxyTwinkleSpeed);
											CFG::SaveCFG();
										}
										if (ImGui::IsItemClicked())
											Effects::g_EffectsManager.CreateRipple(ImGui::GetMousePos(), 30.0f);
									}

									ImGui::PopStyleColor(5);
								}
							}
							ImGui::PopStyleColor(3);

							ImGui::Separator();

							const char* Language[] = {
								"EN",
								"ES",
								"CN",
								"RU",
							};
							ImGui::CustomCombo("Menu Language", &CFG::cfg_Language, Language, IM_ARRAYSIZE(Language));
							ImGui::QuickTooltip("Translations not implemented yet.");
							ImGui::EndChild();
							ImGui::SetCursorPos(ImVec2(207 + frame_size.x, 72));
							ImGui::BeginChild("MusicPlayer", ImVec2(frame_size.x, 532));
							{
								Browser::RenderMusicPlayer();
								ImGui::Separator();
								ImGui::Text("How To Add Songs?");
								ImGui::QuickTooltip("Make a Folder named 'Songs' where the Config File is. Put them in that.");
								ImGui::Text("Where is the Config File?");
								ImGui::QuickTooltip("-Steam-steamapps-common-The First Descendant-M1-Binaries-Win64");
								ImGui::Separator();
								ImGui::CustomCheckbox("Show Enabled Features", &CFG::cfg_Menu_ShowEnabledFeatures);
								ImGui::QuickTooltip("Show overlay listing all currently enabled features");
								ImGui::Separator();
								ImGui::Text("If you turn Tooltips off then ask for help.\n"
									"Talk to a psychologist for the disability. ");
								ImGui::CustomCheckbox("Enable Tooltips", &CFG::cfg_Menu_EnableTooltips);
								ImGui::QuickTooltip("Toggle tooltips on/off for all menu elements");
								ImGui::Separator();
								ImGui::EndChild();
							}
						}
					}
				}
		}
			ImGui::EndGroup();
			ImGui::End();
	}
		if (effectsInitialized)
		{
			ImGuiIO& io = ImGui::GetIO();
			if (io.DeltaTime > 0.0f && io.DeltaTime < 1.0f)
			{
				Effects::g_EffectsManager.UpdateCursor(io.DeltaTime);
			}
		}

		if (ShowMenu && CFG::cfg_Cursor_UseCustomCursor)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_None);

			ImDrawList* drawList = ImGui::GetForegroundDrawList();
			Effects::g_EffectsManager.RenderCustomCursor(drawList);
		}
		else if (ShowMenu)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
		}
}
}

