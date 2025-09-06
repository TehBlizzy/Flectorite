#include "WeaponModifications.h"

namespace Cheat
{
    void WeaponModifications()
    {
        if (!LocalPlayerCharacter)
            return;

        auto* WeaponControl = LocalPlayerCharacter->WeaponSlotControl;
        if (!WeaponControl)
            return;

        auto* WeaponBase = WeaponControl->ActivatedWeaponSlot.WeaponSlot.Weapon;
        if (!WeaponBase || !WeaponBase->IsA(TFD::AM1RangedWeapon::StaticClass()))
            return;

        TFD::AM1RangedWeapon* Weapon = static_cast<TFD::AM1RangedWeapon*>(WeaponBase);


        if (CFG::cfg_Aim_RapidFire && Weapon->FireLoopComponent)
        {
            auto& Params = Weapon->FireLoopComponent->CurrFireParams;
            if (!Params.IsSet())
                return;

            float& FireInterval = Params.GetValueRef().FireInterval;
            float& ElapsedTime = Weapon->FireLoopComponent->ElapsedTimeAfterFire;

            FireInterval -= FireInterval * (CFG::cfg_Aim_FireRate / 100.0f);
            ElapsedTime -= ElapsedTime * (CFG::cfg_Aim_FireRate / 100.0f);
        }

        if (CFG::cfg_Aim_NoReload && Weapon->RoundsComponent)
        {
            if (Weapon->RoundsComponent->CurrentRounds < 3)
            {
                Weapon->RoundsComponent->ClientFillCurrentRoundByServer();
            }
        }

        if (CFG::cfg_Aim_NoRecoilAndSpread && Weapon->SprayPatternComponent)
        {
            auto* Spray = Weapon->SprayPatternComponent;

            if (Spray->CrosshairSizeBase != 0.0f) Spray->CrosshairSizeBase = 0.0f;
            if (Spray->bApplySpreadSize != false) Spray->bApplySpreadSize = false;
            if (Spray->CurrentSpreadSize != 0.0f) Spray->CurrentSpreadSize = 0.0f;
            if (Spray->CurrAccuracySizeInterp != 0.000001f) Spray->CurrAccuracySizeInterp = 0.000001f;
            if (Spray->CurrBaseSizeInterp != 0.000001f) Spray->CurrBaseSizeInterp = 0.000001f;
            if (Spray->RecoverRecoilStartDelayTime != 0.000001f) Spray->RecoverRecoilStartDelayTime = 0.000001f;

            if (Spray->RecoilData->RecoilApplyInterpSpeed != 0.000001f) Spray->RecoilData->RecoilApplyInterpSpeed = 0.000001f;
            if (Spray->RecoilData->RecoilRecoverInterpSpeed != 0.000001f) Spray->RecoilData->RecoilRecoverInterpSpeed = 0.000001f;
            if (Spray->RecoilData->RecoilRecoverStartDelay != 0.000001f) Spray->RecoilData->RecoilRecoverStartDelay = 0.000001f;
            if (Spray->RecoilData->RecoilResetTimeAfterFire != 0.000001f) Spray->RecoilData->RecoilResetTimeAfterFire = 0.000001f;
            if (Spray->ZoomRecoilData->RecoilApplyInterpSpeed != 0.000001f) Spray->ZoomRecoilData->RecoilApplyInterpSpeed = 0.000001f;
            if (Spray->ZoomRecoilData->RecoilRecoverInterpSpeed != 0.000001f) Spray->ZoomRecoilData->RecoilRecoverInterpSpeed = 0.000001f;
            if (Spray->ZoomRecoilData->RecoilRecoverStartDelay != 0.000001f) Spray->ZoomRecoilData->RecoilRecoverStartDelay = 0.000001f;
            if (Spray->ZoomRecoilData->RecoilResetTimeAfterFire != 0.000001f) Spray->ZoomRecoilData->RecoilResetTimeAfterFire = 0.000001f;

            LocalPlayerController->PlayerCameraManager->StopAllCameraShakes(true);
        }
    }

    void ModifyGrapple()
    {
        if (!Cheat::GWorld || !LocalPlayerController || !LocalPlayerCharacter)
            return;

        auto* Caster = LocalPlayerCharacter->WireCaster;
        auto* WeaponControl = LocalPlayerCharacter->WeaponSlotControl;

        if (!Caster || !WeaponControl)
            return;

        auto* AbilityComponent = WeaponControl->Ability_Component;

        if (!AbilityComponent)
            return;

        for (auto* Ability : AbilityComponent->RegisteredAbilities)
        {
            if (!Ability || !Ability->IsA(TFD::UM1WireSkillAbility::StaticClass()))
                continue;
            auto* WireAbility = static_cast<TFD::UM1WireSkillAbility*>(Ability);

            if (WireAbility->FireMaxDistance != CFG::cfg_Abilities_GrappleRange)
            {
                Caster->bValidLandingSpot = true;
                WireAbility->FireMinDistance = 0.0f;
                WireAbility->FireMaxDistance = CFG::cfg_Abilities_GrappleRange;
                WireAbility->bAllowedAirDestination = true;
            }
        }
    }
}
