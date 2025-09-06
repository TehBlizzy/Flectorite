#pragma once

#include "../Core/SDK.h"
#include "../Menu/Config.h"
#include "References.h"
#include <unordered_map>

namespace Cheat
{
    struct CONTROLLER_STATE
    {
        XINPUT_STATE state;
        bool bConnected;
    };
    extern CONTROLLER_STATE g_Controllers[4];
    void Aimbot();
    void FindAimTarget(TFD::AActor* &ActorOut, int& BoneIndexOut);
    void SilentAim();
    TFD::AActor* GetClosestEnemyForTeleport();
    HRESULT UpdateControllerState();
}
