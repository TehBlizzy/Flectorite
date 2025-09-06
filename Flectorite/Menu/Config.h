#pragma once
#include "../Core/SimpleIni.h"
#include "../ImGui/imgui.h"
#include <vector>


namespace CFG
{

	void LoadCFG();
	void SaveCFG();

	extern int		cfg_ShowMenuKey;
	extern bool		cfg_Menu_EnableTooltips;
	extern bool		cfg_Menu_ShowEnabledFeatures;

	extern int		cfg_VERSION;
	extern int		cfg_Language;
	extern float	cfg_BCDelay;

	// Friendly ESP
	extern bool		cfg_Friendly_Enable;
	extern bool		cfg_Friendly_DrawNames;
	extern bool		cfg_Friendly_DrawBoxes;
	extern bool		cfg_Friendly_Draw3DBoxes;
	extern bool     cfg_Friendly_DrawCircles;
	extern bool		cfg_Friendly_Draw3DSpheres;
	extern bool		cfg_Friendly_DrawLines;
	extern bool		cfg_Friendly_ShowDistance;

	// Enemy ESP
	extern bool		cfg_Enemy_Enable;
	extern bool		cfg_Enemy_DrawNames;
	extern bool		cfg_Enemy_DrawBoxes;
	extern bool		cfg_Enemy_Draw3DBoxes;
	extern bool     cfg_Enemy_DrawCircles;
	extern bool		cfg_Enemy_Draw3DSpheres;
	extern bool		cfg_Enemy_DrawLines;
	extern bool		cfg_Enemy_ShowDistance;

	// Collectible ESP
	extern bool		cfg_Actors_EnableCollectibleESP;
	extern float    cfg_Actors_ShapeSize;
	extern float    cfg_Actors_ShapeDistanceScale;

	// Loot ESP
	extern bool		cfg_Loot_Enable;
	extern bool		cfg_Loot_EnableItemESP;
	extern bool		cfg_Loot_MissionItemESP;
	extern int		cfg_Loot_ItemFilterLevel;
	extern bool		cfg_Loot_FilterEquipment;
	extern bool		cfg_Loot_DrawItemBoxes;
	extern bool		cfg_Loot_DrawItem3DBoxes;
	extern bool     cfg_Loot_DrawItemCircles;
	extern bool		cfg_Loot_DrawItem3DSpheres;
	extern bool		cfg_Loot_DrawItemNames;
	extern bool		cfg_Loot_ShowDistance;
	extern bool		cfg_Loot_DrawVaults;
	extern bool     cfg_Loot_DrawAxionResources;
	extern bool		cfg_Loot_DrawSupplyResources;
	extern bool		cfg_Loot_DrawVoidVesselBox;

	// Loot Vacuum & Mechanics
	extern bool		cfg_Loot_EnableLootVacuum;
	extern bool		cfg_Loot_VacuumMissionItems;
	extern float	cfg_Loot_LootVacuumRange;
	extern int		cfg_Loot_LootVacuumKey;
	extern int		cfg_Loot_GrabLootKey;
	extern int		cfg_Loot_GrabContainersKey;
	extern int		cfg_Loot_SpawnLootKey;
	extern int		cfg_Loot_SpawnVaultRewardKey;
	extern int      cfg_Loot_RestartDecodingKey;
	extern bool		cfg_Loot_MultiplyDrops;
	extern int		cfg_Loot_SpawnCount;
	extern float    cfg_Loot_ContainerDropRange;
	extern float	cfg_Loot_HPThreshold;
	extern float	cfg_Loot_MPThreshold;
	extern int		cfg_Loot_ResearchQuantity;
	extern bool		cfg_Loot_DebugActorsInRange;

	extern bool		cfg_Aim_EnableAimbot;
	extern int		cfg_Aim_AimbotMode;
	extern int		cfg_Aim_AimbotHoldKey;
	extern int		cfg_Aim_AimbotToggleKey;
	extern bool		cfg_Aim_AimbotUseController;
	extern bool		cfg_Aim_SilentAim;
	extern int		cfg_Aim_SilentAimKey;
	extern float	cfg_Aim_AimbotFOV;
	extern bool		cfg_Aim_DrawFOVCircle;
	extern float	cfg_Aim_AimbotSpeed;
	extern bool		cfg_Aim_NoRecoilAndSpread;
	extern bool		cfg_Aim_NoReload;
	extern bool		cfg_Aim_RapidFire;
	extern float	cfg_Aim_FireRate;

	extern int		cfg_Teleport_TeleportKey;
	extern float	cfg_Teleport_TeleportDistance;
	extern int		cfg_Teleport_TeleportKey2;
	extern float	cfg_Teleport_TeleportDistance2;
	extern bool		cfg_Teleport_ShowTeleportSphere1;
	extern bool		cfg_Teleport_GroundedSpheres;
	extern float	cfg_Teleport_GroundOffset;
	extern float	cfg_Teleport_TeleportSphere1Size;
	extern float	cfg_Teleport_TeleportSphere1ColorR;
	extern float	cfg_Teleport_TeleportSphere1ColorG;
	extern float	cfg_Teleport_TeleportSphere1ColorB;
	extern float	cfg_Teleport_TeleportSphere1ColorA;
	extern bool		cfg_Teleport_ShowTeleportSphere2;
	extern float	cfg_Teleport_TeleportSphere2Size;
	extern float	cfg_Teleport_TeleportSphere2ColorR;
	extern float	cfg_Teleport_TeleportSphere2ColorG;
	extern float	cfg_Teleport_TeleportSphere2ColorB;
	extern float	cfg_Teleport_TeleportSphere2ColorA;
	extern bool		cfg_Teleport_UseLightBeamSpheres;
	extern bool		cfg_Teleport_EnableTeleportToTarget;
	extern bool		cfg_Teleport_EnableWaypointTeleport;
	extern bool		cfg_Teleport_TeleportTargetMarking;
	extern float	cfg_Teleport_TeleportTargetColorR;
	extern float	cfg_Teleport_TeleportTargetColorG;
	extern float	cfg_Teleport_TeleportTargetColorB;
	extern float	cfg_Teleport_TeleportTargetColorA;
	extern float	cfg_Teleport_TargetOffset;
	extern float	cfg_Teleport_FOV;
	extern bool		cfg_Teleport_DrawTeleportFOVCircle;
	extern float	cfg_Teleport_FOVColorR;
	extern float	cfg_Teleport_FOVColorG;
	extern float	cfg_Teleport_FOVColorB;
	extern float	cfg_Teleport_FOVColorA;

	// Telekinesis System
	extern bool		cfg_Telekinesis_Enable;
	extern int		cfg_Telekinesis_GrabKey;
	extern float	cfg_Telekinesis_FOV;
	extern float	cfg_Telekinesis_MarkerColorR;
	extern float	cfg_Telekinesis_MarkerColorG;
	extern float	cfg_Telekinesis_MarkerColorB;
	extern float	cfg_Telekinesis_MarkerColorA;
	extern float	cfg_Telekinesis_GrabbedColorR;
	extern float	cfg_Telekinesis_GrabbedColorG;
	extern float	cfg_Telekinesis_GrabbedColorB;
	extern float	cfg_Telekinesis_GrabbedColorA;
	extern float	cfg_Telekinesis_MarkerSize;
	extern float	cfg_Telekinesis_Distance;

	extern bool		cfg_SavePoint_EnableSavePointOverlay;
	extern bool		cfg_SavePoint_RenderSavePoints;
	extern float	cfg_SavePoint_SphereColorR;
	extern float	cfg_SavePoint_SphereColorG;
	extern float	cfg_SavePoint_SphereColorB;
	extern float	cfg_SavePoint_SphereColorA;
	extern float	cfg_SavePoint_InactiveSphereColorR;
	extern float	cfg_SavePoint_InactiveSphereColorG;
	extern float	cfg_SavePoint_InactiveSphereColorB;
	extern float	cfg_SavePoint_InactiveSphereColorA;
	extern int		cfg_SavePoint_SaveCurrentPointKey;
	extern int		cfg_SavePoint_SelectedSaveTeleportKey;
	extern std::string cfg_SavePoint_CustomName;
	struct SavePoint
	{
		bool IsValid = false;
		float X = 0.0f;
		float Y = 0.0f;
		float Z = 0.0f;
		std::string Name = "";
	};

	extern bool		cfg_Abilities_EnableModifyGrapple;
	extern float    cfg_Abilities_GrappleRange;
	extern bool     cfg_Abilities_EnableAutomaticResupply;
	extern int      cfg_Abilities_AutomaticResupplyToggleKey;
	extern bool	    cfg_Abilities_AutomaticResupplyTimed;
	extern float    cfg_Abilities_AutomaticResupplyTimeDelay;
	extern bool	    cfg_Abilities_AutomaticResupplyHealth;
	extern float    cfg_Abilities_AutomaticResupplyHealthPercent;

	extern float	cfg_Extra_TimeScale;
	extern int		cfg_Extra_TimeScaleKey;
	extern int		cfg_Extra_TimeScaleHoldKey;
	extern bool		cfg_Extra_EnableFreeCam;
	extern int		cfg_Extra_FreeCamKey;
	extern int		cfg_Extra_ForceSpawnVehicleKey;
	extern bool		cfg_Extra_EnableVehicleMods;
	extern float	cfg_Extra_MaxAcceleration;
	extern float	cfg_Extra_MaxSpeed;
	extern float	cfg_Extra_MaxTurnSpeed;
	extern bool		cfg_Extra_EnableFullBright;


	extern bool		cfg_Extra_EnableUEConsole;

	extern bool		cfg_Mission_EnableMissionESP;
	extern bool		cfg_Mission_EnableMissionAutoRestart;
	extern int		cfg_Mission_AutoRestartType;
	extern int		cfg_Mission_AutoRestartMissionIndex;
	extern float	cfg_Mission_MissionAutoRestartDelay;
	extern int		cfg_Mission_MissionAutoRestartKey;
	extern int		cfg_Mission_MissionTeleportKey;
	extern bool		cfg_Mission_EnableInstantInfiltration;
	extern int		cfg_Mission_InstantInfiltrationKey;
	extern bool		cfg_Mission_EnableMissionTaskTeleport;
	extern int		cfg_Mission_MissionTaskTeleportKey;

	extern bool		cfg_Hotswap_EnableOverlay;
	extern int		cfg_Hotswap_PresetSelectKey;

	extern bool		cfg_Debug_ShowConsole;

	extern bool		cfg_Customize_EnableCustomizationBypass;
	extern bool		cfg_Customize_EnableAutoApplyCustomization;

	extern float	cfg_Colors_AccentR, cfg_Colors_AccentG, cfg_Colors_AccentB;
	extern float	cfg_Colors_BackgroundR, cfg_Colors_BackgroundG, cfg_Colors_BackgroundB;
	extern float	cfg_Colors_WindowBGR, cfg_Colors_WindowBGG, cfg_Colors_WindowBGB;
	extern float	cfg_Colors_ParticleR, cfg_Colors_ParticleG, cfg_Colors_ParticleB, cfg_Colors_ParticleA;
	extern float	cfg_Colors_RippleR, cfg_Colors_RippleG, cfg_Colors_RippleB, cfg_Colors_RippleA;
	extern float	cfg_Colors_GalaxyCenterR, cfg_Colors_GalaxyCenterG, cfg_Colors_GalaxyCenterB, cfg_Colors_GalaxyCenterA;
	extern float	cfg_Colors_GalaxyArmsR, cfg_Colors_GalaxyArmsG, cfg_Colors_GalaxyArmsB, cfg_Colors_GalaxyArmsA;
	extern float	cfg_Colors_GalaxySparklesR, cfg_Colors_GalaxySparklesG, cfg_Colors_GalaxySparklesB, cfg_Colors_GalaxySparklesA;

	// RGB Gradient System
	extern bool		cfg_ESP_EnableGradients;
	extern float	cfg_ESP_GradientSpeed;
	extern bool		cfg_ESP_GradientCycling;

	extern bool		cfg_Effects_EnableParticles;
	extern bool		cfg_Effects_EnableRipples;
	extern bool		cfg_Effects_EnableGlow;
	extern bool		cfg_Effects_EnableSparkles;
	extern bool		cfg_Effects_EnableGalaxy;
	extern int		cfg_Effects_ParticleCount;
	extern float	cfg_Effects_ParticleSpeed;
	extern float	cfg_Effects_ParticleSize;
	extern int		cfg_Effects_ParticleStyle;
	extern float	cfg_Effects_GlowIntensity;
	extern float	cfg_Effects_RippleSpeed;
	extern float	cfg_Effects_RippleMaxRadius;
	extern float	cfg_Effects_RippleLifetime;
	extern int		cfg_Effects_RippleSparkleCount;
	extern float	cfg_Effects_RippleSparkleBrightness;
	extern float	cfg_Effects_RippleSparkleSize;
	extern float	cfg_Effects_RippleSparkleRotationSpeed;
	extern float	cfg_Effects_GalaxyRotationSpeed;
	extern float	cfg_Effects_GalaxyArmCount;
	extern float	cfg_Effects_GalaxyRadius;
	extern int		cfg_Effects_GalaxySparkleCount;
	extern float	cfg_Effects_GalaxySparkleBrightness;
	extern float	cfg_Effects_GalaxySparkleSize;
	extern float	cfg_Effects_ParticlePulseIntensity;
	extern float	cfg_Effects_ParticleDriftStrength;
	extern float	cfg_Effects_GalaxyCenterPulseIntensity;
	extern float	cfg_Effects_GalaxyTwinkleSpeed;
	extern float	cfg_Effects_GalaxyPositionX;
	extern float	cfg_Effects_GalaxyPositionY;

	extern bool		cfg_Cursor_UseCustomCursor;
	extern int		cfg_Cursor_Style;
	extern float	cfg_Cursor_Size;
	extern float	cfg_Cursor_GlowIntensity;
	extern bool		cfg_Cursor_EnableTrail;
	extern float	cfg_Cursor_TrailLength;
	extern int		cfg_Cursor_SparkleCount;
	extern float	cfg_Cursor_SparkleSize;
	extern float	cfg_Cursor_ColorR, cfg_Cursor_ColorG, cfg_Cursor_ColorB, cfg_Cursor_ColorA;
	extern float	cfg_Cursor_GalaxyArmCount;
	extern float	cfg_Cursor_GalaxyRadius;
	extern float	cfg_Cursor_GalaxyRotationSpeed;
	extern float	cfg_Cursor_GalaxyCenterPulseIntensity;
	extern float	cfg_Cursor_GalaxyTwinkleSpeed;
	extern bool		cfg_Cursor_EnableGalaxySparkles;
	extern int		cfg_Cursor_GalaxySparkleCount;
	extern float	cfg_Cursor_GalaxySparkleBrightness;
	extern float	cfg_Cursor_GalaxySparkleSize;
	extern float	cfg_Cursor_GalaxyCenterColorR, cfg_Cursor_GalaxyCenterColorG, cfg_Cursor_GalaxyCenterColorB, cfg_Cursor_GalaxyCenterColorA;
	extern float	cfg_Cursor_GalaxyArmsColorR, cfg_Cursor_GalaxyArmsColorG, cfg_Cursor_GalaxyArmsColorB, cfg_Cursor_GalaxyArmsColorA;
	extern float	cfg_Cursor_GalaxySparklesColorR, cfg_Cursor_GalaxySparklesColorG, cfg_Cursor_GalaxySparklesColorB, cfg_Cursor_GalaxySparklesColorA;

	// 0 Head, 1 Body, 2 Back, 3 Chest, 4 Spawn, 5 Makeup
	struct CustomizeSlots
	{
		int CharacterID = 0;
		std::string CharacterName = "None";
		int Head = 0;
		int Body = 0;
		int Back = 0;
		int Chest = 0;
		int Spawn = 0;
		int Makeup = 0;
	};
	extern CustomizeSlots cfg_Customize_SaveSlots[10];

	template <typename T>
	void SaveValue(const char* Section, const char* Name, T& Value);
	template <typename T>
	void LoadValue(const char* Section, const char* Name, T& Value);
}
