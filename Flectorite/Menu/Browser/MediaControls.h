#pragma once
#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <string>
#include <mmsystem.h>
#include "../imgui_custom.h"

#pragma comment(lib, "winmm.lib")

namespace MediaControls
{
    struct SongInfo
    {
        std::string filename;
        std::string title;
        std::string artist;
        std::string fullPath;
        float duration = 0.0f;
    };

    struct MediaInfo
    {
        char title[256] = "No song loaded";
        char artist[256] = "";
        char album[256] = "";
        bool isPlaying = false;
        bool canPlay = true;
        bool canPause = false;
        bool canNext = true;
        bool canPrevious = true;
        float currentTime = 0.0f;
        float totalTime = 0.0f;
        int currentSongIndex = -1;
    };

    bool Initialize();
    void Shutdown();
    void Update();

    void ScanSongsFolder();
    void LoadSong(int index);
    void PlayPause();
    void Next();
    void Previous();
    void Stop();
    void SetVolume(float volume);
    void SeekTo(float position);

    const MediaInfo& GetCurrentMedia();
    const std::vector<SongInfo>& GetPlaylist();
    void UpdatePlaybackProgress();

    void RenderMediaControls();
}
