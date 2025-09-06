#include "MediaControls.h"
#include "../Config.h"
#include <windows.h>
#include <shellapi.h>
#include <filesystem>
#include <algorithm>
#include <psapi.h>
#include <mmsystem.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <propvarutil.h>
#include <mferror.h>
#include <evr.h>


#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "propsys.lib")
#pragma comment(lib, "mfuuid.lib")

namespace MediaControls
{
    static bool g_Initialized = false;
    static MediaInfo g_CurrentMedia;
    static std::vector<SongInfo> g_Playlist;
    static float g_Volume = 0.25f;
    static bool g_SongsScanned = false;
    static bool g_MFInitialized = false;

    static IMFMediaSession* g_pSession = nullptr;
    static IMFTopology* g_pTopology = nullptr;
    static bool g_AudioEngineReady = false;

    static DWORD g_PlaybackStartTime = 0;
    static bool g_ShuffleEnabled = false;

    bool Initialize()
    {
        if (g_Initialized)
            return true;

        if (SUCCEEDED(MFStartup(MF_VERSION)))
        {
            g_MFInitialized = true;

            if (SUCCEEDED(MFCreateMediaSession(nullptr, &g_pSession)))
            {
                g_AudioEngineReady = true;
            }
        }

        g_Initialized = true;
        strcpy_s(g_CurrentMedia.title, "Local Music Player");
        strcpy_s(g_CurrentMedia.artist, "Add songs to Songs/ folder next to .exe");

        ScanSongsFolder();

        return true;
    }

    void Shutdown()
    {
        Stop();
        g_Initialized = false;
    }

    void Update()
    {
        if (!g_Initialized)
            return;

        if (g_CurrentMedia.isPlaying && g_CurrentMedia.currentSongIndex >= 0)
        {
            static DWORD lastTime = GetTickCount();
            DWORD currentTime = GetTickCount();

            if (currentTime - lastTime >= 1000)    
            {
                g_CurrentMedia.currentTime += 1.0f;
                lastTime = currentTime;

                if (g_CurrentMedia.currentTime >= g_CurrentMedia.totalTime && g_CurrentMedia.totalTime > 0)
                {
                    Next();
                }
            }
        }

        static DWORD lastScan = 0;
        DWORD currentTime = GetTickCount();
        if (currentTime - lastScan > 10000)    
        {
            ScanSongsFolder();
            lastScan = currentTime;
        }
    }

    void ScanSongsFolder()
    {
        g_Playlist.clear();

        try
        {
            char processPath[MAX_PATH];
            DWORD processId = GetCurrentProcessId();
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);

            std::string processDir;
            if (hProcess)
            {
                if (GetModuleFileNameExA(hProcess, NULL, processPath, MAX_PATH))
                {
                    processDir = std::string(processPath);
                    size_t lastSlash = processDir.find_last_of("\\/");
                    if (lastSlash != std::string::npos)
                    {
                        processDir = processDir.substr(0, lastSlash + 1);
                    }
                }
                CloseHandle(hProcess);
            }

            if (processDir.empty())
            {
                char cwd[MAX_PATH];
                GetCurrentDirectoryA(MAX_PATH, cwd);
                processDir = std::string(cwd) + "\\";
            }

            std::vector<std::string> pathsToTry = {
                processDir + "Songs\\",
                processDir + "Songs/",
                "Songs\\",
                "Songs/",
                ".\\Songs\\",
                "./Songs/",
                processDir + "x64\\Release\\Songs\\",
                processDir + "x64/Release/Songs/",
                "x64\\Release\\Songs\\",
                "x64/Release/Songs/"
            };

            bool foundPath = false;
            std::string songsPath;
            for (const auto& path : pathsToTry)
            {
                if (std::filesystem::exists(path))
                {
                    songsPath = path;
                    foundPath = true;
                    break;
                }
            }

            if (foundPath)
            {
                for (const auto& entry : std::filesystem::directory_iterator(songsPath))
                {
                    if (entry.is_regular_file())
                    {
                        std::string extension = entry.path().extension().string();
                        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                        if (extension == ".mp3" || extension == ".wav" || extension == ".ogg" ||
                            extension == ".flac" || extension == ".m4a" || extension == ".wma")
                        {
                            SongInfo song;
                            song.filename = entry.path().filename().string();
                            song.fullPath = entry.path().string();

                            song.title = song.filename.substr(0, song.filename.find_last_of('.'));

                            size_t dashPos = song.title.find(" - ");
                            if (dashPos != std::string::npos)
                            {
                                song.artist = song.title.substr(0, dashPos);
                                song.title = song.title.substr(dashPos + 3);
                            }
                            else
                            {
                                song.artist = "Unknown Artist";
                            }

                            song.duration = 180.0f;    

                            g_Playlist.push_back(song);
                        }
                    }
                }
            }
        }
        catch (const std::exception&)
        {
        }

        if (g_Playlist.empty())
        {
            char debugPath[512];
            char exePath[MAX_PATH];
            GetModuleFileNameA(NULL, exePath, MAX_PATH);
            std::string exeDir = std::string(exePath);
            size_t lastSlash = exeDir.find_last_of("\\/");
            if (lastSlash != std::string::npos)
            {
                exeDir = exeDir.substr(0, lastSlash + 1);
            }

            char processPath[MAX_PATH];
            DWORD processId = GetCurrentProcessId();
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);

            std::string processDir;
            if (hProcess)
            {
                if (GetModuleFileNameExA(hProcess, NULL, processPath, MAX_PATH))
                {
                    processDir = std::string(processPath);
                    size_t lastSlash = processDir.find_last_of("\\/");
                    if (lastSlash != std::string::npos)
                    {
                        processDir = processDir.substr(0, lastSlash + 1);
                    }
                }
                CloseHandle(hProcess);
            }

            std::vector<std::string> pathsToTry = {
                processDir + "Songs\\",
                processDir + "x64\\Release\\Songs\\",
                "Songs\\",
                "x64\\Release\\Songs\\"
            };

            std::string foundPath = "None found";
            for (const auto& path : pathsToTry)
            {
                if (std::filesystem::exists(path))
                {
                    foundPath = path;
                    break;
                }
            }

            sprintf_s(debugPath, "DLL Context - Found: %s", foundPath.c_str());
            strcpy_s(g_CurrentMedia.title, "No Songs Found");
            strcpy_s(g_CurrentMedia.artist, debugPath);
        }
        else if (g_CurrentMedia.currentSongIndex < 0)
        {
            char status[256];
            sprintf_s(status, "%d Songs Available", (int)g_Playlist.size());
            strcpy_s(g_CurrentMedia.title, status);
            strcpy_s(g_CurrentMedia.artist, "Click a Song to Start");
        }

        g_SongsScanned = true;
    }

    float GetAudioDuration(const std::wstring& filePath)
    {
        if (!g_MFInitialized)
            return 180.0f;     

        IMFSourceResolver* pSourceResolver = nullptr;
        HRESULT hr = MFCreateSourceResolver(&pSourceResolver);
        if (FAILED(hr)) return 180.0f;

        MF_OBJECT_TYPE objectType = MF_OBJECT_INVALID;
        IUnknown* pSource = nullptr;
        hr = pSourceResolver->CreateObjectFromURL(filePath.c_str(), MF_RESOLUTION_MEDIASOURCE, nullptr, &objectType, &pSource);
        pSourceResolver->Release();
        if (FAILED(hr)) return 180.0f;

        IMFMediaSource* pMediaSource = nullptr;
        hr = pSource->QueryInterface(IID_PPV_ARGS(&pMediaSource));
        pSource->Release();
        if (FAILED(hr)) return 180.0f;

        IMFPresentationDescriptor* pPD = nullptr;
        hr = pMediaSource->CreatePresentationDescriptor(&pPD);
        if (FAILED(hr))
        {
            pMediaSource->Release();
            return 180.0f;
        }

        UINT64 duration = 0;
        hr = pPD->GetUINT64(MF_PD_DURATION, &duration);
        float durationSeconds = 180.0f;   

        if (SUCCEEDED(hr))
        {
            durationSeconds = (float)(duration / 10000000.0);
        }

        pPD->Release();
        pMediaSource->Release();

        return durationSeconds;
    }

    void LoadSong(int index)
    {
        if (index < 0 || index >= (int)g_Playlist.size())
            return;

        g_CurrentMedia.isPlaying = false;
        if (g_pSession && g_AudioEngineReady)
        {
            g_pSession->Stop();
            g_pSession->ClearTopologies();
        }
        if (g_pTopology)
        {
            g_pTopology->Release();
            g_pTopology = nullptr;
        }

        g_CurrentMedia.currentSongIndex = index;
        const SongInfo& song = g_Playlist[index];

        strcpy_s(g_CurrentMedia.title, song.title.c_str());
        strcpy_s(g_CurrentMedia.artist, song.artist.c_str());

        std::wstring wPath(song.fullPath.begin(), song.fullPath.end());
        g_CurrentMedia.totalTime = GetAudioDuration(wPath);

        g_CurrentMedia.currentTime = 0.0f;
        g_CurrentMedia.canPlay = true;
        g_CurrentMedia.canPause = false;
        g_CurrentMedia.isPlaying = false;
    }

    HRESULT CreatePlaybackTopology(const std::wstring& filePath)
    {
        if (!g_pSession || !g_AudioEngineReady)
            return E_FAIL;

        if (g_pTopology)
        {
            g_pTopology->Release();
            g_pTopology = nullptr;
        }

        IMFSourceResolver* pSourceResolver = nullptr;
        HRESULT hr = MFCreateSourceResolver(&pSourceResolver);
        if (FAILED(hr)) return hr;

        MF_OBJECT_TYPE objectType = MF_OBJECT_INVALID;
        IUnknown* pSource = nullptr;
        hr = pSourceResolver->CreateObjectFromURL(filePath.c_str(), MF_RESOLUTION_MEDIASOURCE, nullptr, &objectType, &pSource);
        pSourceResolver->Release();
        if (FAILED(hr)) return hr;

        IMFMediaSource* pMediaSource = nullptr;
        hr = pSource->QueryInterface(IID_PPV_ARGS(&pMediaSource));
        pSource->Release();
        if (FAILED(hr)) return hr;

        hr = MFCreateTopology(&g_pTopology);
        if (FAILED(hr))
        {
            pMediaSource->Release();
            return hr;
        }

        IMFPresentationDescriptor* pPD = nullptr;
        hr = pMediaSource->CreatePresentationDescriptor(&pPD);
        if (FAILED(hr))
        {
            pMediaSource->Release();
            return hr;
        }

        DWORD cStreams = 0;
        hr = pPD->GetStreamDescriptorCount(&cStreams);
        if (FAILED(hr))
        {
            pPD->Release();
            pMediaSource->Release();
            return hr;
        }

        for (DWORD i = 0; i < cStreams; i++)
        {
            BOOL fSelected = FALSE;
            IMFStreamDescriptor* pSD = nullptr;
            hr = pPD->GetStreamDescriptorByIndex(i, &fSelected, &pSD);
            if (SUCCEEDED(hr) && fSelected)
            {
                IMFTopologyNode* pSourceNode = nullptr;
                hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pSourceNode);
                if (SUCCEEDED(hr))
                {
                    pSourceNode->SetUnknown(MF_TOPONODE_SOURCE, pMediaSource);
                    pSourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPD);
                    pSourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSD);
                    g_pTopology->AddNode(pSourceNode);

                    IMFTopologyNode* pOutputNode = nullptr;
                    hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pOutputNode);
                    if (SUCCEEDED(hr))
                    {
                        IMFActivate* pRendererActivate = nullptr;
                        hr = MFCreateAudioRendererActivate(&pRendererActivate);
                        if (SUCCEEDED(hr))
                        {
                            pOutputNode->SetObject(pRendererActivate);
                            g_pTopology->AddNode(pOutputNode);

                            pSourceNode->ConnectOutput(0, pOutputNode, 0);
                            pRendererActivate->Release();
                        }
                        pOutputNode->Release();
                    }
                    pSourceNode->Release();
                }
                pSD->Release();
            }
        }

        pPD->Release();
        pMediaSource->Release();
        return hr;
    }

    void PlayPause()
    {
        if (g_CurrentMedia.currentSongIndex < 0 && !g_Playlist.empty())
        {
            LoadSong(0);       
        }

        if (g_CurrentMedia.currentSongIndex >= 0 && g_AudioEngineReady)
        {
            if (g_CurrentMedia.isPlaying)
            {
                if (g_pSession)
                {
                    g_pSession->Pause();
                }
                g_CurrentMedia.isPlaying = false;
                g_CurrentMedia.canPlay = true;
                g_CurrentMedia.canPause = false;
            }
            else
            {
                if (g_pSession)
                {
                    g_pSession->Stop();
                    g_pSession->ClearTopologies();
                }

                if (g_pTopology)
                {
                    g_pTopology->Release();
                    g_pTopology = nullptr;
                }

                const SongInfo& song = g_Playlist[g_CurrentMedia.currentSongIndex];
                std::wstring wPath(song.fullPath.begin(), song.fullPath.end());

                if (SUCCEEDED(CreatePlaybackTopology(wPath)))
                {
                    g_pSession->SetTopology(0, g_pTopology);

                    PROPVARIANT varStart;
                    PropVariantInit(&varStart);
                    g_pSession->Start(&GUID_NULL, &varStart);
                    PropVariantClear(&varStart);

                    g_CurrentMedia.isPlaying = true;
                    g_CurrentMedia.canPlay = false;
                    g_CurrentMedia.canPause = true;

                    g_CurrentMedia.currentTime = 0.0f;
                    g_PlaybackStartTime = GetTickCount();
                }
            }
        }
    }

    void Next()
    {
        if (g_Playlist.empty()) return;

        int nextIndex;
        if (g_ShuffleEnabled)
        {
            nextIndex = rand() % (int)g_Playlist.size();
            if (nextIndex == g_CurrentMedia.currentSongIndex && g_Playlist.size() > 1)
            {
                nextIndex = (nextIndex + 1) % (int)g_Playlist.size();
            }
        }
        else
        {
            nextIndex = g_CurrentMedia.currentSongIndex + 1;
            if (nextIndex >= (int)g_Playlist.size())
                nextIndex = 0;    
        }

        if (g_pSession && g_AudioEngineReady)
        {
            g_pSession->Shutdown();
            g_pSession->Release();
            g_pSession = nullptr;
        }

        if (g_pTopology)
        {
            g_pTopology->Release();
            g_pTopology = nullptr;
        }

        if (SUCCEEDED(MFCreateMediaSession(nullptr, &g_pSession)))
        {
            g_AudioEngineReady = true;

            g_CurrentMedia.currentSongIndex = nextIndex;
            const SongInfo& nextSong = g_Playlist[nextIndex];

            strcpy_s(g_CurrentMedia.title, nextSong.title.c_str());
            strcpy_s(g_CurrentMedia.artist, nextSong.artist.c_str());

            std::wstring nextPath(nextSong.fullPath.begin(), nextSong.fullPath.end());
            g_CurrentMedia.totalTime = GetAudioDuration(nextPath);

            if (SUCCEEDED(CreatePlaybackTopology(nextPath)))
            {
                g_pSession->SetTopology(0, g_pTopology);

                PROPVARIANT varStart;
                PropVariantInit(&varStart);
                g_pSession->Start(&GUID_NULL, &varStart);
                PropVariantClear(&varStart);

                g_CurrentMedia.isPlaying = true;
                g_CurrentMedia.currentTime = 0.0f;
                g_PlaybackStartTime = GetTickCount();
            }
        }
    }

    void Previous()
    {
        if (g_Playlist.empty())
            return;

        int prevIndex = g_CurrentMedia.currentSongIndex - 1;
        if (prevIndex < 0)
            prevIndex = (int)g_Playlist.size() - 1;    

        LoadSong(prevIndex);
        if (g_CurrentMedia.isPlaying)
            PlayPause();    
    }

    void Stop()
    {
        PlaySoundA(NULL, NULL, SND_PURGE);

        g_CurrentMedia.isPlaying = false;
        g_CurrentMedia.canPlay = true;
        g_CurrentMedia.canPause = false;
        g_CurrentMedia.currentTime = 0.0f;

        if (g_pSession && g_AudioEngineReady)
        {
            g_pSession->Stop();
            g_pSession->ClearTopologies();
        }

        if (g_pTopology)
        {
            g_pTopology->Release();
            g_pTopology = nullptr;
        }
    }

    void SetVolume(float volume)
    {
        g_Volume = (std::max)(0.0f, (std::min)(1.0f, volume));

        if (g_pSession && g_AudioEngineReady)
        {
            IMFSimpleAudioVolume* pAudioVolume = nullptr;
            HRESULT hr = MFGetService(g_pSession, MR_POLICY_VOLUME_SERVICE, __uuidof(IMFSimpleAudioVolume), (void**)&pAudioVolume);
            if (SUCCEEDED(hr) && pAudioVolume)
            {
                pAudioVolume->SetMasterVolume(volume);
                pAudioVolume->Release();
            }
        }
    }

    void SeekTo(float position)
    {
        if (g_CurrentMedia.totalTime > 0)
        {
            g_CurrentMedia.currentTime = (std::max)(0.0f, (std::min)(position, g_CurrentMedia.totalTime));
        }
    }

    void UpdatePlaybackProgress()
    {
        if (g_CurrentMedia.isPlaying && g_PlaybackStartTime > 0)
        {
            DWORD currentTick = GetTickCount();
            g_CurrentMedia.currentTime = (currentTick - g_PlaybackStartTime) / 1000.0f;
            if (g_CurrentMedia.currentTime >= g_CurrentMedia.totalTime)
            {
                g_CurrentMedia.currentTime = g_CurrentMedia.totalTime;

                if (g_CurrentMedia.isPlaying)
                {
                    Next();
                }
            }
        }
    }

    const MediaInfo& GetCurrentMedia()
    {
        return g_CurrentMedia;
    }

    void RenderMediaControls()
    {
        ImGui::Text("Now: %s", g_CurrentMedia.title);
        if (strlen(g_CurrentMedia.artist) > 0 && strcmp(g_CurrentMedia.artist, "Unknown Artist") != 0)
        {
            ImGui::Text("By: %s", g_CurrentMedia.artist);
        }

        UpdatePlaybackProgress();

        if (g_CurrentMedia.isPlaying && g_PlaybackStartTime > 0)
        {
            DWORD currentTick = GetTickCount();
            g_CurrentMedia.currentTime = (currentTick - g_PlaybackStartTime) / 1000.0f;
            if (g_CurrentMedia.currentTime >= g_CurrentMedia.totalTime)
            {
                g_CurrentMedia.currentTime = g_CurrentMedia.totalTime;

                if (g_CurrentMedia.isPlaying)
                {
                    Next();
                }
            }
        }

        if (g_CurrentMedia.currentSongIndex >= 0)
        {
            float progress = g_CurrentMedia.totalTime > 0 ? g_CurrentMedia.currentTime / g_CurrentMedia.totalTime : 0.0f;
            char progressText[64];
            sprintf_s(progressText, "%.0f:%.02d / %.0f:%.02d",
                floor(g_CurrentMedia.currentTime / 60), (int)g_CurrentMedia.currentTime % 60,
                floor(g_CurrentMedia.totalTime / 60), (int)g_CurrentMedia.totalTime % 60);

            if (ImGui::CustomSliderFloat("Time", &progress, 0.0f, 1.0f, progressText))
            {
                if (g_CurrentMedia.totalTime > 0 && g_pSession && g_AudioEngineReady)
                {
                    float newTime = progress * g_CurrentMedia.totalTime;
                    LONGLONG seekTime = (LONGLONG)(newTime * 10000000);     

                    PROPVARIANT varSeek;
                    PropVariantInit(&varSeek);
                    varSeek.vt = VT_I8;
                    varSeek.hVal.QuadPart = seekTime;

                    g_pSession->Start(&GUID_NULL, &varSeek);
                    PropVariantClear(&varSeek);

                    g_CurrentMedia.currentTime = newTime;
                    g_PlaybackStartTime = GetTickCount() - (DWORD)(newTime * 1000);
                }
            }
        }

        ImGui::Spacing();

        ImGui::Separator();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(CFG::cfg_Colors_BackgroundR, CFG::cfg_Colors_BackgroundG, CFG::cfg_Colors_BackgroundB, 0.95f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 0.7f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 0.5f));     

        if (ImGui::Button("Last", ImVec2(40, 20)))
        {
            Previous();
        }

        ImGui::SameLine();
        const char* playPauseText = g_CurrentMedia.isPlaying ? "Pause" : "Play";
        if (ImGui::Button(playPauseText, ImVec2(40, 20)))
        {
            PlayPause();
        }

        ImGui::SameLine();
        if (ImGui::Button("Next", ImVec2(40, 20)))
        {
            Next();
        }

        ImGui::SameLine();
        if (ImGui::Button("Stop", ImVec2(40, 20)))
        {
            Stop();
        }

        ImGui::SameLine();
        if (g_ShuffleEnabled)
        {
            ImColor main_color = ImGui::GetStyleColorVec4(ImGuiCol_CheckMark);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(main_color.Value.x, main_color.Value.y, main_color.Value.z, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(main_color.Value.x * 1.2f, main_color.Value.y * 1.2f, main_color.Value.z * 1.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(main_color.Value.x * 1.4f, main_color.Value.y * 1.4f, main_color.Value.z * 1.4f, 1.0f));
        }
        if (ImGui::Button("Mix", ImVec2(40, 20)))
        {
            g_ShuffleEnabled = !g_ShuffleEnabled;
        }
        if (g_ShuffleEnabled)
        {
            ImGui::PopStyleColor(3);
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);


        ImGui::Spacing();

        float volumePercent = g_Volume * 100.0f;
        if (ImGui::CustomSliderFloat("Volume", &volumePercent, 0.0f, 100.0f, "%.0f%%"))
        {
            g_Volume = volumePercent / 100.0f;
            SetVolume(g_Volume);
        }

        ImGui::Separator();

        ImGui::Text("Playlist (%d songs):", (int)g_Playlist.size());

        if (g_Playlist.empty())
        {
            ImGui::Text("No songs found in Songs/ folder");
            ImGui::Text("Supported: .mp3, .wav, .ogg, .flac, .m4a, .wma");
            ImGui::Text("Place music files next to the .exe/.cfg");
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.08f, 0.9f));
            ImGui::BeginChild("Playlist", ImVec2(0, 150), true);

            for (int i = 0; i < (int)g_Playlist.size(); i++)
            {
                const SongInfo& song = g_Playlist[i];
                bool isCurrentSong = (i == g_CurrentMedia.currentSongIndex);

                char displayText[512];
                if (song.artist != "Unknown Artist")
                {
                    sprintf_s(displayText, "%s - %s", song.artist.c_str(), song.title.c_str());
                }
                else
                {
                    strcpy_s(displayText, song.title.c_str());
                }

                bool isHovered = false;
                ImVec2 itemPos = ImGui::GetCursorScreenPos();
                ImVec2 itemSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeight());

                if (ImGui::IsMouseHoveringRect(itemPos, ImVec2(itemPos.x + itemSize.x, itemPos.y + itemSize.y)))
                {
                    isHovered = true;
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    ImU32 glowColor = IM_COL32(
                        (int)(main_color.Value.x * 255),
                        (int)(main_color.Value.y * 255),
                        (int)(main_color.Value.z * 255),
                        30      
                    );
                    drawList->AddRectFilled(itemPos, ImVec2(itemPos.x + itemSize.x, itemPos.y + itemSize.y), glowColor, 3.0f);
                }

                if (isCurrentSong)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(main_color.Value.x, main_color.Value.y, main_color.Value.z, 1.0f));
                }

                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));

                if (ImGui::Selectable(displayText, isCurrentSong))
                {
                    if (g_pSession && g_AudioEngineReady)
                    {
                        g_pSession->Shutdown();
                        g_pSession->Release();
                        g_pSession = nullptr;
                    }

                    if (g_pTopology)
                    {
                        g_pTopology->Release();
                        g_pTopology = nullptr;
                    }

                    if (SUCCEEDED(MFCreateMediaSession(nullptr, &g_pSession)))
                    {
                        g_AudioEngineReady = true;

                        g_CurrentMedia.currentSongIndex = i;
                        const SongInfo& selectedSong = g_Playlist[i];

                        strcpy_s(g_CurrentMedia.title, selectedSong.title.c_str());
                        strcpy_s(g_CurrentMedia.artist, selectedSong.artist.c_str());

                        std::wstring selectedPath(selectedSong.fullPath.begin(), selectedSong.fullPath.end());
                        g_CurrentMedia.totalTime = GetAudioDuration(selectedPath);

                        if (SUCCEEDED(CreatePlaybackTopology(selectedPath)))
                        {
                            g_pSession->SetTopology(0, g_pTopology);

                            PROPVARIANT varStart;
                            PropVariantInit(&varStart);
                            g_pSession->Start(&GUID_NULL, &varStart);
                            PropVariantClear(&varStart);

                            g_CurrentMedia.isPlaying = true;
                            g_CurrentMedia.currentTime = 0.0f;
                            g_PlaybackStartTime = GetTickCount();
                        }
                    }
                }

                ImGui::PopStyleColor(3);

                if (isCurrentSong)
                {
                    ImGui::PopStyleColor();
                }
            }

            ImGui::EndChild();
            ImGui::PopStyleColor();     
        }
    }

    void RenderQuickLaunch()
    {
    }



}
