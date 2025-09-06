#pragma once
#include <cstddef>
#include "../Core/SDK.h"
#include "../Core/Cheat.h"
#include "../ImGui/imgui.h"



namespace Render
{
	/* FUNCTIONS */
	void RenderOverlay();

	static void* R_GetCmdBuffer(int bytes);
	void R_DrawText(float x, float y, UC::FString text, SDK::FLinearColor* color, bool background = false);
	void R_DrawText(float x, float y, std::string text, SDK::FLinearColor* color, bool background = false);
	void R_DrawRect(float x, float y, float w, float h, SDK::FLinearColor* color);
	void R_DrawFillRect(float x, float y, float w, float h, SDK::FLinearColor* color);
	void R_DrawLine(float x, float y, float x2, float y2, SDK::FLinearColor* color);
	void R_DrawCircle(float x, float y, float radius, SDK::FLinearColor* color);
	void R_Draw3DBox(const SDK::FVector& center, const SDK::FVector& extent, SDK::FLinearColor* color);
	void R_Draw3DBoxRotated(const SDK::FVector& center, const SDK::FVector& extent, const SDK::FRotator& rotation, SDK::FLinearColor* color);
	void R_Draw3DSphere(const SDK::FVector& center, float radius, SDK::FLinearColor* color);
	void R_Draw3DSphereRotated(const SDK::FVector& center, float radius, const SDK::FRotator& rotation, SDK::FLinearColor* color);
	void R_Draw3DSphereLightBeam(const SDK::FVector& center, float radius, const SDK::FRotator& rotation, float beamHeight, float beamIntensity, SDK::FLinearColor* color);
	void R_Draw3DBoxGradient(const SDK::FVector& center, const SDK::FVector& extent, SDK::FLinearColor* color1, SDK::FLinearColor* color2);
	void R_Draw3DSphereGradient(const SDK::FVector& center, float radius, SDK::FLinearColor* color1, SDK::FLinearColor* color2);
	void R_Draw3DSphereLightBeamGradient(const SDK::FVector& center, float radius, const SDK::FRotator& rotation, float beamHeight, float beamIntensity, SDK::FLinearColor* color1, SDK::FLinearColor* color2);
	void R_DrawCircleGradient(float x, float y, float radius, SDK::FLinearColor* color1, SDK::FLinearColor* color2);
	void R_DrawRectGradient(float x, float y, float w, float h, SDK::FLinearColor* color1, SDK::FLinearColor* color2);
	void R_DrawLineGradient(float x, float y, float x2, float y2, SDK::FLinearColor* color1, SDK::FLinearColor* color2);
	void R_DrawTextGradient(float x, float y, std::string text, SDK::FLinearColor* color1, SDK::FLinearColor* color2, bool background = false);
	SDK::FLinearColor GetGradientColor(SDK::FLinearColor* color1, SDK::FLinearColor* color2, float t);
	SDK::FLinearColor GetFlowingRainbowColor(float position, float time);
	SDK::FLinearColor GetCyclingGradientColor(SDK::FLinearColor* color1, SDK::FLinearColor* color2, float time);
	float GetSharedGradientTime();
	void R_End();

	/* TYPE OF RENDER COMMANDS */
	enum rd_cmdIDs
	{
		RD_END,
		RD_TEXT,
		RD_RECT,
		RD_FILLRECT,
		RD_LINE,
		RD_CIRCLE,
		RD_3DBOX,
		RD_3DBOX_ROTATED,
		RD_3DSPHERE,
		RD_3DSPHERE_ROTATED,
		RD_3DSPHERE_LIGHTBEAM,
		RD_3DBOX_GRADIENT,
		RD_3DSPHERE_GRADIENT,
		RD_CIRCLE_GRADIENT,
		RD_RECT_GRADIENT,
		RD_LINE_GRADIENT,
		RD_TEXT_GRADIENT,
		RD_3DSPHERE_LIGHTBEAM_GRADIENT
	};

	/* STORES RENDER COMMANDS */
	struct rd_Commands
	{
		byte cmds[0x80000];
		int used;
	};

	static rd_Commands Commands;

	/* RENDER COMMANDS DATA */
	struct rd_cmd
	{
		int cmdID;
	};

	struct rd_text : rd_cmd
	{
		char txt[64];
		float x, y;
		float color[4];
		bool bg;
	};

	struct rd_rect : rd_cmd
	{
		float x, y, w, h;
		float color[4];
	};

	struct rd_fillrect : rd_cmd
	{
		float x, y, w, h;
		float color[4];
	};

	struct rd_line : rd_cmd
	{
		float x, y, x2, y2;
		float color[4];
	};

	struct rd_circle : rd_cmd
	{
		float x, y;
		float radius;
		float color[4];
	};

	struct rd_3dbox : rd_cmd
	{
		float center[3];
		float extent[3];
		float color[4];
	};

	struct rd_3dbox_rotated : rd_cmd
	{
		float center[3];
		float extent[3];
		float rotation[3];
		float color[4];
	};

	struct rd_3dsphere : rd_cmd
	{
		float center[3];
		float radius;
		float color[4];
	};

	struct rd_3dsphere_rotated : rd_cmd
	{
		float center[3];
		float radius;
		float rotation[3];
		float color[4];
	};

	struct rd_3dsphere_lightbeam : rd_cmd
	{
		float center[3];
		float radius;
		float rotation[3];
		float beamHeight;
		float beamIntensity;
		float color[4];
	};

	struct rd_3dsphere_lightbeam_gradient : rd_cmd
	{
		float center[3];
		float radius;
		float rotation[3];
		float beamHeight;
		float beamIntensity;
		float color1[4];
		float color2[4];
	};

	struct rd_3dbox_gradient : rd_cmd
	{
		float center[3];
		float extent[3];
		float color1[4];
		float color2[4];
	};

	struct rd_3dsphere_gradient : rd_cmd
	{
		float center[3];
		float radius;
		float color1[4];
		float color2[4];
	};

	struct rd_circle_gradient : rd_cmd
	{
		float x, y;
		float radius;
		float color1[4];
		float color2[4];
	};

	struct rd_rect_gradient : rd_cmd
	{
		float x, y, w, h;
		float color1[4];
		float color2[4];
	};

	struct rd_line_gradient : rd_cmd
	{
		float x, y, x2, y2;
		float color1[4];
		float color2[4];
	};

	struct rd_text_gradient : rd_cmd
	{
		char txt[64];
		float x, y;
		float color1[4];
		float color2[4];
		bool bg;
	};

	struct rd_end : rd_cmd
	{

	};

}