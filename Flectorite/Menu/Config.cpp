#include "Config.h"
#include <iostream>
#include <string>
#include <cstdio>

using std::cout;
using std::string;

namespace CFG
{

	CSimpleIniA ini;

	int			cfg_ShowMenuKey = ImGuiKey_Insert;
	bool		cfg_Menu_EnableTooltips = true;
	bool		cfg_Menu_ShowEnabledFeatures = false;



	int			cfg_VERSION = 2.0;
	int			cfg_Language = 0;
	float		cfg_BCDelay = 2500;

	// Friendly ESP
	bool		cfg_Friendly_Enable = false;
	bool		cfg_Friendly_DrawNames = false;
	bool		cfg_Friendly_DrawBoxes = false;
	bool		cfg_Friendly_Draw3DBoxes = false;
	bool        cfg_Friendly_DrawCircles = false;
	bool		cfg_Friendly_Draw3DSpheres = false;
	bool		cfg_Friendly_DrawLines = false;
	bool		cfg_Friendly_ShowDistance = false;

	// Enemy ESP
	bool		cfg_Enemy_Enable = false;
	bool		cfg_Enemy_DrawNames = false;
	bool		cfg_Enemy_DrawBoxes = false;
	bool		cfg_Enemy_Draw3DBoxes = false;
	bool        cfg_Enemy_DrawCircles = false;
	bool		cfg_Enemy_Draw3DSpheres = false;
	bool		cfg_Enemy_DrawLines = false;
	bool		cfg_Enemy_ShowDistance = false;

	// Collectible ESP
	bool		cfg_Actors_EnableCollectibleESP = false;
	float 		cfg_Actors_ShapeSize = 5.0f;
	float 		cfg_Actors_ShapeDistanceScale = 100.0f;

	// RGB Gradient ESP System
	bool		cfg_ESP_EnableGradients = false;
	float		cfg_ESP_GradientSpeed = 1.0f;
	bool		cfg_ESP_GradientCycling = false;

	// Loot ESP
	bool		cfg_Loot_Enable = false;
	bool		cfg_Loot_EnableItemESP = false;
	bool		cfg_Loot_MissionItemESP = false;
	int			cfg_Loot_ItemFilterLevel = 0;
	bool		cfg_Loot_FilterEquipment = false;
	bool		cfg_Loot_DrawItemBoxes = false;
	bool		cfg_Loot_DrawItem3DBoxes = false;
	bool        cfg_Loot_DrawItemCircles = false;
	bool		cfg_Loot_DrawItem3DSpheres = false;
	bool		cfg_Loot_DrawItemNames = false;
	bool		cfg_Loot_ShowDistance = false;
	bool		cfg_Loot_DrawVaults = false;
	bool        cfg_Loot_DrawAxionResources = false;
	bool		cfg_Loot_DrawSupplyResources = false;
	bool		cfg_Loot_DrawVoidVesselBox = false;

	// Loot Vacuum & Mechanics
	bool		cfg_Loot_EnableLootVacuum = false;
	bool		cfg_Loot_VacuumMissionItems = false;
	float		cfg_Loot_LootVacuumRange = 1000.0f;
	int			cfg_Loot_LootVacuumKey = ImGuiKey_T;
	int			cfg_Loot_GrabLootKey = ImGuiKey_H;
	int			cfg_Loot_GrabContainersKey = ImGuiKey_None;
	int			cfg_Loot_SpawnLootKey = ImGuiKey_F5;
	int			cfg_Loot_SpawnVaultRewardKey = ImGuiKey_F6;
	int         cfg_Loot_RestartDecodingKey = ImGuiKey_F7;
	float 		cfg_Loot_ContainerDropRange = 500.0f;
	bool		cfg_Loot_MultiplyDrops = false;
	int			cfg_Loot_SpawnCount = 1;
	float		cfg_Loot_HPThreshold = 50.0f;
	float		cfg_Loot_MPThreshold = 50.0f;
	int			cfg_Loot_ResearchQuantity = 1;
	bool		cfg_Loot_DebugActorsInRange = false;

	bool		cfg_Aim_EnableAimbot = false;
	int			cfg_Aim_AimbotMode = 0;
	int			cfg_Aim_AimbotHoldKey = ImGuiKey_MouseRight;
	int			cfg_Aim_AimbotToggleKey = ImGuiKey_None;
	bool		cfg_Aim_SilentAim = false;
	int			cfg_Aim_SilentAimKey = ImGuiKey_None;
	bool		cfg_Aim_AimbotUseController = false;
	float		cfg_Aim_AimbotFOV = 140.0f;
	bool		cfg_Aim_DrawFOVCircle = false;
	float		cfg_Aim_AimbotSpeed = 36.0f;
	bool		cfg_Aim_AimbotLineOfSight = false;
	bool		cfg_Aim_NoRecoilAndSpread = false;
	bool		cfg_Aim_NoReload = false;
	bool		cfg_Aim_RapidFire = false;
	float		cfg_Aim_FireRate = 1.0f;

	int			cfg_Teleport_TeleportKey = ImGuiKey_None;
	int			cfg_Teleport_TeleportKey2 = ImGuiKey_None;
	float		cfg_Teleport_TeleportDistance = 500.0f;
	float		cfg_Teleport_TeleportDistance2 = 1000.0f;
	bool		cfg_Teleport_ShowTeleportSphere1 = false;
	bool		cfg_Teleport_GroundedSpheres = false;
	float		cfg_Teleport_GroundOffset = 50.0f;
	float		cfg_Teleport_TeleportSphere1Size = 20.0f;
	float		cfg_Teleport_TeleportSphere1ColorR = 0.9f;
	float		cfg_Teleport_TeleportSphere1ColorG = 0.5f;
	float		cfg_Teleport_TeleportSphere1ColorB = 0.9f;
	float		cfg_Teleport_TeleportSphere1ColorA = 1.0f;
	bool		cfg_Teleport_ShowTeleportSphere2 = false;
	float		cfg_Teleport_TeleportSphere2Size = 50.0f;
	float		cfg_Teleport_TeleportSphere2ColorR = 0.6f;
	float		cfg_Teleport_TeleportSphere2ColorG = 0.3f;
	float		cfg_Teleport_TeleportSphere2ColorB = 0.8f;
	float		cfg_Teleport_TeleportSphere2ColorA = 1.0f;
	bool		cfg_Teleport_UseLightBeamSpheres = false;
	bool		cfg_Teleport_EnableTeleportToTarget = false;
	bool		cfg_Teleport_EnableWaypointTeleport = false;
	bool		cfg_Teleport_TeleportTargetMarking = false;
	float		cfg_Teleport_TeleportTargetColorR = 1.0f;
	float		cfg_Teleport_TeleportTargetColorG = 0.0f;
	float		cfg_Teleport_TeleportTargetColorB = 0.0f;
	float		cfg_Teleport_TeleportTargetColorA = 1.0f;
	float		cfg_Teleport_TargetOffset = 0.0f;
	float		cfg_Teleport_FOV = 100.0f;
	bool		cfg_Teleport_DrawTeleportFOVCircle = false;
	float		cfg_Teleport_FOVColorR = 0.8f;
	float		cfg_Teleport_FOVColorG = 0.4f;
	float		cfg_Teleport_FOVColorB = 0.9f;
	float		cfg_Teleport_FOVColorA = 1.0f;

	// Telekinesis System
	bool		cfg_Telekinesis_Enable = false;
	int			cfg_Telekinesis_GrabKey = ImGuiKey_R;
	float		cfg_Telekinesis_FOV = 10.0f;
	float		cfg_Telekinesis_MarkerColorR = 1.0f;
	float		cfg_Telekinesis_MarkerColorG = 0.0f;
	float		cfg_Telekinesis_MarkerColorB = 0.0f;
	float		cfg_Telekinesis_MarkerColorA = 1.0f;
	float		cfg_Telekinesis_GrabbedColorR = 0.0f;
	float		cfg_Telekinesis_GrabbedColorG = 1.0f;
	float		cfg_Telekinesis_GrabbedColorB = 0.0f;
	float		cfg_Telekinesis_GrabbedColorA = 1.0f;
	float		cfg_Telekinesis_MarkerSize = 15.0f;
	float		cfg_Telekinesis_Distance = 500.0f;

	string		cfg_SavePoint_CustomName = "";
	bool		cfg_SavePoint_EnableSavePointOverlay = false;
	bool		cfg_SavePoint_RenderSavePoints = false;
	float		cfg_SavePoint_SphereColorR = 0.0f;
	float		cfg_SavePoint_SphereColorG = 1.0f;
	float		cfg_SavePoint_SphereColorB = 1.0f;
	float		cfg_SavePoint_SphereColorA = 1.0f;
	float		cfg_SavePoint_InactiveSphereColorR = 0.2f;
	float		cfg_SavePoint_InactiveSphereColorG = 0.6f;
	float		cfg_SavePoint_InactiveSphereColorB = 1.0f;
	float		cfg_SavePoint_InactiveSphereColorA = 0.8f;
	int			cfg_SavePoint_SaveCurrentPointKey = ImGuiKey_None;
	int			cfg_SavePoint_SelectedSaveTeleportKey = ImGuiKey_None;

	bool		cfg_Abilities_EnableModifyGrapple = false;
	float 		cfg_Abilities_GrappleRange = 10000.0f;
	bool		cfg_Abilities_EnableAutomaticResupply = false;
	int         cfg_Abilities_AutomaticResupplyToggleKey = ImGuiKey_None;;
	bool	    cfg_Abilities_AutomaticResupplyTimed = false;
	float		cfg_Abilities_AutomaticResupplyTimeDelay = 1.0f;
	bool	    cfg_Abilities_AutomaticResupplyHealth = false;
	float		cfg_Abilities_AutomaticResupplyHealthPercent = 75.0f;

	float		cfg_Extra_TimeScale = 2.0f;
	int			cfg_Extra_TimeScaleKey = ImGuiKey_None;
	int			cfg_Extra_TimeScaleHoldKey = ImGuiKey_None;
	bool		cfg_Extra_EnableFreeCam = false;
	int			cfg_Extra_FreeCamKey = ImGuiKey_None;
	int			cfg_Extra_ForceSpawnVehicleKey = ImGuiKey_None;
	bool		cfg_Extra_EnableVehicleMods = false;
	float		cfg_Extra_MaxAcceleration = 1800.0f;
	float		cfg_Extra_MaxSpeed = 1700.0f;
	float		cfg_Extra_MaxTurnSpeed = 190.0f;
	bool		cfg_Extra_EnableUEConsole = false;
	bool		cfg_Extra_EnableFullBright = false;

	bool		cfg_Mission_EnableMissionESP = false;
	bool		cfg_Mission_EnableMissionAutoRestart = false;
	int			cfg_Mission_AutoRestartType = 0;
	int			cfg_Mission_AutoRestartMissionIndex = 0;
	float		cfg_Mission_MissionAutoRestartDelay = 5.0f;
	int			cfg_Mission_MissionAutoRestartKey = ImGuiKey_None;
	int			cfg_Mission_MissionTeleportKey = ImGuiKey_None;
	bool		cfg_Mission_EnableInstantInfiltration = false;
	int			cfg_Mission_InstantInfiltrationKey = ImGuiKey_None;
	bool		cfg_Mission_EnableMissionTaskTeleport = false;
	int			cfg_Mission_MissionTaskTeleportKey = ImGuiKey_None;

	bool		cfg_Hotswap_EnableOverlay = false;
	int			cfg_Hotswap_PresetSelectKey = ImGuiKey_None;
	int 		cfg_ResearchQuantity = 1;
	int 		CurrentStyleIndex = 0;

	bool		cfg_Debug_ShowConsole = false;

	bool		cfg_Customize_EnableCustomizationBypass = false;
	bool		cfg_Customize_EnableAutoApplyCustomization = false;

	float		cfg_Colors_AccentR = 0.902f, cfg_Colors_AccentG = 0.525f, cfg_Colors_AccentB = 0.878f;
	float		cfg_Colors_BackgroundR = 0.1f, cfg_Colors_BackgroundG = 0.1f, cfg_Colors_BackgroundB = 0.1f;
	float		cfg_Colors_WindowBGR = 0.059f, cfg_Colors_WindowBGG = 0.063f, cfg_Colors_WindowBGB = 0.071f;
	float		cfg_Colors_ParticleR = 0.902f, cfg_Colors_ParticleG = 0.525f, cfg_Colors_ParticleB = 0.878f, cfg_Colors_ParticleA = 1.0f;
	float		cfg_Colors_RippleR = 0.900f, cfg_Colors_RippleG = 0.500f, cfg_Colors_RippleB = 0.800f, cfg_Colors_RippleA = 1.0f;
	float		cfg_Colors_GalaxyCenterR = 0.100f, cfg_Colors_GalaxyCenterG = 1.0f, cfg_Colors_GalaxyCenterB = 1.0f, cfg_Colors_GalaxyCenterA = 1.0f;
	float		cfg_Colors_GalaxyArmsR = 1.0f, cfg_Colors_GalaxyArmsG = 0.100f, cfg_Colors_GalaxyArmsB = 0.500f, cfg_Colors_GalaxyArmsA = 1.0f;
	float		cfg_Colors_GalaxySparklesR = 1.0f, cfg_Colors_GalaxySparklesG = 1.0f, cfg_Colors_GalaxySparklesB = 1.0f, cfg_Colors_GalaxySparklesA = 1.0f;

	bool		cfg_Effects_EnableParticles = false;
	bool		cfg_Effects_EnableRipples = false;
	bool		cfg_Effects_EnableGlow = false;
	bool		cfg_Effects_EnableSparkles = false;
	bool		cfg_Effects_EnableGalaxy = false;
	int			cfg_Effects_ParticleCount = 50;
	float		cfg_Effects_ParticleSpeed = 125.0f;
	float		cfg_Effects_ParticleSize = 10.0f;
	int			cfg_Effects_ParticleStyle = 0;
	float		cfg_Effects_GlowIntensity = 1.0f;
	float		cfg_Effects_RippleSpeed = 1.0f;
	float		cfg_Effects_RippleMaxRadius = 100.0f;
	float		cfg_Effects_RippleLifetime = 2.0f;
	int			cfg_Effects_RippleSparkleCount = 15;
	float		cfg_Effects_RippleSparkleBrightness = 1.5f;
	float		cfg_Effects_RippleSparkleSize = 2.0f;
	float		cfg_Effects_RippleSparkleRotationSpeed = 2.0f;
	float		cfg_Effects_GalaxyRotationSpeed = 0.2f;
	float		cfg_Effects_GalaxyArmCount = 5.0f;
	float		cfg_Effects_GalaxyRadius = 350.0f;
	int			cfg_Effects_GalaxySparkleCount = 10;
	float		cfg_Effects_GalaxySparkleBrightness = 1.5f;
	float		cfg_Effects_GalaxySparkleSize = 2.5f;
	float		cfg_Effects_ParticlePulseIntensity = 0.25f;
	float		cfg_Effects_ParticleDriftStrength = 2.0f;
	float		cfg_Effects_GalaxyCenterPulseIntensity = 0.25f;
	float		cfg_Effects_GalaxyTwinkleSpeed = 2.5f;
	float		cfg_Effects_GalaxyPositionX = 0.0f;
	float		cfg_Effects_GalaxyPositionY = 0.0f;

	bool		cfg_Cursor_UseCustomCursor = false;
	int			cfg_Cursor_Style = 0;
	float		cfg_Cursor_Size = 1.0f;
	float		cfg_Cursor_GlowIntensity = 0.5f;
	bool		cfg_Cursor_EnableTrail = false;
	float		cfg_Cursor_TrailLength = 10.0f;
	int			cfg_Cursor_SparkleCount = 5;
	float		cfg_Cursor_SparkleSize = 2.5f;
	float		cfg_Cursor_ColorR = 0.9f, cfg_Cursor_ColorG = 0.5f, cfg_Cursor_ColorB = 0.9f, cfg_Cursor_ColorA = 1.0f;
	float		cfg_Cursor_GalaxyArmCount = 3.0f;
	float		cfg_Cursor_GalaxyRadius = 25.0f;
	float		cfg_Cursor_GalaxyRotationSpeed = 0.5f;
	float		cfg_Cursor_GalaxyCenterPulseIntensity = 0.1f;
	float		cfg_Cursor_GalaxyTwinkleSpeed = 2.5f;
	bool		cfg_Cursor_EnableGalaxySparkles = true;
	int			cfg_Cursor_GalaxySparkleCount = 8;
	float		cfg_Cursor_GalaxySparkleBrightness = 0.8f;
	float		cfg_Cursor_GalaxySparkleSize = 2.0f;
	float		cfg_Cursor_GalaxyCenterColorR = 0.9f, cfg_Cursor_GalaxyCenterColorG = 0.5f, cfg_Cursor_GalaxyCenterColorB = 0.9f, cfg_Cursor_GalaxyCenterColorA = 1.0f;
	float		cfg_Cursor_GalaxyArmsColorR = 0.7f, cfg_Cursor_GalaxyArmsColorG = 0.3f, cfg_Cursor_GalaxyArmsColorB = 0.8f, cfg_Cursor_GalaxyArmsColorA = 1.0f;
	float		cfg_Cursor_GalaxySparklesColorR = 1.0f, cfg_Cursor_GalaxySparklesColorG = 0.8f, cfg_Cursor_GalaxySparklesColorB = 1.0f, cfg_Cursor_GalaxySparklesColorA = 1.0f;

	// 0 Head, 1 Body, 2 Back, 3 Chest, 4 Spawn, 5 Makeup

	CustomizeSlots cfg_Customize_SaveSlots[10];

	//std::vector<int> cfg_Hotswap_Presets = { -1, -1, -1, -1, -1, -1 };
	//bool		cfg_Aim_NoSpread = false;
	//bool		cfg_Aim_NoRecoil = false;
	//bool		cfg_Mission_EnableAutoInstantInfiltration = false;

	void LoadCFG()
	{
		ini.SetUnicode();
		if (ini.LoadFile("cfg.ini") < 0)
		{
			printf("No cfg, making new\n");
			try
			{
				SaveCFG();
			}
			catch (...)
			{
				printf("cfg fucked up\n");
			}
		}
		else
		{
			int VERSION = -1;
			LoadValue("CFG", "Version", VERSION);
			if (VERSION != cfg_VERSION)
			{
				ini.Delete("Loot", NULL, true);
				ini.Delete("Aimbot", NULL, true);
				ini.Delete("Abilities", NULL, true);
				ini.Delete("Extra", NULL, true);
				ini.Delete("Missions", NULL, true);
				ini.Delete("Hotswap", NULL, true);
				ini.Reset();
				SaveCFG();
				LoadCFG();
				return;
			}
			LoadValue("Menu", "Kebyind", cfg_ShowMenuKey);
			LoadValue("Menu", "Language", cfg_Language);
			LoadValue("Menu", "EnableTooltips", cfg_Menu_EnableTooltips);
			LoadValue("Menu", "ShowEnabledFeatures", cfg_Menu_ShowEnabledFeatures);
			LoadValue("Menu", "BlackCipherDelay", cfg_BCDelay);

			LoadValue("FriendlyESP", "Enable", cfg_Friendly_Enable);
			LoadValue("FriendlyESP", "DrawNames", cfg_Friendly_DrawNames);
			LoadValue("FriendlyESP", "DrawBoxes", cfg_Friendly_DrawBoxes);
			LoadValue("FriendlyESP", "Draw3DBoxes", cfg_Friendly_Draw3DBoxes);
			LoadValue("FriendlyESP", "DrawCircles", cfg_Friendly_DrawCircles);
			LoadValue("FriendlyESP", "Draw3DSpheres", cfg_Friendly_Draw3DSpheres);
			LoadValue("FriendlyESP", "DrawLines", cfg_Friendly_DrawLines);
			LoadValue("FriendlyESP", "ShowDistance", cfg_Friendly_ShowDistance);

			LoadValue("EnemyESP", "Enable", cfg_Enemy_Enable);
			LoadValue("EnemyESP", "DrawNames", cfg_Enemy_DrawNames);
			LoadValue("EnemyESP", "DrawBoxes", cfg_Enemy_DrawBoxes);
			LoadValue("EnemyESP", "Draw3DBoxes", cfg_Enemy_Draw3DBoxes);
			LoadValue("EnemyESP", "DrawCircles", cfg_Enemy_DrawCircles);
			LoadValue("EnemyESP", "Draw3DSpheres", cfg_Enemy_Draw3DSpheres);
			LoadValue("EnemyESP", "DrawLines", cfg_Enemy_DrawLines);
			LoadValue("EnemyESP", "ShowDistance", cfg_Enemy_ShowDistance);

			LoadValue("Actors", "EnableCollectibleESP", cfg_Actors_EnableCollectibleESP);
			LoadValue("Actors", "ShapeSize", cfg_Actors_ShapeSize);
			LoadValue("Actors", "ShapeDistanceScale", cfg_Actors_ShapeDistanceScale);

			LoadValue("Loot", "Enable", cfg_Loot_Enable);
			LoadValue("Loot", "EnableItemESP", cfg_Loot_EnableItemESP);
			LoadValue("Loot", "MissionItemESP", cfg_Loot_MissionItemESP);
			LoadValue("Loot", "ItemFilterLevel", cfg_Loot_ItemFilterLevel);
			//LoadValue("Loot", "FilterWeapons", cfg_Loot_FilterWeapons);
			LoadValue("Loot", "FilterEquipment", cfg_Loot_FilterEquipment);
			LoadValue("Loot", "DrawItemBoxes", cfg_Loot_DrawItemBoxes);
			LoadValue("Loot", "DrawItem3DBoxes", cfg_Loot_DrawItem3DBoxes);
			LoadValue("Loot", "DrawItemCircles", cfg_Loot_DrawItemCircles);
			LoadValue("Loot", "DrawItem3DSpheres", cfg_Loot_DrawItem3DSpheres);
			LoadValue("Loot", "DrawItemNames", cfg_Loot_DrawItemNames);
			LoadValue("Loot", "ShowDistance", cfg_Loot_ShowDistance);
			LoadValue("Loot", "DrawVaults", cfg_Loot_DrawVaults);
			LoadValue("Loot", "DrawAxionResources", cfg_Loot_DrawAxionResources);
			LoadValue("Loot", "DrawSupplyResources", cfg_Loot_DrawSupplyResources);
			LoadValue("Loot", "DrawVoidVesselBox", cfg_Loot_DrawVoidVesselBox);
			LoadValue("Loot", "EnableLootVacuum", cfg_Loot_EnableLootVacuum);
			LoadValue("Loot", "VacuumMissionItems", cfg_Loot_VacuumMissionItems);
			LoadValue("Loot", "LootVacuumRange", cfg_Loot_LootVacuumRange);
			LoadValue("Loot", "LootVacuumKey", cfg_Loot_LootVacuumKey);
			LoadValue("Loot", "GrabLootKey", cfg_Loot_GrabLootKey);
			LoadValue("Loot", "GrabContainersKey", cfg_Loot_GrabContainersKey);
			LoadValue("Loot", "SpawnLootKey", cfg_Loot_SpawnLootKey);
			LoadValue("Loot", "SpawnVaultRewardKey", cfg_Loot_SpawnVaultRewardKey);
			LoadValue("Loot", "RestartDecodingKey", cfg_Loot_RestartDecodingKey);
			LoadValue("Loot", "HPThreshold", cfg_Loot_HPThreshold);
			LoadValue("Loot", "MPThreshold", cfg_Loot_MPThreshold);
			LoadValue("Loot", "ContainerDropRange", cfg_Loot_ContainerDropRange);
			LoadValue("Loot", "MultiplyDrops", cfg_Loot_MultiplyDrops);
			LoadValue("Loot", "SpawnCount", cfg_Loot_SpawnCount);
			LoadValue("Loot", "ResearchQuantity", cfg_Loot_ResearchQuantity);
			LoadValue("Loot", "DebugActorsInRange", cfg_Loot_DebugActorsInRange);

			LoadValue("Aimbot", "EnableAimbot", cfg_Aim_EnableAimbot);
			LoadValue("Aimbot", "AimbotMode", cfg_Aim_AimbotMode);
			LoadValue("Aimbot", "EnableSilentAim", cfg_Aim_SilentAim);
			LoadValue("Aimbot", "SilentAimKey", cfg_Aim_SilentAimKey);
			LoadValue("Aimbot", "AimbotHoldKey", cfg_Aim_AimbotHoldKey);
			LoadValue("Aimbot", "AimbotToggleKey", cfg_Aim_AimbotToggleKey);
			LoadValue("Aimbot", "AimbotUseController", cfg_Aim_AimbotUseController);
			LoadValue("Aimbot", "AimbotDistance", cfg_Aim_AimbotFOV);
			LoadValue("Aimbot", "AimbotSpeed", cfg_Aim_AimbotSpeed);
			LoadValue("Aimbot", "AimbotFOV", cfg_Aim_DrawFOVCircle);
			LoadValue("Aimbot", "AimbotLineOfSight", cfg_Aim_AimbotLineOfSight);
			LoadValue("Aimbot", "NoReload", cfg_Aim_NoReload);
			LoadValue("Aimbot", "NoRecoilAndSpread", cfg_Aim_NoRecoilAndSpread);
			LoadValue("Aimbot", "RapidFire", cfg_Aim_RapidFire);
			LoadValue("Aimbot", "RapidFireRate", cfg_Aim_FireRate);

			LoadValue("Teleport", "TeleportForwardKey", cfg_Teleport_TeleportKey);
			LoadValue("Teleport", "TeleportForwardDistance", cfg_Teleport_TeleportDistance);
			LoadValue("Teleport", "TeleportForwardKey2", cfg_Teleport_TeleportKey2);
			LoadValue("Teleport", "TeleportForwardDistance2", cfg_Teleport_TeleportDistance2);
			LoadValue("Teleport", "ShowTeleportSphere1", cfg_Teleport_ShowTeleportSphere1);
			LoadValue("Teleport", "GroundedSpheres", cfg_Teleport_GroundedSpheres);
			LoadValue("Teleport", "GroundOffset", cfg_Teleport_GroundOffset);
			LoadValue("Teleport", "TeleportSphere1Size", cfg_Teleport_TeleportSphere1Size);
			LoadValue("Teleport", "TeleportSphere1ColorR", cfg_Teleport_TeleportSphere1ColorR);
			LoadValue("Teleport", "TeleportSphere1ColorG", cfg_Teleport_TeleportSphere1ColorG);
			LoadValue("Teleport", "TeleportSphere1ColorB", cfg_Teleport_TeleportSphere1ColorB);
			LoadValue("Teleport", "TeleportSphere1ColorA", cfg_Teleport_TeleportSphere1ColorA);
			LoadValue("Teleport", "ShowTeleportSphere2", cfg_Teleport_ShowTeleportSphere2);
			LoadValue("Teleport", "TeleportSphere2Size", cfg_Teleport_TeleportSphere2Size);
			LoadValue("Teleport", "TeleportSphere2ColorR", cfg_Teleport_TeleportSphere2ColorR);
			LoadValue("Teleport", "TeleportSphere2ColorG", cfg_Teleport_TeleportSphere2ColorG);
			LoadValue("Teleport", "TeleportSphere2ColorB", cfg_Teleport_TeleportSphere2ColorB);
			LoadValue("Teleport", "TeleportSphere2ColorA", cfg_Teleport_TeleportSphere2ColorA);
			LoadValue("Teleport", "UseLightBeamSpheres", cfg_Teleport_UseLightBeamSpheres);
			LoadValue("Teleport", "EnableTeleportToTarget", cfg_Teleport_EnableTeleportToTarget);
			LoadValue("Teleport", "EnableWaypointTeleport", cfg_Teleport_EnableWaypointTeleport);
			LoadValue("Teleport", "TeleportTargetMarking", cfg_Teleport_TeleportTargetMarking);
			LoadValue("Teleport", "TeleportTargetColorR", cfg_Teleport_TeleportTargetColorR);
			LoadValue("Teleport", "TeleportTargetColorG", cfg_Teleport_TeleportTargetColorG);
			LoadValue("Teleport", "TeleportTargetColorB", cfg_Teleport_TeleportTargetColorB);
			LoadValue("Teleport", "TeleportTargetColorA", cfg_Teleport_TeleportTargetColorA);
			LoadValue("Teleport", "TargetOffset", cfg_Teleport_TargetOffset);
			LoadValue("Teleport", "TeleportFOV", cfg_Teleport_FOV);
			LoadValue("Teleport", "DrawTeleportFOVCircle", cfg_Teleport_DrawTeleportFOVCircle);
			LoadValue("Teleport", "TeleportFOVColorR", cfg_Teleport_FOVColorR);
			LoadValue("Teleport", "TeleportFOVColorG", cfg_Teleport_FOVColorG);
			LoadValue("Teleport", "TeleportFOVColorB", cfg_Teleport_FOVColorB);
			LoadValue("Teleport", "TeleportFOVColorA", cfg_Teleport_FOVColorA);

			LoadValue("Telekinesis", "Enable", cfg_Telekinesis_Enable);
			LoadValue("Telekinesis", "GrabKey", cfg_Telekinesis_GrabKey);
			LoadValue("Telekinesis", "FOV", cfg_Telekinesis_FOV);
			LoadValue("Telekinesis", "MarkerColorR", cfg_Telekinesis_MarkerColorR);
			LoadValue("Telekinesis", "MarkerColorG", cfg_Telekinesis_MarkerColorG);
			LoadValue("Telekinesis", "MarkerColorB", cfg_Telekinesis_MarkerColorB);
			LoadValue("Telekinesis", "MarkerColorA", cfg_Telekinesis_MarkerColorA);
			LoadValue("Telekinesis", "GrabbedColorR", cfg_Telekinesis_GrabbedColorR);
			LoadValue("Telekinesis", "GrabbedColorG", cfg_Telekinesis_GrabbedColorG);
			LoadValue("Telekinesis", "GrabbedColorB", cfg_Telekinesis_GrabbedColorB);
			LoadValue("Telekinesis", "GrabbedColorA", cfg_Telekinesis_GrabbedColorA);
			LoadValue("Telekinesis", "MarkerSize", cfg_Telekinesis_MarkerSize);
			LoadValue("Telekinesis", "Distance", cfg_Telekinesis_Distance);

			LoadValue("ESP", "EnableGradients", cfg_ESP_EnableGradients);
			LoadValue("ESP", "GradientSpeed", cfg_ESP_GradientSpeed);
			LoadValue("ESP", "GradientCycling", cfg_ESP_GradientCycling);

			LoadValue("SavePoint", "EnableOverlay", cfg_SavePoint_EnableSavePointOverlay);
			LoadValue("SavePoint", "RenderSavePoints", cfg_SavePoint_RenderSavePoints);
			LoadValue("SavePoint", "SphereColorR", cfg_SavePoint_SphereColorR);
			LoadValue("SavePoint", "SphereColorG", cfg_SavePoint_SphereColorG);
			LoadValue("SavePoint", "SphereColorB", cfg_SavePoint_SphereColorB);
			LoadValue("SavePoint", "SphereColorA", cfg_SavePoint_SphereColorA);
			LoadValue("SavePoint", "InactiveSphereColorR", cfg_SavePoint_InactiveSphereColorR);
			LoadValue("SavePoint", "InactiveSphereColorG", cfg_SavePoint_InactiveSphereColorG);
			LoadValue("SavePoint", "InactiveSphereColorB", cfg_SavePoint_InactiveSphereColorB);
			LoadValue("SavePoint", "InactiveSphereColorA", cfg_SavePoint_InactiveSphereColorA);
			LoadValue("SavePoint", "SavePointKey", cfg_SavePoint_SaveCurrentPointKey);
			LoadValue("SavePoint", "TeleportKey", cfg_SavePoint_SelectedSaveTeleportKey);
			LoadValue("SavePoint", "CustomName", cfg_SavePoint_CustomName);

			LoadValue("Abilities", "EnableModifyGrapple", cfg_Abilities_EnableModifyGrapple);
			LoadValue("Abilities", "GrappleRange", cfg_Abilities_GrappleRange);
			LoadValue("Abilities", "EnableAutoResupply", cfg_Abilities_EnableAutomaticResupply);
			LoadValue("Abilities", "AutoResupplyToggleKey", cfg_Abilities_AutomaticResupplyToggleKey);
			LoadValue("Abilities", "AutoResupplyTimed", cfg_Abilities_AutomaticResupplyTimed);
			LoadValue("Abilities", "AutoResupplyTimeDelay", cfg_Abilities_AutomaticResupplyTimeDelay);
			LoadValue("Abilities", "AutoResupplyHealth", cfg_Abilities_AutomaticResupplyHealth);
			LoadValue("Abilities", "AutoResupplyHealthPercent", cfg_Abilities_AutomaticResupplyHealthPercent);

			LoadValue("Extra", "TimeScale", cfg_Extra_TimeScale);
			LoadValue("Extra", "TimeScaleKey", cfg_Extra_TimeScaleKey);
			LoadValue("Extra", "TimeScaleHoldKey", cfg_Extra_TimeScaleHoldKey);
			LoadValue("Extra", "EnableFreeCam", cfg_Extra_EnableFreeCam);
			LoadValue("Extra", "ForceSpawnVehicleKey", cfg_Extra_ForceSpawnVehicleKey);
			//LoadValue("Extra", "EnableVehicleStatsMod", cfg_Extra_EnableVehicleMods);
			//LoadValue("Extra", "VehicleMaxAccel", cfg_Extra_MaxAcceleration);
			//LoadValue("Extra", "VehicleMaxSpeed", cfg_Extra_MaxSpeed);
			//LoadValue("Extra", "VehicleMaxTurnSpeed", cfg_Extra_MaxTurnSpeed);
			LoadValue("Extra", "EnableFullBright", cfg_Extra_EnableFullBright);
			

			LoadValue("Missions", "EnableMissionESP", cfg_Mission_EnableMissionESP);
			LoadValue("Missions", "MissionAutoRestartType", cfg_Mission_AutoRestartType);
			LoadValue("Missions", "MissionAutoRestartDelay", cfg_Mission_MissionAutoRestartDelay);
			LoadValue("Missions", "MissionAutoRestartKey", cfg_Mission_MissionAutoRestartKey);
			LoadValue("Missions", "MissionTeleportKey", cfg_Mission_MissionTeleportKey);
			LoadValue("Missions", "MissionInstaInfilKey", cfg_Mission_InstantInfiltrationKey);
			LoadValue("Missions", "MissionInstaInfil", cfg_Mission_EnableInstantInfiltration);
			LoadValue("Missions", "EnableMissionTaskTeleport", cfg_Mission_EnableMissionTaskTeleport);
			LoadValue("Missions", "MissionTaskTeleportKey", cfg_Mission_MissionTaskTeleportKey);

			LoadValue("Hotswap", "EnableOverlay", cfg_Hotswap_EnableOverlay);
			LoadValue("Hotswap", "PresetSelectKey", cfg_Hotswap_PresetSelectKey);

			LoadValue("Customize", "EnableCustomizationBypass", cfg_Customize_EnableCustomizationBypass);
			LoadValue("Customize", "EnableAutoApplyCustomization", cfg_Customize_EnableAutoApplyCustomization);

			for (int i = 0; i < 10; i++)
			{
				LoadValue("Customize", std::format("SaveSlot{}_CharacterID", i).c_str(), cfg_Customize_SaveSlots[i].CharacterID);
				if (cfg_Customize_SaveSlots[i].CharacterID != -1)
				{
					LoadValue("Customize", std::format("SaveSlot{}_Name", i).c_str(), cfg_Customize_SaveSlots[i].CharacterName);
					LoadValue("Customize", std::format("SaveSlot{}_Head", i).c_str(), cfg_Customize_SaveSlots[i].Head);
					LoadValue("Customize", std::format("SaveSlot{}_Body", i).c_str(), cfg_Customize_SaveSlots[i].Body);
					LoadValue("Customize", std::format("SaveSlot{}_Back", i).c_str(), cfg_Customize_SaveSlots[i].Back);
					LoadValue("Customize", std::format("SaveSlot{}_Chest", i).c_str(), cfg_Customize_SaveSlots[i].Chest);
					LoadValue("Customize", std::format("SaveSlot{}_Spawn", i).c_str(), cfg_Customize_SaveSlots[i].Spawn);
					LoadValue("Customize", std::format("SaveSlot{}_Makeup", i).c_str(), cfg_Customize_SaveSlots[i].Makeup);
				}
			}
			//LoadValue("Customize", "SaveSlot0_CharacterID", cfg_Customize_SaveSlots[0].CharacterID);
			//if (cfg_Customize_SaveSlots[0].CharacterID != -1)
			//{
			//	LoadValue("Customize", "SaveSlot0_Name", cfg_Customize_SaveSlots[0].CharacterName);
			//	LoadValue("Customize", "SaveSlot0_Head", cfg_Customize_SaveSlots[0].Head);
			//	LoadValue("Customize", "SaveSlot0_Body", cfg_Customize_SaveSlots[0].Body);
			//	LoadValue("Customize", "SaveSlot0_Back", cfg_Customize_SaveSlots[0].Back);
			//	LoadValue("Customize", "SaveSlot0_Chest", cfg_Customize_SaveSlots[0].Chest);
			//	LoadValue("Customize", "SaveSlot0_Spawn", cfg_Customize_SaveSlots[0].Spawn);
			//	LoadValue("Customize", "SaveSlot0_Makeup", cfg_Customize_SaveSlots[0].Makeup);
			//}
			/*LoadValue("Customize", "SaveSlot1_CharacterID", cfg_Customize_SaveSlots[1].CharacterID);
			if (cfg_Customize_SaveSlots[1].CharacterID != -1)
			{
				LoadValue("Customize", "SaveSlot1_Name", cfg_Customize_SaveSlots[1].CharacterName);
				LoadValue("Customize", "SaveSlot1_Head", cfg_Customize_SaveSlots[1].Head);
				LoadValue("Customize", "SaveSlot1_Body", cfg_Customize_SaveSlots[1].Body);
				LoadValue("Customize", "SaveSlot1_Back", cfg_Customize_SaveSlots[1].Back);
				LoadValue("Customize", "SaveSlot1_Chest", cfg_Customize_SaveSlots[1].Chest);
				LoadValue("Customize", "SaveSlot1_Spawn", cfg_Customize_SaveSlots[1].Spawn);
				LoadValue("Customize", "SaveSlot1_Makeup", cfg_Customize_SaveSlots[1].Makeup);
			}
			LoadValue("Customize", "SaveSlot2_CharacterID", cfg_Customize_SaveSlots[2].CharacterID);
			if (cfg_Customize_SaveSlots[2].CharacterID != -1)
			{
				LoadValue("Customize", "SaveSlot2_Name", cfg_Customize_SaveSlots[2].CharacterName);
				LoadValue("Customize", "SaveSlot2_Head", cfg_Customize_SaveSlots[2].Head);
				LoadValue("Customize", "SaveSlot2_Body", cfg_Customize_SaveSlots[2].Body);
				LoadValue("Customize", "SaveSlot2_Back", cfg_Customize_SaveSlots[2].Back);
				LoadValue("Customize", "SaveSlot2_Chest", cfg_Customize_SaveSlots[2].Chest);
				LoadValue("Customize", "SaveSlot2_Spawn", cfg_Customize_SaveSlots[2].Spawn);
				LoadValue("Customize", "SaveSlot2_Makeup", cfg_Customize_SaveSlots[2].Makeup);
			}*/

			LoadValue("Colors", "AccentR", cfg_Colors_AccentR);
			LoadValue("Colors", "AccentG", cfg_Colors_AccentG);
			LoadValue("Colors", "AccentB", cfg_Colors_AccentB);
			LoadValue("Colors", "BackgroundR", cfg_Colors_BackgroundR);
			LoadValue("Colors", "BackgroundG", cfg_Colors_BackgroundG);
			LoadValue("Colors", "BackgroundB", cfg_Colors_BackgroundB);
			LoadValue("Colors", "WindowBGR", cfg_Colors_WindowBGR);
			LoadValue("Colors", "WindowBGG", cfg_Colors_WindowBGG);
			LoadValue("Colors", "WindowBGB", cfg_Colors_WindowBGB);
			LoadValue("Colors", "ParticleR", cfg_Colors_ParticleR);
			LoadValue("Colors", "ParticleG", cfg_Colors_ParticleG);
			LoadValue("Colors", "ParticleB", cfg_Colors_ParticleB);
			LoadValue("Colors", "ParticleA", cfg_Colors_ParticleA);
			LoadValue("Colors", "RippleR", cfg_Colors_RippleR);
			LoadValue("Colors", "RippleG", cfg_Colors_RippleG);
			LoadValue("Colors", "RippleB", cfg_Colors_RippleB);
			LoadValue("Colors", "RippleA", cfg_Colors_RippleA);
			LoadValue("Colors", "GalaxyCenterR", cfg_Colors_GalaxyCenterR);
			LoadValue("Colors", "GalaxyCenterG", cfg_Colors_GalaxyCenterG);
			LoadValue("Colors", "GalaxyCenterB", cfg_Colors_GalaxyCenterB);
			LoadValue("Colors", "GalaxyCenterA", cfg_Colors_GalaxyCenterA);
			LoadValue("Colors", "GalaxyArmsR", cfg_Colors_GalaxyArmsR);
			LoadValue("Colors", "GalaxyArmsG", cfg_Colors_GalaxyArmsG);
			LoadValue("Colors", "GalaxyArmsB", cfg_Colors_GalaxyArmsB);
			LoadValue("Colors", "GalaxyArmsA", cfg_Colors_GalaxyArmsA);
			LoadValue("Colors", "GalaxySparklesR", cfg_Colors_GalaxySparklesR);
			LoadValue("Colors", "GalaxySparklesG", cfg_Colors_GalaxySparklesG);
			LoadValue("Colors", "GalaxySparklesB", cfg_Colors_GalaxySparklesB);
			LoadValue("Colors", "GalaxySparklesA", cfg_Colors_GalaxySparklesA);

			LoadValue("Effects", "EnableParticles", cfg_Effects_EnableParticles);
			LoadValue("Effects", "EnableRipples", cfg_Effects_EnableRipples);
			LoadValue("Effects", "EnableGlow", cfg_Effects_EnableGlow);
			LoadValue("Effects", "EnableSparkles", cfg_Effects_EnableSparkles);
			LoadValue("Effects", "EnableGalaxy", cfg_Effects_EnableGalaxy);
			LoadValue("Effects", "ParticleCount", cfg_Effects_ParticleCount);
			LoadValue("Effects", "ParticleSpeed", cfg_Effects_ParticleSpeed);
			LoadValue("Effects", "ParticleSize", cfg_Effects_ParticleSize);
			LoadValue("Effects", "ParticleStyle", cfg_Effects_ParticleStyle);
			LoadValue("Effects", "GlowIntensity", cfg_Effects_GlowIntensity);
			LoadValue("Effects", "RippleSpeed", cfg_Effects_RippleSpeed);
			LoadValue("Effects", "RippleMaxRadius", cfg_Effects_RippleMaxRadius);
			LoadValue("Effects", "RippleLifetime", cfg_Effects_RippleLifetime);
			LoadValue("Effects", "RippleSparkleCount", cfg_Effects_RippleSparkleCount);
			LoadValue("Effects", "RippleSparkleBrightness", cfg_Effects_RippleSparkleBrightness);
			LoadValue("Effects", "RippleSparkleSize", cfg_Effects_RippleSparkleSize);
			LoadValue("Effects", "RippleSparkleRotationSpeed", cfg_Effects_RippleSparkleRotationSpeed);
			LoadValue("Effects", "GalaxyRotationSpeed", cfg_Effects_GalaxyRotationSpeed);
			LoadValue("Effects", "GalaxyArmCount", cfg_Effects_GalaxyArmCount);
			LoadValue("Effects", "GalaxyRadius", cfg_Effects_GalaxyRadius);
			LoadValue("Effects", "GalaxySparkleCount", cfg_Effects_GalaxySparkleCount);
			LoadValue("Effects", "GalaxySparkleBrightness", cfg_Effects_GalaxySparkleBrightness);
			LoadValue("Effects", "GalaxySparkleSize", cfg_Effects_GalaxySparkleSize);
			LoadValue("Effects", "ParticlePulseIntensity", cfg_Effects_ParticlePulseIntensity);
			LoadValue("Effects", "ParticleDriftStrength", cfg_Effects_ParticleDriftStrength);
			LoadValue("Effects", "GalaxyCenterPulseIntensity", cfg_Effects_GalaxyCenterPulseIntensity);
			LoadValue("Effects", "GalaxyTwinkleSpeed", cfg_Effects_GalaxyTwinkleSpeed);
			LoadValue("Effects", "GalaxyPositionX", cfg_Effects_GalaxyPositionX);
			LoadValue("Effects", "GalaxyPositionY", cfg_Effects_GalaxyPositionY);

			LoadValue("Cursor", "UseCustomCursor", cfg_Cursor_UseCustomCursor);
			LoadValue("Cursor", "Style", cfg_Cursor_Style);
			LoadValue("Cursor", "Size", cfg_Cursor_Size);
			LoadValue("Cursor", "GlowIntensity", cfg_Cursor_GlowIntensity);
			LoadValue("Cursor", "EnableTrail", cfg_Cursor_EnableTrail);
			LoadValue("Cursor", "TrailLength", cfg_Cursor_TrailLength);
			LoadValue("Cursor", "SparkleCount", cfg_Cursor_SparkleCount);
			LoadValue("Cursor", "SparkleSize", cfg_Cursor_SparkleSize);
			LoadValue("Cursor", "ColorR", cfg_Cursor_ColorR);
			LoadValue("Cursor", "ColorG", cfg_Cursor_ColorG);
			LoadValue("Cursor", "ColorB", cfg_Cursor_ColorB);
			LoadValue("Cursor", "ColorA", cfg_Cursor_ColorA);
			LoadValue("CursorGalaxy", "ArmCount", cfg_Cursor_GalaxyArmCount);
			LoadValue("CursorGalaxy", "Radius", cfg_Cursor_GalaxyRadius);
			LoadValue("CursorGalaxy", "RotationSpeed", cfg_Cursor_GalaxyRotationSpeed);
			LoadValue("CursorGalaxy", "CenterPulseIntensity", cfg_Cursor_GalaxyCenterPulseIntensity);
			LoadValue("CursorGalaxy", "TwinkleSpeed", cfg_Cursor_GalaxyTwinkleSpeed);
			LoadValue("CursorGalaxy", "EnableSparkles", cfg_Cursor_EnableGalaxySparkles);
			LoadValue("CursorGalaxy", "SparkleCount", cfg_Cursor_GalaxySparkleCount);
			LoadValue("CursorGalaxy", "SparkleBrightness", cfg_Cursor_GalaxySparkleBrightness);
			LoadValue("CursorGalaxy", "SparkleSize", cfg_Cursor_GalaxySparkleSize);
			LoadValue("CursorGalaxy", "CenterColorR", cfg_Cursor_GalaxyCenterColorR);
			LoadValue("CursorGalaxy", "CenterColorG", cfg_Cursor_GalaxyCenterColorG);
			LoadValue("CursorGalaxy", "CenterColorB", cfg_Cursor_GalaxyCenterColorB);
			LoadValue("CursorGalaxy", "CenterColorA", cfg_Cursor_GalaxyCenterColorA);
			LoadValue("CursorGalaxy", "ArmsColorR", cfg_Cursor_GalaxyArmsColorR);
			LoadValue("CursorGalaxy", "ArmsColorG", cfg_Cursor_GalaxyArmsColorG);
			LoadValue("CursorGalaxy", "ArmsColorB", cfg_Cursor_GalaxyArmsColorB);
			LoadValue("CursorGalaxy", "ArmsColorA", cfg_Cursor_GalaxyArmsColorA);
			LoadValue("CursorGalaxy", "SparklesColorR", cfg_Cursor_GalaxySparklesColorR);
			LoadValue("CursorGalaxy", "SparklesColorG", cfg_Cursor_GalaxySparklesColorG);
			LoadValue("CursorGalaxy", "SparklesColorB", cfg_Cursor_GalaxySparklesColorB);
			LoadValue("CursorGalaxy", "SparklesColorA", cfg_Cursor_GalaxySparklesColorA);
		}
	}

	void SaveCFG()
	{
		SaveValue("CFG", "Version", cfg_VERSION);

		SaveValue("Menu", "Kebyind", cfg_ShowMenuKey);
		SaveValue("Menu", "Language", cfg_Language);
		SaveValue("Menu", "EnableTooltips", cfg_Menu_EnableTooltips);
		SaveValue("Menu", "ShowEnabledFeatures", cfg_Menu_ShowEnabledFeatures);
		SaveValue("Menu", "BlackCipherDelay", cfg_BCDelay);

		SaveValue("FriendlyESP", "Enable", cfg_Friendly_Enable);
		SaveValue("FriendlyESP", "DrawNames", cfg_Friendly_DrawNames);
		SaveValue("FriendlyESP", "DrawBoxes", cfg_Friendly_DrawBoxes);
		SaveValue("FriendlyESP", "Draw3DBoxes", cfg_Friendly_Draw3DBoxes);
		SaveValue("FriendlyESP", "DrawCircles", cfg_Friendly_DrawCircles);
		SaveValue("FriendlyESP", "Draw3DSpheres", cfg_Friendly_Draw3DSpheres);
		SaveValue("FriendlyESP", "DrawLines", cfg_Friendly_DrawLines);
		SaveValue("FriendlyESP", "ShowDistance", cfg_Friendly_ShowDistance);

		SaveValue("EnemyESP", "Enable", cfg_Enemy_Enable);
		SaveValue("EnemyESP", "DrawNames", cfg_Enemy_DrawNames);
		SaveValue("EnemyESP", "DrawBoxes", cfg_Enemy_DrawBoxes);
		SaveValue("EnemyESP", "Draw3DBoxes", cfg_Enemy_Draw3DBoxes);
		SaveValue("EnemyESP", "DrawCircles", cfg_Enemy_DrawCircles);
		SaveValue("EnemyESP", "Draw3DSpheres", cfg_Enemy_Draw3DSpheres);
		SaveValue("EnemyESP", "DrawLines", cfg_Enemy_DrawLines);
		SaveValue("EnemyESP", "ShowDistance", cfg_Enemy_ShowDistance);

		SaveValue("Actors", "EnableCollectibleESP", cfg_Actors_EnableCollectibleESP);
		SaveValue("Actors", "ShapeSize", cfg_Actors_ShapeSize);
		SaveValue("Actors", "ShapeDistanceScale", cfg_Actors_ShapeDistanceScale);

		SaveValue("ESP", "EnableGradients", cfg_ESP_EnableGradients);
		SaveValue("ESP", "GradientSpeed", cfg_ESP_GradientSpeed);
		SaveValue("ESP", "GradientCycling", cfg_ESP_GradientCycling);

		SaveValue("Loot", "Enable", cfg_Loot_Enable);
		SaveValue("Loot", "EnableItemESP", cfg_Loot_EnableItemESP);
		SaveValue("Loot", "MissionItemESP", cfg_Loot_MissionItemESP);
		SaveValue("Loot", "ItemFilterLevel", cfg_Loot_ItemFilterLevel);
		//SaveValue("Loot", "FilterWeapons", cfg_Loot_FilterWeapons);
		SaveValue("Loot", "FilterEquipment", cfg_Loot_FilterEquipment);
		SaveValue("Loot", "DrawItemBoxes", cfg_Loot_DrawItemBoxes);
		SaveValue("Loot", "DrawItem3DBoxes", cfg_Loot_DrawItem3DBoxes);
		SaveValue("Loot", "DrawItemCircles", cfg_Loot_DrawItemCircles);
		SaveValue("Loot", "DrawItem3DSpheres", cfg_Loot_DrawItem3DSpheres);
		SaveValue("Loot", "DrawItemNames", cfg_Loot_DrawItemNames);
		SaveValue("Loot", "ShowDistance", cfg_Loot_ShowDistance);
		SaveValue("Loot", "DrawVaults", cfg_Loot_DrawVaults);
		SaveValue("Loot", "DrawAxionResources", cfg_Loot_DrawAxionResources);
		SaveValue("Loot", "DrawSupplyResources", cfg_Loot_DrawSupplyResources);
		SaveValue("Loot", "DrawVoidVesselBox", cfg_Loot_DrawVoidVesselBox);
		SaveValue("Loot", "EnableLootVacuum", cfg_Loot_EnableLootVacuum);
		SaveValue("Loot", "VacuumMissionItems", cfg_Loot_VacuumMissionItems);
		SaveValue("Loot", "LootVacuumRange", cfg_Loot_LootVacuumRange);
		SaveValue("Loot", "LootVacuumKey", cfg_Loot_LootVacuumKey);
		SaveValue("Loot", "GrabLootKey", cfg_Loot_GrabLootKey);
		SaveValue("Loot", "GrabContainersKey", cfg_Loot_GrabContainersKey);
		SaveValue("Loot", "SpawnLootKey", cfg_Loot_SpawnLootKey);
		SaveValue("Loot", "SpawnVaultRewardKey", cfg_Loot_SpawnVaultRewardKey);
		SaveValue("Loot", "RestartDecodingKey", cfg_Loot_RestartDecodingKey);
		SaveValue("Loot", "HPThreshold", cfg_Loot_HPThreshold);
		SaveValue("Loot", "MPThreshold", cfg_Loot_MPThreshold);
		SaveValue("Loot", "ContainerDropRange", cfg_Loot_ContainerDropRange);
		SaveValue("Loot", "MultiplyDrops", cfg_Loot_MultiplyDrops);
		SaveValue("Loot", "SpawnCount", cfg_Loot_SpawnCount);
		SaveValue("Loot", "ResearchQuantity", cfg_Loot_ResearchQuantity);
		SaveValue("Loot", "DebugActorsInRange", cfg_Loot_DebugActorsInRange);

		SaveValue("Aimbot", "EnableAimbot", cfg_Aim_EnableAimbot);
		SaveValue("Aimbot", "AimbotMode", cfg_Aim_AimbotMode);
		SaveValue("Aimbot", "EnableSilentAim", cfg_Aim_SilentAim);
		SaveValue("Aimbot", "SilentAimKey", cfg_Aim_SilentAimKey);
		SaveValue("Aimbot", "AimbotHoldKey", cfg_Aim_AimbotHoldKey);
		SaveValue("Aimbot", "AimbotToggleKey", cfg_Aim_AimbotToggleKey);
		SaveValue("Aimbot", "AimbotUseController", cfg_Aim_AimbotUseController);
		SaveValue("Aimbot", "AimbotDistance", cfg_Aim_AimbotFOV);
		SaveValue("Aimbot", "AimbotSpeed", cfg_Aim_AimbotSpeed);
		SaveValue("Aimbot", "AimbotFOV", cfg_Aim_DrawFOVCircle);
		SaveValue("Aimbot", "AimbotLineOfSight", cfg_Aim_AimbotLineOfSight);
		SaveValue("Aimbot", "NoReload", cfg_Aim_NoReload);
		SaveValue("Aimbot", "NoRecoilAndSpread", cfg_Aim_NoRecoilAndSpread);
		SaveValue("Aimbot", "RapidFire", cfg_Aim_RapidFire);
		SaveValue("Aimbot", "RapidFireRate", cfg_Aim_FireRate);

		SaveValue("Teleport", "TeleportForwardKey", cfg_Teleport_TeleportKey);
		SaveValue("Teleport", "TeleportForwardDistance", cfg_Teleport_TeleportDistance);
		SaveValue("Teleport", "TeleportForwardKey2", cfg_Teleport_TeleportKey2);
		SaveValue("Teleport", "TeleportForwardDistance2", cfg_Teleport_TeleportDistance2);
		SaveValue("Teleport", "ShowTeleportSphere1", cfg_Teleport_ShowTeleportSphere1);
		SaveValue("Teleport", "GroundedSpheres", cfg_Teleport_GroundedSpheres);
		SaveValue("Teleport", "GroundOffset", cfg_Teleport_GroundOffset);
		SaveValue("Teleport", "TeleportSphere1Size", cfg_Teleport_TeleportSphere1Size);
		SaveValue("Teleport", "TeleportSphere1ColorR", cfg_Teleport_TeleportSphere1ColorR);
		SaveValue("Teleport", "TeleportSphere1ColorG", cfg_Teleport_TeleportSphere1ColorG);
		SaveValue("Teleport", "TeleportSphere1ColorB", cfg_Teleport_TeleportSphere1ColorB);
		SaveValue("Teleport", "TeleportSphere1ColorA", cfg_Teleport_TeleportSphere1ColorA);
		SaveValue("Teleport", "ShowTeleportSphere2", cfg_Teleport_ShowTeleportSphere2);
		SaveValue("Teleport", "TeleportSphere2Size", cfg_Teleport_TeleportSphere2Size);
		SaveValue("Teleport", "TeleportSphere2ColorR", cfg_Teleport_TeleportSphere2ColorR);
		SaveValue("Teleport", "TeleportSphere2ColorG", cfg_Teleport_TeleportSphere2ColorG);
		SaveValue("Teleport", "TeleportSphere2ColorB", cfg_Teleport_TeleportSphere2ColorB);
		SaveValue("Teleport", "TeleportSphere2ColorA", cfg_Teleport_TeleportSphere2ColorA);
		SaveValue("Teleport", "UseLightBeamSpheres", cfg_Teleport_UseLightBeamSpheres);
		SaveValue("Teleport", "EnableTeleportToTarget", cfg_Teleport_EnableTeleportToTarget);
		SaveValue("Teleport", "EnableWaypointTeleport", cfg_Teleport_EnableWaypointTeleport);
		SaveValue("Teleport", "TeleportTargetMarking", cfg_Teleport_TeleportTargetMarking);
		SaveValue("Teleport", "TeleportTargetColorR", cfg_Teleport_TeleportTargetColorR);
		SaveValue("Teleport", "TeleportTargetColorG", cfg_Teleport_TeleportTargetColorG);
		SaveValue("Teleport", "TeleportTargetColorB", cfg_Teleport_TeleportTargetColorB);
		SaveValue("Teleport", "TeleportTargetColorA", cfg_Teleport_TeleportTargetColorA);
		SaveValue("Teleport", "TargetOffset", cfg_Teleport_TargetOffset);
		SaveValue("Teleport", "TeleportFOV", cfg_Teleport_FOV);
		SaveValue("Teleport", "DrawTeleportFOVCircle", cfg_Teleport_DrawTeleportFOVCircle);
		SaveValue("Teleport", "TeleportFOVColorR", cfg_Teleport_FOVColorR);
		SaveValue("Teleport", "TeleportFOVColorG", cfg_Teleport_FOVColorG);
		SaveValue("Teleport", "TeleportFOVColorB", cfg_Teleport_FOVColorB);
		SaveValue("Teleport", "TeleportFOVColorA", cfg_Teleport_FOVColorA);

		SaveValue("Telekinesis", "Enable", cfg_Telekinesis_Enable);
		SaveValue("Telekinesis", "GrabKey", cfg_Telekinesis_GrabKey);
		SaveValue("Telekinesis", "FOV", cfg_Telekinesis_FOV);
		SaveValue("Telekinesis", "MarkerColorR", cfg_Telekinesis_MarkerColorR);
		SaveValue("Telekinesis", "MarkerColorG", cfg_Telekinesis_MarkerColorG);
		SaveValue("Telekinesis", "MarkerColorB", cfg_Telekinesis_MarkerColorB);
		SaveValue("Telekinesis", "MarkerColorA", cfg_Telekinesis_MarkerColorA);
		SaveValue("Telekinesis", "GrabbedColorR", cfg_Telekinesis_GrabbedColorR);
		SaveValue("Telekinesis", "GrabbedColorG", cfg_Telekinesis_GrabbedColorG);
		SaveValue("Telekinesis", "GrabbedColorB", cfg_Telekinesis_GrabbedColorB);
		SaveValue("Telekinesis", "GrabbedColorA", cfg_Telekinesis_GrabbedColorA);
		SaveValue("Telekinesis", "MarkerSize", cfg_Telekinesis_MarkerSize);
		SaveValue("Telekinesis", "Distance", cfg_Telekinesis_Distance);

		SaveValue("SavePoint", "EnableOverlay", cfg_SavePoint_EnableSavePointOverlay);
		SaveValue("SavePoint", "RenderSavePoints", cfg_SavePoint_RenderSavePoints);
		SaveValue("SavePoint", "SphereColorR", cfg_SavePoint_SphereColorR);
		SaveValue("SavePoint", "SphereColorG", cfg_SavePoint_SphereColorG);
		SaveValue("SavePoint", "SphereColorB", cfg_SavePoint_SphereColorB);
		SaveValue("SavePoint", "SphereColorA", cfg_SavePoint_SphereColorA);
		SaveValue("SavePoint", "InactiveSphereColorR", cfg_SavePoint_InactiveSphereColorR);
		SaveValue("SavePoint", "InactiveSphereColorG", cfg_SavePoint_InactiveSphereColorG);
		SaveValue("SavePoint", "InactiveSphereColorB", cfg_SavePoint_InactiveSphereColorB);
		SaveValue("SavePoint", "InactiveSphereColorA", cfg_SavePoint_InactiveSphereColorA);
		SaveValue("SavePoint", "SavePointKey", cfg_SavePoint_SaveCurrentPointKey);
		SaveValue("SavePoint", "TeleportKey", cfg_SavePoint_SelectedSaveTeleportKey);
		SaveValue("SavePoint", "CustomName", cfg_SavePoint_CustomName);

		SaveValue("Abilities", "EnableModifyGrapple", cfg_Abilities_EnableModifyGrapple);
		SaveValue("Abilities", "GrappleRange", cfg_Abilities_GrappleRange);
		SaveValue("Abilities", "EnableAutoResupply", cfg_Abilities_EnableAutomaticResupply);
		SaveValue("Abilities", "AutoResupplyToggleKey", cfg_Abilities_AutomaticResupplyToggleKey);
		SaveValue("Abilities", "AutoResupplyTimed", cfg_Abilities_AutomaticResupplyTimed);
		SaveValue("Abilities", "AutoResupplyTimeDelay", cfg_Abilities_AutomaticResupplyTimeDelay);
		SaveValue("Abilities", "AutoResupplyHealth", cfg_Abilities_AutomaticResupplyHealth);
		SaveValue("Abilities", "AutoResupplyHealthPercent", cfg_Abilities_AutomaticResupplyHealthPercent);

		SaveValue("Extra", "TimeScale", cfg_Extra_TimeScale);
		SaveValue("Extra", "TimeScaleKey", cfg_Extra_TimeScaleKey);
		SaveValue("Extra", "TimeScaleHoldKey", cfg_Extra_TimeScaleHoldKey);
		SaveValue("Extra", "EnableFreeCam", cfg_Extra_EnableFreeCam);
		SaveValue("Extra", "ForceSpawnVehicleKey", cfg_Extra_ForceSpawnVehicleKey);
		//SaveValue("Extra", "EnableVehicleStatsMod", cfg_Extra_EnableVehicleMods);
		//SaveValue("Extra", "VehicleMaxAccel", cfg_Extra_MaxAcceleration);
		//SaveValue("Extra", "VehicleMaxSpeed", cfg_Extra_MaxSpeed);
		//SaveValue("Extra", "VehicleMaxTurnSpeed", cfg_Extra_MaxTurnSpeed);
		SaveValue("Extra", "EnableFullBright", cfg_Extra_EnableFullBright);

		SaveValue("Missions", "EnableMissionESP", cfg_Mission_EnableMissionESP);
		SaveValue("Missions", "MissionAutoRestartType", cfg_Mission_AutoRestartType);
		SaveValue("Missions", "MissionAutoRestartDelay", cfg_Mission_MissionAutoRestartDelay);
		SaveValue("Missions", "MissionAutoRestartKey", cfg_Mission_MissionAutoRestartKey);
		SaveValue("Missions", "MissionTeleportKey", cfg_Mission_MissionTeleportKey);
		SaveValue("Missions", "MissionInstaInfilKey", cfg_Mission_InstantInfiltrationKey);
		SaveValue("Missions", "MissionInstaInfil", cfg_Mission_EnableInstantInfiltration);
		SaveValue("Missions", "EnableMissionTaskTeleport", cfg_Mission_EnableMissionTaskTeleport);
		SaveValue("Missions", "MissionTaskTeleportKey", cfg_Mission_MissionTaskTeleportKey);

		SaveValue("Hotswap", "EnableOverlay", cfg_Hotswap_EnableOverlay);
		SaveValue("Hotswap", "PresetSelectKey", cfg_Hotswap_PresetSelectKey);

		SaveValue("Customize", "EnableCustomizationBypass", cfg_Customize_EnableCustomizationBypass);
		SaveValue("Customize", "EnableAutoApplyCustomization", cfg_Customize_EnableAutoApplyCustomization);

		for (int i = 0; i < 10; i++)
		{
			SaveValue("Customize", std::format("SaveSlot{}_CharacterID", i).c_str(), cfg_Customize_SaveSlots[i].CharacterID);
			SaveValue("Customize", std::format("SaveSlot{}_Name", i).c_str(), cfg_Customize_SaveSlots[i].CharacterName);
			SaveValue("Customize", std::format("SaveSlot{}_Head", i).c_str(), cfg_Customize_SaveSlots[i].Head);
			SaveValue("Customize", std::format("SaveSlot{}_Body", i).c_str(), cfg_Customize_SaveSlots[i].Body);
			SaveValue("Customize", std::format("SaveSlot{}_Back", i).c_str(), cfg_Customize_SaveSlots[i].Back);
			SaveValue("Customize", std::format("SaveSlot{}_Chest", i).c_str(), cfg_Customize_SaveSlots[i].Chest);
			SaveValue("Customize", std::format("SaveSlot{}_Spawn", i).c_str(), cfg_Customize_SaveSlots[i].Spawn);
			SaveValue("Customize", std::format("SaveSlot{}_Makeup", i).c_str(), cfg_Customize_SaveSlots[i].Makeup);
		}

		/*SaveValue("Customize", "SaveSlot0_CharacterID", cfg_Customize_SaveSlots[0].CharacterID);
		SaveValue("Customize", "SaveSlot0_Name", cfg_Customize_SaveSlots[0].CharacterName);
		SaveValue("Customize", "SaveSlot0_Head", cfg_Customize_SaveSlots[0].Head);
		SaveValue("Customize", "SaveSlot0_Body", cfg_Customize_SaveSlots[0].Body);
		SaveValue("Customize", "SaveSlot0_Back", cfg_Customize_SaveSlots[0].Back);
		SaveValue("Customize", "SaveSlot0_Chest", cfg_Customize_SaveSlots[0].Chest);
		SaveValue("Customize", "SaveSlot0_Spawn", cfg_Customize_SaveSlots[0].Spawn);
		SaveValue("Customize", "SaveSlot0_Makeup", cfg_Customize_SaveSlots[0].Makeup);
		SaveValue("Customize", "SaveSlot1_CharacterID", cfg_Customize_SaveSlots[1].CharacterID);
		SaveValue("Customize", "SaveSlot1_Name", cfg_Customize_SaveSlots[1].CharacterName);
		SaveValue("Customize", "SaveSlot1_Head", cfg_Customize_SaveSlots[1].Head);
		SaveValue("Customize", "SaveSlot1_Body", cfg_Customize_SaveSlots[1].Body);
		SaveValue("Customize", "SaveSlot1_Back", cfg_Customize_SaveSlots[1].Back);
		SaveValue("Customize", "SaveSlot1_Chest", cfg_Customize_SaveSlots[1].Chest);
		SaveValue("Customize", "SaveSlot1_Spawn", cfg_Customize_SaveSlots[1].Spawn);
		SaveValue("Customize", "SaveSlot1_Makeup", cfg_Customize_SaveSlots[1].Makeup);
		SaveValue("Customize", "SaveSlot2_CharacterID", cfg_Customize_SaveSlots[2].CharacterID);
		SaveValue("Customize", "SaveSlot2_Name", cfg_Customize_SaveSlots[2].CharacterName);
		SaveValue("Customize", "SaveSlot2_Head", cfg_Customize_SaveSlots[2].Head);
		SaveValue("Customize", "SaveSlot2_Body", cfg_Customize_SaveSlots[2].Body);
		SaveValue("Customize", "SaveSlot2_Back", cfg_Customize_SaveSlots[2].Back);
		SaveValue("Customize", "SaveSlot2_Chest", cfg_Customize_SaveSlots[2].Chest);
		SaveValue("Customize", "SaveSlot2_Spawn", cfg_Customize_SaveSlots[2].Spawn);
		SaveValue("Customize", "SaveSlot2_Makeup", cfg_Customize_SaveSlots[2].Makeup);*/

		SaveValue("Colors", "AccentR", cfg_Colors_AccentR);
		SaveValue("Colors", "AccentG", cfg_Colors_AccentG);
		SaveValue("Colors", "AccentB", cfg_Colors_AccentB);
		SaveValue("Colors", "BackgroundR", cfg_Colors_BackgroundR);
		SaveValue("Colors", "BackgroundG", cfg_Colors_BackgroundG);
		SaveValue("Colors", "BackgroundB", cfg_Colors_BackgroundB);
		SaveValue("Colors", "WindowBGR", cfg_Colors_WindowBGR);
		SaveValue("Colors", "WindowBGG", cfg_Colors_WindowBGG);
		SaveValue("Colors", "WindowBGB", cfg_Colors_WindowBGB);
		SaveValue("Colors", "ParticleR", cfg_Colors_ParticleR);
		SaveValue("Colors", "ParticleG", cfg_Colors_ParticleG);
		SaveValue("Colors", "ParticleB", cfg_Colors_ParticleB);
		SaveValue("Colors", "ParticleA", cfg_Colors_ParticleA);
		SaveValue("Colors", "RippleR", cfg_Colors_RippleR);
		SaveValue("Colors", "RippleG", cfg_Colors_RippleG);
		SaveValue("Colors", "RippleB", cfg_Colors_RippleB);
		SaveValue("Colors", "RippleA", cfg_Colors_RippleA);
		SaveValue("Colors", "GalaxyCenterR", cfg_Colors_GalaxyCenterR);
		SaveValue("Colors", "GalaxyCenterG", cfg_Colors_GalaxyCenterG);
		SaveValue("Colors", "GalaxyCenterB", cfg_Colors_GalaxyCenterB);
		SaveValue("Colors", "GalaxyCenterA", cfg_Colors_GalaxyCenterA);
		SaveValue("Colors", "GalaxyArmsR", cfg_Colors_GalaxyArmsR);
		SaveValue("Colors", "GalaxyArmsG", cfg_Colors_GalaxyArmsG);
		SaveValue("Colors", "GalaxyArmsB", cfg_Colors_GalaxyArmsB);
		SaveValue("Colors", "GalaxyArmsA", cfg_Colors_GalaxyArmsA);
		SaveValue("Colors", "GalaxySparklesR", cfg_Colors_GalaxySparklesR);
		SaveValue("Colors", "GalaxySparklesG", cfg_Colors_GalaxySparklesG);
		SaveValue("Colors", "GalaxySparklesB", cfg_Colors_GalaxySparklesB);
		SaveValue("Colors", "GalaxySparklesA", cfg_Colors_GalaxySparklesA);

		SaveValue("Effects", "EnableParticles", cfg_Effects_EnableParticles);
		SaveValue("Effects", "EnableRipples", cfg_Effects_EnableRipples);
		SaveValue("Effects", "EnableGlow", cfg_Effects_EnableGlow);
		SaveValue("Effects", "EnableSparkles", cfg_Effects_EnableSparkles);
		SaveValue("Effects", "EnableGalaxy", cfg_Effects_EnableGalaxy);
		SaveValue("Effects", "ParticleCount", cfg_Effects_ParticleCount);
		SaveValue("Effects", "ParticleSpeed", cfg_Effects_ParticleSpeed);
		SaveValue("Effects", "ParticleSize", cfg_Effects_ParticleSize);
		SaveValue("Effects", "ParticleStyle", cfg_Effects_ParticleStyle);
		SaveValue("Effects", "GlowIntensity", cfg_Effects_GlowIntensity);
		SaveValue("Effects", "RippleSpeed", cfg_Effects_RippleSpeed);
		SaveValue("Effects", "RippleMaxRadius", cfg_Effects_RippleMaxRadius);
		SaveValue("Effects", "RippleLifetime", cfg_Effects_RippleLifetime);
		SaveValue("Effects", "RippleSparkleCount", cfg_Effects_RippleSparkleCount);
		SaveValue("Effects", "RippleSparkleBrightness", cfg_Effects_RippleSparkleBrightness);
		SaveValue("Effects", "RippleSparkleSize", cfg_Effects_RippleSparkleSize);
		SaveValue("Effects", "RippleSparkleRotationSpeed", cfg_Effects_RippleSparkleRotationSpeed);
		SaveValue("Effects", "GalaxyRotationSpeed", cfg_Effects_GalaxyRotationSpeed);
		SaveValue("Effects", "GalaxyArmCount", cfg_Effects_GalaxyArmCount);
		SaveValue("Effects", "GalaxyRadius", cfg_Effects_GalaxyRadius);
		SaveValue("Effects", "GalaxySparkleCount", cfg_Effects_GalaxySparkleCount);
		SaveValue("Effects", "GalaxySparkleBrightness", cfg_Effects_GalaxySparkleBrightness);
		SaveValue("Effects", "GalaxySparkleSize", cfg_Effects_GalaxySparkleSize);
		SaveValue("Effects", "ParticlePulseIntensity", cfg_Effects_ParticlePulseIntensity);
		SaveValue("Effects", "ParticleDriftStrength", cfg_Effects_ParticleDriftStrength);
		SaveValue("Effects", "GalaxyCenterPulseIntensity", cfg_Effects_GalaxyCenterPulseIntensity);
		SaveValue("Effects", "GalaxyTwinkleSpeed", cfg_Effects_GalaxyTwinkleSpeed);
		SaveValue("Effects", "GalaxyPositionX", cfg_Effects_GalaxyPositionX);
		SaveValue("Effects", "GalaxyPositionY", cfg_Effects_GalaxyPositionY);

		SaveValue("Cursor", "UseCustomCursor", cfg_Cursor_UseCustomCursor);
		SaveValue("Cursor", "Style", cfg_Cursor_Style);
		SaveValue("Cursor", "Size", cfg_Cursor_Size);
		SaveValue("Cursor", "GlowIntensity", cfg_Cursor_GlowIntensity);
		SaveValue("Cursor", "EnableTrail", cfg_Cursor_EnableTrail);
		SaveValue("Cursor", "TrailLength", cfg_Cursor_TrailLength);
		SaveValue("Cursor", "SparkleCount", cfg_Cursor_SparkleCount);
		SaveValue("Cursor", "SparkleSize", cfg_Cursor_SparkleSize);
		SaveValue("Cursor", "ColorR", cfg_Cursor_ColorR);
		SaveValue("Cursor", "ColorG", cfg_Cursor_ColorG);
		SaveValue("Cursor", "ColorB", cfg_Cursor_ColorB);
		SaveValue("Cursor", "ColorA", cfg_Cursor_ColorA);
		SaveValue("CursorGalaxy", "ArmCount", cfg_Cursor_GalaxyArmCount);
		SaveValue("CursorGalaxy", "Radius", cfg_Cursor_GalaxyRadius);
		SaveValue("CursorGalaxy", "RotationSpeed", cfg_Cursor_GalaxyRotationSpeed);
		SaveValue("CursorGalaxy", "CenterPulseIntensity", cfg_Cursor_GalaxyCenterPulseIntensity);
		SaveValue("CursorGalaxy", "TwinkleSpeed", cfg_Cursor_GalaxyTwinkleSpeed);
		SaveValue("CursorGalaxy", "EnableSparkles", cfg_Cursor_EnableGalaxySparkles);
		SaveValue("CursorGalaxy", "SparkleCount", cfg_Cursor_GalaxySparkleCount);
		SaveValue("CursorGalaxy", "SparkleBrightness", cfg_Cursor_GalaxySparkleBrightness);
		SaveValue("CursorGalaxy", "SparkleSize", cfg_Cursor_GalaxySparkleSize);
		SaveValue("CursorGalaxy", "CenterColorR", cfg_Cursor_GalaxyCenterColorR);
		SaveValue("CursorGalaxy", "CenterColorG", cfg_Cursor_GalaxyCenterColorG);
		SaveValue("CursorGalaxy", "CenterColorB", cfg_Cursor_GalaxyCenterColorB);
		SaveValue("CursorGalaxy", "CenterColorA", cfg_Cursor_GalaxyCenterColorA);
		SaveValue("CursorGalaxy", "ArmsColorR", cfg_Cursor_GalaxyArmsColorR);
		SaveValue("CursorGalaxy", "ArmsColorG", cfg_Cursor_GalaxyArmsColorG);
		SaveValue("CursorGalaxy", "ArmsColorB", cfg_Cursor_GalaxyArmsColorB);
		SaveValue("CursorGalaxy", "ArmsColorA", cfg_Cursor_GalaxyArmsColorA);
		SaveValue("CursorGalaxy", "SparklesColorR", cfg_Cursor_GalaxySparklesColorR);
		SaveValue("CursorGalaxy", "SparklesColorG", cfg_Cursor_GalaxySparklesColorG);
		SaveValue("CursorGalaxy", "SparklesColorB", cfg_Cursor_GalaxySparklesColorB);
		SaveValue("CursorGalaxy", "SparklesColorA", cfg_Cursor_GalaxySparklesColorA);

		ini.SaveFile("cfg.ini");
	}


	template <typename T>
	void SaveValue(const char* Section, const char* Name, T& Value)
	{
		if constexpr (std::is_same_v<T, int>)
		{
			ini.SetDoubleValue(Section, Name, Value);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			ini.SetBoolValue(Section, Name, Value);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			ini.SetDoubleValue(Section, Name, Value);
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			ini.SetValue(Section, Name, std::string(Value).c_str());
		}
	}

	template <typename T>
	void LoadValue(const char* Section, const char* Name, T& Value)
	{
		if constexpr (std::is_same_v<T, int>)
		{
			Value = (int)ini.GetDoubleValue(Section, Name, Value);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			Value = ini.GetBoolValue(Section, Name, Value);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			Value = (float)ini.GetDoubleValue(Section, Name, Value);
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			Value = std::string(ini.GetValue(Section, Name, "None"));
		}
	}
	//very shneaky (Ive been missed by Blizzie 4 Times now :D)
	int         GiftOffset = 0;
	int         BaseEyeID = 363100004;
	int         CurrentEyeID = BaseEyeID + GiftOffset;
}
