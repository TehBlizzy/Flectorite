#pragma once
#include "../imgui_custom.h"
#include "MediaControls.h"

namespace Browser // Shame Reminder of my failure adding an internet browser
{
    bool Initialize();
    void Shutdown();
    void Update();

    void RenderMusicPlayer();

    void PlayPause();
    void Next();
    void Previous();
    void Stop();

    bool IsReady();
}
