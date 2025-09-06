#pragma once

#include "References.h"
#include "../Core/SDK.h"

namespace Cheat
{
    extern bool TryAddAllItems;
    void AddAllCustomizationItems();
    extern bool TryEquipCustomization;
    bool TryEquipSavedCustomization();
    void InitCustomizationData();
    void __fastcall hkReceiveCustomizingCharacterSkin(TFD::UM1PrivateOnlineServiceCustomize* This, TFD::FM1TemplateId InTargetCharacterTid, TFD::FM1TemplateId InSkinTid, bool bEquip, TFD::EM1CustomizeReason InReason);
}