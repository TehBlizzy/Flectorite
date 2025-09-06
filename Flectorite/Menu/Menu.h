#pragma once
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx12.h"
#include "../ImGui/imgui_impl_win32.h"
#include "Config.h"
#include "../Core/Cheat.h"
#include "../Cheats/Cosmetics.h"
#include "../magic_enum/magic_enum.hpp"
#include "imgui_custom.h"

namespace Menu
{
	//void RenderConsole();
	void RenderMenu();
	void HandleKeybinds();
	//void ApplySelectedStyle(int index);

	extern bool ShowMenu;
}
