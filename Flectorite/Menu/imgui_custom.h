#pragma once
#include "../ImGui/imgui_internal.h"
#include <string>
#include <d3d12.h>
#include "d3dx12.h"
#include <directxmath.h>
#ifdef _MSC_VER
#pragma comment(lib, "d3dcompiler") // Automatically link with d3dcompiler.lib as we are using D3DCompile() below.
#endif

inline ImColor main_color(230, 134, 224, 255);

inline ImColor text_color[3] = { ImColor(255, 255, 255, 255), ImColor(200, 200, 200, 255), ImColor(150, 150, 150, 255) };

inline ImColor foreground_color(1.f, 1.f, 1.f, 0.15f);

inline ImColor background_color(13, 14, 16, 200);

inline ImColor second_color(255, 255, 255, 20);

inline ImColor stroke_color(35, 35, 35, 255);

inline ImColor child_color(19, 19, 19, 255);

inline ImColor scroll_bg_col(24, 24, 24, 255);

inline ImColor winbg_color(15, 16, 18, 200);

inline ImFont* icon_font;

inline ImFont* logo_font;

inline ImFont* icon_big_font;

inline ImFont* medium_font;

inline ImFont* small_font;

inline ImFont* small_icon_font;

inline ImFont* arrow_icons;

inline ImFont* font_awesome;

inline ImVec2 frame_size(305, 46);

inline float anim_speed = 0.05f;

inline ImVec2 GetTextCenterPosition(ImVec2 min, ImVec2 max, const char* text)
{
    return min + (max - min) / 2 - ImGui::CalcTextSize(text) / 2;
}

inline ImVec4 ImColorToImVec4(const ImColor& color)
{
    return ImVec4(color.Value.x, color.Value.y, color.Value.z, color.Value.w);
}

inline ImColor GetColorWithAlpha(ImColor color, float alpha)
{
    return ImColor(color.Value.x, color.Value.y, color.Value.z, alpha);
}

struct Tab_State {
    bool IsActiveTab = false;
    float AnimValue = 0.0f;
    ImVec4 rect_color;
    ImVec4 text_color;
    ImVec4 icon_color;
};

struct Checkbox_State {
    float AnimValue;
    float circle_offset;
    ImVec4 rect_color;
    ImVec4 circle_color;
    ImVec4 shadow_color;
};

struct Slider_State {
    ImVec2 grab_bb_center_offset;
    ImVec4 hint_color;
    ImVec4 hint_frame_color;
};

static float ColorCopied[4];

struct ColorEdit_State
{
    ImVec4 color;
    bool picker_active;
};

struct ColorPicker_State
{
    float sv_cursor_rad;
    float bar0_line_y;
    float bar1_line_y;
    ImVec2 sv_cursor_pos;
    float size;
};

struct ToggleButton_State {
    ImVec4 rect_color;
    ImVec4 frame_color;
};

static bool picker = false;

namespace ImGui
{
#ifdef IMGUI_BLUR_TEST
    static enum BlurState : uint8_t
    {
        TempOriginal = 0,
        ConvertTemp = 1,
        TempA = 2,
        TempB = 3,
    };

    struct BlurResources {
        ID3D12RootSignature* rootSignature = nullptr;
        ID3D12PipelineState* pipelineState = nullptr;
        ID3D12PipelineState* convertPipelineState = nullptr;
        ID3D12DescriptorHeap* BlurDescriptor = nullptr;
        ID3D12Resource* tempA = nullptr;
        ID3D12Resource* tempB = nullptr;
        ID3D12Resource* tempOriginal = nullptr;
        ID3D12Resource* convertTemp = nullptr;
        D3D12_GPU_DESCRIPTOR_HANDLE srvTempA = {};
        D3D12_GPU_DESCRIPTOR_HANDLE srvTempB = {};
        D3D12_GPU_DESCRIPTOR_HANDLE srvTempOriginal = {};
        D3D12_GPU_DESCRIPTOR_HANDLE srvConvertTemp = {};
        D3D12_GPU_DESCRIPTOR_HANDLE uavTempA = {};
        D3D12_GPU_DESCRIPTOR_HANDLE uavTempB = {};
        D3D12_GPU_DESCRIPTOR_HANDLE uavConvertTemp = {};
        D3D12_RESOURCE_STATES States[4] = { D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COMMON };

    };

    extern BlurResources BLUR;

    struct BlurParams {
        DirectX::XMFLOAT2 texelSize;
        int horizontal;
        int padding;
    };
#endif
    bool VerticalTab(const char* label, const char* icon, int* v, int number);
    bool CustomCheckbox(const char* label, bool* v);
    bool CustomSliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool CustomSliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool CustomSliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags);
    bool CustomInputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0);
    bool CustomColorEdit(const char* label, float col[4], ImGuiColorEditFlags flags = 0, bool render_frame = true);
    bool ToggleButton(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags, bool* toggled);
    bool CustomColorPicker(const char* label, float col[4], ImGuiColorEditFlags flags = 0);
    bool RGBColorEdit3(const char* label, float* col);
    bool RGBColorEdit4(const char* label, float* col);
    void QuickTooltip(const char* fmt, ...);
    bool CustomCombo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items = -1);
    bool CustomCombo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items);
    bool BeginCustomCombo(const char* label, const char* preview_value, ImGuiComboFlags flags, int value);
    bool CustomSelectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
    bool CustomSelectable(const char* label, bool selected, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
    void EndCustomCombo();
    bool KeybindWithToggle(const char* label, bool* v, int* key);
    bool KeybindBox(const char* label, int* key);
    void Keybind(const char* label, int* key);
    bool CustomButton(const char* label, const ImVec2& size_arg = ImVec2(0, 0), ImGuiButtonFlags flags = ImGuiButtonFlags_None);
    bool IconSelector(const char* id, const char** selected_icon, ImVec4 iconColor = ImVec4(1, 1, 1, 1), float size = 20.0f, bool rightAlign = false);

    extern bool IsBindingKey;
#ifdef IMGUI_BLUR_TEST
    void CustomBlur(const ImVec2& min, const ImVec2& max);
    bool InitializeBlurResources(
        ID3D12Device* device,
        UINT width,
        UINT height,
        DXGI_FORMAT format);
    void ApplyBlurToBackBuffer(
        ID3D12GraphicsCommandList* cmdList,
        ID3D12Resource* backBuffer,
        UINT width,
        UINT height);

    void ReportHR(HRESULT hr);
#endif
}
