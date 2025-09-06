#include <windows.h>
#include "imgui_custom.h"
#include "Config.h"
#include <map>
#include "../ImGui/Fonts/IconsFontAwesome6.h"
#include <vector>

#ifdef IMGUI_BLUR_TEST
#include <wrl.h>
#include "BlurCS.h"
#include <debugapi.h>
#include <d3dcompiler.h>
#include <iostream>
using namespace Microsoft::WRL;

namespace ImGui
{
	BlurResources BLUR;
}
#endif

namespace ImGui
{
	bool IsBindingKey = false;
}

bool ImGui::VerticalTab(const char* label, const char* icon, int* v, int number)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	const float square_sz = GetFrameHeight();
	const ImVec2 pos = window->DC.CursorPos;
	const ImRect total_bb(pos, pos + ImVec2(167, 40));

	ItemSize(total_bb, style.FramePadding.y);
	ItemAdd(total_bb, id);

	static std::map<ImGuiID, Tab_State> TabStates;
	auto State = TabStates.find(id);
	if (TabStates.empty())
	{
		TabStates.insert({ id, { true, 1.0f, second_color, text_color[0], main_color  } });
		State = TabStates.find(id);
	}

	else if (State == TabStates.end())
	{
		TabStates.insert({ id, { false, 0.0f, GetColorWithAlpha(second_color, 0.f), text_color[1], text_color[1] } });
		State = TabStates.find(id);
	}

	bool hovered, held;
	bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
	{
		*v = number;
		MarkItemEdited(id);
		for (auto& st : TabStates)
			st.second.IsActiveTab = false;
		State->second.IsActiveTab = true;
	}

	RenderNavHighlight(total_bb, id);

	if (State->second.IsActiveTab)
		State->second.AnimValue = ImMin(State->second.AnimValue + anim_speed, 1.0f); // Increment smoothly towards 1.0
	else
		State->second.AnimValue = ImMax(State->second.AnimValue - anim_speed, 0.0f); // Decrement smoothly towards 0.0

	if (State->second.IsActiveTab)
	{
		State->second.icon_color = ImLerp(State->second.icon_color, main_color, State->second.AnimValue);// : text_color[1], anim_speed);
		State->second.text_color = ImLerp(State->second.text_color, text_color[0], State->second.AnimValue);// : text_color[1], anim_speed);
		State->second.rect_color = ImLerp(State->second.rect_color, second_color, State->second.AnimValue);// : GetColorWithAlpha(second_color, 0.f), anim_speed);
	}
	else
	{
		State->second.icon_color = ImLerp(State->second.icon_color, text_color[1], State->second.AnimValue);// : text_color[1], anim_speed);
		State->second.text_color = ImLerp(State->second.text_color, text_color[1], State->second.AnimValue);// : text_color[1], anim_speed);
		State->second.rect_color = ImLerp(State->second.rect_color, GetColorWithAlpha(second_color, 0.f), State->second.AnimValue);// : GetColorWithAlpha(second_color, 0.f), anim_speed);
	}

	window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, GetColorU32(State->second.rect_color), style.FrameRounding);

	window->DrawList->AddText(ImVec2(total_bb.Min.x + 45, GetTextCenterPosition(total_bb.Min, total_bb.Max, icon).y), GetColorU32(State->second.text_color), label);

	PushFont(icon_big_font);
	window->DrawList->AddText(ImVec2(total_bb.Min.x + 10, GetTextCenterPosition(total_bb.Min, total_bb.Max, icon).y), GetColorU32(State->second.icon_color), icon);
	PopFont();


	return pressed;
}

bool ImGui::CustomCheckbox(const char* label, bool* v)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	const float square_sz = GetFrameHeight();
	const ImVec2 pos = window->DC.CursorPos;
	const ImRect total_bb(pos, pos + frame_size);

	const ImRect check_bb(total_bb.Max - ImVec2(45, 32), total_bb.Max - ImVec2(14, 14));
	ItemSize(total_bb, style.FramePadding.y);
	ItemAdd(total_bb, id);

	static std::map<ImGuiID, Checkbox_State> States;
	auto State = States.find(id);
	if (State == States.end())
	{
		if (*v)
			States.insert({ id, { 1.0f, 13.0f, GetColorWithAlpha(main_color, 0.3f), main_color, main_color} });
		else
			States.insert({ id, { 0.0f, 0.0f, second_color, foreground_color, GetColorWithAlpha(main_color, 0.f) } });
		State = States.find(id);
	}

	bool hovered, held;
	bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
	{
		*v = !(*v);
		MarkItemEdited(id);
	}

	RenderNavHighlight(total_bb, id);

	if (*v)
		State->second.AnimValue = ImMin(State->second.AnimValue + anim_speed, 1.0f);
	else
		State->second.AnimValue = ImMax(State->second.AnimValue - anim_speed, 0.0f);


	State->second.rect_color = ImLerp(State->second.rect_color, *v ? GetColorWithAlpha(main_color, 0.3f) : second_color, State->second.AnimValue);
	State->second.circle_color = ImLerp(State->second.circle_color, *v ? main_color : foreground_color, State->second.AnimValue);
	State->second.shadow_color = ImLerp(State->second.shadow_color, *v ? main_color : GetColorWithAlpha(main_color, 0.f), State->second.AnimValue);
	State->second.circle_offset = ImLerp(State->second.circle_offset, *v ? 13 : 0.f, State->second.AnimValue);

	PushStyleVar(ImGuiStyleVar_WindowRounding, style.FrameRounding);
	//CustomBlur(total_bb.Min, total_bb.Max);
	PopStyleVar();

	window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, background_color, style.FrameRounding);

	window->DrawList->AddRectFilled(check_bb.Min, check_bb.Max, GetColorU32(State->second.rect_color), 360);

	window->DrawList->AddShadowCircle(check_bb.Min + ImVec2(9 + State->second.circle_offset, 8.5f), 5.5f, ImColor(0.f, 0.f, 0.f, 0.3f), 10.f, ImVec2(1, 1), 0, 360);

	window->DrawList->AddCircleFilled(check_bb.Min + ImVec2(9 + State->second.circle_offset, 8.5f), 5.5f, GetColorU32(State->second.circle_color), 360);

	//Glow
	window->DrawList->AddShadowCircle(check_bb.Min + ImVec2(9 + State->second.circle_offset, 8.5f), 3.5f, GetColorU32(State->second.shadow_color), 70.f, ImVec2(0, 0), 0, 360);

	window->DrawList->AddText(ImVec2(total_bb.Min.x + 12.f, GetTextCenterPosition(total_bb.Min, total_bb.Max, label).y), text_color[0], label);

	return pressed;
}

bool ImGui::CustomSliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return CustomSliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}

bool ImGui::CustomSliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	return CustomSliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}

bool ImGui::CustomInputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const ImVec2 item_size = ImVec2(frame_size.x, 35); // Smaller height than default

	const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + item_size);
	const ImRect input_bb(total_bb.Min + ImVec2(12, 8), total_bb.Max - ImVec2(14, 8));

	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id, &input_bb))
		return false;

	const bool hovered = ItemHoverable(input_bb, id, ImGuiItemFlags_::ImGuiItemFlags_None);
	const bool user_clicked = hovered && IsMouseClicked(0);
	const bool user_nav_input_start = (g.ActiveId != id) && (g.NavActivateId == id);

	const bool user_wants_to_activate_widget = (user_clicked || user_nav_input_start);
	if (user_wants_to_activate_widget)
	{
		if (user_clicked)
			SetKeyOwner(ImGuiKey_MouseLeft, id);
		SetActiveID(id, window);
		SetFocusID(id, window);
		FocusWindow(window);
	}

	// Draw background
	window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, background_color, 5);

	// Draw input field background
	ImU32 input_bg_color = hovered ? GetColorWithAlpha(second_color, 0.8f) : second_color;
	window->DrawList->AddRectFilled(input_bb.Min, input_bb.Max, input_bg_color, 3);

	// Draw label
	const ImVec2 label_pos = ImVec2(total_bb.Min.x + 15, total_bb.Min.y + (item_size.y - label_size.y) * 0.5f);
	window->DrawList->AddText(label_pos, text_color[0], label);

	// Calculate input text area (leave space for label)
	const float label_width = label_size.x + 20;
	const ImRect text_bb(input_bb.Min + ImVec2(label_width, 0), input_bb.Max - ImVec2(8, 0));

	// Handle input text
	bool value_changed = false;
	if (g.ActiveId == id)
	{
		// Create a temporary child window for the input text to avoid conflicts
		ImGui::SetCursorScreenPos(text_bb.Min);
		ImGui::PushClipRect(text_bb.Min, text_bb.Max, true);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0)); // Transparent background
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(0, 0, 0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::SetNextItemWidth(text_bb.GetWidth());
		value_changed = ImGui::InputText("##custominput", buf, buf_size, flags | ImGuiInputTextFlags_NoHorizontalScroll);
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::PopClipRect();
	}
	else
	{
		// Draw the text when not active
		const char* buf_display = buf;
		if (flags & ImGuiInputTextFlags_Password)
		{
			static const char* password_display = "********";
			buf_display = password_display;
		}

		const ImVec2 text_pos = ImVec2(text_bb.Min.x + 5, text_bb.Min.y + (text_bb.GetHeight() - GetTextLineHeight()) * 0.5f);
		window->DrawList->AddText(text_pos, text_color[1], buf_display);
	}

	return value_changed;
}

bool ImGui::CustomSliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float w = CalcItemWidth();

	const ImVec2 item_size = frame_size + ImVec2(0, 15);

	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + item_size);

	const ImRect slider_bb(total_bb.Min + ImVec2(12, 41), total_bb.Max - ImVec2(14, 15));

	const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
	ItemSize(total_bb, style.FramePadding.y);
	ItemAdd(total_bb, id, &total_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0);

	if (format == NULL)
		format = DataTypeGetInfo(data_type)->PrintFmt;

	static std::map<ImGuiID, Slider_State> anim;
	auto it_anim = anim.find(id);

	if (it_anim == anim.end())
	{
		anim.insert({ id, Slider_State() });
		it_anim = anim.find(id);
	}

	const bool hovered = ItemHoverable(total_bb, id, ImGuiItemFlags_::ImGuiItemFlags_None);
	bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
	if (!temp_input_is_active)
	{
		const bool clicked = hovered && IsMouseClicked(0, ImGuiInputFlags_::ImGuiInputFlags_None, id);
		const bool make_active = (clicked || g.NavActivateId == id);
		if (make_active && clicked)
			SetKeyOwner(ImGuiKey_MouseLeft, id);
		if (make_active && temp_input_allowed)
			if ((clicked && g.IO.KeyCtrl) || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
				temp_input_is_active = true;

		if (make_active && temp_input_allowed)
			if (make_active && !temp_input_is_active)
			{
				SetActiveID(id, window);
				SetFocusID(id, window);
				FocusWindow(window);
				g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
			}
	}

	if (temp_input_is_active)
	{
		const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
		return TempInputScalar(total_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
	}

	RenderNavHighlight(total_bb, id);

	ImRect grab_bb;
	const bool value_changed = SliderBehavior(slider_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
	if (value_changed)
		MarkItemEdited(id);

	it_anim->second.grab_bb_center_offset = ImLerp(it_anim->second.grab_bb_center_offset, grab_bb.Max - slider_bb.Min, 1.0f);

	char value_buf[64];
	const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

	PushFont(small_font);
	const ImRect input_bb(total_bb.Min + ImVec2(275 - CalcTextSize(value_buf).x, 7), total_bb.Max - ImVec2(14, 28));
	PopFont();

	it_anim->second.hint_frame_color = ImLerp(it_anim->second.hint_frame_color, hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left) ? GetColorWithAlpha(main_color, 0.3f) : second_color, anim_speed);
	it_anim->second.hint_color = ImLerp(it_anim->second.hint_color, hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left) ? main_color : text_color[1], anim_speed);

	PushStyleVar(ImGuiStyleVar_WindowRounding, style.FrameRounding);
	//CustomBlur(total_bb.Min, total_bb.Max);
	PopStyleVar();

	window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, background_color, 5);

	window->DrawList->AddRectFilled(slider_bb.Min, slider_bb.Max, second_color, 360);

	window->DrawList->AddRectFilled(input_bb.Min, input_bb.Max, GetColorU32(it_anim->second.hint_frame_color), 5);

	window->DrawList->AddRectFilled(slider_bb.Min, ImVec2(slider_bb.Min.x + it_anim->second.grab_bb_center_offset.x, slider_bb.Max.y), main_color, 360);

	window->DrawList->AddShadowRect(slider_bb.Min, ImVec2(slider_bb.Min.x + it_anim->second.grab_bb_center_offset.x - 5, slider_bb.Max.y), main_color, 30.f, ImVec2(0, 0), 0, 360);

	window->DrawList->AddCircleFilled(ImVec2(slider_bb.Min.x + it_anim->second.grab_bb_center_offset.x - 5, slider_bb.GetCenter().y), 5.5f, text_color[0]);

	window->DrawList->AddCircleFilled(ImVec2(slider_bb.Min.x + it_anim->second.grab_bb_center_offset.x - 5, slider_bb.GetCenter().y), 3.5f, text_color[2]);

	// Handle mouse wheel for different data types
	if (hovered && ImGui::GetIO().MouseWheel != 0.0f)
	{
		if (data_type == ImGuiDataType_Float)
		{
			float wheel_delta = ImGui::GetIO().MouseWheel * 10.0f; // Adjust step size as needed
			float new_value = *(float*)p_data + wheel_delta;
			float min_val = *(float*)p_min;
			float max_val = *(float*)p_max;
			new_value = ImClamp(new_value, min_val, max_val);
			*(float*)p_data = new_value;
		}
		else if (data_type == ImGuiDataType_S32)
		{
			if (ImGui::GetIO().MouseWheel > 0 && *(int*)p_data + 1 < *(int*)p_max)
				*(int*)p_data += (int)ImGui::GetIO().MouseWheel;
			if (ImGui::GetIO().MouseWheel < 0 && *(int*)p_data - 1 > *(int*)p_min)
				*(int*)p_data += (int)ImGui::GetIO().MouseWheel;
		}
	}

	window->DrawList->AddText(total_bb.Min + ImVec2(12, 10), text_color[0], label);

	PushFont(small_font);
	window->DrawList->AddText(GetTextCenterPosition(input_bb.Min, input_bb.Max, value_buf), GetColorU32(it_anim->second.hint_color), value_buf);
	PopFont();

	return value_changed;
}

bool ImGui::CustomColorEdit(const char* label, float col[4], ImGuiColorEditFlags flags, bool render_frame)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const float square_sz = GetFrameHeight();
	const float w_full = CalcItemWidth();
	const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
	const float w_inputs = w_full;
	const char* label_display_end = FindRenderedTextEnd(label);
	g.NextItemData.ClearFlags();

	const ImVec2 pos = window->DC.CursorPos;

	const ImRect total_bb(pos, pos + frame_size);

	ItemSize(frame_size - ImVec2(0, 5), style.FramePadding.y);
	ItemAdd(total_bb, GetID(label));

	BeginGroup();
	PushID(label);

	// If we're not showing any slider there's no point in doing any HSV conversions
	const ImGuiColorEditFlags flags_untouched = flags;
	if (flags & ImGuiColorEditFlags_NoInputs)
		flags = (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

	// Context menu: display and modify options (before defaults are applied)
	//if (!(flags & ImGuiColorEditFlags_NoOptions))
	//    ColorEditOptionsPopup(col, flags);

	// Read stored options
	if (!(flags & ImGuiColorEditFlags_DisplayMask_))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DisplayMask_);
	if (!(flags & ImGuiColorEditFlags_DataTypeMask_))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DataTypeMask_);
	if (!(flags & ImGuiColorEditFlags_PickerMask_))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_);
	if (!(flags & ImGuiColorEditFlags_InputMask_))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags_InputMask_);
	flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_));
	IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_DisplayMask_)); // Check that only 1 is selected
	IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));   // Check that only 1 is selected

	const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
	const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
	const int components = alpha ? 4 : 3;

	// Convert to the formats we need
	float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
	if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
		ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
	else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
	{
		// Hue is lost when converting from greyscale rgb (saturation=0). Restore it.
		ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
		//ColorEditRestoreHS(col, &f[0], &f[1], &f[2]);
	}
	int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

	bool value_changed = false;
	bool value_changed_as_float = false;

	const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;
	window->DC.CursorPos.x = pos.x;
	window->DC.CursorPos.y = pos.y - 10;

	if ((flags & ImGuiColorEditFlags_DisplayHex) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
	{
		PushFont(small_font);
		// RGB Hexadecimal Input
		char buf[64];
		if (alpha)
			ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
		else
			ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));

		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(10, 10));
		ImGui::BeginGroup();
		{
			SetNextItemWidth(w_inputs - 120);
			if (InputText("##Text", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase))
			{
				value_changed = true;
				char* p = buf;
				while (*p == '#' || ImCharIsBlankA(*p))
					p++;
				i[0] = i[1] = i[2] = 0;
				i[3] = 0xFF; // alpha default to 255 is not parsed by scanf (e.g. inputting #FFFFFF omitting alpha)
				int r;
				if (alpha)
					r = sscanf(p, "%02X%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2], (unsigned int*)&i[3]); // Treat at unsigned (%X is unsigned)
				else
					r = sscanf(p, "%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2]);
				IM_UNUSED(r); // Fixes C6031: Return value ignored: 'sscanf'.
			}

			if (!(flags & ImGuiColorEditFlags_NoOptions))
				OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
			PopFont();

			PushFont(icon_font);

			ImGui::SameLine();
			if (ImGui::Button("6", ImVec2(35, 35)))
			{
				for (int i = 0; i < 4; i++)
					ColorCopied[i] = col[i];
			}

			ImGui::SameLine();

			if (ImGui::Button("4", ImVec2(35, 35)))
			{
				for (int i = 0; i < 4; i++)
					col[i] = ColorCopied[i];
			}

			ImGui::SameLine();

			if (ImGui::ToggleButton("3", ImVec2(35, 35), 0, &picker))
			{
				picker = !picker;
			}

			PopFont();

			static COLORREF color;

			static COLORREF color2;

			static float picker_rect_color[9][4];

			if (picker)
			{
				POINT p;
				GetCursorPos(&p);

				HDC dc = GetDC(0);
				color = GetPixel(dc, p.x, p.y);
				ReleaseDC(NULL, dc);

				ImVec2 picker_rect_start = ImGui::GetMousePos() - ImVec2(20, 20);

				GetForegroundDrawList()->AddCircleFilled(picker_rect_start, 15.f, ImColor((float)GetRValue(color) / 255.f, (float)GetGValue(color) / 255.f, (float)GetBValue(color) / 255.f, 1.f));
				GetForegroundDrawList()->AddCircle(picker_rect_start, 15.f, stroke_color);

				if (IsMouseReleased(ImGuiMouseButton_Left) && !IsItemHovered()) {
					col[0] = (float)GetRValue(color) / 255.f;
					col[1] = (float)GetGValue(color) / 255.f;
					col[2] = (float)GetBValue(color) / 255.f;
					col[3] = 255.f;

					picker = false;
				}
			}
		}
		ImGui::EndGroup();
	}

	static std::map<ImGuiID, ColorEdit_State> anim;
	auto it_anim = anim.find(GetID(label));

	if (it_anim == anim.end())
	{
		anim.insert({ GetID(label), ColorEdit_State() });
		it_anim = anim.find(GetID(label));
	}

	ImGuiWindow* picker_active_window = NULL;
	if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
	{
		const float button_offset_x = ((flags & ImGuiColorEditFlags_NoInputs) || (style.ColorButtonPosition == ImGuiDir_Left)) ? 0.0f : w_inputs + style.ItemInnerSpacing.x;

		const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);

		if (render_frame) {
			PushStyleVar(ImGuiStyleVar_WindowRounding, style.FrameRounding);
			//CustomBlur(total_bb.Min, total_bb.Max);
			PopStyleVar();

			window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, background_color, style.FrameRounding);
		}

		ImGui::SetCursorScreenPos(total_bb.Max - ImVec2(50, 44.0f));

		if (ColorButton("##ColorButton", col_v4, flags))
		{
			if (!(flags & ImGuiColorEditFlags_NoPicker))
			{
				// Store current color and open a picker
				g.ColorPickerRef = col_v4;
				it_anim->second.picker_active = !it_anim->second.picker_active;
				SetNextWindowPos(g.LastItemData.Rect.GetBL() + ImVec2(0.0f, style.ItemSpacing.y));
			}
		}

		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

		PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
		PushStyleColor(ImGuiCol_PopupBg, ImColorToImVec4(child_color));
		PushStyleColor(ImGuiCol_Border, ImColorToImVec4(child_color));
		if (it_anim->second.picker_active)
		{
			ImGui::SetNextWindowSize(ImVec2(310, 260));
			ImGui::Begin("picker_edit", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysUseWindowPadding);
			{
				ImRect picker_bb(GetWindowPos(), GetWindowPos() + ImGui::GetWindowSize());

				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseHoveringRect(picker_bb.Min, picker_bb.Max, false) && !picker)
					it_anim->second.picker_active = false;

				picker_active_window = g.CurrentWindow;
				ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
				ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
				SetNextItemWidth(square_sz * 16.0f); // Use 256 + bar sizes?
				value_changed |= CustomColorPicker("##picker", col, picker_flags);
			}
			End();
		}
		PopStyleVar(3);
		PopStyleColor(2);
	}

	it_anim->second.color = ImLerp(it_anim->second.color, picker_active_window == NULL ? text_color[1] : text_color[0], anim_speed);

	if (label != label_display_end && !(flags & ImGuiColorEditFlags_NoLabel))
	{
		// Position not necessarily next to last submitted button (e.g. if style.ColorButtonPosition == ImGuiDir_Left),
		// but we need to use SameLine() to setup baseline correctly. Might want to refactor SameLine() to simplify this.
		SameLine(0.0f, style.ItemInnerSpacing.x);
		window->DC.CursorPos.x = pos.x + ((flags & ImGuiColorEditFlags_NoInputs) ? w_button : w_full + style.ItemInnerSpacing.x);

		if (render_frame)
			window->DrawList->AddText(ImVec2(total_bb.Min.x + 12.f, GetTextCenterPosition(total_bb.Min, total_bb.Max, label).y), text_color[0], label);
	}

	// Convert back
	if (value_changed && picker_active_window == NULL)
	{
		if (!value_changed_as_float)
			for (int n = 0; n < 4; n++)
				f[n] = i[n] / 255.0f;
		if ((flags & ImGuiColorEditFlags_DisplayHSV) && (flags & ImGuiColorEditFlags_InputRGB))
		{
			g.ColorEditSavedHue = f[0];
			g.ColorEditSavedSat = f[1];
			ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
			g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(f[0], f[1], f[2], 0));
		}
		if ((flags & ImGuiColorEditFlags_DisplayRGB) && (flags & ImGuiColorEditFlags_InputHSV))
			ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

		col[0] = f[0];
		col[1] = f[1];
		col[2] = f[2];
		if (alpha)
			col[3] = f[3];
	}

	PopID();
	EndGroup();

	// When picker is being actively used, use its active id so IsItemActive() will function on ColorEdit4().
	if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
		g.LastItemData.ID = g.ActiveId;

	if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
		MarkItemEdited(g.LastItemData.ID);

	return value_changed;
}

static float picker_offset;

bool ImGui::CustomColorPicker(const char* label, float col[4], ImGuiColorEditFlags flags)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImDrawList* draw_list = GetWindowDrawList();
	ImGuiStyle& style = g.Style;
	ImGuiIO& io = g.IO;


	const ImVec2 color_frame_size(280, 160);
	const float width = CalcItemWidth();
	g.NextItemData.ClearFlags();

	PushID(label);
	BeginGroup();

	if (!(flags & ImGuiColorEditFlags_NoSidePreview))
		flags |= ImGuiColorEditFlags_NoSmallPreview;

	// Context menu: display and store options.
	if (!(flags & ImGuiColorEditFlags_NoOptions))
		ColorPickerOptionsPopup(col, flags);

	// Read stored options
	if (!(flags & ImGuiColorEditFlags_PickerMask_))
		flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_PickerMask_;
	if (!(flags & ImGuiColorEditFlags_InputMask_))
		flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_InputMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_InputMask_;
	IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_)); // Check that only 1 is selected
	IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));  // Check that only 1 is selected
	if (!(flags & ImGuiColorEditFlags_NoOptions))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

	static std::map<ImGuiID, ColorPicker_State> anim;
	auto it_anim = anim.find(GetID(label));

	if (it_anim == anim.end())
	{
		anim.insert({ GetID(label), ColorPicker_State() });
		it_anim = anim.find(GetID(label));
	}

	// Setup
	int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
	bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
	ImVec2 picker_pos = window->DC.CursorPos + ImVec2(15, 15);
	float square_sz = GetFrameHeight();
	float bars_width = 10; // Arbitrary smallish width of Hue/Alpha picking bars
	float sv_picker_size = ImMax(bars_width * 1, width - (alpha_bar ? 2 : 1) * (bars_width + style.ItemInnerSpacing.x)) / 2.5f; // Saturation/Value picking box
	float bar0_pos_x = picker_pos.x + sv_picker_size + style.ItemInnerSpacing.x;
	float bar1_pos_x = bar0_pos_x + bars_width + style.ItemInnerSpacing.x;
	float bars_triangles_half_sz = IM_FLOOR(bars_width * 0.20f);

	float backup_initial_col[4];
	memcpy(backup_initial_col, col, components * sizeof(float));

	float wheel_thickness = sv_picker_size * 0.08f;
	float wheel_r_outer = sv_picker_size * 0.50f;
	float wheel_r_inner = wheel_r_outer - wheel_thickness;
	ImVec2 wheel_center(picker_pos.x + (sv_picker_size + bars_width) * 0.5f, picker_pos.y + sv_picker_size * 0.5f);

	// Note: the triangle is displayed rotated with triangle_pa pointing to Hue, but most coordinates stays unrotated for logic.
	float triangle_r = wheel_r_inner - (int)(sv_picker_size * 0.027f);
	ImVec2 triangle_pa = ImVec2(triangle_r, 0.0f); // Hue point.
	ImVec2 triangle_pb = ImVec2(triangle_r * -0.5f, triangle_r * -0.866025f); // Black point.
	ImVec2 triangle_pc = ImVec2(triangle_r * -0.5f, triangle_r * +0.866025f); // White point.

	float H = col[0], S = col[1], V = col[2];
	float R = col[0], G = col[1], B = col[2];
	if (flags & ImGuiColorEditFlags_InputRGB)
	{
		// Hue is lost when converting from greyscale rgb (saturation=0). Restore it.
		ColorConvertRGBtoHSV(R, G, B, H, S, V);
		//ColorEditRestoreHS(col, &H, &S, &V);
	}
	else if (flags & ImGuiColorEditFlags_InputHSV)
	{
		ColorConvertHSVtoRGB(H, S, V, R, G, B);
	}

	bool value_changed = false, value_changed_h = false, value_changed_sv = false;

	PushItemFlag(ImGuiItemFlags_NoNav, true);
	if (flags & ImGuiColorEditFlags_PickerHueWheel)
	{
		// Hue wheel + SV triangle logic
		InvisibleButton("hsv", ImVec2(sv_picker_size + style.ItemInnerSpacing.x + bars_width, sv_picker_size));
		if (IsItemActive())
		{
			ImVec2 initial_off = g.IO.MouseClickedPos[0] - wheel_center;
			ImVec2 current_off = g.IO.MousePos - wheel_center;
			float initial_dist2 = ImLengthSqr(initial_off);
			if (initial_dist2 >= (wheel_r_inner - 1) * (wheel_r_inner - 1) && initial_dist2 <= (wheel_r_outer + 1) * (wheel_r_outer + 1))
			{
				// Interactive with Hue wheel
				H = ImAtan2(current_off.y, current_off.x) / IM_PI * 0.5f;
				if (H < 0.0f)
					H += 1.0f;
				value_changed = value_changed_h = true;
			}
			float cos_hue_angle = ImCos(-H * 2.0f * IM_PI);
			float sin_hue_angle = ImSin(-H * 2.0f * IM_PI);
			if (ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, ImRotate(initial_off, cos_hue_angle, sin_hue_angle)))
			{
				// Interacting with SV triangle
				ImVec2 current_off_unrotated = ImRotate(current_off, cos_hue_angle, sin_hue_angle);
				if (!ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated))
					current_off_unrotated = ImTriangleClosestPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated);
				float uu, vv, ww;
				ImTriangleBarycentricCoords(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated, uu, vv, ww);
				V = ImClamp(1.0f - vv, 0.0001f, 1.0f);
				S = ImClamp(uu / V, 0.0001f, 1.0f);
				value_changed = value_changed_sv = true;
			}
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
	}
	else if (flags & ImGuiColorEditFlags_PickerHueBar)
	{

		//CustomBlur(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize());
		GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize(), GetColorWithAlpha(background_color, 0.4f), 10.f);
		GetWindowDrawList()->AddRect(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize(), stroke_color, 10.f);

		// SV rectangle logic
		InvisibleButton("sv", color_frame_size);
		if (IsItemActive())
		{
			S = ImSaturate((io.MousePos.x - picker_pos.x) / (color_frame_size.x - 1));
			V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (color_frame_size.y - 1));

			// Greatly reduces hue jitter and reset to 0 when hue == 255 and color is rapidly modified using SV square.
			if (g.ColorEditSavedColor == ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
				H = g.ColorEditSavedHue;
			value_changed = value_changed_sv = true;
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

		// Hue bar logic
		SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y + color_frame_size.y + 10));
		InvisibleButton("hue", ImVec2(picker_pos.x + color_frame_size.x, 10));
		if (IsItemActive())
		{
			H = ImSaturate((io.MousePos.x - picker_pos.x) / (color_frame_size.x - 1));
			value_changed = value_changed_h = true;
		}
	}

	// Alpha bar logic
	if (alpha_bar)
	{
		SetCursorScreenPos(ImVec2(bar1_pos_x, picker_pos.y));
		InvisibleButton("alpha", ImVec2(bars_width, sv_picker_size));
		if (IsItemActive())
		{
			col[3] = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
			value_changed = true;
		}
	}
	PopItemFlag(); // ImGuiItemFlags_NoNav

	// Convert back color to RGB
	if (value_changed_h || value_changed_sv)
	{
		if (flags & ImGuiColorEditFlags_InputRGB)
		{
			ColorConvertHSVtoRGB(H, S, V, col[0], col[1], col[2]);
			g.ColorEditSavedHue = H;
			g.ColorEditSavedSat = S;
			g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0));
		}
		else if (flags & ImGuiColorEditFlags_InputHSV)
		{
			col[0] = H;
			col[1] = S;
			col[2] = V;
		}
	}

	// R,G,B and H,S,V slider color editor
	bool value_changed_fix_hue_wrap = false;
	if ((flags & ImGuiColorEditFlags_NoInputs) == 0)
	{
		PushItemWidth((alpha_bar ? bar1_pos_x : bar0_pos_x) + bars_width - picker_pos.x);
		ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
		ImGuiColorEditFlags sub_flags = (flags & sub_flags_to_forward) | ImGuiColorEditFlags_NoPicker;

		if (flags & ImGuiColorEditFlags_DisplayHex || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
			value_changed |= ColorEdit4("##hex", col, sub_flags | ImGuiColorEditFlags_DisplayHex);
		PopItemWidth();
	}

	// Try to cancel hue wrap (after ColorEdit4 call), if any
	if (value_changed_fix_hue_wrap && (flags & ImGuiColorEditFlags_InputRGB))
	{
		float new_H, new_S, new_V;
		ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);
		if (new_H <= 0 && H > 0)
		{
			if (new_V <= 0 && V != new_V)
				ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
			else if (new_S <= 0)
				ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
		}
	}

	if (value_changed)
	{
		if (flags & ImGuiColorEditFlags_InputRGB)
		{
			R = col[0];
			G = col[1];
			B = col[2];
			ColorConvertRGBtoHSV(R, G, B, H, S, V);
			//ColorEditRestoreHS(col, &H, &S, &V);   // Fix local Hue as display below will use it immediately.
		}
		else if (flags & ImGuiColorEditFlags_InputHSV)
		{
			H = col[0];
			S = col[1];
			V = col[2];
			ColorConvertHSVtoRGB(H, S, V, R, G, B);
		}
	}

	const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
	const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
	const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
	const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
	const ImU32 col_hues[6 + 1] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(0,255,255,style_alpha8), IM_COL32(0,0,255,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

	ImVec4 hue_color_f(1, 1, 1, style.Alpha); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
	ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
	ImU32 user_col32_striped_of_alpha = ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha)); // Important: this is still including the main rendering/style alpha!!

	ImVec2 sv_cursor_pos;

	if (flags & ImGuiColorEditFlags_PickerHueWheel)
	{
		// Render Hue Wheel
		const float aeps = 0.5f / wheel_r_outer; // Half a pixel arc length in radians (2pi cancels out).
		const int segment_per_arc = ImMax(4, (int)wheel_r_outer / 12);
		for (int n = 0; n < 6; n++)
		{
			const float a0 = (n) / 6.0f * 2.0f * IM_PI - aeps;
			const float a1 = (n + 1.0f) / 6.0f * 2.0f * IM_PI + aeps;
			const int vert_start_idx = draw_list->VtxBuffer.Size;
			draw_list->PathArcTo(wheel_center, (wheel_r_inner + wheel_r_outer) * 0.5f, a0, a1, segment_per_arc);
			draw_list->PathStroke(col_white, 0, wheel_thickness);
			const int vert_end_idx = draw_list->VtxBuffer.Size;

			// Paint colors over existing vertices
			ImVec2 gradient_p0(wheel_center.x + ImCos(a0) * wheel_r_inner, wheel_center.y + ImSin(a0) * wheel_r_inner);
			ImVec2 gradient_p1(wheel_center.x + ImCos(a1) * wheel_r_inner, wheel_center.y + ImSin(a1) * wheel_r_inner);
			ShadeVertsLinearColorGradientKeepAlpha(draw_list, vert_start_idx, vert_end_idx, gradient_p0, gradient_p1, col_hues[n], col_hues[n + 1]);
		}

		// Render Cursor + preview on Hue Wheel
		float cos_hue_angle = ImCos(H * 2.0f * IM_PI);
		float sin_hue_angle = ImSin(H * 2.0f * IM_PI);
		ImVec2 hue_cursor_pos(wheel_center.x + cos_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f, wheel_center.y + sin_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f);
		float hue_cursor_rad = value_changed_h ? wheel_thickness * 0.65f : wheel_thickness * 0.55f;
		int hue_cursor_segments = ImClamp((int)(hue_cursor_rad / 1.4f), 9, 32);
		draw_list->AddCircleFilled(hue_cursor_pos, hue_cursor_rad, hue_color32, hue_cursor_segments);
		draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad + 1, col_midgrey, hue_cursor_segments);
		draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad, col_white, hue_cursor_segments);

		// Render SV triangle (rotated according to hue)
		ImVec2 tra = wheel_center + ImRotate(triangle_pa, cos_hue_angle, sin_hue_angle);
		ImVec2 trb = wheel_center + ImRotate(triangle_pb, cos_hue_angle, sin_hue_angle);
		ImVec2 trc = wheel_center + ImRotate(triangle_pc, cos_hue_angle, sin_hue_angle);
		ImVec2 uv_white = GetFontTexUvWhitePixel();
		draw_list->PrimReserve(6, 6);
		draw_list->PrimVtx(tra, uv_white, hue_color32);
		draw_list->PrimVtx(trb, uv_white, hue_color32);
		draw_list->PrimVtx(trc, uv_white, col_white);
		draw_list->PrimVtx(tra, uv_white, 0);
		draw_list->PrimVtx(trb, uv_white, col_black);
		draw_list->PrimVtx(trc, uv_white, 0);
		draw_list->AddTriangle(tra, trb, trc, col_midgrey, 1.5f);
		it_anim->second.sv_cursor_pos = ImLerp(ImLerp(trc, tra, ImSaturate(S)), trb, ImSaturate(1 - V));
	}
	else if (flags & ImGuiColorEditFlags_PickerHueBar)
	{
		// Render SV Square
		draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + color_frame_size, col_white, hue_color32, hue_color32, col_white);// , 5.f, ImDrawFlags_RoundCornersAll);
		draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + color_frame_size, 0, 0, col_black, col_black);// , 3.f, ImDrawFlags_RoundCornersAll);

		it_anim->second.sv_cursor_pos.x = ImLerp(it_anim->second.sv_cursor_pos.x, picker_pos.x - ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * color_frame_size.x), picker_pos.x + 2, picker_pos.x + color_frame_size.x - 2), g.IO.DeltaTime * 30.f); // Sneakily prevent the circle to stick out too much
		it_anim->second.sv_cursor_pos.y = ImLerp(it_anim->second.sv_cursor_pos.y, picker_pos.y - ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * color_frame_size.y), picker_pos.y + 2, picker_pos.y + color_frame_size.y - 2), g.IO.DeltaTime * 30.f);

		for (int i = 0; i < 6; ++i) {
			draw_list->AddRectFilledMultiColor(ImVec2(picker_pos.x + i * (color_frame_size.x / 6), picker_pos.y + color_frame_size.y + 10),
				ImVec2(picker_pos.x + (i + 1) * (color_frame_size.x / 5.95), picker_pos.y + color_frame_size.y + 20),
				col_hues[i],
				col_hues[i + 1],
				col_hues[i + 1],
				col_hues[i]);
		}

		picker_offset += ImGui::GetIO().MouseWheel;

		it_anim->second.bar0_line_y = ImLerp(it_anim->second.bar0_line_y, picker_pos.x - IM_ROUND((picker_pos.x + 5.f) + H * (color_frame_size.x - 12.f)), anim_speed);
		it_anim->second.sv_cursor_rad = ImLerp(it_anim->second.sv_cursor_rad, value_changed_sv ? 8.0f : 5.0f, io.DeltaTime * 6.f);
		draw_list->AddCircleFilled(it_anim->second.sv_cursor_pos, it_anim->second.sv_cursor_rad, user_col32_striped_of_alpha, 60);
		draw_list->AddCircle(picker_pos - it_anim->second.sv_cursor_pos, it_anim->second.sv_cursor_rad + 1, col_midgrey, 60);
		draw_list->AddCircle(picker_pos - it_anim->second.sv_cursor_pos, it_anim->second.sv_cursor_rad, col_white, 60);

		draw_list->AddCircle(ImVec2(picker_pos.x - it_anim->second.bar0_line_y, picker_pos.y + color_frame_size.y + 15), 4, col_midgrey, 60);
		draw_list->AddCircle(ImVec2(picker_pos.x - it_anim->second.bar0_line_y, picker_pos.y + color_frame_size.y + 15), 3, col_white, 60);

	}

	// Render cursor/preview circle (clamp S/V within 0..1 range because floating points colors may lead HSV values to be out of range)
	it_anim->second.sv_cursor_rad = ImLerp(it_anim->second.sv_cursor_rad, value_changed_sv ? 6.0f : 3.0f, anim_speed * 2);
	draw_list->AddCircleFilled(it_anim->second.sv_cursor_pos, it_anim->second.sv_cursor_rad, user_col32_striped_of_alpha, 12);
	draw_list->AddCircle(it_anim->second.sv_cursor_pos, it_anim->second.sv_cursor_rad + 1, col_midgrey, 12);
	draw_list->AddCircle(it_anim->second.sv_cursor_pos, it_anim->second.sv_cursor_rad, col_white, 12);

	// Render alpha bar
	if (alpha_bar)
	{
		float alpha = ImSaturate(col[3]);
		ImRect bar1_bb(bar1_pos_x, picker_pos.y, bar1_pos_x + bars_width, picker_pos.y + sv_picker_size);
		RenderColorRectWithAlphaCheckerboard(draw_list, bar1_bb.Min, bar1_bb.Max, 0, bar1_bb.GetWidth() / 2.0f, ImVec2(0.0f, 0.0f), 10.f, ImDrawFlags_RoundCornersAll);
		draw_list->AddRectFilledMultiColor(bar1_bb.Min, bar1_bb.Max, user_col32_striped_of_alpha, user_col32_striped_of_alpha, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, user_col32_striped_of_alpha & ~IM_COL32_A_MASK);// , 10.f, ImDrawFlags_RoundCornersAll);
		it_anim->second.bar1_line_y = ImLerp(it_anim->second.bar1_line_y, IM_ROUND(picker_pos.y + (1.0f - alpha) * sv_picker_size), anim_speed * 2);

		draw_list->AddRect(ImVec2(bar1_pos_x - 2, it_anim->second.bar1_line_y - 4), ImVec2(bar1_pos_x - 1 + bars_width + 3.0f, it_anim->second.bar1_line_y + 4), col_midgrey, 30.f);
		draw_list->AddRect(ImVec2(bar1_pos_x - 1, it_anim->second.bar1_line_y - 3), ImVec2(bar1_pos_x - 1 + bars_width + 2.0f, it_anim->second.bar1_line_y + 3), col_white, 30.f);
	}

	EndGroup();


	if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0)
		value_changed = false;
	if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
		MarkItemEdited(g.LastItemData.ID);

	PopID();

	return value_changed;
}

bool ImGui::ToggleButton(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags, bool* toggled)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);


	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	static std::map<ImGuiID, ToggleButton_State> anim;
	auto it_anim = anim.find(id);

	if (it_anim == anim.end())
	{
		anim.insert({ id, ToggleButton_State() });
		it_anim = anim.find(id);
	}

	const ImRect bb(pos, pos + size);
	ItemSize(size, style.FramePadding.y);
	ItemAdd(bb, id);

	//if (g.LastItemData.ItemFlags & ImGuiItemFlags_ButtonRepeat)
	//    flags |= ImGuiButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	it_anim->second.frame_color = ImLerp(it_anim->second.frame_color, *toggled ? main_color : GetColorWithAlpha(main_color, 0.3f), anim_speed);

	PushStyleVar(ImGuiStyleVar_WindowRounding, style.FrameRounding);
	//CustomBlur(bb.Min, bb.Max);
	PopStyleVar();

	window->DrawList->AddRectFilled(bb.Min, bb.Max, background_color, style.FrameRounding);

	window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(it_anim->second.frame_color), style.FrameRounding);

	window->DrawList->AddText(GetTextCenterPosition(bb.Min, bb.Max, label), text_color[0], label);

	return pressed;
}

static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = (const char* const*)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}

static float CalcMaxPopupHeightFromItemCount(int items_count)
{
	ImGuiContext& g = *GImGui;
	if (items_count <= 0)
		return FLT_MAX;
	return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool ImGui::CustomCombo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
{
	const bool value_changed = CustomCombo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
	return value_changed;
}

bool ImGui::CustomCombo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
{
	ImGuiContext& g = *GImGui;

	// Call the getter to obtain the preview string which is a parameter to BeginCombo()
	const char* preview_value = NULL;
	if (*current_item >= 0 && *current_item < items_count)
		items_getter(data, *current_item, &preview_value);

	// The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
	if (popup_max_height_in_items != -1 && !(g.NextWindowData.WindowFlags & ImGuiNextWindowDataFlags_HasSizeConstraint))
		SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

	if (!BeginCustomCombo(label, preview_value, ImGuiComboFlags_None, items_count))
		return false;

	// Display items
	// FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
	bool value_changed = false;
	for (int i = 0; i < items_count; i++)
	{
		PushID(i);
		const bool item_selected = (i == *current_item);
		const char* item_text;
		if (!items_getter(data, i, &item_text))
			item_text = "*Unknown item*";
		if (CustomSelectable(item_text, item_selected))
		{
			value_changed = true;
			*current_item = i;
		}
		if (item_selected)
			SetItemDefaultFocus();
		PopID();
	}

	EndCustomCombo();

	if (value_changed)
		MarkItemEdited(g.LastItemData.ID);

	return value_changed;
}

void ImGui::EndCustomCombo()
{
	PopStyleVar(2);
	End();
}

struct combo_state {
	bool hovered;
	bool opened_combo;
	float combo_size;
	float size_offset;
	ImVec4 text_color;

	ImVec4 frame_color;
	ImVec4 preview_text_color;
	float text_size;

	float combo_rotate;

	ImVec4 border_color;

	ImVec4 arrow_color;

	float preview_value_size;
};

bool ImGui::BeginCustomCombo(const char* label, const char* preview_value, ImGuiComboFlags flags, int value)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();

	ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.WindowFlags;
	g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
	if (window->SkipItems)
		return false;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	static std::map<ImGuiID, combo_state> anim;
	auto it_anim = anim.find(id);

	if (it_anim == anim.end())
	{
		anim.insert({ id, combo_state() });
		it_anim = anim.find(id);
	}

	ImVec2 pos = window->DC.CursorPos;

	PushFont(small_font);
	it_anim->second.preview_value_size = ImLerp(it_anim->second.preview_value_size, CalcTextSize(preview_value).x > 100.f ? CalcTextSize(preview_value).x : it_anim->second.opened_combo ? 100.f : CalcTextSize(preview_value).x, anim_speed);
	PopFont();

	const ImRect total_bb(pos, pos + frame_size);
	const ImRect rect_bb(total_bb.Min + ImVec2(249 - it_anim->second.preview_value_size, 11), total_bb.Max - ImVec2(14, 11));
	ItemSize(total_bb, style.FramePadding.y);
	ItemAdd(total_bb, id);

	bool hovered, held;
	bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

	it_anim->second.combo_size = ImLerp(it_anim->second.combo_size, it_anim->second.opened_combo ? value * 30.f : 0.f, anim_speed * 3);
	it_anim->second.combo_rotate = ImLerp(it_anim->second.combo_rotate, it_anim->second.opened_combo ? IM_PI / 2 : IM_PI / 1.5f, anim_speed);
	//it_anim->second.text_color = ImLerp(it_anim->second.text_color, it_anim->second.opened_combo ? text_color[0] : hovered ? text_color[1] : text_color[2], anim_speed);
	it_anim->second.preview_text_color = ImLerp(it_anim->second.preview_text_color, it_anim->second.opened_combo ? main_color : text_color[0], anim_speed);
	it_anim->second.frame_color = ImLerp(it_anim->second.frame_color, it_anim->second.opened_combo ? GetColorWithAlpha(main_color, 0.3f) : second_color, anim_speed);

	it_anim->second.text_size = ImLerp(it_anim->second.text_size, CalcTextSize(preview_value).x, anim_speed);
	it_anim->second.arrow_color = ImLerp(it_anim->second.arrow_color, it_anim->second.opened_combo ? main_color : text_color[0], anim_speed);

	PushStyleVar(ImGuiStyleVar_WindowRounding, style.FrameRounding);
	//CustomBlur(total_bb.Min, total_bb.Max);
	PopStyleVar();

	window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, background_color, style.FrameRounding);

	window->DrawList->AddRectFilled(rect_bb.Min, rect_bb.Max, GetColorU32(it_anim->second.frame_color), style.FrameRounding);

	window->DrawList->AddRectFilled(rect_bb.Max - ImVec2(25, 20), rect_bb.Max - ImVec2(4, 4), background_color, style.FrameRounding);

	PushFont(icon_font);
	window->DrawList->AddText(GetTextCenterPosition(rect_bb.Max - ImVec2(25, 20), rect_bb.Max - ImVec2(4, 4), "7"), GetColorU32(it_anim->second.arrow_color), "7");
	PopFont();

	window->DrawList->AddText(total_bb.Min + ImVec2(12, 12), text_color[0], label);

	ImVec2 label_size = CalcTextSize(label);

	const int vtx_idx_1 = window->DrawList->VtxBuffer.Size;

	PushFont(small_font);
	window->DrawList->AddText(ImVec2(rect_bb.Min.x + 9.5f, GetTextCenterPosition(rect_bb.Min, rect_bb.Max, preview_value).y), GetColorU32(it_anim->second.preview_text_color), preview_value);
	PopFont();

	const int vtx_idx_2 = window->DrawList->VtxBuffer.Size;

	if (hovered && g.IO.MouseClicked[0] || it_anim->second.opened_combo && g.IO.MouseClicked[0] && !it_anim->second.hovered)
		it_anim->second.opened_combo = !it_anim->second.opened_combo;

	if (!it_anim->second.opened_combo && it_anim->second.combo_size < 2.f)
		return false;

	ImGui::SetNextWindowPos(ImVec2(rect_bb.Min.x, rect_bb.Max.y));

	ImGui::SetNextWindowSize(ImVec2(rect_bb.GetWidth(), it_anim->second.combo_size));
	PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground;
	bool ret = Begin(label, NULL, window_flags);

	GetWindowDrawList()->AddRectFilled(GetWindowPos(), GetWindowPos() + GetWindowSize(), background_color, 10.f);
	//CustomBlur(GetWindowPos(), GetWindowPos() + GetWindowSize());

	it_anim->second.hovered = IsWindowHovered();
	PopStyleVar();

	return true;
}

struct selectable_state
{
	ImVec4 rect_color, text_color, frame_color, checkmark_color;
};

bool ImGui::CustomSelectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
	if (CustomSelectable(label, *p_selected, flags, size_arg))
	{
		*p_selected = !*p_selected;
		return true;
	}
	return false;
}

bool ImGui::CustomSelectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	// Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
	ImGuiID id = window->GetID(label);
	ImVec2 label_size = CalcTextSize(label, NULL, true);
	ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
	ImVec2 pos = window->DC.CursorPos;
	pos.y += window->DC.CurrLineTextBaseOffset;
	ItemSize(ImVec2(ImGui::GetWindowSize().x, 30), 0.0f);

	// Fill horizontal space
	// We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make explicitly right-aligned sizes not visibly match other widgets.
	const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;
	const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
	const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;
	if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
		size.x = ImMax(label_size.x, max_x - min_x);

	// Text stays at the submission position, but bounding box may be extended on both sides
	const ImVec2 text_min = pos;
	const ImVec2 text_max(min_x + size.x, pos.y + size.y);

	// Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
	ImRect bb(ImVec2(window->Pos.x, pos.y), ImVec2(window->Pos.x, pos.y) + ImVec2(ImGui::GetWindowSize().x, 30));
	//if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255)); }

	// Modify ClipRect for the ItemAdd(), faster than doing a PushColumnsBackground/PushTableBackground for every Selectable..
	const float backup_clip_rect_min_x = window->ClipRect.Min.x;
	const float backup_clip_rect_max_x = window->ClipRect.Max.x;
	if (span_all_columns)
	{
		window->ClipRect.Min.x = window->ParentWorkRect.Min.x;
		window->ClipRect.Max.x = window->ParentWorkRect.Max.x;
	}

	const bool disabled_item = (flags & ImGuiSelectableFlags_Disabled) != 0;
	const bool item_add = ItemAdd(bb, id, NULL, disabled_item ? ImGuiItemFlags_Disabled : ImGuiItemFlags_None);
	if (span_all_columns)
	{
		window->ClipRect.Min.x = backup_clip_rect_min_x;
		window->ClipRect.Max.x = backup_clip_rect_max_x;
	}

	if (!item_add)
		return false;

	static std::map<ImGuiID, selectable_state> anim;
	auto it_anim = anim.find(id);

	if (it_anim == anim.end())
	{
		anim.insert({ id, selectable_state() });
		it_anim = anim.find(id);
	}

	const bool disabled_global = (g.CurrentItemFlags & ImGuiItemFlags_Disabled) != 0;
	if (disabled_item && !disabled_global) // Only testing this as an optimization
		BeginDisabled();

	// FIXME: We can standardize the behavior of those two, we could also keep the fast path of override ClipRect + full push on render only,
	// which would be advantageous since most selectable are not selected.
	if (span_all_columns && window->DC.CurrentColumns)
		PushColumnsBackground();
	else if (span_all_columns && g.CurrentTable)
		TablePushBackgroundChannel();

	// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
	ImGuiButtonFlags button_flags = 0;
	if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
	if (flags & ImGuiSelectableFlags_NoSetKeyOwner) { button_flags |= ImGuiButtonFlags_NoSetKeyOwner; }
	if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
	if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
	if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
	if (flags & ImGuiSelectableFlags_AllowItemOverlap) { button_flags |= ImGuiButtonFlags_AllowOverlap; }

	const bool was_selected = selected;
	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);

	// Auto-select when moved into
	// - This will be more fully fleshed in the range-select branch
	// - This is not exposed as it won't nicely work with some user side handling of shift/control
	// - We cannot do 'if (g.NavJustMovedToId != id) { selected = false; pressed = was_selected; }' for two reasons
	//   - (1) it would require focus scope to be set, need exposing PushFocusScope() or equivalent (e.g. BeginSelection() calling PushFocusScope())
	//   - (2) usage will fail with clipped items
	//   The multi-select API aim to fix those issues, e.g. may be replaced with a BeginSelection() API.
	if ((flags & ImGuiSelectableFlags_SelectOnNav) && g.NavJustMovedToId != 0 && g.NavJustMovedToFocusScopeId == g.CurrentFocusScopeId)
		if (g.NavJustMovedToId == id)
			selected = pressed = true;

	// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
	if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
	{
		if (!g.NavHighlightItemUnderNav && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
		{
			SetNavID(id, window->DC.NavLayerCurrent, g.CurrentFocusScopeId, WindowRectAbsToRel(window, bb)); // (bb == NavRect)
			g.NavHighlightItemUnderNav = true;
		}
	}
	if (pressed)
		MarkItemEdited(id);

	if (flags & ImGuiSelectableFlags_AllowItemOverlap)
		SetItemAllowOverlap();

	// In this branch, Selectable() cannot toggle the selection so this will never trigger.
	if (selected != was_selected) //-V547
		g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

	RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_Compact | ImGuiNavHighlightFlags_NoRounding);

	if (span_all_columns && window->DC.CurrentColumns)
		PopColumnsBackground();
	else if (span_all_columns && g.CurrentTable)
		TablePopBackgroundChannel();

	it_anim->second.text_color = ImLerp(it_anim->second.text_color, selected ? text_color[0] : text_color[1], anim_speed);

	it_anim->second.rect_color = ImLerp(it_anim->second.rect_color, selected ? main_color : foreground_color, anim_speed);
	it_anim->second.checkmark_color = ImLerp(it_anim->second.checkmark_color, selected ? background_color : GetColorWithAlpha(foreground_color, 0.f), anim_speed);

	const ImRect check_bb(ImVec2(bb.Min.x + 10, bb.GetCenter().y - 7.5f), ImVec2(bb.Min.x + 25, bb.GetCenter().y + 7.5f));

	window->DrawList->AddRectFilled(check_bb.Min, check_bb.Max, GetColorU32(it_anim->second.rect_color), 2.5f);

	RenderCheckMark(window->DrawList, check_bb.GetCenter() - ImVec2(4.5f, 4.5f), GetColorU32(it_anim->second.checkmark_color), 9.f);



	PushFont(small_font);
	window->DrawList->AddText(ImVec2(bb.Min.x + 35.f, GetTextCenterPosition(bb.Min, bb.Max, label).y), GetColorU32(it_anim->second.text_color), label);
	PopFont();


	// Automatically close popups
	if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(g.LastItemData.StatusFlags & ImGuiItemFlags_AutoClosePopups))
		CloseCurrentPopup();

	if (disabled_item && !disabled_global)
		EndDisabled();

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return pressed; //-V1020
}

bool ImGui::KeybindWithToggle(const char* label, bool* v, int* key)
{
	std::string picker_name = "##keybind" + std::string(label);

	frame_size = ImVec2(235, 46);

	bool Ret = ImGui::CustomCheckbox(label, v);

	ImGui::SameLine();

	frame_size = ImVec2(60, 46);

	ImGui::KeybindBox(picker_name.c_str(), key);

	frame_size = ImVec2(305, 46);

	return Ret;
}

void ImGui::Keybind(const char* label, int* key)
{
	std::string picker_name = "##keybind" + std::string(label);

	frame_size = ImVec2(235, 46);

	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	const float square_sz = GetFrameHeight();
	const ImVec2 pos = window->DC.CursorPos;
	const ImRect total_bb(pos, pos + frame_size);

	const ImRect check_bb(total_bb.Max - ImVec2(45, 32), total_bb.Max - ImVec2(14, 14));
	ItemSize(total_bb, style.FramePadding.y);
	ItemAdd(total_bb, id);

	window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, background_color, style.FrameRounding);
	window->DrawList->AddText(ImVec2(total_bb.Min.x + 12.f, GetTextCenterPosition(total_bb.Min, total_bb.Max, label).y), text_color[0], label);
	//bool Ret = ImGui::CustomCheckbox(label, v);

	ImGui::SameLine();

	frame_size = ImVec2(60, 46);

	ImGui::KeybindBox(picker_name.c_str(), key);

	frame_size = ImVec2(305, 46);
}

struct keybind_state {
	ImVec2 key_size;
	ImVec4 text_color;
	ImVec4 key_color;
	bool mode;
	const char* key_name;
};

bool ImGui::KeybindBox(const char* label, int* key)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	const float square_sz = GetFrameHeight();
	const ImVec2 pos = window->DC.CursorPos;
	const ImRect total_bb(pos, pos + frame_size);

	ImRect button_bb(total_bb.Min + ImVec2(10, 20), total_bb.Max - ImVec2(10, 20));

	std::string label_str = label;
	if (label_str.find("##keybind") != std::string::npos)
		button_bb = ImRect(total_bb.Min, total_bb.Max);

	ItemSize(button_bb, style.FramePadding.y);
	ItemAdd(button_bb, id);

	static std::map<ImGuiID, keybind_state> anim;
	auto it_anim = anim.find(id);

	if (it_anim == anim.end())
	{
		anim.insert({ id, keybind_state() });
		it_anim = anim.find(id);
	}

	bool hovered, held;
	bool pressed = ButtonBehavior(button_bb, id, &hovered, &held);

	it_anim->second.text_color = ImLerp(it_anim->second.text_color, it_anim->second.mode ? text_color[0] : text_color[1], anim_speed);
	it_anim->second.key_color = ImLerp(it_anim->second.key_color, it_anim->second.mode ? main_color : text_color[1], anim_speed);

	if (pressed)
		it_anim->second.mode = true;

	if (it_anim->second.mode)
	{
		IsBindingKey = true;
		for (auto i = 0; i < 5; i++) {
			if (g.IO.MouseDown[i]) {
				switch (i) {
				case 0:
					*key = 1001;
					break;
				case 1:
					*key = 1002;
					break;
				case 2:
					*key = 1003;
					break;
				case 3:
					*key = 1004;
					break;
				case 4:
					*key = 1005;
					break;
				}
				it_anim->second.mode = false;
			}
		}

		for (int i = (int)ImGuiKey_Tab; i < (int)ImGuiKey_COUNT; i++)
		{
			if (IsKeyDown(ImGuiKey(i)))
			{
				*key = i;
				it_anim->second.mode = false;
				IsBindingKey = false;
			}
		}
	}

	PushStyleVar(ImGuiStyleVar_WindowRounding, style.FrameRounding);
	//CustomBlur(total_bb.Min, total_bb.Max);
	PopStyleVar();

	window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, background_color, style.FrameRounding);

	PushFont(small_font);
	if (label_str.find("##keybind") == std::string::npos)
		window->DrawList->AddText(ImVec2(total_bb.Min.x + 20, GetTextCenterPosition(total_bb.Min, total_bb.Max, label).y), GetColorU32(it_anim->second.text_color), label);
	PopFont();

	if (*key > 0 && !it_anim->second.mode)
	{
		it_anim->second.key_size = ImLerp(it_anim->second.key_size, CalcTextSize(it_anim->second.key_name), anim_speed);
		if (*key == (int)ImGuiKey_Backspace || *key == (int)ImGuiKey_Escape)
		{
			it_anim->second.mode = false;
			*key = 0;
		}
		else
		{
			if (*key == (int)ImGuiKey_MouseLeft)
			{
				it_anim->second.key_name = "LMB";
			}
			else if (*key == (int)ImGuiKey_MouseRight)
			{
				it_anim->second.key_name = "RMB";
			}
			else if (*key == (int)ImGuiKey_MouseMiddle)
			{
				it_anim->second.key_name = "MMB";
			}
			else if (*key == (int)ImGuiKey_MouseX1)
			{
				it_anim->second.key_name = "MX1";
			}
			else if (*key == (int)ImGuiKey_MouseX2)
			{
				it_anim->second.key_name = "MX2";
			}
			else
				it_anim->second.key_name = GetKeyName(ImGuiKey(*key));
			PushFont(small_font);
			window->DrawList->AddText(GetTextCenterPosition(total_bb.Min, total_bb.Max, it_anim->second.key_name), GetColorU32(it_anim->second.key_color), it_anim->second.key_name);
			PopFont();
		}
	}

	if (*key == (int)ImGuiKey_Backspace)
	{
		it_anim->second.mode = false;
		*key = 0;
	}

	if (it_anim->second.mode)
	{
		PushFont(small_font);
		window->DrawList->AddText(GetTextCenterPosition(total_bb.Min, total_bb.Max, "..."), GetColorU32(it_anim->second.key_color), "...");
		PopFont();
	}

	if (*key == 0 && !it_anim->second.mode)
	{
		it_anim->second.key_size = ImLerp(it_anim->second.key_size, CalcTextSize("None"), anim_speed);
		PushFont(small_font);
		window->DrawList->AddText(GetTextCenterPosition(total_bb.Min, total_bb.Max, "None"), GetColorU32(it_anim->second.key_color), "None");
		PopFont();
	}


	return pressed;
}

bool ImGui::CustomButton(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	const float square_sz = GetFrameHeight();
	const ImVec2 pos = window->DC.CursorPos;
	const ImRect total_bb(pos, pos + frame_size);

	ItemSize(total_bb, style.FramePadding.y);
	ItemAdd(total_bb, id);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	//ImVec2 pos = window->DC.CursorPos;
	//if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
	//    pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	//const ImRect bb(pos, pos + size);
	//ItemSize(size, style.FramePadding.y);
	//if (!ItemAdd(total_bb, id))
	//    return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(total_bb, id, &hovered, &held, flags);
	ImVec2 TextPos = GetTextCenterPosition(total_bb.Min, total_bb.Max, label);

	if (hovered)
	{
		window->DrawList->AddShadowRect(total_bb.Min, total_bb.Max, main_color, 10.f, ImVec2(0, 0), 0, 360);
		window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, GetColorWithAlpha(background_color, 1.0), style.FrameRounding);
		window->DrawList->AddText(ImVec2(TextPos.x, TextPos.y), main_color, label);
	}
	else
	{
		window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, background_color, style.FrameRounding);
		window->DrawList->AddText(ImVec2(TextPos.x, TextPos.y), text_color[0], label);
	}

	// Render
	//const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	//RenderNavCursor(total_bb, id);
   // RenderFrame(total_bb.Min, total_bb.Max, col, true, style.FrameRounding);

	//if (g.LogEnabled)
	//    LogSetNextTextDecoration("[", "]");
   // RenderTextClipped(total_bb.Min + style.FramePadding, total_bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &total_bb);

	// Automatically close popups
	//if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
	//    CloseCurrentPopup();

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return pressed;
}
#ifdef IMGUI_BLUR_TEST
void ImGui::CustomBlur(const ImVec2& min, const ImVec2& max)
{
	ImGuiWindow* window = GetCurrentWindow();
	ImGuiContext& g = *GImGui;
	ImGuiIO& io = g.IO;

	window->DrawList->AddImageRounded((ImTextureID)BLUR.srvTempB.ptr,
		min, max,
		{ min.x / io.DisplaySize.x , min.y / io.DisplaySize.y },
		{ max.x / io.DisplaySize.x, max.y / io.DisplaySize.y },
		ImColor(255, 255, 255, 255),
		ImGui::GetStyle().WindowRounding);
}


bool ImGui::InitializeBlurResources(
	ID3D12Device* device,
	UINT width,
	UINT height,
	DXGI_FORMAT format
)
{
	// ---------------------------
	// BLUR ROOT SIG
	// ---------------------------
	CD3DX12_ROOT_PARAMETER1 rootParams[3];
	CD3DX12_DESCRIPTOR_RANGE1 srv = CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	CD3DX12_DESCRIPTOR_RANGE1 uav = CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

	rootParams[0].InitAsConstants(sizeof(BlurParams) / 4, 0, 0, D3D12_SHADER_VISIBILITY_ALL); // b0
	rootParams[1].InitAsDescriptorTable(1, &srv, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[2].InitAsDescriptorTable(1, &uav, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.Init_1_1(_countof(rootParams), rootParams, 0, nullptr);

	ComPtr<ID3DBlob> sigBlob, errorBlob;
	HRESULT hr = D3DX12SerializeVersionedRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, &sigBlob, &errorBlob);
	if (FAILED(hr))
	{
		std::cout << "Serialize Failed: " << hr << "\n";
		return false;
	}

	hr = device->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&BLUR.rootSignature));
	if (FAILED(hr))
	{
		std::cout << "CreateRootSignature Failed: " << hr << "\n";
		return false;
	}

	// ---------------------------
	// BLUR PIPELINE
	// ---------------------------
	{
		ComPtr<ID3DBlob> csBlob;
		hr = D3DCreateBlob(BlurShaderSize, &csBlob);
		if (FAILED(hr)) return false;
		memcpy(csBlob->GetBufferPointer(), BlurShader, BlurShaderSize);

		D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.pRootSignature = BLUR.rootSignature;
		psoDesc.CS = { csBlob->GetBufferPointer(), csBlob->GetBufferSize() };
		hr = device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&BLUR.pipelineState));
		if (FAILED(hr))
		{
			std::cout << "CreateComputePipelineState (Blur) Failed: " << hr << "\n";
			return false;
		}
	}

	// ---------------------------
	// CONVERSION PIPELINE
	// ---------------------------
	{
		ComPtr<ID3DBlob> convBlob;
		hr = D3DCreateBlob(ConvertShaderSize, &convBlob);
		if (FAILED(hr)) return false;
		memcpy(convBlob->GetBufferPointer(), ConvertShader, ConvertShaderSize); // compiled DXGI_FORMAT_R10G10B10A2_UNORM -> float4 compute shader

		D3D12_COMPUTE_PIPELINE_STATE_DESC convDesc = {};
		convDesc.pRootSignature = BLUR.rootSignature; // same root sig (SRV + UAV)
		convDesc.CS = { convBlob->GetBufferPointer(), convBlob->GetBufferSize() };
		hr = device->CreateComputePipelineState(&convDesc, IID_PPV_ARGS(&BLUR.convertPipelineState));
		if (FAILED(hr))
		{
			std::cout << "CreateComputePipelineState (Conversion) Failed: " << hr << "\n";
			return false;
		}
	}

	// ---------------------------
	// BLUR TEMP TEXTURES
	// ---------------------------
	D3D12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R16G16B16A16_FLOAT, width, height, 1, 1);
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

	hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&BLUR.tempA)
	);
	if (FAILED(hr))
	{
		ReportHR(hr);
		return false;
	}

	hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&BLUR.tempB)
	);
	if (FAILED(hr))
	{
		ReportHR(hr);
		return false;
	}

	// ---------------------------
	// CONVERSION TEMP TEXTURE
	// ---------------------------
	{
		D3D12_RESOURCE_DESC tempOrigDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R10G10B10A2_UNORM, width, height, 1, 1);
		tempOrigDesc.Flags = D3D12_RESOURCE_FLAG_NONE; // just SRV usage
		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

		hr = device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&tempOrigDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&BLUR.tempOriginal));


		D3D12_RESOURCE_DESC convTexDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R16G16B16A16_FLOAT, // destination of conversion
			width,
			height,
			1,
			1
		);
		convTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		hr = device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&convTexDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&BLUR.convertTemp)
		);
		if (FAILED(hr))
		{
			ReportHR(hr);
			return false;
		}
	}

	return true;
}

void ImGui::ApplyBlurToBackBuffer(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* backBuffer, UINT width, UINT height)
{

	{
		cmdList->SetDescriptorHeaps(1, &BLUR.BlurDescriptor);

		D3D12_RESOURCE_BARRIER Barrier;
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Barrier.Transition.pResource = backBuffer;
		Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		//1 backBuffer from RENDER_TARGET to COPY_SOURCE
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
		cmdList->ResourceBarrier(1, &Barrier);

		//2 tempOriginal from previous state (starts as COMMON) to COPY_DEST
		Barrier.Transition.pResource = BLUR.tempOriginal;
		Barrier.Transition.StateBefore = BLUR.States[BlurState::TempOriginal];
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		cmdList->ResourceBarrier(1, &Barrier);

		//3 Copy backBuffer to tempOriginal
		cmdList->CopyResource(BLUR.tempOriginal, backBuffer);

		//4 backBuffer from COPY_SOURCE back to RENDER_TARGET since we're done with it
		Barrier.Transition.pResource = backBuffer;
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		cmdList->ResourceBarrier(1, &Barrier);

		cmdList->SetDescriptorHeaps(1, &BLUR.BlurDescriptor);

		//5 tempOriginal from COPY_DEST to D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
		Barrier.Transition.pResource = BLUR.tempOriginal;
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		cmdList->ResourceBarrier(1, &Barrier);





	}

	//D3D12_RESOURCE_BARRIER Barrier;
	//Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//Barrier.Transition.pResource = backBuffer;
	//Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	////1 backBuffer from RENDER_TARGET to COPY_SOURCE
	//Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
	//cmdList->ResourceBarrier(1, &Barrier);

	////2 tempOriginal from previous state (starts as COMMON) to COPY_DEST
	//Barrier.Transition.pResource = BLUR.tempOriginal;
	//Barrier.Transition.StateBefore = BLUR.States[BlurState::TempOriginal];
	//Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	//cmdList->ResourceBarrier(1, &Barrier);

	////3 Copy backBuffer to tempOriginal
	//cmdList->CopyResource(BLUR.tempOriginal, backBuffer);

	////4 backBuffer from COPY_SOURCE back to RENDER_TARGET since we're done with it
	//Barrier.Transition.pResource = backBuffer;
	//Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
	//Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//cmdList->ResourceBarrier(1, &Barrier);

	//cmdList->SetDescriptorHeaps(1, &BLUR.BlurDescriptor);

	////5 tempOriginal from COPY_DEST to D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	//Barrier.Transition.pResource = BLUR.tempOriginal;
	//Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	//Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	//cmdList->ResourceBarrier(1, &Barrier);

	////6 convertTemp from previous state (starts as COMMON) to D3D12_RESOURCE_STATE_UNORDERED_ACCESS (if needed)
	//Barrier.Transition.pResource = BLUR.convertTemp;
	//Barrier.Transition.StateBefore = BLUR.States[BlurState::ConvertTemp];
	//Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	//cmdList->ResourceBarrier(1, &Barrier);



	////7 Run tempOriginal through the conversion shader into convertTemp
	//cmdList->SetPipelineState(BLUR.convertPipelineState);
	//cmdList->SetComputeRootSignature(BLUR.rootSignature);
	//cmdList->SetComputeRootDescriptorTable(1, BLUR.srvTempOriginal);
	//cmdList->SetComputeRootDescriptorTable(2, BLUR.uavConvertTemp);
	//cmdList->Dispatch((width + 7) / 8, (height + 7) / 8, 1);

	////8 UAV barrier for convertTemp
	//D3D12_RESOURCE_BARRIER uavBarrier = {};
	//uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	//uavBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//uavBarrier.UAV.pResource = BLUR.convertTemp;
	//cmdList->ResourceBarrier(1, &uavBarrier);

	////9 Done with tempOriginal, save last state as D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE for next time around
	//BLUR.States[BlurState::TempOriginal] = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

	//////10 tempA from previous state (starts as COMMON) to D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	////Barrier.Transition.pResource = BLUR.tempA;
	////Barrier.Transition.StateBefore = BLUR.States[BlurState::TempA];
	////Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	////cmdList->ResourceBarrier(1, &Barrier);

	////11 convertTemp from D3D12_RESOURCE_STATE_UNORDERED_ACCESS to D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	//Barrier.Transition.pResource = BLUR.convertTemp;
	//Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	//Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//cmdList->ResourceBarrier(1, &Barrier);

	////12 First blur pass, convertTemp into tempA
	//BlurParams blurParams = {};
	//blurParams.texelSize = { 1.0f / width, 1.0f / height };
	//blurParams.horizontal = 1;
	//cmdList->SetPipelineState(BLUR.pipelineState);
	//cmdList->SetComputeRootSignature(BLUR.rootSignature);
	//cmdList->SetComputeRoot32BitConstants(0, sizeof(BlurParams) / 4, &blurParams, 0);
	//cmdList->SetComputeRootDescriptorTable(1, BLUR.srvConvertTemp); // input
	//cmdList->SetComputeRootDescriptorTable(2, BLUR.uavTempA); // output
	//cmdList->Dispatch((width + 7) / 8, (height + 7) / 8, 1);

	////13 Done with convertTemp, save last state as D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE for next time around
	//BLUR.States[BlurState::ConvertTemp] = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

	////14 UAV barrier for tempA
	//uavBarrier.UAV.pResource = BLUR.tempA;
	//cmdList->ResourceBarrier(1, &uavBarrier);

	////15 tempA from D3D12_RESOURCE_STATE_UNORDERED_ACCESS to D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	//Barrier.Transition.pResource = BLUR.tempA;
	//Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	//Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	//cmdList->ResourceBarrier(1, &Barrier);

	////16 tempB from previous state (starts as COMMON) to D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	//Barrier.Transition.pResource = BLUR.tempB;
	//Barrier.Transition.StateBefore = BLUR.States[BlurState::TempB];
	//Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	//cmdList->ResourceBarrier(1, &Barrier);

	////17 Second blur pass, tempA into tempB
	//blurParams.horizontal = 0;
	//cmdList->SetComputeRoot32BitConstants(0, sizeof(BlurParams) / 4, &blurParams, 0);
	//cmdList->SetComputeRootDescriptorTable(1, BLUR.srvTempA); // input
	//cmdList->SetComputeRootDescriptorTable(2, BLUR.uavTempB); // output
	//cmdList->Dispatch((width + 7) / 8, (height + 7) / 8, 1);

	////18 Done with tempA, save last state as D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE for next time around
	//BLUR.States[BlurState::TempA] = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

	////19 UAV barrier for tempA
	//uavBarrier.UAV.pResource = BLUR.tempB;
	//cmdList->ResourceBarrier(1, &uavBarrier);

	////20 tempB from D3D12_RESOURCE_STATE_UNORDERED_ACCESS to D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE for ImGui rendering
	//Barrier.Transition.pResource = BLUR.tempB;
	//Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	//Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//cmdList->ResourceBarrier(1, &Barrier);

	////21 Done with tempB, save last state as D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE for next time around
	//BLUR.States[BlurState::TempB] = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
}


void ImGui::ReportHR(HRESULT hr)
{
	char* errorMsg = nullptr;

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&errorMsg,
		0,
		nullptr);

	if (errorMsg)
	{
		std::cout << "HRESULT tempB failed: 0x" << std::hex << hr << " - " << errorMsg << std::endl;
		LocalFree(errorMsg);
	}
	else
	{
		std::cout << "HRESULT tempB failed: 0x" << std::hex << hr << " (No error message found)" << std::endl;
	}

}
#endif

// RGB-only color editor that bypasses HSV conversion
bool ImGui::RGBColorEdit3(const char* label, float* col)
{
	bool changed = false;

	// Color preview box - clickable to open color wheel
	ImVec2 previewSize = ImVec2(80.0f, 20.0f);
	ImVec2 previewPos = ImGui::GetCursorScreenPos();

	ImGui::InvisibleButton(("##ColorPreview" + std::string(label)).c_str(), previewSize);
	bool preview_clicked = ImGui::IsItemClicked();

	// Draw color preview
	ImU32 preview_color = IM_COL32((int)(col[0] * 255), (int)(col[1] * 255), (int)(col[2] * 255), 255);
	ImGui::GetWindowDrawList()->AddRectFilled(previewPos, ImVec2(previewPos.x + previewSize.x, previewPos.y + previewSize.y), preview_color, 3.0f);
	ImGui::GetWindowDrawList()->AddRect(previewPos, ImVec2(previewPos.x + previewSize.x, previewPos.y + previewSize.y), IM_COL32(100, 100, 100, 255), 3.0f, 0, 1.0f);

	// Handle color picker popup
	if (preview_clicked)
	{
		ImGui::OpenPopup(("ColorPicker##" + std::string(label)).c_str());
	}

	if (ImGui::BeginPopup(("ColorPicker##" + std::string(label)).c_str()))
	{
		float col4[4] = { col[0], col[1], col[2], 1.0f };
		if (ImGui::ColorPicker4(("##picker" + std::string(label)).c_str(), col4, ImGuiColorEditFlags_NoAlpha))
		{
			col[0] = col4[0];
			col[1] = col4[1];
			col[2] = col4[2];
			changed = true;
		}
		ImGui::EndPopup();
	}

	ImGui::SameLine();
	ImGui::Text("%s", label);

	// Compact RGB sliders with smaller text and no handles
	ImGui::PushItemWidth(45.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f)); // Smaller padding
	ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 0.0f); // Remove slider handles
	ImGui::PushFont(small_font); // Smaller text

	changed |= ImGui::SliderFloat(("R##" + std::string(label)).c_str(), &col[0], 0.0f, 1.0f, "%.2f");
	ImGui::SameLine();
	changed |= ImGui::SliderFloat(("G##" + std::string(label)).c_str(), &col[1], 0.0f, 1.0f, "%.2f");
	ImGui::SameLine();
	changed |= ImGui::SliderFloat(("B##" + std::string(label)).c_str(), &col[2], 0.0f, 1.0f, "%.2f");

	ImGui::PopFont();
	ImGui::PopStyleVar(2);
	ImGui::PopItemWidth();

	return changed;
}

// RGBA version with alpha support
bool ImGui::RGBColorEdit4(const char* label, float* col)
{
	bool changed = false;

	// Color preview box - clickable to open color wheel
	ImVec2 previewSize = ImVec2(80.0f, 20.0f);
	ImVec2 previewPos = ImGui::GetCursorScreenPos();

	ImGui::InvisibleButton(("##ColorPreview" + std::string(label)).c_str(), previewSize);
	bool preview_clicked = ImGui::IsItemClicked();

	// Draw color preview with alpha
	ImU32 preview_color = IM_COL32((int)(col[0] * 255), (int)(col[1] * 255), (int)(col[2] * 255), (int)(col[3] * 255));
	ImGui::GetWindowDrawList()->AddRectFilled(previewPos, ImVec2(previewPos.x + previewSize.x, previewPos.y + previewSize.y), preview_color, 3.0f);
	ImGui::GetWindowDrawList()->AddRect(previewPos, ImVec2(previewPos.x + previewSize.x, previewPos.y + previewSize.y), IM_COL32(100, 100, 100, 255), 3.0f, 0, 1.0f);

	// Handle color picker popup
	if (preview_clicked)
	{
		ImGui::OpenPopup(("ColorPicker##" + std::string(label)).c_str());
	}

	if (ImGui::BeginPopup(("ColorPicker##" + std::string(label)).c_str()))
	{
		if (ImGui::ColorPicker4(("##picker" + std::string(label)).c_str(), col, ImGuiColorEditFlags_AlphaBar))
		{
			changed = true;
		}
		ImGui::EndPopup();
	}

	ImGui::SameLine();
	ImGui::Text("%s", label);

	// Compact RGBA sliders with smaller text and no handles - horizontal layout
	ImGui::PushItemWidth(40.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f)); // Smaller padding
	ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 0.0f); // Remove slider handles
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(background_color.Value.x + 0.1f, background_color.Value.y + 0.1f, background_color.Value.z + 0.1f, background_color.Value.w));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(background_color.Value.x + 0.15f, background_color.Value.y + 0.15f, background_color.Value.z + 0.15f, background_color.Value.w));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(background_color.Value.x + 0.2f, background_color.Value.y + 0.2f, background_color.Value.z + 0.2f, background_color.Value.w));
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color.Value);
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(main_color.Value.x + 0.2f, main_color.Value.y + 0.2f, main_color.Value.z + 0.2f, main_color.Value.w));
	ImGui::PushFont(small_font); // Smaller text

	changed |= ImGui::SliderFloat(("R##" + std::string(label)).c_str(), &col[0], 0.0f, 1.0f, "%.2f");
	ImGui::SameLine();
	changed |= ImGui::SliderFloat(("G##" + std::string(label)).c_str(), &col[1], 0.0f, 1.0f, "%.2f");
	ImGui::SameLine();
	changed |= ImGui::SliderFloat(("B##" + std::string(label)).c_str(), &col[2], 0.0f, 1.0f, "%.2f");
	ImGui::SameLine();
	changed |= ImGui::SliderFloat(("A##" + std::string(label)).c_str(), &col[3], 0.0f, 1.0f, "%.2f");

	ImGui::PopFont();
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(5);
	ImGui::PopItemWidth();

	return changed;
}

void ImGui::QuickTooltip(const char* fmt, ...)
{
	if (ImGui::IsItemHovered() && CFG::cfg_Menu_EnableTooltips)
	{
		va_list args;
		va_start(args, fmt);
		ImGui::SetTooltipV(fmt, args);
		va_end(args);
	}
}