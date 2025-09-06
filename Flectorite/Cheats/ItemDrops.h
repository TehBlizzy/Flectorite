#pragma once

#include "../Core/SDK.h"
#include "../pch.h"
#include "../Menu/Config.h"
#include "../Cheats/References.h"
#include "../Core/Cheat.h"
#include "../Menu/Render.h"
#include "../Menu/Menu.h"

namespace Cheat
{
    extern bool TrySpawnGenericLoot;
    extern bool TrySpawnVaultLoot;
    extern bool RestartDecoding;

    void Loot();
    void GrabLoot();
    void MultiplyDrops(TFD::AActor* Actor, int Amount, bool isVault);
    bool GetSpareRounds(TFD::EM1RoundsType RoundsType, int RoundsPerLoot);
    void DebugActorsInRange();
    void GrabContainers();
}
