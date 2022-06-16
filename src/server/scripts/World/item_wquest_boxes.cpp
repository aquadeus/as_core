////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2016 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "ScriptPCH.h"
#include "ScriptMgr.h"

enum Items : uint32
{
    BloodOfSargeras         = 124124,
    HeavyArmorSet           = 136412,
    FortifiedArmorSet       = 137207,
    IndestructibleArmorSet  = 137208,
    TornInvitation          = 140495
};

/// All of the chests
/// Legionfall Spoils      : 000000 (7.0.3) - 000000 (7.1.5) - 152649 (7.3.0) - 154910 (7.3.2) - 157830 (7.3.5) Quest : 48641
/// Scuffed Krokul Cache   : 000000 (7.0.3) - 000000 (7.1.5) - 152650 (7.3.0) - 154911 (7.3.2) - 157831 (7.3.5) Quest : 48642
/// Gilded Trunk           : 000000 (7.0.3) - 000000 (7.1.5) - 152652 (7.3.0) - 154912 (7.3.2) - 157829 (7.3.5) Quest : 48639
/// Kirin Tor Chest        : 141350 (7.0.3) - 146753 (7.1.5) - 151470 (7.2.5) - 154909 (7.3.2) - 157828 (7.3.5) Quest : 43179
/// Highmountain Tribute   : 137561 (7.0.3) - 146748 (7.1.5) - 151465 (7.2.5) - 154904 (7.3.2) - 157823 (7.3.5) Quest : 42233
/// Dreamweaver Provisions : 137560 (7.0.3) - 146747 (7.1.5) - 151464 (7.2.5) - 154903 (7.3.2) - 157822 (7.3.5) Quest : 42170
/// Warden's Field Kit     : 137565 (7.0.3) - 146752 (7.1.5) - 151469 (7.2.5) - 154908 (7.3.2) - 157827 (7.3.5) Quest : 42422
/// Nightfallen Hoard      : 137564 (7.0.3) - 146751 (7.1.5) - 151468 (7.2.5) - 154907 (7.3.2) - 157826 (7.3.5) Quest : 42421
/// Valarjar Cache         : 137562 (7.0.3) - 146749 (7.1.5) - 151466 (7.2.5) - 154905 (7.3.2) - 157824 (7.3.5) Quest : 42234
/// Farondis Lockbox       : 137563 (7.0.3) - 146750 (7.1.5) - 151467 (7.2.5) - 154906 (7.3.2) - 157825 (7.3.5) Quest : 42420

/// 137562, 137564, 137565, 137560, 137563, 137561, 141350
class item_wquest_boxes : public ItemScript
{
    public:
        item_wquest_boxes() : ItemScript("item_wquest_boxes") { }

        enum eLegoUpgradeData
        {
            ItemMinCount    = 24,
            ItemMaxCount    = 42
        };

        enum eEmissaryBoxes
        {
            LegionfallBox = 152649,
            NightfallenHoard = 137564
        };

        bool OnOpen(Player* p_Player, Item* p_Item)
        {
            ItemTemplate const* l_Proto = p_Item->GetTemplate();
            LootTemplate const* l_LootTemplate = LootTemplates_Item.GetLootFor(141350); ///< Pointlessly dont add a shitton of loot, have it grouped
            if (!l_LootTemplate)
                return false;

            std::list<ItemTemplate const*> l_LootTable;
            std::vector<uint32> l_Items;
            l_LootTemplate->FillAutoAssignationLoot(l_LootTable);
            uint32 l_SpecID = p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID();

            for (ItemTemplate const* l_Template : l_LootTable)
            {
                if ((l_Template->AllowableClass && !(l_Template->AllowableClass & p_Player->getClassMask())) ||
                    (l_Template->AllowableRace && !(l_Template->AllowableRace & p_Player->getRaceMask())))
                    continue;

                if (l_Template->HasSpec(static_cast<SpecIndex>(l_SpecID), p_Player->getLevel()))
                    l_Items.push_back(l_Template->ItemId);
            }

            uint32 l_ItemEntry = p_Item->GetEntry();
            /// Remove self first because of inventory space
            p_Player->DestroyItem(p_Item->GetBagSlot(), p_Item->GetSlot(), true);
            p_Item = nullptr; ///< has been deleted in DestroyItem;

            if (l_Items.empty())
                return true;

            std::random_device l_RandomDevice;
            std::mt19937 l_RandomGenerator(l_RandomDevice());
            std::shuffle(l_Items.begin(), l_Items.end(), l_RandomGenerator);

            std::vector<uint32> l_DummyBonuses;
            Item* l_Item = p_Player->AddItem(l_Items[0], 1, {}, false, 0, false, true, (ItemContext)p_Player->GetWorldQuestManager()->GenerateItemContext(MS::WorldQuest::WorldQuestType::Bounty, sObjectMgr->GetItemTemplate(l_Items[0]), l_DummyBonuses));
            if (l_Item)
                p_Player->SendDisplayToast(l_Items[0], 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false, l_Item->GetAllItemBonuses());

            /// Either gold or resources 50% chance
            if (roll_chance_i(50))
            {
                uint32 l_GoldAmount = uint32(urand(70, 89) * 10 * GOLD * sWorld->getRate(RATE_DROP_MONEY));

                p_Player->ModifyMoney(l_GoldAmount, "item_wquest_boxes");
                p_Player->SendDisplayToast(0, l_GoldAmount, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_MONEY, false, false);
            }
            else
            {
                /// swap between legionfall and other emissaries
                if (l_ItemEntry == eEmissaryBoxes::LegionfallBox) ///< Legionfall spoils contain 500 nethershards instead of order ressources
                {
                    p_Player->ModifyCurrency(CURRENCY_TYPE_NETHERSHARD, 500);
                    p_Player->SendDisplayToast(CURRENCY_TYPE_NETHERSHARD, 500, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_NEW_CURRENCY, false, false);
                }

                int32 l_CurrencyAmount = urand(700, 890);

                p_Player->ModifyCurrency(CURRENCY_TYPE_ORDER_RESSOURCES, l_CurrencyAmount);
                p_Player->SendDisplayToast(CURRENCY_TYPE_ORDER_RESSOURCES, l_CurrencyAmount, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_NEW_CURRENCY, false, false);
            }

            /// Blood of sargeras with a 25% chance
            if (roll_chance_i(25))
            {
                Item* l_Item = p_Player->AddItem(Items::BloodOfSargeras, 1, {}, false, 0, false, true, ItemContext::None);
                if (l_Item)
                    p_Player->SendDisplayToast(Items::BloodOfSargeras, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false, l_Item->GetAllItemBonuses());
            }

            /// Curious coin 20% chance
            if (roll_chance_i(20))
            {
                p_Player->ModifyCurrency(CURRENCY_TYPE_CURIOUS_COIN, 1);
                p_Player->SendDisplayToast(CURRENCY_TYPE_CURIOUS_COIN, 1, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_NEW_CURRENCY, false, false);
            }

            /// Small chance to get follower equipement
            if (roll_chance_i(10))
            {
                uint32 l_ItemId = Items::HeavyArmorSet;

                if (roll_chance_i(30))
                    l_ItemId = Items::FortifiedArmorSet;
                else if (roll_chance_i(5))
                    l_ItemId = Items::IndestructibleArmorSet;

                Item* l_Item = p_Player->AddItem(l_ItemId, 1, {}, false, 0, false, true, ItemContext::None);
                if (l_Item)
                    p_Player->SendDisplayToast(l_ItemId, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false, l_Item->GetAllItemBonuses());
            }

            /// Wakening Essences
            {
                uint32 l_Count = urand(eLegoUpgradeData::ItemMinCount, eLegoUpgradeData::ItemMaxCount);

                p_Player->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_WAKENING_ESSENCE, l_Count);
                p_Player->SendDisplayToast(CurrencyTypes::CURRENCY_TYPE_WAKENING_ESSENCE, l_Count, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_NEW_CURRENCY, false, false);
            }

            /// Small chance to get quest item
            if (l_ItemEntry == eEmissaryBoxes::NightfallenHoard)
            {
                if (roll_chance_i(10))
                {
                    Item* l_Item = p_Player->AddItem(Items::TornInvitation, 1, {}, false, 0, false, true, ItemContext::None);
                    if (l_Item)
                        p_Player->SendDisplayToast(Items::TornInvitation, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false, l_Item->GetAllItemBonuses());
                }
            }

            p_Player->CompleteLegendaryActivity(LegendaryActivity::Emissary);
            return true;
        }
};

/// Opening - 263156 (Legionfall Spoils), 263154 (Kirin Tor Chest), 249085 (Dreamweaver Cache)
/// Opening - 243128 (Valarjar Strongbox), 249084 (Farondis Chest), 249086 (Highmountain Supplies)
/// Opening - 249087 (Nightfallen Cache), 249090 (Warden's Supply Kit), 249091 (Legionfall Chest)
/// Opening - 253747 (Brittle Krokul Chest), 253748 (Gleaming Footlocker), 263155 (Gilded Trunk), 263157 (Scuffed Krokul Cache)
class spell_wquest_boxes : public SpellScriptLoader
{
    public:
        spell_wquest_boxes() : SpellScriptLoader("spell_wquest_boxes") { }

        enum eLegoUpgradeData
        {
            ItemMinCount    = 24,
            ItemMaxCount    = 42
        };

        enum eEmissaryBoxes
        {
            SuppliesFromTheValarjar         = 146901,
            SuppliesFromTheCourt            = 152102,
            SuppliesFromTheDreamweavers     = 152103,
            SuppliesFromHighmountain        = 152104,
            SuppliesFromTheNightfallen      = 152105,
            SuppliesFromTheWardens          = 152107,
            TheBountiesOfLegionfall         = 152108,
            SuppliesFromTheArgussianReach   = 152922,
            SuppliesFromTheArmyOfTheLight   = 152923,

            GildedTrunk                     = 263155,
            ScuffedKrokulCache              = 263157
        };

        enum eParagonItems
        {
            /// Mounts
            ItemValarjarStormwing       = 147805,
            ItemHighmountainElderhorn   = 147807,
            ItemCloudwingHippogryph     = 147806,
            ItemLeywovenFlyingCarpet    = 143764,
            ItemWildDreamrunner         = 147804,
            ItemGloriousFelcrusher      = 153042,
            ItemBlessedFelcrusher       = 153043,
            ItemAvengingFelcrusher      = 153044,

            /// Toys
            ItemSirasExtraCloak         = 147843,
            ItemHolyLightsphere         = 153182,

            /// Pet
            ItemOrphanedFelbat          = 147841
        };

        class spell_wquest_boxes_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_wquest_boxes_SpellScript);

            void HandleCreateItem(SpellEffIndex p_EffectIndex)
            {
                PreventHitDefaultEffect(p_EffectIndex);

                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                Item* l_CastItem = l_Player->GetItemByGuid(GetSpell()->m_castItemGUID);
                if (!l_CastItem)
                    return;

                ItemTemplate const* l_Proto = l_CastItem->GetTemplate();
                if (!l_Proto)
                    return;

                LootTemplate const* l_LootTemplate = LootTemplates_Item.GetLootFor(141350); ///< Pointlessly dont add a shitton of loot, have it grouped
                if (!l_LootTemplate)
                    return;

                std::list<ItemTemplate const*> l_LootTable;
                std::vector<uint32> l_Items;

                if (m_scriptSpellId == eEmissaryBoxes::GildedTrunk || m_scriptSpellId == eEmissaryBoxes::ScuffedKrokulCache)
                {
                    LootTemplate const* l_LootTemplate = LootTemplates_Item.GetLootFor(152650); ///< Loot for Argus emissary chests
                    if (!l_LootTemplate)
                        return;

                    for (auto l_Itr : l_LootTemplate->GetItemList())
                        l_Items.push_back(l_Itr.itemid);
                }
                else
                {
                    LootTemplate const* LootTemplate = LootTemplates_Item.GetLootFor(141350); ///< Pointlessly dont add a shitton of loot, have it grouped
                    if (!LootTemplate)
                        return;

                    LootTemplate->FillAutoAssignationLoot(l_LootTable);

                    uint32 l_SpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

                    for (ItemTemplate const* l_Template : l_LootTable)
                    {
                        if ((l_Template->AllowableClass && !(l_Template->AllowableClass & l_Player->getClassMask())) ||
                            (l_Template->AllowableRace && !(l_Template->AllowableRace & l_Player->getRaceMask())))
                            continue;

                        if (l_Template->HasSpec(static_cast<SpecIndex>(l_SpecID), l_Player->getLevel()))
                            l_Items.push_back(l_Template->ItemId);
                    }
                }

                /// Remove self first because of inventory space
                l_Player->DestroyItem(l_CastItem->GetBagSlot(), l_CastItem->GetSlot(), true);

                if (l_Items.empty())
                    return;

                std::random_device l_RandomDevice;
                std::mt19937 l_RandomGenerator(l_RandomDevice());
                std::shuffle(l_Items.begin(), l_Items.end(), l_RandomGenerator);

                std::vector<uint32> l_Dummys;

                Item* l_Item = l_Player->AddItem(l_Items[0], 1, {}, false, 0, false, true, (ItemContext)l_Player->GetWorldQuestManager()->GenerateItemContext(MS::WorldQuest::WorldQuestType::Bounty, sObjectMgr->GetItemTemplate(l_Items[0]), l_Dummys));
                if (l_Item)
                    l_Player->SendDisplayToast(l_Items[0], 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false, l_Item->GetAllItemBonuses());

                /// Either gold or resources 50% chance
                if (roll_chance_i(50))
                {
                    uint32 l_GoldAmount = uint32(urand(70, 89) * 10 * GOLD * sWorld->getRate(RATE_DROP_MONEY));

                    l_Player->ModifyMoney(l_GoldAmount, "item_wquest_boxes");
                    l_Player->SendDisplayToast(0, l_GoldAmount, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_MONEY, false, false);
                }

                int32 l_CurrencyAmount = urand(700, 890);
                l_Player->ModifyCurrency(CURRENCY_TYPE_ORDER_RESSOURCES, l_CurrencyAmount);
                l_Player->SendDisplayToast(CURRENCY_TYPE_ORDER_RESSOURCES, l_CurrencyAmount, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_NEW_CURRENCY, false, false);

                /// Blood of sargeras with a 25% chance
                if (roll_chance_i(25))
                {
                    Item* l_Item = l_Player->AddItem(Items::BloodOfSargeras, 1, {}, false, 0, false, true, ItemContext::None);
                    if (l_Item)
                        l_Player->SendDisplayToast(Items::BloodOfSargeras, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false, l_Item->GetAllItemBonuses());
                }

                /// Curious coin 20% chance
                if (roll_chance_i(20))
                {
                    l_Player->ModifyCurrency(CURRENCY_TYPE_CURIOUS_COIN, 1);
                    l_Player->SendDisplayToast(CURRENCY_TYPE_CURIOUS_COIN, 1, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_NEW_CURRENCY, false, false);
                }

                /// Small chance to get follower equipement
                if (roll_chance_i(10))
                {
                    uint32 l_ItemId = Items::HeavyArmorSet;

                    if (roll_chance_i(30))
                        l_ItemId = Items::FortifiedArmorSet;
                    else if (roll_chance_i(5))
                        l_ItemId = Items::IndestructibleArmorSet;

                    Item* l_Item = l_Player->AddItem(l_ItemId, 1, {}, false, 0, false, true, ItemContext::None);
                    if (l_Item)
                        l_Player->SendDisplayToast(l_ItemId, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false, l_Item->GetAllItemBonuses());

                }

                std::set<uint32> l_ParagonChests =
                {
                    eEmissaryBoxes::SuppliesFromTheValarjar,
                    eEmissaryBoxes::SuppliesFromTheCourt,
                    eEmissaryBoxes::SuppliesFromTheDreamweavers,
                    eEmissaryBoxes::SuppliesFromHighmountain,
                    eEmissaryBoxes::SuppliesFromTheNightfallen,
                    eEmissaryBoxes::SuppliesFromTheWardens,
                    eEmissaryBoxes::TheBountiesOfLegionfall,
                    eEmissaryBoxes::SuppliesFromTheArgussianReach,
                    eEmissaryBoxes::SuppliesFromTheArmyOfTheLight,
                };

                /// Wakening Essences
                if (l_ParagonChests.find(l_Proto->ItemId) == l_ParagonChests.end())
                {
                    uint32 l_Count = urand(eLegoUpgradeData::ItemMinCount, eLegoUpgradeData::ItemMaxCount);

                    l_Player->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_WAKENING_ESSENCE, l_Count);
                    l_Player->SendDisplayToast(CurrencyTypes::CURRENCY_TYPE_WAKENING_ESSENCE, l_Count, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_NEW_CURRENCY, false, false);
                }

                /// Veiled Argunite
                if  (m_scriptSpellId == eEmissaryBoxes::GildedTrunk || m_scriptSpellId == eEmissaryBoxes::ScuffedKrokulCache)
                {
                    int32 l_CurrencyAmount = urand(80, 150);

                    l_Player->ModifyCurrency(CURRENCY_TYPE_VEILED_ARGUNITE, l_CurrencyAmount);
                    l_Player->SendDisplayToast(CURRENCY_TYPE_VEILED_ARGUNITE, l_CurrencyAmount, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_NEW_CURRENCY, false, false);
                }

                std::map<uint32, std::vector<uint32>> l_ParagonItems =
                {
                    { eEmissaryBoxes::SuppliesFromTheValarjar,          { eParagonItems::ItemValarjarStormwing } },
                    { eEmissaryBoxes::SuppliesFromTheCourt,             { eParagonItems::ItemCloudwingHippogryph } },
                    { eEmissaryBoxes::SuppliesFromTheDreamweavers,      { eParagonItems::ItemWildDreamrunner } },
                    { eEmissaryBoxes::SuppliesFromHighmountain,         { eParagonItems::ItemHighmountainElderhorn } },
                    { eEmissaryBoxes::SuppliesFromTheNightfallen,       { eParagonItems::ItemLeywovenFlyingCarpet } },
                    { eEmissaryBoxes::SuppliesFromTheWardens,           { eParagonItems::ItemSirasExtraCloak } },
                    { eEmissaryBoxes::TheBountiesOfLegionfall,          { eParagonItems::ItemOrphanedFelbat } },
                    { eEmissaryBoxes::SuppliesFromTheArmyOfTheLight,    { eParagonItems::ItemGloriousFelcrusher, eParagonItems::ItemBlessedFelcrusher, eParagonItems::ItemAvengingFelcrusher, eParagonItems::ItemHolyLightsphere } }
                };

                /// Paragon mounts & toys ~5% drop rate
                auto const& l_Iter = l_ParagonItems.find(l_Proto->ItemId);
                if (l_Iter != l_ParagonItems.end() && roll_chance_i(5))
                {
                    int32 l_ItemID = JadeCore::Containers::SelectRandomContainerElement(l_Iter->second);

                    bool l_AlreadyHasItem = l_Player->GetItemByEntry(l_ItemID);

                    if (!l_AlreadyHasItem)
                    {
                        if (ItemTemplate const* l_ParagonItem = sObjectMgr->GetItemTemplate(l_ItemID))
                        {
                            for (uint8 l_I = 0; l_I < MAX_ITEM_PROTO_SPELLS; ++l_I)
                            {
                                if (l_ParagonItem->Spells[l_I].SpellId && l_Player->HasSpell(l_ParagonItem->Spells[l_I].SpellId))
                                {
                                    l_AlreadyHasItem = true;
                                    break;
                                }
                            }
                        }
                    }

                    if (!l_AlreadyHasItem)
                    {
                        Item* l_Item = l_Player->AddItem(l_ItemID, 1, { }, false, 0, false, true, ItemContext::None);
                        if (l_Item)
                            l_Player->SendDisplayToast(l_ItemID, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);
                    }
                }

                l_Player->CompleteLegendaryActivity(LegendaryActivity::Emissary);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_wquest_boxes_SpellScript::HandleCreateItem, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_LOOT_BONUS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_wquest_boxes_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_item_wquest_boxeses()
{
    new item_wquest_boxes();

    new spell_wquest_boxes();
}
#endif
