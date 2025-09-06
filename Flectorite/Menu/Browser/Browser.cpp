#include "Browser.h"
#include "MediaControls.h"
#include <windows.h>

namespace Browser
{
    static bool g_Initialized = false;

    bool Initialize()
    {
        g_Initialized = MediaControls::Initialize();
        return g_Initialized;
    }

    void Shutdown()
    {
        MediaControls::Shutdown();
        g_Initialized = false;
    }

    void Update()
    {
        if (!g_Initialized) return;
        MediaControls::Update();
    }

    void RenderMusicPlayer()
    {
        MediaControls::RenderMediaControls();
    }


    void PlayPause()
    {
        MediaControls::PlayPause();
    }

    void Next()
    {
        MediaControls::Next();
    }

    void Previous()
    {
        MediaControls::Previous();
    }

    void Stop()
    {
        MediaControls::Stop();
    }


    bool IsReady()
    {
        return g_Initialized;
    }
}
