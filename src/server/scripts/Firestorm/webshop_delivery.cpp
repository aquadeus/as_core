////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptPCH.h"
#include "ScriptMgr.h"
#include "Guild.h"
#include "Player.h"

namespace WebShop
{
    enum Language
    {
        GoldLimitReach      = 3000,
        GoldAddNextLogin    = 3001,
        GoldAdded           = 3002,
        DontHaveEnoughSpace = 3003,
        HasAlreadyTitle     = 3035,
        TitleAdd            = 3036
    };

    /// ...of Draenor Spell
    enum ProfessionBookSpells
    {
        Alchemy         = 195095,
        Blacksmithing   = 195097,
        Enchanting      = 195096,
        Engineering     = 195112,
        Inscription     = 195115,
        JewelCrafting   = 195116,
        LeatherWorking  = 195119,
        Tailoring       = 195126,
        FirstAid        = 195113,
        Cooking         = 195128,
        Herbalism       = 195114,
        Mining          = 195122,
        Skinning        = 195125,
        Archaeology     = 195127,
        Fishing         = 210829,
    };

    enum ProfessionAdditionalSpells
    {
        Prospecting  = 31252,
        FishingDummy = 227511,
    };

    class Delivery_Item : public PlayerScript
    {
        private:

        public:
            Delivery_Item() : PlayerScript("WebshopDelivery_Item")
            {
            }

            void OnLogin(Player* p_Player) override
            {
                uint32 l_LowGuid = p_Player->GetGUIDLow();
                QueryResultFuture& l_Callback = p_Player->m_QueryResultFutures[WEBSHOP_TYPE_ITEM];
                l_Callback = CharacterDatabase.AsyncPQuery("SELECT itemid, ItemBonus, count, transaction FROM webshop_delivery_item WHERE guid = '%u' and delivery = 0", l_LowGuid);
            }

            void OnUpdate(Player* p_Player, uint32 /*p_Diff*/) override
            {
                uint32 l_LowGuid = p_Player->GetGUIDLow();

                auto l_Itr = p_Player->m_QueryResultFutures.find(WEBSHOP_TYPE_ITEM);
                if (l_Itr == p_Player->m_QueryResultFutures.end())
                    return;

                QueryResultFuture& l_Callback = l_Itr->second;
                if (!l_Callback.ready())
                    return;

                QueryResult l_Result;
                l_Callback.get(l_Result);

                if (!l_Result)
                {
                    l_Callback.cancel();
                    p_Player->m_QueryResultFutures.erase(WEBSHOP_TYPE_ITEM);
                    p_Player->SetStoreDeliveryProccesed(StoreCallback::ItemDelivery);
                    return;
                }

                do
                {
                    Field* l_Field = l_Result->Fetch();
                    uint32 l_ItemID = l_Field[0].GetUInt32();
                    std::string l_Bonuses = l_Field[1].GetString();
                    uint32 l_ItemCount = l_Field[2].GetUInt32();
                    uint32 l_Transaction = l_Field[3].GetUInt32();

                    ItemPosCountVec l_Dest;
                    InventoryResult l_Message = p_Player->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Dest, l_ItemID, l_ItemCount);

                    if (l_Message != EQUIP_ERR_OK)
                    {
                        ChatHandler(p_Player).PSendSysMessage(WebShop::Language::DontHaveEnoughSpace);
                        continue;
                    }

                    std::list<uint32> l_BonusesID;
                    Tokenizer l_Tokens(l_Bonuses, ',');

                    for (Tokenizer::const_iterator l_Iter = l_Tokens.begin(); l_Iter != l_Tokens.end(); ++l_Iter)
                        l_BonusesID.push_back(uint32(atol(*l_Iter)));

                    bool l_LoyaltyLootBox = false;// l_ItemID == 300000 && l_Bonuses == std::string("2");

                    switch (l_ItemID)
                    {
                        case 300000:
                        case 300001:
                        case 300010:
                        case 300020:
                        case 310000:
                            l_BonusesID.clear();
                        default:
                            break;
                    }

                    Item* l_Item = p_Player->AddItem(l_ItemID, l_ItemCount, l_BonusesID, true);
                    if (l_Item != nullptr)
                    {
                        CharacterDatabase.PExecute("UPDATE webshop_delivery_item SET delivery = 1 WHERE transaction = %u", l_Transaction);

                        if (l_LoyaltyLootBox)
                            l_Item->ApplyCustomFlags(ItemCustomFlags::LoyaltyLootBox);
                    }
                }
                while (l_Result->NextRow());

                l_Callback.cancel();
                p_Player->m_QueryResultFutures.erase(WEBSHOP_TYPE_ITEM);
                p_Player->SetStoreDeliveryProccesed(StoreCallback::ItemDelivery);
            }
    };

    class Delivery_Gold : public PlayerScript
    {
        public:
            Delivery_Gold() : PlayerScript("WebshopDelivery_Gold")
            {
            }

            void OnLogin(Player* p_Player) override
            {
                uint32 l_LowGuid = p_Player->GetGUIDLow();
                QueryResultFuture& l_Callback = p_Player->m_QueryResultFutures[WEBSHOP_TYPE_GOLD];
                l_Callback = CharacterDatabase.AsyncPQuery("SELECT gold, transaction FROM webshop_delivery_gold WHERE guid = '%u' and delivery = 0", l_LowGuid);
            }

            void OnUpdate(Player* p_Player, uint32 /*p_Diff*/) override
            {
                uint32 l_LowGuid = p_Player->GetGUIDLow();

                auto l_Itr = p_Player->m_QueryResultFutures.find(WEBSHOP_TYPE_GOLD);
                if (l_Itr == p_Player->m_QueryResultFutures.end())
                    return;

                QueryResultFuture& l_Callback = l_Itr->second;
                if (!l_Callback.ready())
                    return;

                QueryResult l_Result;
                l_Callback.get(l_Result);

                if (!l_Result)
                {
                    l_Callback.cancel();
                    p_Player->m_QueryResultFutures.erase(WEBSHOP_TYPE_GOLD);
                    p_Player->SetStoreDeliveryProccesed(StoreCallback::GoldDelivery);
                    return;
                }

                uint64 l_GoldCount = 0;
                do
                {
                    Field* l_Field       = l_Result->Fetch();
                    uint64 l_Gold        = uint64(l_Field[0].GetUInt32()) * GOLD;
                    uint32 l_Transaction = l_Field[1].GetUInt32();

                    if ((p_Player->GetMoney() + l_Gold) > MAX_MONEY_AMOUNT)
                    {
                        ChatHandler(p_Player).PSendSysMessage(WebShop::Language::GoldLimitReach);
                        ChatHandler(p_Player).PSendSysMessage(WebShop::Language::GoldAddNextLogin);
                        break;
                    }

                    l_GoldCount += l_Gold;
                    p_Player->ModifyMoney(l_Gold, "WebshopDelivery_Gold");

                    CharacterDatabase.PExecute("UPDATE webshop_delivery_gold SET delivery = 1 WHERE transaction = %u", l_Transaction);
                }
                while (l_Result->NextRow());

                if (l_GoldCount != 0)
                    ChatHandler(p_Player).PSendSysMessage(WebShop::Language::GoldAdded, l_GoldCount / GOLD);

                l_Callback.cancel();
                p_Player->m_QueryResultFutures.erase(WEBSHOP_TYPE_GOLD);
                p_Player->SetStoreDeliveryProccesed(StoreCallback::GoldDelivery);
            }
    };

    class Delivery_Currency : public PlayerScript
    {
        public:
            Delivery_Currency() : PlayerScript("WebshopDelivery_Currency")
            {
            }

            void OnLogin(Player* p_Player) override
            {
                uint32 l_LowGuid = p_Player->GetGUIDLow();
                QueryResultFuture& l_Callback = p_Player->m_QueryResultFutures[WEBSHOP_TYPE_CURRENCY];
                l_Callback = CharacterDatabase.AsyncPQuery("SELECT currency, amount, transaction FROM webshop_delivery_currency WHERE guid = '%u' and delivery = 0", l_LowGuid);
            }

            void OnUpdate(Player* p_Player, uint32 /*p_Diff*/) override
            {
                uint32 l_LowGuid = p_Player->GetGUIDLow();

                auto l_Itr = p_Player->m_QueryResultFutures.find(WEBSHOP_TYPE_CURRENCY);
                if (l_Itr == p_Player->m_QueryResultFutures.end())
                    return;

                QueryResultFuture& l_Callback = l_Itr->second;
                if (!l_Callback.ready())
                    return;

                QueryResult l_Result;
                l_Callback.get(l_Result);

                if (!l_Result)
                {
                    l_Callback.cancel();
                    p_Player->m_QueryResultFutures.erase(WEBSHOP_TYPE_CURRENCY);
                    p_Player->SetStoreDeliveryProccesed(StoreCallback::CurrencyDelivery);
                    return;
                }

                do
                {
                    Field* l_Field       = l_Result->Fetch();
                    uint32 l_Currency    = l_Field[0].GetUInt32();
                    uint32 l_Amount      = l_Field[1].GetUInt32();
                    uint32 l_Transaction = l_Field[2].GetUInt32();

                    CurrencyTypesEntry const* l_CurrencyEntry = sCurrencyTypesStore.LookupEntry(l_Currency);
                    if (!l_CurrencyEntry)
                        continue;

                    l_Amount *= l_CurrencyEntry->GetPrecision();
                    p_Player->ModifyCurrency(l_Currency, l_Amount, true, true, true);

                    CharacterDatabase.PExecute("UPDATE webshop_delivery_currency SET delivery = 1 WHERE transaction = %u", l_Transaction);
                }
                while (l_Result->NextRow());

                l_Callback.cancel();
                p_Player->m_QueryResultFutures.erase(WEBSHOP_TYPE_CURRENCY);
                p_Player->SetStoreDeliveryProccesed(StoreCallback::CurrencyDelivery);
            }
    };

    class Delivery_Level : public PlayerScript
    {
        public:
            Delivery_Level() : PlayerScript("WebshopDelivery_Level")
            {
            }

            void OnLogin(Player* p_Player) override
            {
                uint32 l_LowGuid = p_Player->GetGUIDLow();
                QueryResultFuture& l_Callback = p_Player->m_QueryResultFutures[WEBSHOP_TYPE_LEVEL];
                l_Callback = CharacterDatabase.AsyncPQuery("SELECT level, transaction FROM webshop_delivery_level WHERE guid = '%u' and delivery = 0", l_LowGuid);
            }

            void OnUpdate(Player* p_Player, uint32 /*p_Diff*/) override
            {
                uint32 l_LowGuid = p_Player->GetGUIDLow();

                auto l_Itr = p_Player->m_QueryResultFutures.find(WEBSHOP_TYPE_LEVEL);
                if (l_Itr == p_Player->m_QueryResultFutures.end())
                    return;

                QueryResultFuture& l_Callback = l_Itr->second;
                if (!l_Callback.ready())
                    return;

                QueryResult l_Result;
                l_Callback.get(l_Result);

                if (!l_Result)
                {
                    l_Callback.cancel();
                    p_Player->m_QueryResultFutures.erase(WEBSHOP_TYPE_LEVEL);
                    p_Player->SetStoreDeliveryProccesed(StoreCallback::LevelDelivery);
                    return;
                }

                do
                {
                    Field* l_Field       = l_Result->Fetch();
                    uint32 l_Level       = l_Field[0].GetUInt32();
                    uint32 l_Transaction = l_Field[1].GetUInt32();

                    p_Player->GiveLevel(l_Level);

                    CharacterDatabase.PExecute("UPDATE webshop_delivery_level SET delivery = 1 WHERE transaction = %u", l_Transaction);
                }
                while (l_Result->NextRow());

                l_Callback.cancel();
                p_Player->m_QueryResultFutures.erase(WEBSHOP_TYPE_LEVEL);
                p_Player->SetStoreDeliveryProccesed(StoreCallback::LevelDelivery);
            }
    };

    class Delivery_Profession : public PlayerScript
    {
        private:
            const uint32 m_SkillValue   = 800;
            const uint32 m_LevelRequire = 100;

        public:
            Delivery_Profession() : PlayerScript("WebshopDelivery_Profession")
            {
            }

            void OnLogin(Player* p_Player) override
            {
                uint32 l_LowGuid = p_Player->GetGUIDLow();
                QueryResultFuture& l_Callback = p_Player->m_QueryResultFutures[WEBSHOP_TYPE_PROFESSION];
                l_Callback = CharacterDatabase.AsyncPQuery("SELECT skill, recipe, transaction FROM webshop_delivery_profession WHERE guid = '%u' and delivery = 0", l_LowGuid);
            }

            void OnUpdate(Player* p_Player, uint32 /*p_Diff*/) override
            {
                uint32 l_LowGuid = p_Player->GetGUIDLow();

                auto l_Itr = p_Player->m_QueryResultFutures.find(WEBSHOP_TYPE_PROFESSION);
                if (l_Itr == p_Player->m_QueryResultFutures.end())
                    return;

                QueryResultFuture& l_Callback = l_Itr->second;
                if (!l_Callback.ready())
                    return;

                QueryResult l_Result;
                l_Callback.get(l_Result);

                if (!l_Result)
                {
                    l_Callback.cancel();
                    p_Player->m_QueryResultFutures.erase(WEBSHOP_TYPE_PROFESSION);
                    p_Player->SetStoreDeliveryProccesed(StoreCallback::ProfessionDelivery);
                    return;
                }

                const std::map<uint32, uint32> l_SkillLearningSpells =
                {
                    { SkillType::SKILL_ALCHEMY, ProfessionBookSpells::Alchemy },
                    { SkillType::SKILL_BLACKSMITHING, ProfessionBookSpells::Blacksmithing },
                    { SkillType::SKILL_ENCHANTING, ProfessionBookSpells::Enchanting },
                    { SkillType::SKILL_ENGINEERING, ProfessionBookSpells::Engineering },
                    { SkillType::SKILL_INSCRIPTION, ProfessionBookSpells::Inscription },
                    { SkillType::SKILL_JEWELCRAFTING, ProfessionBookSpells::JewelCrafting },
                    { SkillType::SKILL_LEATHERWORKING, ProfessionBookSpells::LeatherWorking },
                    { SkillType::SKILL_TAILORING, ProfessionBookSpells::Tailoring },
                    { SkillType::SKILL_FIRST_AID, ProfessionBookSpells::FirstAid },
                    { SkillType::SKILL_COOKING, ProfessionBookSpells::Cooking },
                    { SkillType::SKILL_HERBALISM, ProfessionBookSpells::Herbalism },
                    { SkillType::SKILL_MINING, ProfessionBookSpells::Mining },
                    { SkillType::SKILL_SKINNING, ProfessionBookSpells::Skinning },
                    { SkillType::SKILL_ARCHAEOLOGY, ProfessionBookSpells::Archaeology },
                    { SkillType::SKILL_FISHING, ProfessionBookSpells::Fishing }
                };

                do
                {
                    Field* l_Fields      = l_Result->Fetch();
                    uint32 l_SkillID     = l_Fields[0].GetUInt32();
                    bool   l_Recipe      = l_Fields[1].GetBool();
                    uint32 l_Transaction = l_Fields[2].GetUInt32();

                    auto l_Itr = l_SkillLearningSpells.find(l_SkillID);
                    if (l_Itr == l_SkillLearningSpells.end())
                        continue;

                    uint32 l_SpellID = l_Itr->second;

                    if (p_Player->getLevel() < m_LevelRequire)
                        continue;

                    if (IsPrimaryProfessionSkill(l_SkillID) && !p_Player->HasSkill(l_SkillID) && p_Player->GetFreePrimaryProfessionPoints() == 0)
                        continue;

                    /// Learn the skill to broken isles rank
                    p_Player->learnSpell(l_SpellID, false);

                    /// Up skill to 800
                    p_Player->SetSkill(l_SkillID, p_Player->GetSkillStep(l_SkillID), m_SkillValue, m_SkillValue);

                    if (l_Recipe)
                    {
                        const std::list<SkillLineAbilityEntry const*>& l_Abilities = sSpellMgr->GetTradeSpellFromSkill(l_SkillID);
                        for (auto l_Abilitie : l_Abilities)
                        {
                            if (l_Abilitie->spellId > 194464)   ///< last 6.2.3 spell in SkillLineAbility.dbc
                                continue;

                            /// Don't learn any Legion recipes
                            if (l_Abilitie->TrivialSkillLineRankLow >= 700)
                                continue;

                            /// Don't learn any Enchant Illusion (they have a min_value of 1)
                            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_Abilitie->spellId))
                            {
                                if (l_SpellInfo->HasEffect(SpellEffects::SPELL_EFFECT_APPLY_ENCHANT_ILLUSION))
                                    continue;
                            }

                            p_Player->learnSpell(l_Abilitie->spellId, false);
                        }
                    }

                    if (l_SkillID == SkillType::SKILL_JEWELCRAFTING)
                        p_Player->learnSpell(ProfessionAdditionalSpells::Prospecting, false);             ///< Prospecting

                    /// We also need to learn it for herbalism
                    if (l_SkillID == SkillType::SKILL_HERBALISM || l_SkillID == SkillType::SKILL_MINING)
                        p_Player->learnSpell(l_SpellID, false);

                    if (l_SkillID == SkillType::SKILL_FISHING)
                        p_Player->learnSpell(ProfessionAdditionalSpells::FishingDummy, false);

                    CharacterDatabase.PExecute("UPDATE webshop_delivery_profession SET delivery = 1 WHERE transaction = %u", l_Transaction);
                }
                while (l_Result->NextRow());

                l_Callback.cancel();
                p_Player->m_QueryResultFutures.erase(WEBSHOP_TYPE_PROFESSION);
                p_Player->SetStoreDeliveryProccesed(StoreCallback::ProfessionDelivery);
            }
    };

    class Delivery_Save : public PlayerScript
    {
        public:
            Delivery_Save() : PlayerScript("WebshopDelivery_Save")
            {
            }

            void OnUpdate(Player* p_Player, uint32 /*p_Diff*/) override
            {
                if (p_Player->IsStoreDeliverySaved())
                    return;

                for (uint8 l_I = 0; l_I < StoreCallback::MaxDelivery; l_I++)
                {
                    if (!p_Player->IsStoreDeliveryProccesed((StoreCallback)l_I))
                        return;
                }

                p_Player->SaveToDB();
                p_Player->SetStoreDeliverySaved();
            }
    };



    class Delivery_Title : public PlayerScript
    {
        public:
            Delivery_Title() : PlayerScript("WebshopDelivery_Title")
            {
            }

            void OnLogin(Player* p_Player) override
            {
                uint32 l_LowGuid = p_Player->GetGUIDLow();
                QueryResultFuture& l_Callback = p_Player->m_QueryResultFutures[WEBSHOP_TYPE_TITLE];
                l_Callback = CharacterDatabase.AsyncPQuery("SELECT title, transaction FROM webshop_delivery_title WHERE guid = '%u' and delivery = 0", l_LowGuid);
            }

            void OnUpdate(Player* p_Player, uint32 /*p_Diff*/) override
            {
                uint32 l_LowGuid = p_Player->GetGUIDLow();

                auto l_Itr = p_Player->m_QueryResultFutures.find(WEBSHOP_TYPE_TITLE);
                if (l_Itr == p_Player->m_QueryResultFutures.end())
                    return;

                QueryResultFuture& l_Callback = l_Itr->second;
                if (!l_Callback.ready())
                    return;

                QueryResult l_Result;
                l_Callback.get(l_Result);

                if (!l_Result)
                {
                    l_Callback.cancel();
                    p_Player->m_QueryResultFutures.erase(WEBSHOP_TYPE_TITLE);
                    p_Player->SetStoreDeliveryProccesed(StoreCallback::TitleDelivery);
                    return;
                }

                do
                {
                    Field* l_Field       = l_Result->Fetch();
                    uint64 l_TitleID     = l_Field[0].GetUInt32();
                    uint32 l_Transaction = l_Field[1].GetUInt32();

                    CharTitlesEntry const* l_Title = sCharTitlesStore.LookupEntry(l_TitleID);

                    if (l_Title == nullptr)
                        break;

                    char const* l_TitleName = p_Player->getGender() == Gender::GENDER_MALE ? l_Title->NameMale->Get(p_Player->GetSession()->GetSessionDbLocaleIndex()) : l_Title->NameFemale->Get(p_Player->GetSession()->GetSessionDbLocaleIndex());

                    char const* l_PlayerName = p_Player->GetName();
                    char l_TitleNameStr[80];
                    snprintf(l_TitleNameStr, 80, l_TitleName, l_PlayerName);

                    if (p_Player->HasTitle(l_Title))
                    {
                        ChatHandler(p_Player).PSendSysMessage(WebShop::Language::HasAlreadyTitle, l_TitleNameStr);
                        break;
                    }

                    p_Player->SetTitle(l_Title);
                    ChatHandler(p_Player).PSendSysMessage(WebShop::Language::TitleAdd, l_TitleNameStr);

                    CharacterDatabase.PExecute("UPDATE webshop_delivery_title SET delivery = 1 WHERE transaction = %u", l_Transaction);
                }
                while (l_Result->NextRow());

                l_Callback.cancel();
                p_Player->m_QueryResultFutures.erase(WEBSHOP_TYPE_TITLE);
                p_Player->SetStoreDeliveryProccesed(StoreCallback::TitleDelivery);
            }
    };
}

/// Gear lootbox V1 - 300000 (Mythic+ box)
class item_gear_dungeon_mythic_lootbox : public ItemScript
{
    enum eMaps
    {
        TheNighthold   = 1530,
        TombOfSargeras = 1676,
        Antorus        = 1712
    };

    public:
        item_gear_dungeon_mythic_lootbox() : ItemScript("item_gear_dungeon_mythic_lootbox") { }

        bool OnOpen(Player* p_Player, Item* p_Item)
        {
            p_Player->m_IsInLootboxProcess = true;

            if (p_Item->HasCustomFlags(ItemCustomFlags::LoyaltyLootBox))
                p_Player->m_IsInLoyaltyLootBoxProcess = true;

            p_Player->SetCharacterWorldState(CharacterWorldStates::LootBoxBetterIlvlCounter, p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlCounter) + 1);

            p_Player->DestroyItem(p_Item->GetBagSlot(), p_Item->GetSlot(), true);

            std::ostringstream l_LogResult;
            l_LogResult << "Gear lootbox V1 - 300000 Player " << p_Player->GetName() << " (guid " << p_Player->GetGUIDLow() << ") ";

            uint32 l_UpgradeChance = 50;
            if (p_Player->m_IsInLoyaltyLootBoxProcess)
                l_UpgradeChance = 25;

            /// Money
            uint64 l_Money = 1000;
            while (urand(0, 100) > l_UpgradeChance)
                l_Money += 1000;
            l_Money *= GOLD;

            l_LogResult << l_Money / 1000 << " gold ";

            p_Player->ModifyMoney(l_Money, "lootbox");
            p_Player->SendDisplayToast(0, l_Money, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_MONEY, false, false);

            /// AP                    100    200      400    800     1000
            uint32 l_APItems[5] = { 141701, 138786, 143869, 141667, 139506 };
            uint8 l_I = 0;

            while (l_I < 4 && urand(0, 100) > l_UpgradeChance)
                l_I++;

            p_Player->AddItem(l_APItems[l_I], 1, {}, false, 0, false, true);
            p_Player->SendDisplayToast(l_APItems[l_I], 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            l_LogResult << l_APItems[l_I] << " AP item " << l_APItems[l_I] << " ";

            /// Sargeras
            l_I = 1;
            while (l_I < 5 && urand(0, 100) > l_UpgradeChance)
                l_I++;

            p_Player->AddItem(124124, l_I, {}, false, 0, false, true);
            p_Player->SendDisplayToast(124124, l_I, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            l_LogResult << l_I << " blood of sargeras ";

            if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
            {
                uint32 l_Count = 0;
                ItemTemplate const* l_GearReward = sObjectMgr->GetItemRewardForPvp(p_Player->GetActiveSpecializationID(), roll_chance_i(15) ? MS::Battlegrounds::RewardGearType::Elite : MS::Battlegrounds::RewardGearType::Rated, p_Player);
                while (!l_GearReward || !l_GearReward->IsStuff())
                {
                    /// just in case
                    if (l_Count == 1000)
                        break;

                    l_Count++;
                    l_GearReward = sObjectMgr->GetItemRewardForPvp(p_Player->GetActiveSpecializationID(), roll_chance_i(15) ? MS::Battlegrounds::RewardGearType::Elite : MS::Battlegrounds::RewardGearType::Rated, p_Player);;
                }

                if (l_GearReward)
                {
                    ItemPosCountVec sDest;
                    InventoryResult msg = p_Player->CanStoreNewItem(INVENTORY_SLOT_BAG_0, NULL_SLOT, sDest, l_GearReward->ItemId, 1);

                    if (msg == EQUIP_ERR_OK)
                    {
                        std::vector<uint32> l_ItemBonuses;
                        Item::GenerateItemBonus(l_GearReward->ItemId, p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlCounter) == 5 ? ItemContext::Ilvl950 : ItemContext::Ilvl945, l_ItemBonuses, true, p_Player, nullptr, false);

                        if (Item* l_AddedItem = p_Player->StoreNewItem(sDest, l_GearReward->ItemId, true, Item::GenerateItemRandomPropertyId(l_GearReward->ItemId), l_ItemBonuses))
                        {
                            p_Player->SendNewItem(l_AddedItem, 1, true, false);
                            p_Player->SendDisplayToast(l_GearReward->ItemId, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false, l_AddedItem->GetAllItemBonuses());
                            l_LogResult << l_GearReward->ItemId << " gear item ilvl " << l_AddedItem->GetItemLevel(p_Player) << " ";

                            if (p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlCounter) == 5)
                                p_Player->SetCharacterWorldState(CharacterWorldStates::LootBoxBetterIlvlCounter, 0);
                        }
                    }
                }
            }
            else
            {
                uint32 l_DungeonMapLoot = JadeCore::Containers::SelectRandomContainerElement(*GetChallengeMaps());
                std::vector<uint32> l_DungeonsPossibleLoots = GetItemLootPerMap(l_DungeonMapLoot);
                std::vector<uint32> l_ToSPossibleLoots = GetItemLootPerMap(eMaps::TombOfSargeras);
                std::vector<uint32> l_AntorusPossibleLoots = GetItemLootPerMap(eMaps::Antorus);

                std::vector<uint32> l_PossibleLoots;
                l_PossibleLoots.insert(l_PossibleLoots.end(), l_DungeonsPossibleLoots.begin(), l_DungeonsPossibleLoots.end());
                l_PossibleLoots.insert(l_PossibleLoots.end(), l_ToSPossibleLoots.begin(),      l_ToSPossibleLoots.end());
                l_PossibleLoots.insert(l_PossibleLoots.end(), l_AntorusPossibleLoots.begin(), l_AntorusPossibleLoots.end());

                std::random_device l_RandomDevice;
                std::mt19937 l_RandomGenerator(l_RandomDevice());
                std::shuffle(l_PossibleLoots.begin(), l_PossibleLoots.end(), l_RandomGenerator);

                for (uint32 l_ItemId : l_PossibleLoots)
                {
                    ItemTemplate const* l_ItemTpl = sObjectMgr->GetItemTemplate(l_ItemId);
                    if (!l_ItemTpl || !l_ItemTpl->IsStuff())
                        continue;

                    if (!l_ItemTpl->IsUsableBySpecialization(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player->getLevel()))
                        continue;

                    /// Exclude the pantheons trinket
                    switch (l_ItemId)
                    {
                        case 154172:
                        case 154173:
                        case 154174:
                        case 154175:
                        case 154176:
                        case 154177:
                            continue;
                        default:
                            break;
                    }

                    ItemContext l_Context = ItemContext::RaidNormal;
                    if (std::find(l_DungeonsPossibleLoots.begin(), l_DungeonsPossibleLoots.end(), l_ItemTpl->ItemId) != l_DungeonsPossibleLoots.end())
                        l_Context = ItemContext::DungeonMythic;

                    if (Item* l_Item = p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, false, 0, true, true, l_Context))
                    {
                        p_Player->SendDisplayToast(l_ItemTpl->ItemId, 0, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, true, l_Item->GetAllItemBonuses());
                        l_LogResult << l_ItemTpl->ItemId << " gear item ilvl " << l_Item->GetItemLevel(p_Player) << " ";
                    }
                    else
                        l_LogResult << l_ItemTpl->ItemId << " gear unk item ilvl sent by mail ";

                    if (p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlCounter) == 5)
                        p_Player->SetCharacterWorldState(CharacterWorldStates::LootBoxBetterIlvlCounter, 0);

                    break;
                }
            }

            p_Player->m_IsInLootboxProcess = false;

            /// Legendary
            if (roll_chance_f(p_Player->m_IsInLoyaltyLootBoxProcess ? 0.2f : 0.4f) || (!p_Player->m_IsInLoyaltyLootBoxProcess && p_Player->GetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter) >= k_LegendaryBadLuckProtectionCap))
            {
                if (ItemTemplate const* l_LegendaryItem = sObjectMgr->GetItemLegendary(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player))
                {
                    auto l_Item = p_Player->AddItem(l_LegendaryItem->ItemId, 1, {}, false, 0, false, true);
                    p_Player->SendDisplayToast(l_LegendaryItem->ItemId, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LEGENDARY_LOOTED, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_Item ? l_Item->GetAllItemBonuses() : std::vector<uint32>());

                    l_LogResult << " got a legendary " << l_LegendaryItem->ItemId;

                    p_Player->SetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter, 0);
#ifndef CROSS
                    if (Guild* l_Guild = p_Player->GetGuild())
                    {
                        if (l_Item)
                            l_Guild->GetNewsLog().AddNewEvent(GUILD_NEWS_ITEM_LOOTED, time(nullptr), p_Player->GetGUID(), 0, l_Item->GetGUIDLow(), l_Item);
                    }
#endif
                }
            }
            else if (!p_Player->m_IsInLoyaltyLootBoxProcess)
                p_Player->SetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter, p_Player->GetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter) + 1);

            PreparedStatement* l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_LOOTBOX_LOG);
            l_Statement->setUInt32(0, p_Player->GetGUIDLow());
            l_Statement->setString(1, l_LogResult.str().c_str());
            LogDatabase.Execute(l_Statement);

            p_Player->SaveToDB();
            return true;
        }
};

/// Gear lootbox - 300010 (ToV/EN raid box)
class item_gear_raid_01_lootbox : public ItemScript
{
    public:
        item_gear_raid_01_lootbox() : ItemScript("item_gear_raid_01_lootbox") { }

        enum eMaps
        {
            EmeraldNightmare = 1520,
            TrialOfValor     = 1648
        };

        bool OnOpen(Player* p_Player, Item* p_Item)
        {
            p_Player->m_IsInLootboxProcess = true;

            if (p_Item->HasCustomFlags(ItemCustomFlags::LoyaltyLootBox))
                p_Player->m_IsInLoyaltyLootBoxProcess = true;

            p_Player->SetCharacterWorldState(CharacterWorldStates::LootBoxBetterIlvlRaid01Counter, p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlRaid01Counter) + 1);

            p_Player->DestroyItem(p_Item->GetBagSlot(), p_Item->GetSlot(), true);

            std::ostringstream l_LogResult;
            l_LogResult << "Gear Raid 01 lootbox  - 300001 Player " << p_Player->GetName() << " (guid " << p_Player->GetGUIDLow() << ") ";

            uint32 l_UpgradeChance = 50;
            if (p_Player->m_IsInLoyaltyLootBoxProcess)
                l_UpgradeChance = 25;

            /// Money
            uint64 l_Money = 1000;
            while (urand(0, 100) > l_UpgradeChance)
                l_Money += 1000;
            l_Money *= GOLD;

            l_LogResult << l_Money / 1000 << " gold ";

            p_Player->ModifyMoney(l_Money, "lootbox");
            p_Player->SendDisplayToast(0, l_Money, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_MONEY, false, false);

            /// AP                    100    200      400    800     1000
            uint32 l_APItems[5] = { 141701, 138786, 143869, 141667, 139506 };
            uint8 l_I = 0;

            while (l_I < 4 && urand(0, 100) > l_UpgradeChance)
                l_I++;

            p_Player->AddItem(l_APItems[l_I], 1, {}, false, 0, false, true);
            p_Player->SendDisplayToast(l_APItems[l_I], 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            l_LogResult << l_APItems[l_I] << " AP item " << l_APItems[l_I] << " ";

            /// Sargeras
            l_I = 1;
            while (l_I < 5 && urand(0, 100) > l_UpgradeChance)
                l_I++;

            p_Player->AddItem(124124, l_I, {}, false, 0, false, true);
            p_Player->SendDisplayToast(124124, l_I, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            l_LogResult << l_I << " blood of sargeras ";

            uint32 l_DungeonMapLoot = JadeCore::Containers::SelectRandomContainerElement(*GetChallengeMaps());
            std::vector<uint32> l_PossibleLoots;

            auto l_ENLoots  = GetItemLootPerMap(eMaps::EmeraldNightmare);
            auto l_ToVLoots = GetItemLootPerMap(eMaps::TrialOfValor);

            l_PossibleLoots.insert(l_PossibleLoots.end(), l_ENLoots.begin(), l_ENLoots.end());
            l_PossibleLoots.insert(l_PossibleLoots.end(), l_ToVLoots.begin(), l_ToVLoots.end());

            std::random_device l_RandomDevice;
            std::mt19937 l_RandomGenerator(l_RandomDevice());
            std::shuffle(l_PossibleLoots.begin(), l_PossibleLoots.end(), l_RandomGenerator);

            for (uint32 l_ItemId : l_PossibleLoots)
            {
                ItemTemplate const* l_ItemTpl = sObjectMgr->GetItemTemplate(l_ItemId);
                if (!l_ItemTpl || !l_ItemTpl->IsStuff())
                    continue;

                if (!l_ItemTpl->IsUsableBySpecialization(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player->getLevel()))
                    continue;

                if (Item* l_Item = p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, false, 0, true, true, ItemContext::RaidHeroic))
                {
                    p_Player->SendDisplayToast(l_ItemTpl->ItemId, 0, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, true, l_Item->GetAllItemBonuses());
                    l_LogResult << l_ItemTpl->ItemId << " gear item ilvl " << l_Item->GetItemLevel(p_Player) << " ";
                }
                else
                    l_LogResult << l_ItemTpl->ItemId << " gear unk item ilvl sent by mail ";

                if (p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlRaid01Counter) == 5)
                    p_Player->SetCharacterWorldState(CharacterWorldStates::LootBoxBetterIlvlRaid01Counter, 0);

                break;
            }

            p_Player->m_IsInLootboxProcess = false;

            /// Legendary
            if (roll_chance_f(p_Player->m_IsInLoyaltyLootBoxProcess ? 0.2f : 0.4f) || (!p_Player->m_IsInLoyaltyLootBoxProcess && p_Player->GetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter) >= k_LegendaryBadLuckProtectionCap))
            {
                if (ItemTemplate const* l_LegendaryItem = sObjectMgr->GetItemLegendary(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player))
                {
                    auto l_Item = p_Player->AddItem(l_LegendaryItem->ItemId, 1, { }, false, 0, false, true);
                    p_Player->SendDisplayToast(l_LegendaryItem->ItemId, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LEGENDARY_LOOTED, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_Item ? l_Item->GetAllItemBonuses() : std::vector<uint32>());

                    l_LogResult << " got a legendary " << l_LegendaryItem->ItemId;

                    p_Player->SetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter, 0);
#ifndef CROSS
                    if (Guild* l_Guild = p_Player->GetGuild())
                    {
                        if (l_Item)
                            l_Guild->GetNewsLog().AddNewEvent(GUILD_NEWS_ITEM_LOOTED, time(nullptr), p_Player->GetGUID(), 0, l_Item->GetGUIDLow(), l_Item);
                    }
#endif
                }
            }
            else if (!p_Player->m_IsInLoyaltyLootBoxProcess)
                p_Player->SetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter, p_Player->GetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter) + 1);

            PreparedStatement* l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_LOOTBOX_LOG);
            l_Statement->setUInt32(0, p_Player->GetGUIDLow());
            l_Statement->setString(1, l_LogResult.str().c_str());
            LogDatabase.Execute(l_Statement);

            p_Player->SaveToDB();
            return true;
        }
};

/// Gear lootbox - 300020 (NH raid box)
class item_gear_raid_02_lootbox : public ItemScript
{
    public:
        item_gear_raid_02_lootbox() : ItemScript("item_gear_raid_02_lootbox") { }

        enum eMaps
        {
            TheNighthold = 1530,
        };

        bool OnOpen(Player* p_Player, Item* p_Item)
        {
            p_Player->m_IsInLootboxProcess = true;

            if (p_Item->HasCustomFlags(ItemCustomFlags::LoyaltyLootBox))
                p_Player->m_IsInLoyaltyLootBoxProcess = true;

            p_Player->SetCharacterWorldState(CharacterWorldStates::LootBoxBetterIlvlRaid02Counter, p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlRaid02Counter) + 1);

            p_Player->DestroyItem(p_Item->GetBagSlot(), p_Item->GetSlot(), true);

            std::ostringstream l_LogResult;
            l_LogResult << "Gear Raid 02 lootbox - 300002 Player " << p_Player->GetName() << " (guid " << p_Player->GetGUIDLow() << ") ";

            uint32 l_UpgradeChance = 50;
            if (p_Player->m_IsInLoyaltyLootBoxProcess)
                l_UpgradeChance = 25;

            /// Money
            uint64 l_Money = 1000;
            while (urand(0, 100) > l_UpgradeChance)
                l_Money += 1000;
            l_Money *= GOLD;

            l_LogResult << l_Money / 1000 << " gold ";

            p_Player->ModifyMoney(l_Money, "lootbox");
            p_Player->SendDisplayToast(0, l_Money, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_MONEY, false, false);

            /// AP                    100    200      400    800     1000
            uint32 l_APItems[5] = { 141701, 138786, 143869, 141667, 139506 };
            uint8 l_I = 0;

            while (l_I < 4 && urand(0, 100) > l_UpgradeChance)
                l_I++;

            p_Player->AddItem(l_APItems[l_I], 1, {}, false, 0, false, true);
            p_Player->SendDisplayToast(l_APItems[l_I], 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            l_LogResult << l_APItems[l_I] << " AP item " << l_APItems[l_I] << " ";

            /// Sargeras
            l_I = 1;
            while (l_I < 5 && urand(0, 100) > l_UpgradeChance)
                l_I++;

            p_Player->AddItem(124124, l_I, {}, false, 0, false, true);
            p_Player->SendDisplayToast(124124, l_I, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            l_LogResult << l_I << " blood of sargeras ";

            std::vector<uint32> l_PossibleLoots = GetItemLootPerMap(eMaps::TheNighthold);

            std::random_device l_RandomDevice;
            std::mt19937 l_RandomGenerator(l_RandomDevice());
            std::shuffle(l_PossibleLoots.begin(), l_PossibleLoots.end(), l_RandomGenerator);

            for (uint32 l_ItemId : l_PossibleLoots)
            {
                ItemTemplate const* l_ItemTpl = sObjectMgr->GetItemTemplate(l_ItemId);
                if (!l_ItemTpl || !l_ItemTpl->IsStuff())
                    continue;

                if (!l_ItemTpl->IsUsableBySpecialization(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player->getLevel()))
                    continue;

                if (Item* l_Item = p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, false, 0, true, true, ItemContext::RaidNormal))
                {
                    p_Player->SendDisplayToast(l_ItemTpl->ItemId, 0, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, true, l_Item->GetAllItemBonuses());
                    l_LogResult << l_ItemTpl->ItemId << " gear item ilvl " << l_Item->GetItemLevel(p_Player) << " ";
                }
                else
                    l_LogResult << l_ItemTpl->ItemId << " gear unk item ilvl sent by mail ";

                if (p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlRaid03Counter) == 5)
                    p_Player->SetCharacterWorldState(CharacterWorldStates::LootBoxBetterIlvlRaid03Counter, 0);

                break;
            }

            p_Player->m_IsInLootboxProcess = false;

            /// Legendary
            if (roll_chance_f(p_Player->m_IsInLoyaltyLootBoxProcess ? 0.2f : 0.4f) || (!p_Player->m_IsInLoyaltyLootBoxProcess && p_Player->GetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter) >= k_LegendaryBadLuckProtectionCap))
            {
                if (ItemTemplate const* l_LegendaryItem = sObjectMgr->GetItemLegendary(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player))
                {
                    auto l_Item = p_Player->AddItem(l_LegendaryItem->ItemId, 1, {}, false, 0, false, true);
                    p_Player->SendDisplayToast(l_LegendaryItem->ItemId, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LEGENDARY_LOOTED, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_Item ? l_Item->GetAllItemBonuses() : std::vector<uint32>());

                    l_LogResult << " got a legendary " << l_LegendaryItem->ItemId;

                    p_Player->SetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter, 0);
#ifndef CROSS
                    if (Guild* l_Guild = p_Player->GetGuild())
                    {
                        if (l_Item)
                            l_Guild->GetNewsLog().AddNewEvent(GUILD_NEWS_ITEM_LOOTED, time(nullptr), p_Player->GetGUID(), 0, l_Item->GetGUIDLow(), l_Item);
                    }
#endif
                }
            }
            else if (!p_Player->m_IsInLoyaltyLootBoxProcess)
                p_Player->SetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter, p_Player->GetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter) + 1);

            PreparedStatement* l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_LOOTBOX_LOG);
            l_Statement->setUInt32(0, p_Player->GetGUIDLow());
            l_Statement->setString(1, l_LogResult.str().c_str());
            LogDatabase.Execute(l_Statement);

            p_Player->SaveToDB();
            return true;
        }
};

/// Gear lootbox - 300030 (ToS raid box)
class item_gear_raid_03_lootbox : public ItemScript
{
    public:
        item_gear_raid_03_lootbox() : ItemScript("item_gear_raid_03_lootbox") { }

        enum eMaps
        {
            TombOfSargeras = 1676
        };

        bool OnOpen(Player* p_Player, Item* p_Item)
        {
            p_Player->m_IsInLootboxProcess = true;
            p_Player->m_CurrentLootboxId = 300030;

            if (p_Item->HasCustomFlags(ItemCustomFlags::LoyaltyLootBox))
                p_Player->m_IsInLoyaltyLootBoxProcess = true;

            p_Player->SetCharacterWorldState(CharacterWorldStates::LootBoxBetterIlvlRaid03Counter, p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlRaid03Counter) + 1);

            p_Player->DestroyItem(p_Item->GetBagSlot(), p_Item->GetSlot(), true);

            std::ostringstream l_LogResult;
            l_LogResult << "Gear Raid 03 lootbox - 300030 Player " << p_Player->GetName() << " (guid " << p_Player->GetGUIDLow() << ") ";

            uint32 l_UpgradeChance = 50;
            if (p_Player->m_IsInLoyaltyLootBoxProcess)
                l_UpgradeChance = 25;

            /// Money
            uint64 l_Money = 1000;
            while (urand(0, 100) > l_UpgradeChance)
                l_Money += 1000;
            l_Money *= GOLD;

            l_LogResult << l_Money / 1000 << " gold ";

            p_Player->ModifyMoney(l_Money, "lootbox");
            p_Player->SendDisplayToast(0, l_Money, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_MONEY, false, false);

            /// AP                    100    200      400    800     1000
            uint32 l_APItems[5] = { 141701, 138786, 143869, 141667, 139506 };
            uint8 l_I = 0;

            while (l_I < 4 && urand(0, 100) > l_UpgradeChance)
                l_I++;

            p_Player->AddItem(l_APItems[l_I], 1, {}, false, 0, false, true);
            p_Player->SendDisplayToast(l_APItems[l_I], 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            l_LogResult << l_APItems[l_I] << " AP item " << l_APItems[l_I] << " ";

            /// Sargeras
            l_I = 1;
            while (l_I < 5 && urand(0, 100) > l_UpgradeChance)
                l_I++;

            p_Player->AddItem(124124, l_I, {}, false, 0, false, true);
            p_Player->SendDisplayToast(124124, l_I, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            l_LogResult << l_I << " blood of sargeras ";

            std::vector<uint32> l_PossibleLoots = GetItemLootPerMap(eMaps::TombOfSargeras);

            std::random_device l_RandomDevice;
            std::mt19937 l_RandomGenerator(l_RandomDevice());
            std::shuffle(l_PossibleLoots.begin(), l_PossibleLoots.end(), l_RandomGenerator);

            for (uint32 l_ItemId : l_PossibleLoots)
            {
                ItemTemplate const* l_ItemTpl = sObjectMgr->GetItemTemplate(l_ItemId);
                if (!l_ItemTpl || !l_ItemTpl->IsStuff())
                    continue;

                if (!l_ItemTpl->IsUsableBySpecialization(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player->getLevel()))
                    continue;

                if (Item* l_Item = p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, false, 0, true, true, ItemContext::RaidNormal))
                {
                    p_Player->SendDisplayToast(l_ItemTpl->ItemId, 0, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, true, l_Item->GetAllItemBonuses());
                    l_LogResult << l_ItemTpl->ItemId << " gear item ilvl " << l_Item->GetItemLevel(p_Player) << " ";
                }
                else
                    l_LogResult << l_ItemTpl->ItemId << " gear unk item ilvl sent by mail ";

                if (p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlRaid03Counter) == 5)
                    p_Player->SetCharacterWorldState(CharacterWorldStates::LootBoxBetterIlvlRaid03Counter, 0);

                break;
            }

            p_Player->m_IsInLootboxProcess = false;

            /// Legendary
            if (roll_chance_f(p_Player->m_IsInLoyaltyLootBoxProcess ? 0.2f : 0.4f) || (!p_Player->m_IsInLoyaltyLootBoxProcess && p_Player->GetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter) >= k_LegendaryBadLuckProtectionCap))
            {
                if (ItemTemplate const* l_LegendaryItem = sObjectMgr->GetItemLegendary(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player))
                {
                    auto l_Item = p_Player->AddItem(l_LegendaryItem->ItemId, 1, {}, false, 0, false, true);
                    p_Player->SendDisplayToast(l_LegendaryItem->ItemId, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LEGENDARY_LOOTED, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_Item ? l_Item->GetAllItemBonuses() : std::vector<uint32>());

                    l_LogResult << " got a legendary " << l_LegendaryItem->ItemId;

                    p_Player->SetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter, 0);
#ifndef CROSS
                    if (Guild* l_Guild = p_Player->GetGuild())
                    {
                        if (l_Item)
                            l_Guild->GetNewsLog().AddNewEvent(GUILD_NEWS_ITEM_LOOTED, time(nullptr), p_Player->GetGUID(), 0, l_Item->GetGUIDLow(), l_Item);
                    }
#endif
                }
            }
            else if (!p_Player->m_IsInLoyaltyLootBoxProcess)
                p_Player->SetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter, p_Player->GetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter) + 1);

            PreparedStatement* l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_LOOTBOX_LOG);
            l_Statement->setUInt32(0, p_Player->GetGUIDLow());
            l_Statement->setString(1, l_LogResult.str().c_str());
            LogDatabase.Execute(l_Statement);

            p_Player->SaveToDB();
            return true;
        }
};

/// Gear lootbox - 300031 (Antorus raid box)
class item_gear_raid_04_lootbox : public ItemScript
{
    public:
        item_gear_raid_04_lootbox() : ItemScript("item_gear_raid_04_lootbox") { }

        enum eMaps
        {
            Antorus = 1712
        };

        bool OnOpen(Player* p_Player, Item* p_Item)
        {
            p_Player->m_IsInLootboxProcess = true;
            p_Player->m_CurrentLootboxId = 300031;

            if (p_Item->HasCustomFlags(ItemCustomFlags::LoyaltyLootBox))
                p_Player->m_IsInLoyaltyLootBoxProcess = true;

            p_Player->SetCharacterWorldState(CharacterWorldStates::LootBoxBetterIlvlRaid04Counter, p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlRaid04Counter) + 1);

            p_Player->DestroyItem(p_Item->GetBagSlot(), p_Item->GetSlot(), true);

            std::ostringstream l_LogResult;
            l_LogResult << "Gear Raid 04 (Antorus) lootbox - 300031 Player " << p_Player->GetName() << " (guid " << p_Player->GetGUIDLow() << ") ";

            uint32 l_UpgradeChance = 50;
            if (p_Player->m_IsInLoyaltyLootBoxProcess)
                l_UpgradeChance = 25;

            /// Money
            uint64 l_Money = 1000;
            while (urand(0, 100) > l_UpgradeChance)
                l_Money += 1000;
            l_Money *= GOLD;

            l_LogResult << l_Money / 1000 << " gold ";

            p_Player->ModifyMoney(l_Money, "lootbox");
            p_Player->SendDisplayToast(0, l_Money, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_MONEY, false, false);

            /// AP                    100    200      400    800     1000
            uint32 l_APItems[5] = { 141701, 138786, 143869, 141667, 139506 };
            uint8 l_I = 0;

            while (l_I < 4 && urand(0, 100) > l_UpgradeChance)
                l_I++;

            p_Player->AddItem(l_APItems[l_I], 1, {}, false, 0, false, true);
            p_Player->SendDisplayToast(l_APItems[l_I], 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            l_LogResult << l_APItems[l_I] << " AP item " << l_APItems[l_I] << " ";

            /// Sargeras
            l_I = 1;
            while (l_I < 5 && urand(0, 100) > l_UpgradeChance)
                l_I++;

            p_Player->AddItem(124124, l_I, {}, false, 0, false, true);
            p_Player->SendDisplayToast(124124, l_I, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            l_LogResult << l_I << " blood of sargeras ";

            std::vector<uint32> l_PossibleLoots = GetItemLootPerMap(eMaps::Antorus);

            std::random_device l_RandomDevice;
            std::mt19937 l_RandomGenerator(l_RandomDevice());
            std::shuffle(l_PossibleLoots.begin(), l_PossibleLoots.end(), l_RandomGenerator);

            for (uint32 l_ItemId : l_PossibleLoots)
            {
                ItemTemplate const* l_ItemTpl = sObjectMgr->GetItemTemplate(l_ItemId);
                if (!l_ItemTpl || !l_ItemTpl->IsStuff())
                    continue;

                /// Exclude the pantheons trinket
                switch (l_ItemId)
                {
                    case 154172:
                    case 154173:
                    case 154174:
                    case 154175:
                    case 154176:
                    case 154177:
                        continue;
                    default:
                        break;
                }

                if (!l_ItemTpl->IsUsableBySpecialization(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player->getLevel()))
                    continue;

                if (Item* l_Item = p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, false, 0, true, true, ItemContext::RaidNormal))
                {
                    p_Player->SendDisplayToast(l_ItemTpl->ItemId, 0, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, true, l_Item->GetAllItemBonuses());
                    l_LogResult << l_ItemTpl->ItemId << " gear item ilvl " << l_Item->GetItemLevel(p_Player) << " ";
                }
                else
                    l_LogResult << l_ItemTpl->ItemId << " gear unk item ilvl sent by mail ";

                if (p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlRaid04Counter) == 5)
                    p_Player->SetCharacterWorldState(CharacterWorldStates::LootBoxBetterIlvlRaid04Counter, 0);

                break;
            }

            p_Player->m_IsInLootboxProcess = false;

            /// Legendary
            if (roll_chance_f(p_Player->m_IsInLoyaltyLootBoxProcess ? 0.2f : 0.4f) || (!p_Player->m_IsInLoyaltyLootBoxProcess && p_Player->GetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter) >= k_LegendaryBadLuckProtectionCap))
            {
                if (ItemTemplate const* l_LegendaryItem = sObjectMgr->GetItemLegendary(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player))
                {
                    auto l_Item = p_Player->AddItem(l_LegendaryItem->ItemId, 1, {}, false, 0, false, true);
                    p_Player->SendDisplayToast(l_LegendaryItem->ItemId, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LEGENDARY_LOOTED, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_Item ? l_Item->GetAllItemBonuses() : std::vector<uint32>());

                    l_LogResult << " got a legendary " << l_LegendaryItem->ItemId;

                    p_Player->SetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter, 0);
#ifndef CROSS
                    if (Guild* l_Guild = p_Player->GetGuild())
                    {
                        if (l_Item)
                            l_Guild->GetNewsLog().AddNewEvent(GUILD_NEWS_ITEM_LOOTED, time(nullptr), p_Player->GetGUID(), 0, l_Item->GetGUIDLow(), l_Item);
                    }
#endif
                }
            }
            else if (!p_Player->m_IsInLoyaltyLootBoxProcess)
                p_Player->SetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter, p_Player->GetWorldState(CharacterWorldStates::LootBoxNoLegendaryCounter) + 1);

            PreparedStatement* l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_LOOTBOX_LOG);
            l_Statement->setUInt32(0, p_Player->GetGUIDLow());
            l_Statement->setString(1, l_LogResult.str().c_str());
            LogDatabase.Execute(l_Statement);

            p_Player->SaveToDB();
            return true;
        }
};

/// Legendary lootbox - 300040
class item_legendary_lootbox : public ItemScript
{
    public:
        item_legendary_lootbox() : ItemScript("item_legendary_lootbox") { }

        bool OnOpen(Player* p_Player, Item* p_Item)
        {
            p_Player->DestroyItem(p_Item->GetBagSlot(), p_Item->GetSlot(), true);

            std::ostringstream l_LogResult;
            l_LogResult << "Legendary Gear lootbox - 300040 Player " << p_Player->GetName() << " (guid " << p_Player->GetGUIDLow() << ") ";

            /// Legendary
            if (ItemTemplate const* l_LegendaryItem = sObjectMgr->GetItemLegendary(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player))
            {
                auto l_Item = p_Player->AddItem(l_LegendaryItem->ItemId, 1, {}, false, 0, false, true);
                p_Player->SendDisplayToast(l_LegendaryItem->ItemId, 1, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LEGENDARY_LOOTED, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false, l_Item ? l_Item->GetAllItemBonuses() : std::vector<uint32>());

                l_LogResult << " got a legendary " << l_LegendaryItem->ItemId;

#ifndef CROSS
                if (Guild* l_Guild = p_Player->GetGuild())
                {
                    if (l_Item)
                        l_Guild->GetNewsLog().AddNewEvent(GUILD_NEWS_ITEM_LOOTED, time(nullptr), p_Player->GetGUID(), 0, l_Item->GetGUIDLow(), l_Item);
                }
#endif
            }

            PreparedStatement* l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_LOOTBOX_LOG);
            l_Statement->setUInt32(0, p_Player->GetGUIDLow());
            l_Statement->setString(1, l_LogResult.str().c_str());
            LogDatabase.Execute(l_Statement);

            p_Player->SaveToDB();
            return true;
        }
};

///  XMAS Lootbox V1 - 310000
class item_sylvanas_xmas_lootbox : public ItemScript
{
    public:
        item_sylvanas_xmas_lootbox() : ItemScript("item_sylvanas_xmas_lootbox") { }

        enum eItems
        {
        /// DisposableWinterVeilSuits   = 139337,
            EndothermicBlaster          = 128636,
            WinterVeilDisguiseKit       = 17712,
        /// PineappleLoungeCushion      = 116689,

        /// RedWoodenSled               = 128776,
            Grumpling                   = 128770,
            ClockworkRocketBot          = 34425,
            BlueClockworkRocketBot      = 54436,
            GreenHelperBox              = 21305,
            SnowmanKit                  = 21309,
            JinglingBell                = 21308,

            MinionofGrumpus             = 128671,
            GroveWarden                 = 128422
        };

        enum eSpells
        {
            Reindeer                    = 172568
        };

        enum eTitles
        {
            MerryMaker                  = 134
        };

        bool OnOpen(Player* p_Player, Item* p_Item)
        {
            p_Player->m_IsInLootboxProcess = true;

            p_Player->DestroyItem(p_Item->GetBagSlot(), p_Item->GetSlot(), true);

            std::ostringstream l_LogResult;
            l_LogResult << "XMAS lootbox V1 - 310000 Player " << p_Player->GetName() << " (guid " << p_Player->GetGUIDLow() << ") ";

            uint32 l_UpgradeChance = 50;

            /// Money
            uint64 l_Money = 1000;
            while (urand(0, 100) > l_UpgradeChance)
                l_Money += 1000;
            l_Money *= GOLD;

            l_LogResult << l_Money / 1000 << " gold ";

            p_Player->ModifyMoney(l_Money, "lootbox");
            p_Player->SendDisplayToast(0, l_Money, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_MONEY, false, false);

            /// AP                    100    200      400    800     1000
            uint32 l_APItems[5] = { 141701, 138786, 143869, 141667, 139506 };
            uint8 l_I = 0;

            while (l_I < 4 && urand(0, 100) > l_UpgradeChance)
                l_I++;

            p_Player->AddItem(l_APItems[l_I], 1, {}, false, 0, false, true);
            p_Player->SendDisplayToast(l_APItems[l_I], 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            l_LogResult << l_APItems[l_I] << " AP item " << l_APItems[l_I] << " ";

            /// Sargeras
            l_I = 1;
            while (l_I < 5 && urand(0, 100) > l_UpgradeChance)
                l_I++;

            p_Player->AddItem(124124, l_I, {}, false, 0, false, true);
            p_Player->SendDisplayToast(124124, l_I, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            l_LogResult << l_I << " blood of sargeras ";

            std::vector<CosmeticEntry> const& m_Cosmetics = sObjectMgr->GetLootBoxCosmeticPool();
            if (m_Cosmetics.empty())
                return false;

            uint32 l_Position = urand(0, m_Cosmetics.size() - 1);

            CosmeticEntry l_Entry = m_Cosmetics[l_Position];
            std::string l_Name = sObjectMgr->GetCosmeticName(p_Player, l_Entry.Type, l_Entry.ID);

            std::vector<uint32> l_ItemPool;
            if (!p_Player->HasCosmetic(l_Entry.Type, l_Entry.ID))
            {
                p_Player->AddCosmetic(l_Entry.Type, l_Entry.ID);
                ChatHandler(p_Player->GetSession()).PSendSysMessage(TrinityStrings::ReceivedCosmetic, l_Name.c_str());
                l_LogResult << "cosmetic Type :" << l_Entry.Type <<"cosmetic ID: " << l_Entry.ID << " ";
            }
            else if (urand(0, 3)) /// 75%
            {
                l_ItemPool.push_back(eItems::EndothermicBlaster);
                l_ItemPool.push_back(eItems::WinterVeilDisguiseKit);
            }
            else if (urand(0, 1)) /// 25% * 50% = 12.5%
            {
                l_ItemPool.push_back(eItems::Grumpling);
                l_ItemPool.push_back(eItems::ClockworkRocketBot);
                l_ItemPool.push_back(eItems::BlueClockworkRocketBot);
                l_ItemPool.push_back(eItems::GreenHelperBox);
                l_ItemPool.push_back(eItems::SnowmanKit);
                l_ItemPool.push_back(eItems::JinglingBell);
            }
            else if (!urand(0, 2)) /// 12.5% * 33.3% = 4.16%
            {
                switch (urand(0,3))
                {
                    case 0:
                    {
                        p_Player->AddItem(eItems::MinionofGrumpus, 1, {}, false, 0, false, true);
                        p_Player->SendDisplayToast(eItems::MinionofGrumpus, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);
                        l_LogResult << "ItemID: " << eItems::MinionofGrumpus;
                        break;
                    }
                    case 1:
                    {
                        p_Player->AddItem(eItems::GroveWarden, 1, {}, false, 0, false, true);
                        p_Player->SendDisplayToast(eItems::GroveWarden, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);
                        l_LogResult << "ItemID: " << eItems::GroveWarden;
                        break;
                    }
                    case 2:
                    {
                        p_Player->learnSpell(eSpells::Reindeer, false, true);
                        l_LogResult << "SpellID: " << eSpells::Reindeer;
                        break;
                    }
                    case 3:
                    {
                        CharTitlesEntry const* l_TitleEntry = sCharTitlesStore.LookupEntry(eTitles::MerryMaker);
                        p_Player->SetTitle(l_TitleEntry);
                        l_LogResult << "TitelID: " << eTitles::MerryMaker;
                        break;
                    }
                    default:
                        break;
                }
            }
            else
            {
                ChatHandler(p_Player->GetSession()).PSendSysMessage(TrinityStrings::ReceivedCosmeticAlreadyOwned, l_Name.c_str());
                l_LogResult << "cosmetic Type :" << l_Entry.Type << "cosmetic ID: " << l_Entry.ID << " Already owned ";
            }

            if (!l_ItemPool.empty())
            {
                uint32 l_ItemID = l_ItemPool[urand(0, l_ItemPool.size() - 1)];
                p_Player->AddItem(l_ItemID, 1, {}, false, 0, false, true);
                p_Player->SendDisplayToast(l_ItemID, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);
                l_LogResult << "ItemID: " << l_ItemID;
            }

            PreparedStatement* l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_LOOTBOX_LOG);
            l_Statement->setUInt32(0, p_Player->GetGUIDLow());
            l_Statement->setString(1, l_LogResult.str().c_str());
            LogDatabase.Execute(l_Statement);

            p_Player->SaveToDB();

            p_Player->m_IsInLootboxProcess        = false;

            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_Webshop_Delivery()
{
    /// Delivery events
    new WebShop::Delivery_Item();
    new WebShop::Delivery_Gold();
    new WebShop::Delivery_Currency();
    new WebShop::Delivery_Level();
    new WebShop::Delivery_Profession();
    new WebShop::Delivery_Save();
    new WebShop::Delivery_Title();

    /// Items
    new item_gear_dungeon_mythic_lootbox();
    new item_gear_raid_01_lootbox();
    new item_gear_raid_02_lootbox();
    new item_gear_raid_03_lootbox();
    new item_gear_raid_04_lootbox();
    new item_legendary_lootbox();

    new item_sylvanas_xmas_lootbox();
};
#endif
