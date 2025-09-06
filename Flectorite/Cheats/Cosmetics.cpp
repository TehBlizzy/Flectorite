#include "Cosmetics.h"

#include "../Menu/Config.h"
#include <iostream>
#include <fstream>
#include <format>

namespace Cheat
{

    TFD::TArray<TFD::FM1CustomizingItemData*> CustomizationTableData;
    std::unordered_map<int, int> CategoryToSlotMap = { {1, 0}, {2, 1}, {3, 2}, {4, 3}, {5, 5}, {6, 4} };
    std::unordered_map<int, int> IDToCategoryMap = {};

    void InitCustomizationData()
    {
        TFD::TM1DataTable* CustomizationTable = TFD::native_GetCustomizationTable(false);
        if (!CustomizationTable)
            return;

        static SDK::FString Context(L"UM1AccountInventory");
        TFD::native_GetCustomizationData(CustomizationTable->TableObject, &Context, &CustomizationTableData);

        for (int i = 0; i < CustomizationTableData.Num(); i++)
        {
            TFD::FM1CustomizingItemData* itm = CustomizationTableData[i];
            if (itm->Category == TFD::EM1CustomizingItemCategoryType::CharacterHeadSkin ||
                itm->Category == TFD::EM1CustomizingItemCategoryType::CharacterBodySkin ||
                itm->Category == TFD::EM1CustomizingItemCategoryType::CharacterBackAttachment ||
                itm->Category == TFD::EM1CustomizingItemCategoryType::CharacterChestAttachment ||
                itm->Category == TFD::EM1CustomizingItemCategoryType::CharacterMakeupSkin ||
                itm->Category == TFD::EM1CustomizingItemCategoryType::CharacterSpawnSkin)
            {
                IDToCategoryMap.emplace(itm->TemplateId.ID, (int)itm->Category);
            }
        }
        //std::cout << "Customization count: " << IDToCategoryMap.size() << "\n";
    }



    bool TryAddAllItems = false;
    void AddAllCustomizationItems()
    {
        static TFD::UM1Account* Account;
        static TFD::TArray<TFD::FM1CustomizingInfoWrapper> Items;
        if (Items.NumElements == 0)
        {
            //// Get ALL customization tIDs
            //TFD::TM1DataTable* CustomizationTable = TFD::native_GetCustomizationTable(false);
            //if (!CustomizationTable)
            //    return;

            //SDK::TArray<TFD::FM1CustomizingItemData*> Data;
            //static SDK::FString Context(L"UM1AccountInventory");
            //TFD::native_GetCustomizationData(CustomizationTable->TableObject, &Context, &Data);

            // If you want to dump them all to a file, uncomment this block
            //std::ofstream outFile("customization_data.txt");
            //if (outFile.is_open())
            //{
            //    for (int i = 0; i < Data.Num(); i++)
            //    {
            //        TFD::FM1CustomizingItemData* itm = Data[i];
            //        std::string text = std::format("[{}] - tID: {} - Name: {}", (int)itm->Category, itm->TemplateId.ID, itm->StringId.ToString().c_str());
            //        outFile << text << "\n";
            //    }
            //    outFile.close();
            //}

            int Count = 0;
            for (int i = 0; i < CustomizationTableData.Num(); i++)
            {
                Count++;
            }
            if (Count > 0)
            {
                Items.Data = (TFD::FM1CustomizingInfoWrapper*)TFD::native_FMemMalloc(20 * Count, 0);
                Items.NumElements = 0;
                Items.MaxElements = Count;
                if (!LocalPlayerController->PrivateOnlineServiceComponent)
                    return;
                Account = (TFD::UM1Account*)TFD::native_GetUM1Account(LocalPlayerController->PrivateOnlineServiceComponent);
                for (int i = 0; i < CustomizationTableData.Num(); i++)
                {
                    TFD::FM1CustomizingItemData* itm = CustomizationTableData[i];
                    if (itm->Category == TFD::EM1CustomizingItemCategoryType::CharacterHeadSkin ||
                        itm->Category == TFD::EM1CustomizingItemCategoryType::CharacterBodySkin ||
                        itm->Category == TFD::EM1CustomizingItemCategoryType::CharacterBackAttachment ||
                        itm->Category == TFD::EM1CustomizingItemCategoryType::CharacterChestAttachment ||
                        itm->Category == TFD::EM1CustomizingItemCategoryType::CharacterMakeupSkin ||
                        itm->Category == TFD::EM1CustomizingItemCategoryType::CharacterSpawnSkin)
                    {
                        TFD::FM1CustomizingInfoWrapper ItemWrapper = {};
                        ItemWrapper.bNewItem = true;
                        ItemWrapper.CustomizingItemInfo = {};
                        ItemWrapper.CustomizingItemInfo.StackCount = 1;
                        ItemWrapper.CustomizingItemInfo.Tid = itm->TemplateId;
                        ItemWrapper.CustomizingItemInfo.EvolutionComplete = true;
                        ItemWrapper.CustomizingItemInfo.EvolutionIdx = TFD::native_GetSkinEvolutionIdx(Account->Inventory, itm->TemplateId);
                        Items.Add(ItemWrapper);
                    }
                }
            }
        }
        if (Items.NumElements > 0)
        {
            if (!LocalPlayerController->PrivateOnlineServiceComponent)
                return;
            Account = (TFD::UM1Account*)TFD::native_GetUM1Account(LocalPlayerController->PrivateOnlineServiceComponent);
            TFD::native_AddOrUpdateCustomizingItems(Account->Inventory, &Items, true);
        }
    }
    bool TryEquipCustomization = false;
    bool TryEquipSavedCustomization()
    {
        if (!Cheat::LocalPlayerCharacter)
            return false;
        if (!Cheat::LocalPlayerCharacter->CustomizeComponent)
            return false;
        if (!Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray.IsValid())
            return false;
        if (Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray.Num() != 6)
            return false;

        static TFD::UM1Account* Account = (TFD::UM1Account*)TFD::native_GetUM1Account(LocalPlayerController->PrivateOnlineServiceComponent);
        if (Account)
        {
            int SaveSlot = -1;
            for (int i = 0; i < 3; i++)
                if (CFG::cfg_Customize_SaveSlots[i].CharacterID == LocalPlayerCharacter->CharacterId.ID)
                {
                    SaveSlot = i;
                    break;
                }

            if (SaveSlot != -1)
            {
                if (CFG::cfg_Customize_SaveSlots[SaveSlot].Body != 0)
                    TFD::native_AccountEquipCustomizeCharacterSkin(Account->Inventory, LocalPlayerCharacter->CharacterId, 0, TFD::FM1TemplateId{ CFG::cfg_Customize_SaveSlots[SaveSlot].Body }, true);
                if (CFG::cfg_Customize_SaveSlots[SaveSlot].Head != 0)
                    TFD::native_AccountEquipCustomizeCharacterSkin(Account->Inventory, LocalPlayerCharacter->CharacterId, 0, TFD::FM1TemplateId{ CFG::cfg_Customize_SaveSlots[SaveSlot].Head }, true);
                if (CFG::cfg_Customize_SaveSlots[SaveSlot].Chest != 0)
                    TFD::native_AccountEquipCustomizeCharacterSkin(Account->Inventory, LocalPlayerCharacter->CharacterId, 0, TFD::FM1TemplateId{ CFG::cfg_Customize_SaveSlots[SaveSlot].Chest }, true);
                if (CFG::cfg_Customize_SaveSlots[SaveSlot].Makeup != 0)
                    TFD::native_AccountEquipCustomizeCharacterSkin(Account->Inventory, LocalPlayerCharacter->CharacterId, 0, TFD::FM1TemplateId{ CFG::cfg_Customize_SaveSlots[SaveSlot].Makeup }, true);
                if (CFG::cfg_Customize_SaveSlots[SaveSlot].Back != 0)
                    TFD::native_AccountEquipCustomizeCharacterSkin(Account->Inventory, LocalPlayerCharacter->CharacterId, 0, TFD::FM1TemplateId{ CFG::cfg_Customize_SaveSlots[SaveSlot].Back }, true);
                if (CFG::cfg_Customize_SaveSlots[SaveSlot].Spawn != 0)
                    TFD::native_AccountEquipCustomizeCharacterSkin(Account->Inventory, LocalPlayerCharacter->CharacterId, 0, TFD::FM1TemplateId{ CFG::cfg_Customize_SaveSlots[SaveSlot].Spawn }, true);

                Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[0].SkinTid.ID = CFG::cfg_Customize_SaveSlots[SaveSlot].Head;
                Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[1].SkinTid.ID = CFG::cfg_Customize_SaveSlots[SaveSlot].Body;
                Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[2].SkinTid.ID = CFG::cfg_Customize_SaveSlots[SaveSlot].Back;
                Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[3].SkinTid.ID = CFG::cfg_Customize_SaveSlots[SaveSlot].Chest;
                Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[4].SkinTid.ID = CFG::cfg_Customize_SaveSlots[SaveSlot].Spawn;
                Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[5].SkinTid.ID = CFG::cfg_Customize_SaveSlots[SaveSlot].Makeup;

                //TFD::native_SetCharacterCustomizingAsInventory(LocalPlayerCharacter->Mesh, &LocalPlayerCharacter->CustomizeComponent->CustomizingSkinComps, LocalPlayerCharacter->CharacterId, Account->Inventory);
                TFD::native_OnRep_CustomizeCharacterSkinData(Cheat::LocalPlayerCharacter->CustomizeComponent);
                return true;
            }
            else
            {
                return true;
            }
        }
        return false;
    }

    void __fastcall hkReceiveCustomizingCharacterSkin(TFD::UM1PrivateOnlineServiceCustomize* This, TFD::FM1TemplateId InTargetCharacterTid, TFD::FM1TemplateId InSkinTid, bool bEquip, TFD::EM1CustomizeReason InReason)
    {
        // std::cout << InTargetCharacterTid.ID << " " << InSkinTid.ID << " " << bEquip << " " << (int)InReason << std::endl;
        if (CFG::cfg_Customize_EnableCustomizationBypass)
        {
            if (IDToCategoryMap.contains(InSkinTid.ID))
            {
                int Category = IDToCategoryMap[InSkinTid.ID];
                //std::cout << "Category for ID " << InSkinTid.ID << ": " << Category << "\n";
                int Slot = CategoryToSlotMap[Category];
                //std::cout << "Slot for Category " << Category << ": " << Slot << "\n";
                if (bEquip)
                    LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[Slot].SkinTid.ID = InSkinTid.ID;
                else
                    LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[Slot].SkinTid.ID = 0;

                //if (LocalPlayerCharacter)
                //{
                //    if (LocalPlayerCharacter->CustomizeComponent)
                //    {
                //        if (LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray.IsValid())
                //        {
                //            std::cout << std::hex << static_cast<void*>(LocalPlayerCharacter->CustomizeComponent) << std::dec << std::endl;
                //            std::cout << "Num: " <<  LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray.Num() << " - Max: " << LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray.Max() << "\n";
                //            for (int i = 0; i < LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray.Num(); i++)
                //            {
                //                TFD::FM1CustomizeSkinInfo Info = LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[i];
                //                std::cout << i << " - " << Info.TargetTid.ID << " - " << Info.SkinTid.ID << " - " << Info.LoadoutSlotIndex <<" - " << Info.EvolutionLevelIndex << "\n";

                //            }
                //        
                //        }
                //    }
                //}

                TFD::native_ReceiveCustomizingCharacterSkin(This, InTargetCharacterTid, InSkinTid, bEquip, TFD::EM1CustomizeReason::Success);
                TFD::native_OnRep_CustomizeCharacterSkinData(Cheat::LocalPlayerCharacter->CustomizeComponent);
            }
        }
        else
            return TFD::native_ReceiveCustomizingCharacterSkin(This, InTargetCharacterTid, InSkinTid, bEquip, InReason);
    }
}