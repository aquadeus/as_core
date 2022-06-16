////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "Item.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "DatabaseEnv.h"
#include "ItemEnchantmentMgr.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "ScriptMgr.h"
#include "ConditionMgr.h"
#include "DB2Stores.h"
#include "GameTables.h"
#include "CollectionMgr.hpp"
#include "ScenarioMgr.h"
#include "Scenario.h"
#include "Challenge.h"
#include "ChallengeMgr.h"
#include "Player.h"
#include "RecruitAFriendMgr.hpp"

#ifdef CROSS
# include "InterRealmMgr.h"
#endif

uint32 const SocketColorToGemTypeMask[19] =
{
    0,
    SOCKET_COLOR_META,
    SOCKET_COLOR_RED,
    SOCKET_COLOR_YELLOW,
    SOCKET_COLOR_BLUE,
    SOCKET_COLOR_HYDRAULIC,
    SOCKET_COLOR_COGWHEEL,
    SOCKET_COLOR_PRISMATIC,
    SOCKET_COLOR_RELIC_IRON,
    SOCKET_COLOR_RELIC_BLOOD,
    SOCKET_COLOR_RELIC_SHADOW,
    SOCKET_COLOR_RELIC_FEL,
    SOCKET_COLOR_RELIC_ARCANE,
    SOCKET_COLOR_RELIC_FROST,
    SOCKET_COLOR_RELIC_FIRE,
    SOCKET_COLOR_RELIC_WATER,
    SOCKET_COLOR_RELIC_LIFE,
    SOCKET_COLOR_RELIC_WIND,
    SOCKET_COLOR_RELIC_HOLY
};

void AddItemsSetItem(Player* player, Item* item)
{
    ItemTemplate const* proto = item->GetTemplate();
    uint32 setid = proto->ItemSet;

    ItemSetEntry const* set = sItemSetStore.LookupEntry(setid);

    if (!set)
    {
        sLog->outError(LOG_FILTER_SQL, "Item set %u for item (id %u) not found, mods not applied.", setid, proto->ItemId);
        return;
    }

    if (set->RequiredSkill && player->GetSkillValue(set->RequiredSkill) < set->RequiredSkillRank)
        return;

    if (set->Flags & ITEM_SET_FLAG_LEGACY_INACTIVE)
        return;

    ItemSetEffect* eff = NULL;

    for (size_t x = 0; x < player->ItemSetEff.size(); ++x)
    {
        if (player->ItemSetEff[x] && player->ItemSetEff[x]->ItemSetID == setid)
        {
            eff = player->ItemSetEff[x];
            break;
        }
    }

    if (!eff)
    {
        eff = new ItemSetEffect();
        eff->ItemSetID = setid;
        eff->EquippedItemCount = 0;

        size_t x = 0;
        for (; x < player->ItemSetEff.size(); ++x)
            if (!player->ItemSetEff[x])
                break;

        if (x < player->ItemSetEff.size())
            player->ItemSetEff[x] = eff;
        else
            player->ItemSetEff.push_back(eff);
    }

    ++eff->EquippedItemCount;

    ItemSetSpellsByItemID::const_iterator iter = sItemSetSpellsByItemIDStore.find(setid);
    if (iter == sItemSetSpellsByItemIDStore.end())
        return;

    std::list<ItemSetSpellEntry const*> const& setSpells = iter->second;

    int i = 0;
    for (auto itemSetSpell : setSpells)
    {
        //not enough for  spell
        if (itemSetSpell->Threshold > eff->EquippedItemCount)
            continue;

        if (eff->SetBonuses.count(itemSetSpell))
            continue;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itemSetSpell->SpellID);
        if (!spellInfo)
        {
            sLog->outError(LOG_FILTER_PLAYER_ITEMS, "WORLD: unknown spell id %u in items set %u effects", itemSetSpell->SpellID, setid);
            continue;
        }

        eff->SetBonuses.insert(itemSetSpell);
        // spell cast only if fit form requirement, in other case will cast at form change
        if (!itemSetSpell->SpecializationID || itemSetSpell->SpecializationID == player->GetActiveSpecializationID())
            player->ApplyEquipSpell(spellInfo, NULL, true);
    }
}

void RemoveItemsSetItem(Player*player, ItemTemplate const* proto)
{
    uint32 setid = proto->ItemSet;

    /// If set ID = 0, we don't need to search for ItemSetEntry
    if (!setid)
        return;

    ItemSetEntry const* set = sItemSetStore.LookupEntry(setid);
    if (!set)
    {
        sLog->outError(LOG_FILTER_SQL, "Item set #%u for item #%u not found, mods not removed.", setid, proto->ItemId);
        return;
    }

    ItemSetEffect* eff = NULL;
    size_t setindex = 0;
    for (; setindex < player->ItemSetEff.size(); setindex++)
    {
        if (player->ItemSetEff[setindex] && player->ItemSetEff[setindex]->ItemSetID == setid)
        {
            eff = player->ItemSetEff[setindex];
            break;
        }
    }

    // can be in case now enough skill requirement for set appling but set has been appliend when skill requirement not enough
    if (!eff)
        return;

    --eff->EquippedItemCount;

    ItemSetSpellsByItemID::const_iterator iter = sItemSetSpellsByItemIDStore.find(setid);
    if (iter == sItemSetSpellsByItemIDStore.end())
        return;

    std::list<ItemSetSpellEntry const*> const& setSpells = iter->second;

    for (auto itemSetSpell : setSpells)
    {
        // enough for spell
        if (itemSetSpell->Threshold <= eff->EquippedItemCount)
            continue;

        if (!eff->SetBonuses.count(itemSetSpell))
            continue;

        player->ApplyEquipSpell(sSpellMgr->GetSpellInfo(itemSetSpell->SpellID), nullptr, false);
        eff->SetBonuses.erase(itemSetSpell);
    }

    if (!eff->EquippedItemCount)                                    //all items of a set were removed
    {
        ASSERT(eff == player->ItemSetEff[setindex]);
        delete eff;
        player->ItemSetEff[setindex] = NULL;
    }
}

bool ItemCanGoIntoBag(ItemTemplate const* pProto, ItemTemplate const* pBagProto)
{
    if (!pProto || !pBagProto)
        return false;

    switch (pBagProto->Class)
    {
        case ITEM_CLASS_CONTAINER:
            switch (pBagProto->SubClass)
            {
                case ITEM_SUBCLASS_CONTAINER:
                    return true;
                case ITEM_SUBCLASS_SOUL_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_SOUL_SHARDS))
                        return false;
                    return true;
                case ITEM_SUBCLASS_HERB_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_HERBS))
                        return false;
                    return true;
                case ITEM_SUBCLASS_ENCHANTING_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_ENCHANTING_SUPPLIES))
                        return false;
                    return true;
                case ITEM_SUBCLASS_MINING_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_MINING_SUPPLIES))
                        return false;
                    return true;
                case ITEM_SUBCLASS_ENGINEERING_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_ENGINEERING_SUPPLIES))
                        return false;
                    return true;
                case ITEM_SUBCLASS_GEM_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_GEMS))
                        return false;
                    return true;
                case ITEM_SUBCLASS_LEATHERWORKING_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_LEATHERWORKING_SUPPLIES))
                        return false;
                    return true;
                case ITEM_SUBCLASS_INSCRIPTION_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_INSCRIPTION_SUPPLIES))
                        return false;
                    return true;
                case ITEM_SUBCLASS_TACKLE_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_FISHING_SUPPLIES))
                        return false;
                    return true;
                case ITEM_SUBCLASS_COOKING_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_COOKING_SUPPLIES))
                        return false;
                    return true;
                default:
                    return false;
            }
        case ITEM_CLASS_QUIVER:
            switch (pBagProto->SubClass)
            {
                case ITEM_SUBCLASS_QUIVER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_ARROWS))
                        return false;
                    return true;
                case ITEM_SUBCLASS_AMMO_POUCH:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_BULLETS))
                        return false;
                    return true;
                default:
                    return false;
            }
    }
    return false;
}

ItemModifiers const g_AppearanceModifierSlotBySpec[MAX_SPECIALIZATIONS] =
{
    ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_1,
    ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_2,
    ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_3,
    ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_4
};

static uint32 const g_AppearanceModifierMaskSpecSpecific =
    (1 << ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_1) |
    (1 << ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_2) |
    (1 << ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_3) |
    (1 << ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_4);

ItemModifiers const g_IllusionModifierSlotBySpec[MAX_SPECIALIZATIONS] =
{
    ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_1,
    ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_2,
    ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_3,
    ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_4
};

static uint32 const g_IllusionModifierMaskSpecSpecific =
    (1 << ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_1) |
    (1 << ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_2) |
    (1 << ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_3) |
    (1 << ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_4);

Item::Item()
{
    m_objectType |= TYPEMASK_ITEM;
    m_objectTypeId = TYPEID_ITEM;

    m_updateFlag = 0;

    m_valuesCount = ITEM_END;
    m_slot = 0;
    uState = ITEM_NEW;
    uQueuePos = -1;
    m_container = NULL;
    m_lootGenerated = false;
    mb_in_trade = false;
    m_lastPlayedTimeUpdate = time(NULL);
    m_ArtifactMgr = nullptr;

    m_refundRecipient = 0;
    m_paidMoney = 0;
    m_paidExtendedCost = 0;

    // Fuck default constructor, i don't trust it
    m_text = "";

    m_CustomFlags = 0;

    _dynamicValuesCount = ITEM_DYNAMIC_END;

    memset(m_Modifiers, 0, sizeof (m_Modifiers));
    memset(&_bonusData, 0, sizeof(_bonusData));

    m_scaleLvl = 0;

    m_ChildItem = 0;
    m_BagFlags = eBagSlotFlags::BagSlotFlagNone;

    m_ScaleArtifactKnowledge = 0;
}

bool RemoveItemByDelete(Player* p_Player, Item* p_Item)
{
    for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
    {
        if (Item* l_Item = p_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
        {
            if (l_Item == p_Item)
            {
                p_Player->RemoveItem(INVENTORY_SLOT_BAG_0, i, false);
                return true;
            }
        }
    }

    for (int i = BANK_SLOT_ITEM_START; i < BANK_SLOT_BAG_END; ++i)
    {
        if (Item* l_Item = p_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
        {
            if (l_Item == p_Item)
            {
                p_Player->RemoveItem(INVENTORY_SLOT_BAG_0, i, false);
                return true;
            }
        }
    }

    for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
    {
        if (Bag* l_Bag = p_Player->GetBagByPos(i))
        {
            for (uint32 j = 0; j < l_Bag->GetBagSize(); ++j)
            {
                if (Item* pItem = l_Bag->GetItemByPos(j))
                {
                    if (pItem == p_Item)
                    {
                        l_Bag->RemoveItem(j, false);
                        return true;
                    }
                }
            }
        }
    }

    for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
    {
        if (Bag* l_Bag = p_Player->GetBagByPos(i))
        {
            for (uint32 j = 0; j < l_Bag->GetBagSize(); ++j)
            {
                if (Item* pItem = l_Bag->GetItemByPos(j))
                {
                    if (pItem == p_Item)
                    {
                        l_Bag->RemoveItem(j, false);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

Item::~Item()
{
    if (sWorld->IsStopped())
        return;

    if (Player* l_Player = GetOwner())
    {
        Map* l_ThreadLocalMap = Map::GetCurrentMapThread();
        if (l_Player->GetMap() && l_ThreadLocalMap && l_ThreadLocalMap != l_Player->GetMap())
        {
            sLog->outExtChat("#jarvis-log", "danger", true, "Threadguard triggered for Item (GUID = %u) (Map (Id : %u) modifying Map (Id : %u))", GetGUIDLow(), l_ThreadLocalMap->GetId(), l_Player->GetMap()->GetId());
            if (sWorld->getBoolConfig(CONFIG_ENABLE_UNSAFE_SPELL_AURA_HOOK_STACK_TRACE))
            {
                ACE_Stack_Trace l_StackTrace;
                sLog->outExtChat("#jarvis-log", "danger", true, l_StackTrace.c_str());
            }
        }
    }

    if (m_ArtifactMgr)
        m_ArtifactMgr->SetItemOwner(nullptr);
}

bool Item::Create(uint32 guidlow, uint32 itemid, Player const* owner)
{
    Object::_Create(guidlow, 0, HIGHGUID_ITEM);

    SetEntry(itemid);
    SetObjectScale(1.0f);

#ifdef CROSS
    if (owner)
    {
        uint32 l_LocalRealmGuid = InterRealmClient::GetIRClient((Player*)owner)->GenerateLocalRealmLowGuid(HIGHGUID_ITEM);
        if (l_LocalRealmGuid == 0)
            ((Player*)owner)->AddItemToGuidSync(GetGUID());
        else
            SetRealGUID(MAKE_NEW_GUID(l_LocalRealmGuid, 0, HIGHGUID_ITEM));
    }
#endif

    SetGuidValue(ITEM_FIELD_OWNER, owner ? owner->GetGUID() : 0);
    SetGuidValue(ITEM_FIELD_CONTAINED_IN, owner ? owner->GetGUID() : 0);

    ItemTemplate const* itemProto = sObjectMgr->GetItemTemplate(itemid);
    if (!itemProto)
        return false;

    _bonusData.Initialize(itemProto);

    loot.SetSource(GetGUID());

    m_ArtifactMgr = nullptr;

    SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
    SetUInt32Value(ITEM_FIELD_MAX_DURABILITY, itemProto->MaxDurability);
    SetUInt32Value(ITEM_FIELD_DURABILITY, itemProto->MaxDurability);

    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        SetSpellCharges(i, itemProto->Spells[i].SpellCharges);

        if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itemProto->Spells[i].SpellId))
        {
            if (spellInfo->HasEffect(SPELL_EFFECT_GIVE_ARTIFACT_POWER) && owner)
            {
                uint32 l_PlayerArtifactKnowledge = owner->getLevel() >= 110 ? sWorld->GetCurrentArtifactKnowledge() : 0;

                if (MS::Artifact::Manager const* l_Artifact = owner->GetCurrentlyEquippedArtifact())
                {
                    if (l_Artifact->GetLevel() < 35 && l_Artifact->GetArtifactPower() < 1500000)
                    {
                        if (owner->HasAura(eRecruitAFriendConstants::TrackerSpell))
                            l_PlayerArtifactKnowledge += eRecruitAFriendConstants::AKBonus;
                    }
                }

                SetModifier(ITEM_MODIFIER_ARTIFACT_KNOWLEDGE_LEVEL, l_PlayerArtifactKnowledge + 1);
            }
        }
    }

    SetUInt32Value(ITEM_FIELD_EXPIRATION, itemProto->Duration);
    SetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME, 0);

    /// Init gems data
    ItemDynamicFieldGems gemData[MAX_GEM_SOCKETS];
    memset(gemData, 0, sizeof(gemData));

    for (uint32 i = 0; i < MAX_GEM_SOCKETS; ++i)
        SetGem(i, &gemData[i], 0);

    if (itemProto->NeedScaleOnArtifactKnowledge && owner)
    {
        uint32 l_PlayerArtifactKnowledge = owner->getLevel() >= 110 ? sWorld->GetCurrentArtifactKnowledge() : 0;
        m_ScaleArtifactKnowledge = l_PlayerArtifactKnowledge;
    }

    /// Insignia of the Grand Army is not a "classic" legendary
    /// But still needs to be at 1000 item level
    if (itemid == 152626)
    {
        enum e1000Bonuses
        {
            BonusIlvlDisplay = 1811,
            BonusIlvlUpgrade = 3630
        };

        AddItemBonus(e1000Bonuses::BonusIlvlDisplay);
        AddItemBonus(e1000Bonuses::BonusIlvlUpgrade);
    }

    return true;
}

// Returns true if Item is a bag AND it is not empty.
// Returns false if Item is not a bag OR it is an empty bag.
bool Item::IsNotEmptyBag() const
{
    if (Bag const* bag = ToBag())
        return !bag->IsEmpty();
    return false;
}

void Item::UpdateDuration(Player* owner, uint32 diff)
{
    if (!GetUInt32Value(ITEM_FIELD_EXPIRATION))
        return;

    sLog->outDebug(LOG_FILTER_PLAYER_ITEMS, "Item::UpdateDuration Item (Entry: %u Duration %u Diff %u)", GetEntry(), GetUInt32Value(ITEM_FIELD_EXPIRATION), diff);

    if (GetUInt32Value(ITEM_FIELD_EXPIRATION) <= diff)
    {
        sScriptMgr->OnItemExpire(owner, GetTemplate());
        if (GetEntry() == 138019)
            sLog->outAshran("Mythic Keystone delete [1]");
        owner->DestroyItem(GetBagSlot(), GetSlot(), true);
        return;
    }

    SetUInt32Value(ITEM_FIELD_EXPIRATION, GetUInt32Value(ITEM_FIELD_EXPIRATION) - diff);
    SetState(ITEM_CHANGED, owner);                          // save new time in database
}

void Item::SaveToLogs(Player const* p_Player, uint32 p_ItemEntry, time_t p_Timestamp, uint32 p_ItemGUID)
{
    PreparedStatement* l_Stmt = LogDatabase.GetPreparedStatement(LOG_INSERT_ITEM_GM_LOG);
    l_Stmt->setUInt32(0, p_ItemGUID);
    l_Stmt->setUInt32(1, p_Player->GetGUIDLow());
    l_Stmt->setUInt32(2, p_ItemEntry);
    l_Stmt->setUInt32(3, static_cast<uint32>(p_Timestamp));
    l_Stmt->setString(4, "");
    LogDatabase.Execute(l_Stmt);
}

void Item::UpdateLogs(uint32 p_ItemGUID, std::vector<uint32> p_Bonuses)
{
    std::ostringstream l_String;

    if (p_Bonuses.size() > 0)
    {
        for (uint32 l_Bonus : p_Bonuses)
            l_String << l_Bonus << " ";
    }

    PreparedStatement* l_Stmt = LogDatabase.GetPreparedStatement(LOG_UPDATE_ITEM_GM_LOG);
    l_Stmt->setString(0, l_String.str());
    l_Stmt->setUInt32(1, p_ItemGUID);
    LogDatabase.Execute(l_Stmt);
}

void Item::SaveToDB(SQLTransaction& trans)
{
#ifdef CROSS
    auto l_Database = GetOwner() ? GetOwner()->GetRealmDatabase() : nullptr;
    if (l_Database == nullptr)  ///< Happen only in case of guild bank, and guild bank aren't handle cross-side
        return;
#else
    auto l_Database = &CharacterDatabase;
#endif

    bool isInTransaction = trans.get() != nullptr;
    if (!isInTransaction)
        trans = l_Database->BeginTransaction();

    uint32 guid = GetRealGUIDLow();
    switch (uState)
    {
        case ITEM_NEW:
        case ITEM_CHANGED:
        {
            uint8 index = 0;
            PreparedStatement* stmt = l_Database->GetPreparedStatement(uState == ITEM_NEW ? CHAR_REP_ITEM_INSTANCE : CHAR_UPD_ITEM_INSTANCE);
            stmt->setUInt32(  index, GetEntry());
            stmt->setUInt32(++index, GetOwner() ? GetOwner()->GetRealGUIDLow() : GetOwnerGUID());

            uint32 l_CreatorGuid = GUID_LOPART(GetGuidValue(ITEM_FIELD_CREATOR));

# ifdef CROSS
            if (GetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS) & ITEM_FIELD_FLAG_CHILD)
            {
                if (Player* l_Owner = GetOwner())
                {
                    bool l_Found = false;
                    for (uint8 l_I = PLAYER_SLOT_START; l_I < PLAYER_SLOT_END; l_I++)
                    {
                        Item* l_Item = l_Owner->GetItemBySlot(l_I);
                        if (!l_Item || l_Item->GetGUIDLow() != l_CreatorGuid)
                            continue;

                        l_CreatorGuid = GUID_LOPART(l_Item->GetRealGUID());
                        l_Found = true;
                        break;
                    }

                    if (!l_Found)
                    {
                        for (uint8 l_I = INVENTORY_SLOT_BAG_START; l_I < INVENTORY_SLOT_BAG_END; l_I++)
                        {
                            if (Bag* pBag = l_Owner->GetBagByPos(l_I))
                            {
                                for (uint32 j = 0; j < pBag->GetBagSize(); j++)
                                {
                                    Item* l_Item = l_Owner->GetItemByPos(l_I, j);
                                    if (!l_Item || l_Item->GetGUIDLow() != l_CreatorGuid)
                                        continue;

                                    l_CreatorGuid = GUID_LOPART(l_Item->GetRealGUID());
                                    l_Found = true;
                                    break;
                                }
                            }
                        }
                    }

                    if (!l_Found)
                    {
                        for (uint8 l_I = BANK_SLOT_BAG_START; l_I < BANK_SLOT_BAG_END; l_I++)
                        {
                            if (Bag* pBag = l_Owner->GetBagByPos(l_I))
                            {
                                for (uint32 j = 0; j < pBag->GetBagSize(); j++)
                                {
                                    Item* l_Item = l_Owner->GetItemByPos(l_I, j);
                                    if (!l_Item || l_Item->GetGUIDLow() != l_CreatorGuid)
                                        continue;

                                    l_CreatorGuid = GUID_LOPART(l_Item->GetRealGUID());
                                    break;
                                }
                            }
                        }
                    }
                }
            }
# endif

            stmt->setUInt32(++index, l_CreatorGuid);
            stmt->setUInt32(++index, GUID_LOPART(GetGuidValue(ITEM_FIELD_GIFT_CREATOR)));
            stmt->setUInt32(++index, GetCount());
            stmt->setUInt32(++index, GetUInt32Value(ITEM_FIELD_EXPIRATION));

            std::ostringstream ssSpells;
            for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                ssSpells << GetSpellCharges(i) << ' ';
            stmt->setString(++index, ssSpells.str());

            stmt->setUInt32(++index, GetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS));

            std::ostringstream ssEnchants;
            for (uint8 i = 0; i < MAX_ENCHANTMENT_SLOT; ++i)
            {
                ssEnchants << GetEnchantmentId(EnchantmentSlot(i)) << ' ';
                ssEnchants << GetEnchantmentDuration(EnchantmentSlot(i)) << ' ';
                ssEnchants << GetEnchantmentCharges(EnchantmentSlot(i)) << ' ';
            }
            stmt->setString(++index, ssEnchants.str());

            stmt->setInt16 (++index, GetItemRandomPropertyId());

            std::ostringstream ssBonuses;
            std::vector<uint32> bonuses = GetAllItemBonuses();
            for (uint8 i = 0; i < bonuses.size(); ++i)
            {
                if (!bonuses[i])
                    continue;

                ssBonuses << bonuses[i] << ' ';
            }

            stmt->setString(++index, ssBonuses.str());
            stmt->setUInt32(++index, GetModifier(ItemModifiers::ITEM_MODIFIER_UPGRADE_ID));
            stmt->setUInt16(++index, GetModifier(ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID));
            stmt->setUInt8(++index, GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL));
            stmt->setUInt8(++index, GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1));
            stmt->setUInt8(++index, GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2));
            stmt->setUInt8(++index, GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3));
            stmt->setUInt8(++index, GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_IS_CHARGED));
            stmt->setUInt8(++index, GetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER));
            stmt->setUInt16(++index, GetUInt32Value(ITEM_FIELD_DURABILITY));
            stmt->setUInt32(++index, GetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME));
            stmt->setString(++index, m_text);
            stmt->setUInt32(++index, m_CustomFlags);
            stmt->setUInt32(++index, GetBagFlags());

            ItemTemplate const* l_ItemTemplate = GetTemplate();
            uint32 l_FixedLvlOrArtifactKnowledge = GetModifier(ItemModifiers::ITEM_MODIFIER_SCALING_STAT_DISTRIBUTION_FIXED_LEVEL);

            if (l_ItemTemplate->NeedScaleOnArtifactKnowledge)
                l_FixedLvlOrArtifactKnowledge = m_ScaleArtifactKnowledge;

            stmt->setUInt32(++index, GetQuality() != ItemQualities::ITEM_QUALITY_HEIRLOOM ? l_FixedLvlOrArtifactKnowledge : 0);
            stmt->setUInt32(++index, guid);

            trans->Append(stmt);

            if ((uState == ITEM_CHANGED) && HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FIELD_FLAG_WRAPPED))
            {
                index   = 0;
                stmt    = l_Database->GetPreparedStatement(CHAR_UPD_GIFT_OWNER);
                stmt->setUInt32(index++, GetOwner()->GetRealGUIDLow());
                stmt->setUInt32(index++, guid);
                trans->Append(stmt);
            }

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEM_INSTANCE_GEMS);
            stmt->setUInt64(0, guid);
            trans->Append(stmt);

            bool l_HaveGemsData = false;
            if (uint32 l_GemsSize = GetGems().size())
            {
                for (ItemDynamicFieldGems const& gemData : GetGems())
                {
                    if (gemData.ItemId)
                        l_HaveGemsData = true;
                }
            }

            if (l_HaveGemsData)
            {
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ITEM_INSTANCE_GEMS);
                stmt->setUInt64(0, guid);
                uint32 i = 0;
                uint32 const gemFields = 4;
                for (ItemDynamicFieldGems const& gemData : GetGems())
                {
                    if (gemData.ItemId)
                    {
                        stmt->setUInt32(1 + i * gemFields, gemData.ItemId);
                        std::ostringstream gemBonusListIDs;

                        for (uint8 l_Idx = 0; l_Idx < 16; l_Idx++)
                        {
                            if (gemData.BonusListIDs[l_Idx])
                                gemBonusListIDs << gemData.BonusListIDs[l_Idx] << ' ';
                        }

                        stmt->setString(2 + i * gemFields, gemBonusListIDs.str());
                        stmt->setUInt8(3 + i * gemFields, gemData.Context);
                        stmt->setUInt32(4 + i * gemFields, m_gemScalingLevels[i]);
                    }
                    else
                    {
                        stmt->setUInt32(1 + i * gemFields, 0);
                        stmt->setString(2 + i * gemFields, "");
                        stmt->setUInt8(3 + i * gemFields, 0);
                        stmt->setUInt32(4 + i * gemFields, 0);
                    }
                    ++i;
                }
                for (; i < MAX_GEM_SOCKETS; ++i)
                {
                    stmt->setUInt32(1 + i * gemFields, 0);
                    stmt->setString(2 + i * gemFields, "");
                    stmt->setUInt8(3 + i * gemFields, 0);
                    stmt->setUInt32(4 + i * gemFields, 0);
                }
                trans->Append(stmt);
            }

            if (m_RelicTalentData.NeedSave)
            {
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_ITEM_RELIC_DATA);
                stmt->setUInt32(0, guid);

                std::ostringstream l_RelicTalenData;
                for (uint8 l_RelicSlot = 0; l_RelicSlot < MAX_GEM_SOCKETS; l_RelicSlot++)
                {
                    l_RelicTalenData << m_RelicTalentData.PathMask[l_RelicSlot] << " ";
                    for (uint8 l_TalentIdx = 0; l_TalentIdx < MAX_RELIC_TALENT; l_TalentIdx++)
                        l_RelicTalenData << m_RelicTalentData.RelicTalent[l_RelicSlot][l_TalentIdx] << " ";
                }

                stmt->setString(1, l_RelicTalenData.str());

                trans->Append(stmt);
            }

            static ItemModifiers const l_TransmogMods[10] =
            {
                ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS,
                ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_1,
                ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_2,
                ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_3,
                ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_4,

                ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS,
                ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_1,
                ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_2,
                ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_3,
                ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_4,
            };

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEM_INSTANCE_TRANSMOG);
            stmt->setUInt64(0, GetGUIDLow());
            trans->Append(stmt);

            if (std::find_if(std::begin(l_TransmogMods), std::end(l_TransmogMods), [this](ItemModifiers p_Modifier) { return GetModifier(p_Modifier) != 0; }) != std::end(l_TransmogMods))
            {
                stmt    = CharacterDatabase.GetPreparedStatement(CHAR_INS_ITEM_INSTANCE_TRANSMOG);
                index   = 0;

                stmt->setUInt64(index++, GetGUIDLow());
                stmt->setUInt32(index++, GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS));
                stmt->setUInt32(index++, GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_1));
                stmt->setUInt32(index++, GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_2));
                stmt->setUInt32(index++, GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_3));
                stmt->setUInt32(index++, GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_4));
                stmt->setUInt32(index++, GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS));
                stmt->setUInt32(index++, GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_1));
                stmt->setUInt32(index++, GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_2));
                stmt->setUInt32(index++, GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_3));
                stmt->setUInt32(index++, GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_4));

                trans->Append(stmt);
            }

            break;
        }
        case ITEM_REMOVED:
        {
            sLog->outAshran("Item::SaveToDB: ITEM_REMOVED itementry %u itemguidlow %u ownerguidlow %u", GetEntry(), guid, GUID_LOPART(GetOwnerGUID()));

            PreparedStatement* stmt = l_Database->GetPreparedStatement(CHAR_DEL_ITEM_INSTANCE);
            stmt->setUInt32(0, guid);
            trans->Append(stmt);

            if (HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FIELD_FLAG_WRAPPED))
            {
                stmt = l_Database->GetPreparedStatement(CHAR_DEL_GIFT);
                stmt->setUInt32(0, guid);
                trans->Append(stmt);
            }

            if (!isInTransaction)
                l_Database->CommitTransaction(trans);

            delete this;
            return;
        }
        case ITEM_UNCHANGED:
            break;
    }

    SetState(ITEM_UNCHANGED);

    if (!isInTransaction)
        l_Database->CommitTransaction(trans);
}

bool Item::LoadFromDB(uint32 guid, uint64 owner_guid, Field* fields, uint32 entry)
{
    ///            0                1          2       3        4        5         6               7              8        9          10          11       12     13
    /// SELECT creatorGuid, giftCreatorGuid, count, duration, charges, flags, enchantments, randomPropertyId, bonuses, upgradeId, durability, playedTime, text, custom_flags
    ///                 14                      15                              16                          17                              18
    /// itemModifiedAppearanceAllSpecs, itemModifiedAppearanceSpec1, itemModifiedAppearanceSpec2, itemModifiedAppearanceSpec3, itemModifiedAppearanceSpec4,
    ///             19                          20                          21                          22                      23                   24
    /// spellItemEnchantmentAllSpecs, spellItemEnchantmentSpec1, spellItemEnchantmentSpec2, spellItemEnchantmentSpec3, spellItemEnchantmentSpec4, BagFlags

    // create item before any checks for store correct guid
    // and allow use "FSetState(ITEM_REMOVED); SaveToDB();" for deleting item from DB

#ifndef CROSS
    Object::_Create(guid, 0, HIGHGUID_ITEM);
#else /* CROSS */
    uint32 new_guid = sObjectMgr->GenerateLowGuid(HIGHGUID_ITEM);
    SetRealGUID(MAKE_NEW_GUID(guid, 0, HIGHGUID_ITEM));
    Object::_Create(new_guid, 0, HIGHGUID_ITEM);
#endif /* CROSS */

    // Set entry, MUST be before proto check
    SetEntry(entry);
    SetObjectScale(1.0f);

    ItemTemplate const* proto = GetTemplate();
    if (!proto)
        return false;

    _bonusData.Initialize(proto);

    m_ArtifactMgr = nullptr;

    // set owner (not if item is only loaded for gbank/auction/mail
    if (owner_guid != 0)
        SetOwnerGUID(owner_guid);

    bool need_save = false;                                 // need explicit save data at load fixes

    uint32 l_Flags = fields[ItemInstanceField::Flags].GetUInt32();

    if (!(l_Flags & ITEM_FIELD_FLAG_CHILD))
        SetGuidValue(ITEM_FIELD_CREATOR, MAKE_NEW_GUID(fields[ItemInstanceField::CreatorGuid].GetUInt32(), 0, HIGHGUID_PLAYER));
    else
    {
        SetGuidValue(ITEM_FIELD_CREATOR, MAKE_NEW_GUID(fields[ItemInstanceField::CreatorGuid].GetUInt32(), 0, HIGHGUID_ITEM));

# ifdef CROSS
        if (Player* l_Player = GetOwner())
        {
            uint64 l_RealGuid = GetGuidValue(ITEM_FIELD_CREATOR);
            bool l_Found = false;
            for (uint8 l_I = PLAYER_SLOT_START; l_I < PLAYER_SLOT_END; l_I++)
            {
                Item* l_Item = l_Player->GetItemBySlot(l_I);
                if (!l_Item || l_Item->GetRealGUID() != l_RealGuid)
                    continue;

                SetGuidValue(ITEM_FIELD_CREATOR, l_Item->GetGUID());
                l_Found = true;
                break;
            }

            if (!l_Found)
            {
                for (uint8 l_I = INVENTORY_SLOT_BAG_START; l_I < INVENTORY_SLOT_BAG_END; l_I++)
                {
                    if (Bag* pBag = l_Player->GetBagByPos(l_I))
                    {
                        for (uint32 j = 0; j < pBag->GetBagSize(); j++)
                        {
                            Item* l_Item = l_Player->GetItemByPos(l_I, j);
                            if (!l_Item || l_Item->GetRealGUID() != l_RealGuid)
                                continue;

                            SetGuidValue(ITEM_FIELD_CREATOR, l_Item->GetGUID());
                            l_Found = true;
                            break;
                        }
                    }
                }
            }

            if (!l_Found)
            {
                for (uint8 l_I = BANK_SLOT_BAG_START; l_I < BANK_SLOT_BAG_END; l_I++)
                {
                    if (Bag* pBag = l_Player->GetBagByPos(l_I))
                    {
                        for (uint32 j = 0; j < pBag->GetBagSize(); j++)
                        {
                            Item* l_Item = l_Player->GetItemByPos(l_I, j);
                            if (!l_Item || l_Item->GetRealGUID() != l_RealGuid)
                                continue;

                            SetGuidValue(ITEM_FIELD_CREATOR, l_Item->GetGUID());
                            l_Found = true;
                            break;
                        }
                    }
                }
            }
        }
# endif
    }

    SetGuidValue(ITEM_FIELD_GIFT_CREATOR, MAKE_NEW_GUID(fields[ItemInstanceField::GiftCreatorGuid].GetUInt32(), 0, HIGHGUID_PLAYER));

    uint32 l_StackCount = fields[ItemInstanceField::Count].GetUInt32();

    /// Fix weird stack issues with wrong count (could happens with some bugs ...)
    if (proto->Stackable == 1 && l_StackCount != 1)
        l_StackCount = 1;

    SetCount(l_StackCount);

    uint32 duration = fields[ItemInstanceField::Duration].GetUInt32();
    SetUInt32Value(ITEM_FIELD_EXPIRATION, duration);
    // update duration if need, and remove if not need
    if ((proto->Duration == 0) != (duration == 0)
        && entry != 138019) ///< mythic keystone hackfix
    {
        SetUInt32Value(ITEM_FIELD_EXPIRATION, proto->Duration);
        need_save = true;
    }

    Tokenizer tokens(fields[ItemInstanceField::Charges].GetString(), ' ', MAX_ITEM_PROTO_SPELLS);
    if (tokens.size() == MAX_ITEM_PROTO_SPELLS)
    {
        for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(proto->Spells[i].SpellId))
                if (spellInfo->HasEffect(SPELL_EFFECT_GIVE_ARTIFACT_POWER))
                    if (uint32 artifactKnowledgeLevel = fields[ItemInstanceField::AcquiredLevel].GetUInt32())
                        SetModifier(ITEM_MODIFIER_ARTIFACT_KNOWLEDGE_LEVEL, artifactKnowledgeLevel + 1);

            SetSpellCharges(i, atoi(tokens[i]));
        }
    }

    SetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS, l_Flags);

    // Remove bind flag for items vs NO_BIND set
    if (IsSoulBound() && proto->Bonding == NO_BIND)
    {
        ApplyModFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FIELD_FLAG_SOULBOUND, false);
        need_save = true;
    }

    std::string enchants = fields[ItemInstanceField::Enchantments].GetString();
    _LoadIntoDataField(enchants.c_str(), ITEM_FIELD_ENCHANTMENT, MAX_ENCHANTMENT_SLOT * MAX_ENCHANTMENT_OFFSET, false);

    Tokenizer bonusTokens(fields[ItemInstanceField::Bonuses].GetString(), ' ');
        for (uint8 i = 0; i < bonusTokens.size(); ++i)
            AddItemBonus(atoi(bonusTokens[i]));

    SetModifier(ItemModifiers::ITEM_MODIFIER_UPGRADE_ID,                            fields[ItemInstanceField::UpgradeId].GetUInt32());
    SetModifier(ItemModifiers::ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID,       fields[ItemInstanceField::ChallengeMapMode].GetUInt16());
    SetModifier(ItemModifiers::ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL,              fields[ItemInstanceField::ChallengeKeystoneLevel].GetUInt8());
    SetModifier(ItemModifiers::ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1,         fields[ItemInstanceField::ChallengeKeystoneAffix1].GetUInt8());
    SetModifier(ItemModifiers::ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2,         fields[ItemInstanceField::ChallengeKeystoneAffix2].GetUInt8());
    SetModifier(ItemModifiers::ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3,         fields[ItemInstanceField::ChallengeKeystoneAffix3].GetUInt8());
    SetModifier(ItemModifiers::ITEM_MODIFIER_CHALLENGE_KEYSTONE_IS_CHARGED,         fields[ItemInstanceField::ChallengeKeystoneIsCharged].GetUInt8());
    SetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER,                         fields[ItemInstanceField::ArtifactTier].GetUInt8());
    SetModifier(ItemModifiers::ITEM_MODIFIER_SCALING_STAT_DISTRIBUTION_FIXED_LEVEL, proto->Quality != ItemQualities::ITEM_QUALITY_HEIRLOOM ? fields[ItemInstanceField::AcquiredLevel].GetUInt32() : 0);
    SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS,         fields[ItemInstanceField::ModifiedAppearanceAllSpecs].GetUInt32());
    SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_1,            fields[ItemInstanceField::ModifiedAppearanceSpec1].GetUInt32());
    SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_2,            fields[ItemInstanceField::ModifiedAppearanceSpec2].GetUInt32());
    SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_3,            fields[ItemInstanceField::ModifiedAppearanceSpec3].GetUInt32());
    SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_4,            fields[ItemInstanceField::ModifiedAppearanceSpec4].GetUInt32());
    SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS,            fields[ItemInstanceField::SpellEnchantmentAllSpecs].GetUInt32());
    SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_1,               fields[ItemInstanceField::SpellEnchantmentSpec1].GetUInt32());
    SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_2,               fields[ItemInstanceField::SpellEnchantmentSpec2].GetUInt32());
    SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_3,               fields[ItemInstanceField::SpellEnchantmentSpec3].GetUInt32());
    SetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_SPEC_4,               fields[ItemInstanceField::SpellEnchantmentSpec4].GetUInt32());

    Tokenizer l_RelicTalentTokenizer(fields[ItemInstanceField::RelicTalentData].GetString(), ' ');

    if (l_RelicTalentTokenizer.size() == (MAX_GEM_SOCKETS * (1 + MAX_RELIC_TALENT)))
    {
        uint32 l_RelicTalentDataIdx = 0;
        for (uint8 l_RelicSlot = 0; l_RelicSlot < MAX_GEM_SOCKETS; l_RelicSlot++)
        {
            m_RelicTalentData.PathMask[l_RelicSlot] = atoi(l_RelicTalentTokenizer[l_RelicTalentDataIdx++]);
            for (uint8 l_TalentIdx = 0; l_TalentIdx < MAX_RELIC_TALENT; l_TalentIdx++)
                m_RelicTalentData.RelicTalent[l_RelicSlot][l_TalentIdx] = atoi(l_RelicTalentTokenizer[l_RelicTalentDataIdx++]);

            auto l_DynamicFields = m_RelicTalentData.GetDynamicFieldStruct(l_RelicSlot);
            SetDynamicStructuredValue(ITEM_DYNAMIC_FIELD_RELIC_TALENT_DATA, sizeof(ItemDynamicFieldRelicTalentData) * l_RelicSlot, &l_DynamicFields);
        }
    }

    switch (entry)
    {
        /// Mythic keystone
        case 138019:
        {
# ifndef CROSS  ///< the challenge key reset time isn't sync with the cross realm, let the normal realm only handle this part
            /// Delete expired key
            if (duration == 0)
                return false;

            if (GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1) != 0 && GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1) != sChallengeMgr->GetCurrentAffixes()[0])
                return false;

            if (GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2) != 0 && GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2) != sChallengeMgr->GetCurrentAffixes()[1])
                return false;

            if (GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3) != 0 && GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3) != sChallengeMgr->GetCurrentAffixes()[2])
                return false;
# endif

            break;
        }
        default:
            break;
    }

    uint32 const gemFields = 4;
    ItemDynamicFieldGems gemData[MAX_GEM_SOCKETS];
    memset(gemData, 0, sizeof(gemData));
    for (uint32 i = 0; i < MAX_GEM_SOCKETS; ++i)
    {
        gemData[i].ItemId = fields[ItemInstanceField::GemItemId1 + i * gemFields].GetUInt32();
        Tokenizer gemBonusListIDs(fields[ItemInstanceField::GemBonuses1 + i * gemFields].GetString(), ' ');
        uint32 b = 0;
        for (char const* token : gemBonusListIDs)
            if (uint32 bonusListID = atoul(token))
                gemData[i].BonusListIDs[b++] = bonusListID;

        gemData[i].Context = fields[ItemInstanceField::GemContext1 + i * gemFields].GetUInt8();

        if (gemData[i].ItemId)
            SetGem(i, &gemData[i], fields[ItemInstanceField::gemScalingLevel1 + i * gemFields].GetUInt32());

        CheckRelicTalentData(i);
    }

    SetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID, fields[ItemInstanceField::RandomPropertyId].GetInt16());
    // recalculate suffix factor
    if (GetItemRandomPropertyId() < 0)
        UpdateItemSuffixFactor();

    uint32 durability = fields[ItemInstanceField::Durability].GetUInt16();
    SetUInt32Value(ITEM_FIELD_DURABILITY, durability);
    // update max durability (and durability) if need
    SetUInt32Value(ITEM_FIELD_MAX_DURABILITY, proto->MaxDurability);
    if (durability > proto->MaxDurability)
    {
        SetUInt32Value(ITEM_FIELD_DURABILITY, proto->MaxDurability);
        need_save = true;
    }

    SetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME, fields[ItemInstanceField::PlayedTime].GetUInt32());
    SetText(fields[ItemInstanceField::Text].GetString());
    SetCustomFlags(fields[ItemInstanceField::CustomFlags].GetUInt32());

    SetBagFlags(fields[ItemInstanceField::BagFlags].GetUInt32());

    if (need_save)                                           // normal item changed state set not work at loading
    {
#ifdef CROSS
        Player* l_Owner = GetOwner();
        InterRealmDatabasePool* l_Database = l_Owner ? l_Owner->GetRealmDatabase() : nullptr;
#else
        auto l_Database = &CharacterDatabase;
#endif

        if (l_Database)
        {
            PreparedStatement* stmt = l_Database->GetPreparedStatement(CHAR_UPD_ITEM_INSTANCE_ON_LOAD);
            stmt->setUInt32(0, GetUInt32Value(ITEM_FIELD_EXPIRATION));
            stmt->setUInt32(1, GetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS));
            stmt->setUInt32(2, GetUInt32Value(ITEM_FIELD_DURABILITY));
            stmt->setUInt32(3, guid);
            l_Database->Execute(stmt);
        }
    }

    return true;
}

/*static*/
void Item::DeleteFromDB(SQLTransaction& trans, uint32 itemGuid, uint32 realmId)
{
#ifdef CROSS

    InterRealmClient* l_IRClient = sInterRealmMgr->GetClientByRealmNumber(realmId);
    if (!l_IRClient)
        return;

    InterRealmDatabasePool* l_Database = l_IRClient->GetDatabase();
#else
    auto l_Database = &CharacterDatabase;
#endif

    PreparedStatement* stmt = l_Database->GetPreparedStatement(CHAR_DEL_ITEM_INSTANCE);
    stmt->setUInt32(0, itemGuid);
    trans->Append(stmt);

    sLog->outAshran("Item::DeleteFromDB %u", itemGuid);
}

void Item::DeleteFromDB(SQLTransaction& trans)
{
    Player* l_Owner = GetOwner();
    if (!l_Owner)
        return; ///< GuildBank only

#ifdef CROSS
    uint32 l_RealmID = l_Owner->GetSession()->GetInterRealmNumber();
#else
    uint32 l_RealmID = g_RealmID;
#endif

    sLog->outAshran("Item::DeleteFromDB itementry %u itemguidlow %u ownerguidlow %u", GetEntry(), GetGUIDLow(), l_Owner->GetGUIDLow());

    DeleteFromDB(trans, GetRealGUIDLow(), l_RealmID);
}

/*static*/
void Item::DeleteFromInventoryDB(SQLTransaction& trans, uint32 itemGuid, uint32 realmId)
{
#ifdef CROSS

    InterRealmClient* l_IRClient = sInterRealmMgr->GetClientByRealmNumber(realmId);
    if (!l_IRClient)
        return;

    InterRealmDatabasePool* l_Database = l_IRClient->GetDatabase();
#else
    auto l_Database = &CharacterDatabase;
#endif

    PreparedStatement* stmt = l_Database->GetPreparedStatement(CHAR_DEL_CHAR_INVENTORY_BY_ITEM);
    stmt->setUInt32(0, itemGuid);
    trans->Append(stmt);
}

void Item::DeleteFromInventoryDB(SQLTransaction& trans)
{
    Player* l_Owner = GetOwner();
    if (!l_Owner)
        return; ///< GuildBank only

#ifdef CROSS
    uint32 l_RealmID = l_Owner->GetSession()->GetInterRealmNumber();
#else
    uint32 l_RealmID = g_RealmID;
#endif

    DeleteFromInventoryDB(trans, GetRealGUIDLow(), l_RealmID);
}

ItemTemplate const* Item::GetTemplate() const
{
    return sObjectMgr->GetItemTemplate(GetEntry());
}

Player* Item::GetOwner()const
{
    return ObjectAccessor::FindPlayerInOrOutOfWorld(GetOwnerGUID());
}

uint32 Item::GetSkill() const
{
    return GetTemplate()->GetSkill();
}

bool Item::IsArtifactOrArtifactChild(Player* p_Player) const
{
    if (!p_Player)
        return false;

    return (const_cast<Item*>(this)->GetArtifactManager() || (HasFlag(EItemFields::ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FIELD_FLAG_CHILD) && p_Player->GetItemByGuid(GetGuidValue(ITEM_FIELD_CREATOR)) && p_Player->GetItemByGuid(GetGuidValue(ITEM_FIELD_CREATOR))->GetArtifactManager()));
}

void Item::GenerateItemBonus(uint32 p_ItemId, ItemContext p_Context, std::vector<uint32>& p_ItemBonus, bool p_OnlyDifficulty /*= false*/, Player* p_Player /*=nullptr*/, Loot* p_Loot /*=nullptr*/, bool p_RollIlvl /*=true*/)
{
    auto l_ItemTemplate = sObjectMgr->GetItemTemplate(p_ItemId);
    if (l_ItemTemplate == nullptr)
        return;

    if (p_Context == ItemContext::RaidLfr && p_Player)
    {
        switch (p_Player->GetMapId())
        {
            /// Tomb of Sargeras LFR tokens
            case 1676:
            {
                std::vector<uint32> l_Spells = { 240721, 240717, 240720, 240718, 240719 };

                for (uint32& l_Spell : l_Spells)
                {
                    if (l_ItemTemplate->HasSpell(l_Spell))
                        p_ItemBonus.push_back(ItemBonus::Bonuses::StartTombOfSargerasLFR);
                    /// Kil'Jaeden only - Drops chest
                    else if (l_ItemTemplate->HasSpell(240716))
                        p_ItemBonus.push_back(ItemBonus::Bonuses::StartKilJaedenLFR);
                }

                break;
            }
            /// Antorus the Burning Throne LFR tokens
            case 1712:
            {
                std::vector<uint32> l_Spells = { 251104, 251105, 251107, 251108, 251109, 251110 };

                for (uint32& l_Spell : l_Spells)
                {
                    if (l_ItemTemplate->HasSpell(l_Spell))
                        p_ItemBonus.push_back(ItemBonus::Bonuses::StartAntorusLFR);
                }

                break;
            }
            default:
                break;
        }
    }

    /// Legion legendaries - ilvl 1000 - Same for Insignia of the Grand Army
    if (sObjectMgr->IsLegionLegendaryItem(p_ItemId) || p_ItemId == 152626)
    {
        enum e1000Bonuses
        {
            BonusIlvlDisplay = 1811,
            BonusIlvlUpgrade = 3630
        };

        p_ItemBonus.push_back(e1000Bonuses::BonusIlvlDisplay);
        p_ItemBonus.push_back(e1000Bonuses::BonusIlvlUpgrade);

        return;
    }

    /// Pantheon's trinkets
    switch (p_ItemId)
    {
        case 154173: ///< Aggramar's Conviction
        case 154174: ///< Golganneth's Vitality
        case 154175: ///< Eonar's Compassion
        case 154176: ///< Khaz'goroth's Courage
        case 154177: ///< Norgannon's Prowess
        {
            p_ItemBonus.push_back(3983);
            p_ItemBonus.push_back(3985);
            return;
        }
        default:
            break;
    }

    /// Process item bonus group
    for (auto l_ItemBonusGroupID : l_ItemTemplate->m_ItemBonusGroups)
    {
        ItemBonusGroup const* l_ItemBonusGroup = sObjectMgr->GetItemBonusGroup(l_ItemBonusGroupID);
        if (l_ItemBonusGroup == nullptr)
            continue;

        /// Handle in  Player::RewardQuest
        if (l_ItemBonusGroup->Flags & ItemBonusGroupFlags::QuestRewardQualityUpgrade)
            continue;

        /// Bonus only for LFR
        if ((l_ItemBonusGroup->Flags & ItemBonusGroupFlags::BonusForLFRDifficulty) && p_Context != ItemContext::RaidLfr)
            continue;

        /// Bonus only for Normal
        if ((l_ItemBonusGroup->Flags & ItemBonusGroupFlags::BonusForNormalDifficulty) && p_Context != ItemContext::RaidNormal)
            continue;

        /// Bonus only for Heroic
        if ((l_ItemBonusGroup->Flags & ItemBonusGroupFlags::BonusForHeroicDifficulty) && p_Context != ItemContext::RaidHeroic)
            continue;

        /// Bonus only for Mythic
        if ((l_ItemBonusGroup->Flags & ItemBonusGroupFlags::BonusForMythicDifficulty) && p_Context != ItemContext::RaidMythic)
            continue;

        p_ItemBonus.push_back(l_ItemBonusGroup->Bonuses.at(urand(0, l_ItemBonusGroup->Bonuses.size() - 1)));
    }

    /// Item bonus per item are store in ItemXBonusTree.db2
    if (sItemBonusTreeByID.find(p_ItemId) == sItemBonusTreeByID.end())
        return;

    /// Override ItemContext if needed, to allow players to have some rewards while testing PvE content
    if (p_Player != nullptr && sObjectMgr->IsDisabledMap(p_Player->GetMapId(), p_Player->GetMap()->GetDifficultyID()))
    {
        ItemContext l_NewContext = sObjectMgr->GetItemContextOverride(p_Player->GetMapId(), p_Player->GetMap()->GetDifficultyID());
        if (l_NewContext != ItemContext::None)
            p_Context = l_NewContext;
    }

    std::vector<ItemBonusTreeNodeEntry const*> l_PossibleBonusTreeNodes;

    /// Step one : search for generic bonus we can find in DB2 (90, 92, 94, 95, 97, heroic)
    /// If Context was already specified
    auto& l_ItemBonusTree = sItemBonusTreeByID[p_ItemId];
    std::for_each(l_ItemBonusTree.begin(), l_ItemBonusTree.end(), [&p_Context, &p_ItemBonus, &l_PossibleBonusTreeNodes](ItemXBonusTreeEntry const* p_ItemXBonusTree) -> void
    {
        /// Lookup for the right bonus
        for (uint32 l_Index = 0; l_Index < sItemBonusTreeNodeStore.GetNumRows(); l_Index++)
        {
            auto l_ItemBonusTreeNode = sItemBonusTreeNodeStore.LookupEntry(l_Index);
            if (l_ItemBonusTreeNode == nullptr)
                continue;

            if (l_ItemBonusTreeNode->Category != p_ItemXBonusTree->ItemBonusTreeCategory)
                continue;

            l_PossibleBonusTreeNodes.push_back(l_ItemBonusTreeNode);

            if (l_ItemBonusTreeNode->ItemContext != uint32(p_Context) && l_ItemBonusTreeNode->ItemContext != 0)
                continue;

            auto l_BonusId = l_ItemBonusTreeNode->ChildItemBonusListId;

            /// If no bonusId, we must use LinkedCategory to try to find it ...
            /// ItemBonusTreeNode.db2 havn't full data (6.0.3 19116), in somes case we can't find the item bonus
            /// Maybe we can have full data by bruteforcing on retail ?
            if (l_BonusId == 0)
            {
                for (uint32 l_LinkedIndex = 0; l_LinkedIndex < sItemBonusTreeNodeStore.GetNumRows(); l_LinkedIndex++)
                {
                    auto l_LinkedItemBonusTreeNode = sItemBonusTreeNodeStore.LookupEntry(l_LinkedIndex);
                    if (l_LinkedItemBonusTreeNode == nullptr)
                        continue;

                    if (l_LinkedItemBonusTreeNode->Category != l_ItemBonusTreeNode->ChildItemBonusTreeId)
                        continue;

                    l_BonusId = l_LinkedItemBonusTreeNode->ChildItemBonusListId;
                    break;
                }
            }

            if (l_BonusId != 0)
            {
                p_ItemBonus.push_back(l_BonusId);
            }
        }
    });

    uint32 l_Exp = p_Player ? p_Player->GetMap()->GetEntry()->Expansion() : Expansion::MAX_EXPANSION;

    /// Step two : Roll for stats bonus (Avoidance, Leech & Speed)
    /// Atm, i can't find percentage chance to have stats but it's same pretty low (~ 10%)
    /// Item can have only on stat bonus, and it's only in dungeon/raid
    if (l_Exp >= Expansion::EXPANSION_WARLORDS_OF_DRAENOR
        && (p_Context == ItemContext::RaidNormal
        || p_Context == ItemContext::RaidHeroic
        || p_Context == ItemContext::RaidLfr
        || p_Context == ItemContext::RaidMythic
        || p_Context == ItemContext::DungeonLevelUp1
        || p_Context == ItemContext::DungeonLevelUp2
        || p_Context == ItemContext::DungeonLevelUp3
        || p_Context == ItemContext::DungeonLevelUp4
        || p_Context == ItemContext::DungeonNormal
        || p_Context == ItemContext::DungeonHeroic
        || p_Context == ItemContext::DungeonMythic
        || p_Context == ItemContext::ChallengeMode
        || p_Context == ItemContext::WorldQuest1
        || p_Context == ItemContext::WorldQuest2
        || p_Context == ItemContext::WorldQuest3
        || p_Context == ItemContext::WorldQuest4
        || p_Context == ItemContext::WorldQuest5
        || p_Context == ItemContext::WorldQuest6
        || p_Context == ItemContext::ChallengeModeJackpot
        || p_Context == ItemContext::WorldQuest7
        || p_Context == ItemContext::PvPRanked4
        || p_Context == ItemContext::Ilvl850
        || p_Context == ItemContext::PvPRanked
        || p_Context == ItemContext::Ilvl860
        || p_Context == ItemContext::PvPRanked3
        || p_Context == ItemContext::WorldQuest10
        || p_Context == ItemContext::PvPRanked5
        || p_Context == ItemContext::PvPRanked6
        || p_Context == ItemContext::Ilvl885
        || p_Context == ItemContext::Ilvl890
        || p_Context == ItemContext::Ilvl895
        || p_Context == ItemContext::Ilvl900
        || p_Context == ItemContext::Ilvl905
        || p_Context == ItemContext::Ilvl910
        || p_Context == ItemContext::Ilvl915
        || p_Context == ItemContext::Ilvl920
        || p_Context == ItemContext::Ilvl925
        || p_Context == ItemContext::Ilvl930
        || p_Context == ItemContext::Ilvl935
        || p_Context == ItemContext::Ilvl940
        || p_Context == ItemContext::Ilvl945
        || p_Context == ItemContext::Ilvl950
        || p_Context == ItemContext::Ilvl955))             ///< Only in dungeon & raid & world quests
    {
        std::vector<uint32> l_StatsBonus =
        {
            ItemBonus::Stats::Avoidance,
            ItemBonus::Stats::Speed,
            ItemBonus::Stats::Leech
        };

        if (p_Context == ItemContext::RaidNormal ||
            p_Context == ItemContext::RaidMythic ||
            p_Context == ItemContext::RaidHeroic ||
            p_Context == ItemContext::RaidLfr)
            l_StatsBonus.push_back(ItemBonus::Stats::Indestructible);

        if (roll_chance_f(ItemBonus::Chances::Stats) && !p_OnlyDifficulty)
        {
            /// Could be a good thing to improve performance to declare one random generator somewhere and always use the same instead of declare it new one for each std::shuffle call
            /// Note for developers : std::random_shuffle is c based and will be removed soon (c++x14), so it's a good tips to always use std::shuffle instead
            std::random_device l_RandomDevice;
            std::mt19937 l_RandomGenerator(l_RandomDevice());
            std::shuffle(l_StatsBonus.begin(), l_StatsBonus.end(), l_RandomGenerator);

            p_ItemBonus.push_back(*l_StatsBonus.begin());
        }

        if (l_ItemTemplate->IsStuff() && l_ItemTemplate->Class != ItemClass::ITEM_CLASS_GEM && roll_chance_f(ItemBonus::Chances::PrismaticSocket))
            p_ItemBonus.push_back(ItemBonus::Stats::PrismaticSocket);
    }


    /// Step tree : Roll for Warforged & Prismatic Socket
    /// That roll happen only in heroic dungeons & raid
    /// Exaclty like stats, we don't know the chance to have that kind of bonus ...
    if (p_Player && (p_Context == ItemContext::DungeonHeroic ||
        p_Context == ItemContext::RaidNormal ||
        p_Context == ItemContext::RaidHeroic ||
        p_Context == ItemContext::RaidMythic ||
        p_Context == ItemContext::RaidLfr) && !p_OnlyDifficulty && !p_Player->m_IsInLootboxProcess
        && l_Exp == Expansion::EXPANSION_WARLORDS_OF_DRAENOR)
    {
        if (roll_chance_f(ItemBonus::Chances::Warforged))
            p_ItemBonus.push_back(ItemBonus::HeroicOrRaid::Warforged);
    }

    if (!l_ItemTemplate->IsStuff())
        return;

    GameObjectTemplate const* l_GobLootContainer = sObjectMgr->GetGameObjectTemplate(GUID_ENPART(p_Loot ? p_Loot->source : 0));

    int32 l_Ilvl = 845;
    switch (p_Context)
    {
        case ItemContext::WorldQuest2:
        {
            l_Ilvl = 810;
            break;
        }
        case ItemContext::WorldQuest3:
        {
            l_Ilvl = 815;
            break;
        }
        case ItemContext::WorldQuest4:
        {
            l_Ilvl = 820;
            break;
        }
        case ItemContext::WorldQuest5:
        {
            l_Ilvl = 825;
            break;
        }
        case ItemContext::WorldQuest6:
        {
            l_Ilvl = 830;
            break;
        }
        case ItemContext::ChallengeModeJackpot:
        {
            l_Ilvl = 835;
            break;
        }
        case ItemContext::WorldQuest7:
        {
            l_Ilvl = 840;
            break;
        }
        case ItemContext::PvPRanked4:
        {
            l_Ilvl = 845;
            break;
        }
        case ItemContext::Ilvl850:
        {
            l_Ilvl = 850;
            break;
        }
        case ItemContext::PvPRanked:
        {
            l_Ilvl = 855;
            break;
        }
        case ItemContext::Ilvl860:
        {
            l_Ilvl = 860;
            break;
        }
        case ItemContext::DungeonHeroic:
        case ItemContext::PvPRanked3:
        {
            l_Ilvl = 865;
            break;
        }
        case ItemContext::WorldQuest10:
        {
            l_Ilvl = 870;
            break;
        }
        case ItemContext::PvPRanked5:
        {
            l_Ilvl = 875;
            break;
        }
        case ItemContext::PvPRanked6:
        {
            l_Ilvl = 880;
            break;
        }
        case ItemContext::DungeonMythic:
        case ItemContext::Ilvl885:
        {
            l_Ilvl = 885;
            if (p_Player && p_Player->m_IsInLootboxProcess)
                l_Ilvl = p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlCounter) == 5 ? 950 : 945;
            break;
        }
        case ItemContext::Ilvl890:
        {
            l_Ilvl = 890;
            break;
        }
        case ItemContext::Ilvl895:
        {
            l_Ilvl = 895;
            break;
        }
        case ItemContext::Ilvl900:
        {
            l_Ilvl = 900;
            break;
        }
        case ItemContext::Ilvl905:
        {
            l_Ilvl = 905;
            break;
        }
        case ItemContext::Ilvl910:
        {
            l_Ilvl = 910;
            break;
        }
        case ItemContext::Ilvl915:
        {
            l_Ilvl = 915;
            break;
        }
        case ItemContext::Ilvl920:
        {
            l_Ilvl = 920;
            break;
        }
        case ItemContext::Ilvl925:
        {
            l_Ilvl = 925;
            break;
        }
        case ItemContext::Ilvl930:
        {
            l_Ilvl = 930;
            break;
        }
        case ItemContext::Ilvl935:
        {
            l_Ilvl = 935;
            break;
        }
        case ItemContext::Ilvl940:
        {
            l_Ilvl = 940;
            break;
        }
        case ItemContext::Ilvl945:
        {
            l_Ilvl = 945;
            break;
        }
        case ItemContext::Ilvl950:
        {
            l_Ilvl = 950;
            break;
        }
        case ItemContext::Ilvl955:
        {
            l_Ilvl = 955;
            break;
        }
        case ItemContext::RaidLfr:
        {
            if (l_Exp == Expansion::EXPANSION_LEGION)
                l_Ilvl = 835;

            break;
        }
        case ItemContext::RaidNormal:
        {
            if (l_Exp == Expansion::EXPANSION_LEGION)
                l_Ilvl = 850;

            if (p_Player && p_Player->m_IsInLootboxProcess)
            {
                std::vector<uint32> l_BonusesToRemove =
                {
                    ItemBonus::Bonuses::StartKilJaedenNormal,
                    ItemBonus::Bonuses::StartKilJaedenHeroic,
                    ItemBonus::Bonuses::StartKilJaedenMythic,
                    ItemBonus::Bonuses::StartKilJaedenLFR
                };

                for (uint32& l_Bonus : l_BonusesToRemove)
                {
                    auto l_Itr = std::find(p_ItemBonus.begin(), p_ItemBonus.end(), l_Bonus);
                    if (l_Itr != p_ItemBonus.end())
                        p_ItemBonus.erase(l_Itr);
                }

                if (p_Player->m_CurrentLootboxId == 300031) ///< Antorus lootbox
                {
                    l_Ilvl = p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlRaid03Counter) == 5 ? 955 : 950;
                    p_ItemBonus.push_back(ItemBonus::Bonuses::StartNormalRaid);
                }
                else if (p_Player->m_CurrentLootboxId == 300030) ///< ToS lootbox
                {
                    l_Ilvl = p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlRaid03Counter) == 5 ? 930 : 925;
                    p_ItemBonus.push_back(ItemBonus::Bonuses::StartNormalRaid);
                }
                else
                {
                    l_Ilvl = p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlCounter) == 5 ? 950 : 945;
                    p_ItemBonus.push_back(ItemBonus::Bonuses::StartNormalRaid);
                }
            }

            break;
        }
        case ItemContext::RaidHeroic:
        {
            if (l_Exp == Expansion::EXPANSION_LEGION)
                l_Ilvl = 865;

            if (p_Player && p_Player->m_IsInLootboxProcess)
            {
                p_ItemBonus.push_back(ItemBonus::Bonuses::StartHeroicRaid);
                l_Ilvl = p_Player->GetCharacterWorldStateValue(CharacterWorldStates::LootBoxBetterIlvlCounter) == 5 ? 950 : 945;
            }

            break;
        }
        case ItemContext::RaidMythic:
        {
            if (l_Exp == Expansion::EXPANSION_LEGION)
                l_Ilvl = 880;

            break;
        }
        case ItemContext::ChallengeMode:
        {
            if (l_GobLootContainer && (l_GobLootContainer->IsChallengeChest() || l_GobLootContainer->IsOploteChest()))
            {
                constexpr uint32 l_MythicsLevels = 14;
                std::array<uint32, l_MythicsLevels> l_IlevelPerMythicLvl =
                { {
                    890,    ///< Mythic+2
                    890,    ///< Mythic+3
                    895,    ///< Mythic+4
                    900,    ///< Mythic+5
                    905,    ///< Mythic+6
                    905,    ///< Mythic+7
                    910,    ///< Mythic+8
                    910,    ///< Mythic+9
                    915,    ///< Mythic+10
                    920,    ///< Mythic+11
                    925,    ///< Mythic+12
                    930,    ///< Mythic+13
                    935,    ///< Mythic+14
                    940,    ///< Mythic+15
                } };

                uint32 l_ChallengeLevel = 0;

                if (l_GobLootContainer->IsChallengeChest())
                {
                    if (Map* l_Map = p_Player ? p_Player->GetMap() : nullptr)
                    {
                        if (Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid()))
                        {
                            if (Challenge* l_Challenge = l_Scenario->GetChallenge())
                                l_ChallengeLevel = l_Challenge->GetChallengeLevel();
                        }
                    }
                }

                if (l_GobLootContainer->IsOploteChest())
                {
                    if (auto l_OploteLoot = sChallengeMgr->GetOploteLoot(p_Player->GetGUID()))
                        l_ChallengeLevel = l_OploteLoot->ChallengeLevel;
                }

                /// Apply right ilvl according to the mythic+ level (7.1.0 values)
                uint8 l_MythicIndex = std::min(std::max(0, int(l_ChallengeLevel + l_GobLootContainer->GetChallengeChestLevelBonus()) - 2), (int)l_MythicsLevels - 1);
                l_Ilvl = l_IlevelPerMythicLvl[l_MythicIndex];

                /// Apply the item bonus to display the right "mythic x" on the item tooltip
                if (l_MythicIndex != 0)
                {
                    auto l_Itr = std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartMythicPlusDungeon);
                    if (l_Itr != p_ItemBonus.end())
                        p_ItemBonus.erase(l_Itr);

                    uint32 l_Bonus = 0;

                    if (l_MythicIndex < 9)
                        l_Bonus = ItemBonus::Bonuses::StartMythicPlusDungeon + l_MythicIndex;
                    else
                    {
                        switch (l_MythicIndex)
                        {
                            case 9:      ///< Mythic +11
                                l_Bonus = 3509;
                                break;
                            case 10:     ///< Mythic +12
                                l_Bonus = 3510;
                                break;
                            case 11:     ///< Mythic +13
                                l_Bonus = 3534;
                                break;
                            case 12:     ///< Mythic +14
                                l_Bonus = 3535;
                                break;
                            case 13:     ///< Mythic +15
                                l_Bonus = 3536;
                                break;
                        }
                    }

                    if (l_Bonus != 0)
                        p_ItemBonus.push_back(l_Bonus);
                }
            }
            break;
        }
        case ItemContext::WorldQuest1:
        default:
        {
            l_Ilvl = 805;
            break;
        }
    }

    int32 l_MaxIlvl = ItemBonus::g_MaxIlvlRoll;

    if (p_Player && p_Player->m_IsInLootboxProcess)
    {
        /// Don't override ilvl from lootbox
    }
    /// Karazhan Mythic start at 865iLvl
    else if (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartLowerKarazhan) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartUpperKarazhan) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightbane) != p_ItemBonus.end())
        l_Ilvl = 865;
    /// Trial of Valor raid starts at 845 iLvl - Odyn & Guarm
    else if (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVOdynAndGuarmLFR) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVOdynAndGuarmNormal) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVOdynAndGuarmHeroic) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVOdynAndGuarmMythic) != p_ItemBonus.end())
    {
        switch (p_Context)
        {
            case ItemContext::RaidLfr:
                l_Ilvl = 845;
                break;
            case ItemContext::RaidNormal:
                l_Ilvl = 860;
                break;
            case ItemContext::RaidHeroic:
                l_Ilvl = 875;
                break;
            case ItemContext::RaidMythic:
                l_Ilvl = 890;
                break;
            default:
                break;
        }
    }
    /// Trial of Valor raid starts at 850 iLvl - Helya
    else if (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVHelyaLFR) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVHelyaNormal) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVHelyaHeroic) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVHelyaMythic) != p_ItemBonus.end())
    {
        switch (p_Context)
        {
            case ItemContext::RaidLfr:
                l_Ilvl = 850;
                break;
            case ItemContext::RaidNormal:
                l_Ilvl = 865;
                break;
            case ItemContext::RaidHeroic:
                l_Ilvl = 880;
                break;
            case ItemContext::RaidMythic:
                l_Ilvl = 895;
                break;
            default:
                break;
        }
    }
    /// The Nighthold starts at 855 iLvl - First wing
    else if (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdLFR1stWing) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdNormal1stWing) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdHeroic1stWing) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdMythic1stWing) != p_ItemBonus.end())
    {
        switch (p_Context)
        {
            case ItemContext::RaidLfr:
                l_Ilvl = 855;
                break;
            case ItemContext::RaidNormal:
                l_Ilvl = 870;
                break;
            case ItemContext::RaidHeroic:
                l_Ilvl = 885;
                break;
            case ItemContext::RaidMythic:
                l_Ilvl = 900;
                break;
            default:
                break;
        }
    }
    /// The Nighthold continues at 860 iLvl - 2nd and 3rd wings
    else if (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdLFR2ndWing) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdNormal2ndWing) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdHeroic2ndWing) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdMythic2ndWing) != p_ItemBonus.end())
    {
        switch (p_Context)
        {
            case ItemContext::RaidLfr:
                l_Ilvl = 860;
                break;
            case ItemContext::RaidNormal:
                l_Ilvl = 875;
                break;
            case ItemContext::RaidHeroic:
                l_Ilvl = 890;
                break;
            case ItemContext::RaidMythic:
                l_Ilvl = 905;
                break;
            default:
                break;
        }
    }
    /// The Nighthold finishes at 865 iLvl - Gul'dan
    else if (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdLFR4thWing) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdNormal4thWing) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdHeroic4thWing) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdMythic4thWing) != p_ItemBonus.end())
    {
        switch (p_Context)
        {
            case ItemContext::RaidLfr:
                l_Ilvl = 865;
                break;
            case ItemContext::RaidNormal:
                l_Ilvl = 880;
                break;
            case ItemContext::RaidHeroic:
                l_Ilvl = 895;
                break;
            case ItemContext::RaidMythic:
                l_Ilvl = 910;
                break;
            default:
                break;
        }
    }
    /// Tomb of Sargeras
    else if (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartTombOfSargerasNormal) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartTombOfSargerasHeroic) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartTombOfSargerasMythic) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartTombOfSargerasLFR) != p_ItemBonus.end())
    {
        bool l_KilJaeden = (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenNormal) != p_ItemBonus.end() ||
            std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenHeroic) != p_ItemBonus.end() ||
            std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenMythic) != p_ItemBonus.end() ||
            std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenLFR) != p_ItemBonus.end());

        p_ItemBonus.erase(std::remove(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenNormal), p_ItemBonus.end());
        p_ItemBonus.erase(std::remove(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenHeroic), p_ItemBonus.end());
        p_ItemBonus.erase(std::remove(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenMythic), p_ItemBonus.end());
        p_ItemBonus.erase(std::remove(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenLFR), p_ItemBonus.end());

        switch (p_Context)
        {
            case ItemContext::RaidLfr:
                l_Ilvl = l_KilJaeden ? 895 : 885;
                break;
            case ItemContext::RaidNormal:
                l_Ilvl = l_KilJaeden ? 910 : 900;
                break;
            case ItemContext::RaidHeroic:
                l_Ilvl = l_KilJaeden ? 925 : 915;
                break;
            case ItemContext::RaidMythic:
                l_Ilvl = l_KilJaeden ? 940 : 930;
                break;
            default:
                break;
        }
    }
    /// Antorus the Burning Throne
    else if (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartAntorusNormal) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartAntorusHeroic) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartAntorusMythic) != p_ItemBonus.end() ||
             std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartAntorusLFR) != p_ItemBonus.end())
    {
        bool l_Argus = (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenNormal) != p_ItemBonus.end() ||
            std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenHeroic) != p_ItemBonus.end() ||
            std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenMythic) != p_ItemBonus.end() ||
            std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenLFR) != p_ItemBonus.end());

        p_ItemBonus.erase(std::remove(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenNormal), p_ItemBonus.end());
        p_ItemBonus.erase(std::remove(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenHeroic), p_ItemBonus.end());
        p_ItemBonus.erase(std::remove(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenMythic), p_ItemBonus.end());
        p_ItemBonus.erase(std::remove(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartKilJaedenLFR), p_ItemBonus.end());

        switch (p_Context)
        {
            case ItemContext::RaidLfr:
                l_Ilvl = l_Argus ? 925 : 915;
                break;
            case ItemContext::RaidNormal:
                l_Ilvl = l_Argus ? 940 : 930;
                break;
            case ItemContext::RaidHeroic:
                l_Ilvl = l_Argus ? 955 : 945;
                break;
            case ItemContext::RaidMythic:
                l_Ilvl = l_Argus ? 970 : 960;
                break;
            default:
                break;
        }
    }
    /// Legion World Bosses start at 860 iLvl
    else if (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartLegionWorldBoss) != p_ItemBonus.end())
        l_Ilvl = 860;
    /// Legion 7.2 World Bosses start at 900 iLvl
    else if (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::Start720LegionWorldBoss) != p_ItemBonus.end())
        l_Ilvl = 900;
    /// Legion 7.3 World Bosses start at 930 iLvl
    else if (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::Start730LegionWorldBoss) != p_ItemBonus.end())
        p_ItemBonus.clear();

    /// Handle legion ilvl rolls
    if ((std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNormalDungeon) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartHeroicDungeon) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartMythicDungeon) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartLowerKarazhan) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartUpperKarazhan) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightbane) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVOdynAndGuarmLFR) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVOdynAndGuarmNormal) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVOdynAndGuarmHeroic) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVOdynAndGuarmMythic) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVHelyaLFR) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVHelyaNormal) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVHelyaHeroic) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartToVHelyaMythic) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdLFR1stWing) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdNormal1stWing) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdHeroic1stWing) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdMythic1stWing) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdLFR2ndWing) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdNormal2ndWing) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdHeroic2ndWing) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdMythic2ndWing) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdLFR4thWing) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdNormal4thWing) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdHeroic4thWing) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNightholdMythic4thWing) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartLfrRaid) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartNormalRaid) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartHeroicRaid) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartMythicRaid) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartLegionWorldBoss) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::Start720LegionWorldBoss) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartTombOfSargerasNormal) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartTombOfSargerasHeroic) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartTombOfSargerasMythic) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartTombOfSargerasLFR) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartAntorusNormal) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartAntorusHeroic) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartAntorusMythic) != p_ItemBonus.end()
        || std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartAntorusLFR) != p_ItemBonus.end()
        || p_Context == ItemContext::ChallengeMode
        || p_Context == ItemContext::WorldQuest1
        || p_Context == ItemContext::WorldQuest2
        || p_Context == ItemContext::WorldQuest3
        || p_Context == ItemContext::WorldQuest4
        || p_Context == ItemContext::WorldQuest5
        || p_Context == ItemContext::WorldQuest6
        || p_Context == ItemContext::ChallengeModeJackpot
        || p_Context == ItemContext::WorldQuest7
        || p_Context == ItemContext::PvPRanked4
        || p_Context == ItemContext::Ilvl850
        || p_Context == ItemContext::PvPRanked
        || p_Context == ItemContext::Ilvl860
        || p_Context == ItemContext::PvPRanked3
        || p_Context == ItemContext::WorldQuest10
        || p_Context == ItemContext::PvPRanked5
        || p_Context == ItemContext::PvPRanked6
        || p_Context == ItemContext::Ilvl885
        || p_Context == ItemContext::Ilvl890
        || p_Context == ItemContext::Ilvl895
        || p_Context == ItemContext::Ilvl900
        || p_Context == ItemContext::Ilvl905
        || p_Context == ItemContext::Ilvl910
        || p_Context == ItemContext::Ilvl915
        || p_Context == ItemContext::Ilvl920
        || p_Context == ItemContext::Ilvl925
        || p_Context == ItemContext::Ilvl930
        || p_Context == ItemContext::Ilvl935
        || p_Context == ItemContext::Ilvl940
        || p_Context == ItemContext::Ilvl945
        || p_Context == ItemContext::Ilvl950
        || p_Context == ItemContext::Ilvl955
        || (p_Player && p_Player->m_IsInLootboxProcess)))
    {
        uint8 l_RollSucces = 0;

        /// Weekly class hall chest
        if (l_GobLootContainer && l_GobLootContainer->IsOploteChest())
        {
            uint32 l_ChallengeLevel = 0;

            if (l_GobLootContainer->IsOploteChest())
            {
                if (auto l_OploteLoot = sChallengeMgr->GetOploteLoot(p_Player->GetGUID()))
                    l_ChallengeLevel = l_OploteLoot->ChallengeLevel;
            }

            switch (l_ChallengeLevel)
            {
                case 2:
                case 6:
                case 8:
                    l_Ilvl += 15;
                    break;
                default:
                    l_Ilvl += 20;
                    break;
            }

            l_Ilvl = std::min(l_Ilvl, 960);
        }

        auto baseChanceIlvl = sWorld->getFloatConfig(CONFIG_LOOT_WARFORGE_BASE_CHANCE);
        auto nextTryChance = sWorld->getFloatConfig(CONFIG_LOOT_WARFORGE_NEXT_TRY_CHANCE);
        auto IsFirstTry = true;

        if (l_GobLootContainer && l_GobLootContainer->IsOploteChest())
            baseChanceIlvl += 5.f;

        for (; l_Ilvl < 1000; l_Ilvl += 5)
        {
            auto l_Roll = roll_chance_i(IsFirstTry ? baseChanceIlvl : nextTryChance);
            if (l_Roll && IsFirstTry)
                IsFirstTry = false;

            /// Roll success!
            if (l_Roll)
                l_RollSucces++;

            /// Roll fail or max ilvl reach
            if (!l_Roll || l_Ilvl >= l_MaxIlvl)
            {
                int32 l_BaseIlvl         = l_ItemTemplate->GetBaseItemLevel();
                int32 l_IlvlModification = l_Ilvl - l_BaseIlvl;

                /// @TODO: Implement ItemBonusListLevelDelta.db2 instead this hackfix :D
                if (l_IlvlModification >= ItemBonus::g_ItemBonusIlvlNegativeLimit)
                {
                    if (l_IlvlModification <= ItemBonus::g_ItemBonusIlvlPositiveLimit)
                        p_ItemBonus.push_back((int32)ItemBonus::Bonuses::IlvlModificationBase + l_IlvlModification);
                    if (l_IlvlModification > 200 && l_IlvlModification <= 400)
                        p_ItemBonus.push_back((int32)ItemBonus::Bonuses::IlvlModificationBaseOver200 + (l_IlvlModification - 201));
                }

                /// +15 ilvl give titan forge mention
                if (l_RollSucces >= 3)
                    p_ItemBonus.push_back(ItemBonus::Bonuses::TitanForge);
                /// +10 ilvl give warforge mention
                else if (l_RollSucces)
                {
                    if (std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::StartLegionWorldBoss) != p_ItemBonus.end() ||
                        std::find(p_ItemBonus.begin(), p_ItemBonus.end(), ItemBonus::Bonuses::Start720LegionWorldBoss) != p_ItemBonus.end())
                        p_ItemBonus.push_back(ItemBonus::Bonuses::EpicWarforge);
                    else
                        p_ItemBonus.push_back(ItemBonus::Bonuses::Warforge);
                }

                break;
            }
        }
    }

    /// Handle zone scaling bonus (classic, outland, northrend, pandaria & dreanor leveling zones)
    if (p_ItemBonus.empty() && !l_PossibleBonusTreeNodes.empty() && p_Player)
    {
        bool l_Match = true;

        std::list<ItemBonusEntry const*> l_ItemBonuses;

        /// We try here to do the most correct condition to be sure we have a item that should scale with the zone level
        /// I didnt find any flags or way to do it better with the informations we have in db2 ...
        for (auto l_ItemBonusTreeNode : l_PossibleBonusTreeNodes)
        {
            if (l_ItemBonusTreeNode->ChildItemBonusTreeId != 0 || l_ItemBonusTreeNode->ChildItemLevelSelectorId != 0 || l_ItemBonusTreeNode->Category < 800 || l_ItemBonusTreeNode->ItemContext == 0)
            {
                l_Match = false;
                break;
            }

            auto l_Bonuses = GetItemBonusesByID(l_ItemBonusTreeNode->ChildItemBonusListId);
            if (!l_Bonuses)
            {
                l_Match = false;
                break;
            }

            uint32 l_ValidCount = 0;
            for (auto l_Bonus : *l_Bonuses)
            {
                if (l_Bonus)
                    l_ValidCount++;
            }

            if (l_ValidCount != 2)
            {
                l_Match = false;
                break;
            }

            for (auto l_Bonus : *l_Bonuses)
            {
                if (!l_Bonus)
                    continue;

                if (l_Bonus->Index == 0 && l_Bonus->Type != ItemBonusType::ITEM_BONUS_SCALING_STAT_DISTRIBUTION_2)
                {
                    l_Match = false;
                    break;
                }

                if (l_Bonus->Index == 1 && l_Bonus->Type != ItemBonusType::ITEM_BONUS_OVERRIDE_REQUIRED_LEVEL)
                {
                    l_Match = false;
                    break;
                }

                if (l_Bonus->Index == 1 && l_Bonus->Type == ItemBonusType::ITEM_BONUS_OVERRIDE_REQUIRED_LEVEL)
                    l_ItemBonuses.push_back(l_Bonus);
            }

            if (!l_Match)
                break;
        }

        if (l_Match)
        {
            l_ItemBonuses.sort([](ItemBonusEntry const* l_A, ItemBonusEntry const* l_B) -> bool
            {
                return l_A->Value[0] > l_B->Value[0];
            });

            uint32 l_FinalBonusID = 0;

            for (ItemBonusEntry const* l_Bonus : l_ItemBonuses)
            {
                if (l_Bonus->Value[0] <= p_Player->getLevel())
                {
                    l_FinalBonusID = l_Bonus->ParentItemBonusListId;
                    break;
                }
            }

            p_ItemBonus.push_back(l_FinalBonusID);
        }
    }
}

void Item::BuildDynamicItemDatas(WorldPacket& p_Datas, Item const* p_Item, ItemContext p_Context /*= ItemContext::None*/)
{
    if (p_Item == nullptr)
    {
        p_Datas << uint32(0);                       ///< Item ID
        p_Datas << uint32(0);                       ///< Random Properties Seed
        p_Datas << uint32(0);                       ///< Random Properties ID
        p_Datas.WriteBit(false);                    ///< Has Item Bonuses
        p_Datas.WriteBit(false);                    ///< Has Modifications
        p_Datas.FlushBits();
        return;
    }

    std::vector<uint32> l_Bonuses = p_Item->GetAllItemBonuses();
    std::vector<uint32> l_Modifications = p_Item->GetDynamicValues(EItemDynamicFields::ITEM_DYNAMIC_FIELD_MODIFIERS);

    p_Datas << uint32(p_Item->GetEntry());                  ///< Item ID
    p_Datas << uint32(p_Item->GetItemSuffixFactor());       ///< Random Properties Seed
    p_Datas << uint32(p_Item->GetItemRandomPropertyId());   ///< Random Properties ID
    p_Datas.WriteBit(l_Bonuses.size() != 0);                ///< Has Item Bonuses
    p_Datas.WriteBit(l_Modifications.size() != 0);          ///< Has Modifications
    p_Datas.FlushBits();

    /// Item bonuses
    if (l_Bonuses.size() != 0)
    {
        p_Datas << uint8(p_Context);                        ///< Context
        p_Datas << uint32(l_Bonuses.size());
        for (auto& l_BonusId : l_Bonuses)
            p_Datas << uint32(l_BonusId);
    }

    /// Item modifications
    if (l_Modifications.size() != 0)
    {
        p_Datas << uint32(p_Item->GetUInt32Value(ITEM_FIELD_MODIFIERS_MASK));

        for (auto l_Modifier : l_Modifications)
        {
            if (l_Modifier)
                p_Datas << uint32(l_Modifier);
        }
    }
}

void Item::BuildDynamicItemDatas(ByteBuffer& p_Datas, Item const* p_Item)
{
    if (p_Item == nullptr)
    {
        p_Datas << uint32(0);                       ///< Item ID
        p_Datas << uint32(0);                       ///< Random Properties Seed
        p_Datas << uint32(0);                       ///< Random Properties ID
        p_Datas.WriteBit(false);                    ///< Has Item Bonuses
        p_Datas.WriteBit(false);                    ///< Has Modifications
        p_Datas.FlushBits();
        return;
    }

    std::vector<uint32> l_Bonuses = p_Item->GetAllItemBonuses();
    std::vector<uint32> l_Modifications = p_Item->GetDynamicValues(EItemDynamicFields::ITEM_DYNAMIC_FIELD_MODIFIERS);

    p_Datas << uint32(p_Item->GetEntry());                  ///< Item ID
    p_Datas << uint32(p_Item->GetItemSuffixFactor());       ///< Random Properties Seed
    p_Datas << uint32(p_Item->GetItemRandomPropertyId());   ///< Random Properties ID
    p_Datas.WriteBit(l_Bonuses.size() != 0);                ///< Has Item Bonuses
    p_Datas.WriteBit(l_Modifications.size() != 0);          ///< Has Modifications
    p_Datas.FlushBits();

    /// Item bonuses
    if (l_Bonuses.size() != 0)
    {
        p_Datas << uint8(0);                                ///< Context
        p_Datas << uint32(l_Bonuses.size());
        for (auto& l_BonusId : l_Bonuses)
            p_Datas << uint32(l_BonusId);
    }

    /// Item modifications
    if (l_Modifications.size() != 0)
    {
        p_Datas << uint32(p_Item->GetUInt32Value(ITEM_FIELD_MODIFIERS_MASK));

        for (auto l_Modifier : l_Modifications)
        {
            if (l_Modifier)
                p_Datas << uint32(l_Modifier);
        }
    }
}

void Item::BuildDynamicItemDatas(WorldPacket& p_Datas, VoidStorageItem const p_Item)
{
    p_Datas << uint32(p_Item.ItemEntry);            ///< Item ID
    p_Datas << uint32(p_Item.ItemSuffixFactor);     ///< Random Properties Seed
    p_Datas << uint32(p_Item.ItemRandomPropertyId); ///< Random Properties ID
    p_Datas.WriteBit(p_Item.Bonuses.size() != 0);   ///< Has Item Bonuses
    p_Datas.WriteBit(false);                        ///< Has Modifications
    p_Datas.FlushBits();

    /// Item bonuses
    if (p_Item.Bonuses.size() != 0)
    {
        p_Datas << uint8(0);                        ///< Context
        p_Datas << uint32(p_Item.Bonuses.size());

        for (auto& l_BonusId : p_Item.Bonuses)
            p_Datas << uint32(l_BonusId);
    }
}

void Item::BuildDynamicItemDatas(ByteBuffer& p_Datas, LootItem const p_Item)
{
    std::vector<uint32> l_Bonuses = p_Item.itemBonuses;

    p_Datas << uint32(p_Item.itemid);               ///< Item ID
    p_Datas << uint32(p_Item.randomSuffix);         ///< Random Properties Seed
    p_Datas << uint32(p_Item.randomPropertyId);     ///< Random Properties ID
    p_Datas.WriteBit(l_Bonuses.size() != 0);        ///< Has Item Bonuses
    p_Datas.WriteBit(false);                        ///< Has Modifications
    p_Datas.FlushBits();

    /// Item bonuses
    if (l_Bonuses.size() != 0)
    {
        p_Datas << uint8(0);                        ///< Context
        p_Datas << uint32(l_Bonuses.size());
        for (auto& l_BonusId : l_Bonuses)
            p_Datas << uint32(l_BonusId);
    }
}

void Item::BuildDynamicItemDatas(ByteBuffer& p_Datas, uint32 p_Entry, std::vector<uint32> p_ItemBonuses)
{
    ItemTemplate const* l_Template = sObjectMgr->GetItemTemplate(p_Entry);
    if (l_Template == nullptr)
    {
        p_Datas << uint32(p_Entry);                 ///< Item ID
        p_Datas << uint32(0);                       ///< Random Properties Seed
        p_Datas << uint32(0);                       ///< Random Properties ID
        p_Datas.WriteBit(false);                    ///< Has Item Bonuses
        p_Datas.WriteBit(false);                    ///< Has Modifications
        p_Datas.FlushBits();
        return;
    }

    p_Datas << uint32(p_Entry);                     ///< Item ID
    p_Datas << uint32(l_Template->RandomProperty);  ///< Random Properties Seed
    p_Datas << uint32(l_Template->RandomSuffix);    ///< Random Properties ID

    p_Datas.WriteBit(p_ItemBonuses.size() != 0);    ///< Has Item Bonuses
    p_Datas.WriteBit(false);                        ///< Has Modifications
    p_Datas.FlushBits();

    /// Item bonuses
    if (p_ItemBonuses.size() != 0)
    {
        p_Datas << uint8(0);                        ///< Context
        p_Datas << uint32(p_ItemBonuses.size());
        for (auto& l_BonusId : p_ItemBonuses)
            p_Datas << uint32(l_BonusId);
    }
}

void Item::BuildDynamicItemDatas(WorldPacket& p_Datas, uint32 p_Entry, std::vector<uint32> p_ItemBonuses)
{
    ItemTemplate const* l_Template = sObjectMgr->GetItemTemplate(p_Entry);
    if (l_Template == nullptr)
    {
        p_Datas << uint32(p_Entry);                 ///< Item ID
        p_Datas << uint32(0);                       ///< Random Properties Seed
        p_Datas << uint32(0);                       ///< Random Properties ID
        p_Datas.WriteBit(false);                    ///< Has Item Bonuses
        p_Datas.WriteBit(false);                    ///< Has Modifications
        p_Datas.FlushBits();
        return;
    }

    p_Datas << uint32(p_Entry);                     ///< Item ID
    p_Datas << uint32(l_Template->RandomProperty);  ///< Random Properties Seed
    p_Datas << uint32(l_Template->RandomSuffix);    ///< Random Properties ID

    p_Datas.WriteBit(p_ItemBonuses.size() != 0);    ///< Has Item Bonuses
    p_Datas.WriteBit(false);                        ///< Has Modifications
    p_Datas.FlushBits();

    /// Item bonuses
    if (p_ItemBonuses.size() != 0)
    {
        p_Datas << uint8(0);                        ///< Context
        p_Datas << uint32(p_ItemBonuses.size());
        for (auto& l_BonusId : p_ItemBonuses)
            p_Datas << uint32(l_BonusId);
    }
}

int32 Item::GenerateItemRandomPropertyId(uint32 item_id)
{
    ItemTemplate const* itemProto = sObjectMgr->GetItemTemplate(item_id);

    if (!itemProto)
        return 0;

    // item must have one from this field values not null if it can have random enchantments
    if ((!itemProto->RandomProperty) && (!itemProto->RandomSuffix))
        return 0;

    // item can have not null only one from field values
    if ((itemProto->RandomProperty) && (itemProto->RandomSuffix))
    {
        sLog->outError(LOG_FILTER_SQL, "Item template %u have RandomProperty == %u and RandomSuffix == %u, but must have one from field =0", itemProto->ItemId, itemProto->RandomProperty, itemProto->RandomSuffix);
        return 0;
    }

    // RandomProperty case
    if (itemProto->RandomProperty)
    {
        uint32 randomPropId = GetItemEnchantMod(itemProto->RandomProperty, ENCHANTMENT_RANDOM_PROPERTY);
        ItemRandomPropertiesEntry const* random_id = sItemRandomPropertiesStore.LookupEntry(randomPropId);
        if (!random_id)
        {
            sLog->outError(LOG_FILTER_SQL, "Enchantment id #%u used but it doesn't have records in 'ItemRandomProperties.db2'", randomPropId);
            return 0;
        }

        return random_id->ID;
    }
    // RandomSuffix case
    else
    {
        uint32 randomPropId = GetItemEnchantMod(itemProto->RandomSuffix, ENCHANTMENT_RANDOM_SUFFIX);
        ItemRandomSuffixEntry const* random_id = sItemRandomSuffixStore.LookupEntry(randomPropId);
        if (!random_id)
        {
            sLog->outError(LOG_FILTER_SQL, "Enchantment id #%u used but it doesn't have records in sItemRandomSuffixStore.", randomPropId);
            return 0;
        }

        return -int32(random_id->ID);
    }
}

void Item::SetItemRandomProperties(int32 randomPropId, Player* p_Player /*=nullptr*/)
{
    if (!randomPropId)
        return;

    if (randomPropId > 0)
    {
        ItemRandomPropertiesEntry const* item_rand = sItemRandomPropertiesStore.LookupEntry(randomPropId);
        if (item_rand)
        {
            if (GetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID) != int32(item_rand->ID))
            {
                SetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID, item_rand->ID);
                SetState(ITEM_CHANGED, p_Player ? p_Player : GetOwner());
            }
            for (uint32 i = PROP_ENCHANTMENT_SLOT_1; i < PROP_ENCHANTMENT_SLOT_1 + 3; ++i)
                SetEnchantment(EnchantmentSlot(i), item_rand->enchant_id[i - PROP_ENCHANTMENT_SLOT_1], 0, 0);
        }
    }
    else
    {
        ItemRandomSuffixEntry const* item_rand = sItemRandomSuffixStore.LookupEntry(-randomPropId);
        if (item_rand)
        {
            if (GetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID) != -int32(item_rand->ID) ||
                !GetItemSuffixFactor())
            {
                SetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID, -int32(item_rand->ID));
                UpdateItemSuffixFactor();
                SetState(ITEM_CHANGED, p_Player ? p_Player : GetOwner());
            }

            for (uint32 i = PROP_ENCHANTMENT_SLOT_0; i <= PROP_ENCHANTMENT_SLOT_4; ++i)
                SetEnchantment(EnchantmentSlot(i), item_rand->Enchantment[i - PROP_ENCHANTMENT_SLOT_0], 0, 0);
        }
    }
}

void Item::UpdateItemSuffixFactor()
{
    uint32 suffixFactor = GenerateEnchSuffixFactor(GetEntry());
    if (GetItemSuffixFactor() == suffixFactor)
        return;
    SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, suffixFactor);
}

void Item::SetState(ItemUpdateState state, Player* forplayer)
{
#ifdef CROSS
    if (state == ITEM_REMOVED && GetEntry() == 138019)
    {
        ACE_Stack_Trace l_Trace;
        sLog->outAshran("Set state ITEM_REMOVED (%u, %u)", GetEntry(), GetOwnerGUID());
        sLog->outAshran(l_Trace.c_str());
    }
#endif

    if (uState == ITEM_NEW && state == ITEM_REMOVED)
    {
        // pretend the item never existed
        RemoveFromUpdateQueueOf(forplayer);
        forplayer->DeleteRefundReference(GetGUIDLow());
        delete this;
        return;
    }
    if (state != ITEM_UNCHANGED)
    {
        // new items must stay in new state until saved
        if (uState != ITEM_NEW)
            uState = state;

        if (forplayer)
            AddToUpdateQueueOf(forplayer);
    }
    else
    {
        // unset in queue
        // the item must be removed from the queue manually
        uQueuePos = -1;
        uState = ITEM_UNCHANGED;
    }
}

void Item::AddToUpdateQueueOf(Player* player)
{
    if (IsInUpdateQueue())
        return;

    ASSERT(player != NULL);

    if (player->GetGUID() != GetOwnerGUID())
    {
        sLog->outDebug(LOG_FILTER_PLAYER_ITEMS, "Item::AddToUpdateQueueOf - Owner's guid (%u) and player's guid (%u) don't match!", GUID_LOPART(GetOwnerGUID()), player->GetGUIDLow());
        return;
    }

    if (player->m_itemUpdateQueueBlocked)
        return;

    player->m_itemUpdateQueue.push_back(this);
    uQueuePos = player->m_itemUpdateQueue.size()-1;
}

void Item::RemoveFromUpdateQueueOf(Player* player)
{
    if (!IsInUpdateQueue())
        return;

    //ASSERT(player != NULL)

    if (player && player->GetGUID() != GetOwnerGUID())
    {
        sLog->outDebug(LOG_FILTER_PLAYER_ITEMS, "Item::RemoveFromUpdateQueueOf - Owner's guid (%u) and player's guid (%u) don't match!", GUID_LOPART(GetOwnerGUID()), player->GetGUIDLow());
        return;
    }

    if (player && player->m_itemUpdateQueueBlocked)
        return;

    if (player)
        player->m_itemUpdateQueue[uQueuePos] = NULL;

    uQueuePos = -1;
}

uint8 Item::GetBagSlot() const
{
    return m_container ? m_container->GetSlot() : uint8(INVENTORY_SLOT_BAG_0);
}

bool Item::IsEquipped() const
{
    return !IsInBag() && m_slot < EQUIPMENT_SLOT_END;
}

bool Item::CanBeTraded(bool mail, bool trade) const
{
    if (m_lootGenerated)
        return false;

    if ((!mail || !IsBoundAccountWide()) && (IsSoulBound() && (!HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FIELD_FLAG_BOP_TRADEABLE) || !trade)))
        return false;

    if (IsBag() && (Player::IsBagPos(GetPos()) || !((Bag const*)this)->IsEmpty()))
        return false;

    if (Player* owner = GetOwner())
    {
        if (owner->CanUnequipItem(GetPos(), false) != EQUIP_ERR_OK)
            return false;
        if (owner->GetLootGUID() == GetGUID())
            return false;
    }

    if (IsBoundByEnchant())
        return false;

    return true;
}

bool Item::HasEnchantRequiredSkill(const Player* player) const
{
    // Check all enchants for required skill
    for (uint32 enchant_slot = PERM_ENCHANTMENT_SLOT; enchant_slot < MAX_ENCHANTMENT_SLOT; ++enchant_slot)
    {
        if (enchant_slot > ENGINEERING_ENCHANTMENT_SLOT && enchant_slot < PROP_ENCHANTMENT_SLOT_0)    // not holding enchantment id
            continue;

        if (uint32 enchant_id = GetEnchantmentId(EnchantmentSlot(enchant_slot)))
            if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id))
                if (enchantEntry->requiredSkill && player->GetSkillValue(enchantEntry->requiredSkill) < enchantEntry->requiredSkillValue)
                    return false;
    }

  return true;
}

uint32 Item::GetEnchantRequiredLevel() const
{
    uint32 level = 0;

    // Check all enchants for required level
    for (uint32 enchant_slot = PERM_ENCHANTMENT_SLOT; enchant_slot < MAX_ENCHANTMENT_SLOT; ++enchant_slot)
    {
        if (enchant_slot > ENGINEERING_ENCHANTMENT_SLOT && enchant_slot < PROP_ENCHANTMENT_SLOT_0)    // not holding enchantment id
            continue;

        if (uint32 enchant_id = GetEnchantmentId(EnchantmentSlot(enchant_slot)))
            if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id))
                if (enchantEntry->requiredLevel > level)
                    level = enchantEntry->requiredLevel;
    }

    return level;
}

bool Item::IsBoundByEnchant() const
{
    // Check all enchants for soulbound
    for (uint32 enchant_slot = PERM_ENCHANTMENT_SLOT; enchant_slot < MAX_ENCHANTMENT_SLOT; ++enchant_slot)
    {
        if (enchant_slot > ENGINEERING_ENCHANTMENT_SLOT && enchant_slot < PROP_ENCHANTMENT_SLOT_0)    // not holding enchantment id
            continue;

        if (uint32 enchant_id = GetEnchantmentId(EnchantmentSlot(enchant_slot)))
            if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id))
                if (enchantEntry->Flags & ENCHANTMENT_CAN_SOULBOUND)
                    return true;
    }

    return false;
}

InventoryResult Item::CanBeMergedPartlyWith(ItemTemplate const* proto) const
{
    // not allow merge looting currently items
    if (m_lootGenerated)
        return EQUIP_ERR_LOOT_GONE;

    // check item type
    if (GetEntry() != proto->ItemId)
        return EQUIP_ERR_CANT_STACK;

    // check free space (full stacks can't be target of merge
    if (GetCount() >= proto->GetMaxStackSize())
        return EQUIP_ERR_CANT_STACK;

    return EQUIP_ERR_OK;
}

bool Item::IsFitToSpellRequirements(SpellInfo const* spellInfo) const
{
    ItemTemplate const* proto = GetTemplate();

    if (spellInfo->EquippedItemClass != -1)                 // -1 == any item class
    {
        // Special case - accept vellum for armor/weapon requirements
        if ((spellInfo->EquippedItemClass == ITEM_CLASS_ARMOR ||
            spellInfo->EquippedItemClass == ITEM_CLASS_WEAPON) && proto->IsVellum())
            if (spellInfo->IsAbilityOfSkillType(SKILL_ENCHANTING)) // only for enchanting spells
                return true;

        if (spellInfo->EquippedItemClass != int32(proto->Class))
            return false;                                   //  wrong item class

        if (spellInfo->EquippedItemSubClassMask != 0)        // 0 == any subclass
        {
            if ((spellInfo->EquippedItemSubClassMask & (1 << proto->SubClass)) == 0)
                return false;                               // subclass not present in mask
        }
    }

    if (spellInfo->EquippedItemInventoryTypeMask != 0)       // 0 == any inventory type
    {
        // Special case - accept weapon type for main and offhand requirements
        if ((proto->InventoryType == INVTYPE_WEAPON || proto->InventoryType == INVTYPE_2HWEAPON) &&
            (spellInfo->EquippedItemInventoryTypeMask & (1 << INVTYPE_WEAPONMAINHAND) ||
             spellInfo->EquippedItemInventoryTypeMask & (1 << INVTYPE_WEAPONOFFHAND)))
            return true;
        else if ((spellInfo->EquippedItemInventoryTypeMask & (1 << proto->InventoryType)) == 0)
            return false;                                   // inventory type not present in mask
    }

    return true;
}

void Item::SetEnchantment(EnchantmentSlot slot, uint32 id, uint32 duration, uint32 charges)
{
    // Better lost small time at check in comparison lost time at item save to DB.
    if ((GetEnchantmentId(slot) == id) && (GetEnchantmentDuration(slot) == duration) && (GetEnchantmentCharges(slot) == charges))
        return;

    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_ID_OFFSET, id);
    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_DURATION_OFFSET, duration);
    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_CHARGES_OFFSET, charges);
    SetState(ITEM_CHANGED, GetOwner());
}

void Item::SetEnchantmentDuration(EnchantmentSlot slot, uint32 duration, Player* owner)
{
    if (GetEnchantmentDuration(slot) == duration)
        return;

    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_DURATION_OFFSET, duration);
    SetState(ITEM_CHANGED, owner);
    // Cannot use GetOwner() here, has to be passed as an argument to avoid freeze due to hashtable locking
}

void Item::SetEnchantmentCharges(EnchantmentSlot slot, uint32 charges)
{
    if (GetEnchantmentCharges(slot) == charges)
        return;

    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_CHARGES_OFFSET, charges);
    SetState(ITEM_CHANGED, GetOwner());
}

void Item::ClearEnchantment(EnchantmentSlot slot)
{
    if (!GetEnchantmentId(slot))
        return;

    for (uint8 x = 0; x < MAX_ENCHANTMENT_SPELLS; ++x)
        SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + x, 0);
    SetState(ITEM_CHANGED, GetOwner());
}

DynamicFieldStructuredView<ItemDynamicFieldGems> Item::GetGems() const
{
    return GetDynamicStructuredValues<ItemDynamicFieldGems>(ITEM_DYNAMIC_FIELD_GEMS);
}

ItemDynamicFieldGems const* Item::GetGem(uint16 slot) const
{
    ASSERT(slot < MAX_GEM_SOCKETS);
    return GetDynamicStructuredValue<ItemDynamicFieldGems>(ITEM_DYNAMIC_FIELD_GEMS, slot);
}

void Item::SetGem(uint16 slot, ItemDynamicFieldGems const* gem, uint32 gemScalingLevel)
{
    ASSERT(slot < MAX_GEM_SOCKETS);
    m_gemScalingLevels[slot] = gemScalingLevel;
    _bonusData.GemItemLevelBonus[slot] = 0;
    if (ItemTemplate const* gemTemplate = sObjectMgr->GetItemTemplate(gem->ItemId))
    {
        if (GemPropertiesEntry const* gemProperties = sGemPropertiesStore.LookupEntry(gemTemplate->GetGemProperties()))
        {
            if (SpellItemEnchantmentEntry const* gemEnchant = sSpellItemEnchantmentStore.LookupEntry(gemProperties->EnchantID))
            {
                BonusData gemBonus;
                gemBonus.Initialize(gemTemplate);
                for (uint16 bonusListId : gem->BonusListIDs)
                    if (auto bonuses = GetItemBonusesByID(bonusListId))
                        for (ItemBonusEntry const* itemBonus : *bonuses)
                            if (itemBonus)
                                gemBonus.AddBonus(itemBonus->Type, itemBonus->Value);

                uint32 gemBaseItemLevel = gemTemplate->GetBaseItemLevel();
                if (ScalingStatDistributionEntry const* ssd = sScalingStatDistributionStore.LookupEntry(gemBonus.ScalingStatDistribution))
                    if (uint32 scaledIlvl = uint32(GetCurveValueAt(ssd->CurveProperties, gemScalingLevel)))
                        gemBaseItemLevel = scaledIlvl;

                for (uint32 i = 0; i < MAX_ITEM_ENCHANTMENT_EFFECTS; ++i)
                {
                    switch (gemEnchant->type[i])
                    {
                    case ITEM_ENCHANTMENT_TYPE_BONUS_LIST_ID:
                        if (auto bonuses = GetItemBonusesByID(gemEnchant->spellid[i]))
                            for (ItemBonusEntry const* itemBonus : *bonuses)
                                if (itemBonus && itemBonus->Type == ITEM_BONUS_ITEM_LEVEL)
                                    _bonusData.GemItemLevelBonus[slot] += itemBonus->Value[0];
                        break;
                    case ITEM_ENCHANTMENT_TYPE_BONUS_LIST_CURVE:
                        if (uint32 bonusListId = GetItemBonusListForItemLevelDelta(int16(GetCurveValueAt(CURVE_ID_ARTIFACT_RELIC_ITEM_LEVEL_BONUS, gemBaseItemLevel + gemBonus.ItemLevelBonus))))
                            if (auto bonuses = GetItemBonusesByID(bonusListId))
                                for (ItemBonusEntry const* itemBonus : *bonuses)
                                    if (itemBonus && itemBonus->Type == ITEM_BONUS_ITEM_LEVEL)
                                        _bonusData.GemItemLevelBonus[slot] += itemBonus->Value[0];
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }

    SetDynamicStructuredValue(ITEM_DYNAMIC_FIELD_GEMS, slot, gem);
}

bool Item::GemsFitSockets() const
{
    uint32 gemSlot = 0;
    for (ItemDynamicFieldGems const& gemData : GetGems())
    {
        uint8 SocketColor = GetTemplate()->GetSocketColor(gemSlot);
        if (!SocketColor) // no socket slot
            continue;

        uint32 GemColor = 0;

        if (ItemTemplate const* gemProto = sObjectMgr->GetItemTemplate(gemData.ItemId))
            if (GemPropertiesEntry const* gemProperty = sGemPropertiesStore.LookupEntry(gemProto->GetGemProperties()))
                GemColor = gemProperty->Type;

        if (!(GemColor & SocketColorToGemTypeMask[SocketColor])) // bad gem color on this socket
            return false;
    }
    return true;
}

uint8 Item::GetGemCountWithID(uint32 GemID) const
{
    return std::count_if(GetGems().begin(), GetGems().end(), [GemID](ItemDynamicFieldGems const& gemData)
    {
        return gemData.ItemId == GemID;
    });
}

uint8 Item::GetGemCountWithLimitCategory(uint32 limitCategory) const
{
    return std::count_if(GetGems().begin(), GetGems().end(), [limitCategory](ItemDynamicFieldGems const& gemData)
    {
        ItemTemplate const* gemProto = sObjectMgr->GetItemTemplate(gemData.ItemId);
        if (!gemProto)
            return false;

        return gemProto->ItemLimitCategory == limitCategory;
    });
}

bool Item::IsLimitedToAnotherMapOrZone(uint32 cur_mapId, uint32 cur_zoneId) const
{
    ItemTemplate const* proto = GetTemplate();
    return proto && ((proto->Map && proto->Map != cur_mapId) || (proto->Area && proto->Area != cur_zoneId));
}

void Item::SendTimeUpdate(Player* p_Owner)
{
    uint32 l_Duration = GetUInt32Value(ITEM_FIELD_EXPIRATION);
    if (!l_Duration)
        return;

    WorldPacket l_Data(SMSG_ITEM_TIME_UPDATE, (8+4));
    l_Data.appendPackGUID(GetGUID());
    l_Data << l_Duration;
    p_Owner->GetSession()->SendPacket(&l_Data);
}

Item* Item::CreateItem(uint32 item, uint32 count, Player const* player)
{
    if (count < 1)
        return NULL;                                        //don't create item at zero count

    ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(item);
    if (pProto)
    {
        if (count > pProto->GetMaxStackSize())
            count = pProto->GetMaxStackSize();

        ASSERT(count != 0 && "pProto->Stackable == 0 but checked at loading already");

        Item* pItem = NewItemOrBag(pProto);
        if (pItem->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_ITEM), item, player))
        {
            pItem->SetCount(count);
            if (!pProto->NeedScaleOnArtifactKnowledge && pProto->Quality != ItemQualities::ITEM_QUALITY_HEIRLOOM)
                pItem->SetModifier(ItemModifiers::ITEM_MODIFIER_SCALING_STAT_DISTRIBUTION_FIXED_LEVEL, player ? player->getLevel() : 0);
            if (player)
                sScriptMgr->OnItemCreate(const_cast<Player*>(player), pProto, pItem);

            if (pItem->GetQuality() >= ITEM_QUALITY_RARE && player)
                pItem->SaveToLogs(player, item, std::time(nullptr), pItem->GetGUIDLow());

            return pItem;
        }
        else
            delete pItem;
    }
    else
        ASSERT(false);
    return NULL;
}

Item* Item::CloneItem(uint32 p_Count, Player const* p_Player) const
{
    Item* l_NewItem = CreateItem(GetEntry(), p_Count, p_Player);
    if (!l_NewItem)
        return NULL;

    l_NewItem->SetGuidValue(ITEM_FIELD_CREATOR,           GetGuidValue(ITEM_FIELD_CREATOR));
    l_NewItem->SetGuidValue(ITEM_FIELD_GIFT_CREATOR,      GetGuidValue(ITEM_FIELD_GIFT_CREATOR));
    l_NewItem->SetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS,   GetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS) & ~(ITEM_FIELD_FLAG_REFUNDABLE | ITEM_FIELD_FLAG_BOP_TRADEABLE));
    l_NewItem->SetUInt32Value(ITEM_FIELD_EXPIRATION,      GetUInt32Value(ITEM_FIELD_EXPIRATION));

    // player CAN be NULL in which case we must not update random properties because that accesses player's item update queue
    if (p_Player)
        l_NewItem->SetItemRandomProperties(GetItemRandomPropertyId(), const_cast<Player*>(p_Player));

    l_NewItem->AddItemBonuses(GetAllItemBonuses());
    l_NewItem->SetCustomFlags(GetCustomFlags());

    return l_NewItem;
}

bool Item::IsBindedNotWith(Player const* player) const
{
    // not binded item
    if (!IsSoulBound())
        return false;

    // own item
    if (GetOwnerGUID() == player->GetGUID())
        return false;

    if (HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FIELD_FLAG_BOP_TRADEABLE))
        if (allowedGUIDs.find(player->GetGUIDLow()) != allowedGUIDs.end())
            return false;

    // BOA item case
    if (IsBoundAccountWide())
        return false;

    return true;
}

void Item::BuildUpdate(UpdateDataMapType& data_map)
{
    if (Player* owner = GetOwner())
        BuildFieldsUpdate(owner, data_map);
    ClearUpdateMask(false);
}

void Item::BuildDynamicValuesUpdate(uint8 p_UpdateType, ByteBuffer* p_Data, Player* p_Target) const
{
    if (p_Target == nullptr)
        return;

    ByteBuffer l_FieldBuffer;
    UpdateMask l_UpdateMask;

    l_UpdateMask.SetCount(_dynamicValuesCount);

    uint32* l_Flags = nullptr;
    uint32 l_VisibleFlags = GetDynamicUpdateFieldData(p_Target, l_Flags);

    for (uint16 l_I = 0; l_I < _dynamicValuesCount; ++l_I)
    {
        ByteBuffer l_Buffer;
        std::vector<uint32> const& l_Values = _dynamicValues[l_I];

        if (_fieldNotifyFlags & l_Flags[l_I] ||
            ((p_UpdateType == OBJECT_UPDATE_TYPE::UPDATETYPE_VALUES ? _dynamicChangesMask.GetBit(l_I) : !l_Values.empty()) && (l_Flags[l_I] & l_VisibleFlags)) ||
            (l_I == EItemDynamicFields::ITEM_DYNAMIC_FIELD_MODIFIERS &&
            (p_UpdateType == OBJECT_UPDATE_TYPE::UPDATETYPE_VALUES ? _changesMask.GetBit(EItemFields::ITEM_FIELD_MODIFIERS_MASK) : GetUInt32Value(ITEM_FIELD_MODIFIERS_MASK) != 0)))
        {
            l_UpdateMask.SetBit(l_I);

            UpdateMask l_ArrayMask;
            if (l_I != EItemDynamicFields::ITEM_DYNAMIC_FIELD_MODIFIERS)
            {
                l_ArrayMask.SetCount(l_Values.size());

                for (std::size_t l_J = 0; l_J < l_Values.size(); ++l_J)
                {
                    if (p_UpdateType != OBJECT_UPDATE_TYPE::UPDATETYPE_VALUES || _dynamicChangesArrayMask[l_I].GetBit(l_J))
                    {
                        l_ArrayMask.SetBit(l_J);
                        l_Buffer << uint32(l_Values[l_J]);
                    }
                }
            }
            else
            {
                uint32 l_Count = 0;
                l_ArrayMask.SetCount(ItemModifiers::MAX_ITEM_MODIFIERS);

                for (uint32 l_J = 0; l_J < ItemModifiers::MAX_ITEM_MODIFIERS; ++l_J)
                {
                    if (uint32 l_Modifier = m_Modifiers[l_J])
                    {
                        l_ArrayMask.SetBit(l_Count++);
                        l_Buffer << uint32(l_Modifier);
                    }
                }
            }

            l_FieldBuffer << uint16(l_ArrayMask.GetBlockCount());
            l_ArrayMask.AppendToPacket(&l_FieldBuffer);
            l_FieldBuffer.append(l_Buffer);
        }
    }

    *p_Data << uint8(l_UpdateMask.GetBlockCount());
    l_UpdateMask.AppendToPacket(p_Data);
    p_Data->append(l_FieldBuffer);
}

void Item::SaveRefundDataToDB()
{
#ifdef CROSS
    Player* l_Owner = GetOwner();
    InterRealmDatabasePool* l_Database = l_Owner ? l_Owner->GetRealmDatabase() : nullptr;
#else
    auto l_Database = &CharacterDatabase;
#endif

    if (l_Database == nullptr)
        return;

    SQLTransaction trans = l_Database->BeginTransaction();

    PreparedStatement* stmt = l_Database->GetPreparedStatement(CHAR_DEL_ITEM_REFUND_INSTANCE);
    stmt->setUInt32(0, GetRealGUIDLow());
    trans->Append(stmt);

    stmt = l_Database->GetPreparedStatement(CHAR_INS_ITEM_REFUND_INSTANCE);
    stmt->setUInt32(0, GetRealGUIDLow());
    stmt->setUInt32(1, GetRefundRecipient());
    stmt->setUInt64(2, GetPaidMoney());
    stmt->setUInt16(3, uint16(GetPaidExtendedCost()));
    trans->Append(stmt);

    l_Database->CommitTransaction(trans);
}

void Item::DeleteRefundDataFromDB(SQLTransaction* trans)
{
    if (trans && trans->get() != nullptr)
    {
#ifdef CROSS
        Player* l_Owner = GetOwner();
        InterRealmDatabasePool* l_Database = l_Owner ? l_Owner->GetRealmDatabase() : nullptr;
#else
        auto l_Database = &CharacterDatabase;
#endif

        if (l_Database)
        {
            PreparedStatement* stmt = l_Database->GetPreparedStatement(CHAR_DEL_ITEM_REFUND_INSTANCE);
            stmt->setUInt32(0, GetRealGUIDLow());
            (*trans)->Append(stmt);
        }
    }
}

void Item::SetNotRefundable(Player* owner, bool changestate /*=true*/, SQLTransaction* trans /*=NULL*/, bool addToCollection /*= true*/)
{
    if (!HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FIELD_FLAG_REFUNDABLE))
        return;

    RemoveFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FIELD_FLAG_REFUNDABLE);
    // Following is not applicable in the trading procedure
    if (changestate)
        SetState(ITEM_CHANGED, owner);

    SetRefundRecipient(0);
    SetPaidMoney(0);
    SetPaidExtendedCost(0);
    DeleteRefundDataFromDB(trans);

    owner->DeleteRefundReference(GetGUIDLow());

    if (addToCollection)
        owner->GetSession()->GetCollectionMgr()->AddItemAppearance(this);
}

void Item::UpdatePlayedTime(Player* owner)
{
    /*  Here we update our played time
        We simply add a number to the current played time,
        based on the time elapsed since the last update hereof.
    */
    // Get current played time
    uint32 current_playtime = GetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME);
    // Calculate time elapsed since last played time update
    time_t curtime = time(NULL);
    uint32 elapsed = uint32(curtime - m_lastPlayedTimeUpdate);
    uint32 new_playtime = current_playtime + elapsed;
    // Check if the refund timer has expired yet
    if (new_playtime <= 2*HOUR)
    {
        // No? Proceed.
        // Update the data field
        SetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME, new_playtime);
        // Flag as changed to get saved to DB
        SetState(ITEM_CHANGED, owner);
        // Speaks for itself
        m_lastPlayedTimeUpdate = curtime;
        return;
    }
    // Yes
    SetNotRefundable(owner);
}

uint32 Item::GetPlayedTime()
{
    time_t curtime = time(NULL);
    uint32 elapsed = uint32(curtime - m_lastPlayedTimeUpdate);
    return GetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME) + elapsed;
}

bool Item::IsRefundExpired()
{
    return (GetPlayedTime() > 2*HOUR);
}

void Item::SetSoulboundTradeable(AllowedLooterSet& allowedLooters)
{
    SetFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FIELD_FLAG_BOP_TRADEABLE);
    allowedGUIDs = allowedLooters;
}

void Item::ClearSoulboundTradeable(Player* currentOwner)
{
    RemoveFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FIELD_FLAG_BOP_TRADEABLE);
    if (allowedGUIDs.empty())
        return;

    currentOwner->GetSession()->GetCollectionMgr()->AddItemAppearance(this);
    allowedGUIDs.clear();
    SetState(ITEM_CHANGED, currentOwner);

#ifdef CROSS
    Player* l_Owner = GetOwner();
    InterRealmDatabasePool* l_Database = l_Owner ? l_Owner->GetRealmDatabase() : nullptr;
    if (l_Database == nullptr)
        return;
#else
    auto l_Database = &CharacterDatabase;
#endif


    PreparedStatement* stmt = l_Database->GetPreparedStatement(CHAR_DEL_ITEM_BOP_TRADE);
    stmt->setUInt32(0, GetRealGUIDLow());
    l_Database->Execute(stmt);
}

bool Item::CheckSoulboundTradeExpire()
{
    // called from owner's update - GetOwner() MUST be valid
    if (GetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME) + 2*HOUR < GetOwner()->GetTotalPlayedTime())
    {
        ClearSoulboundTradeable(GetOwner());
        return true; // remove from tradeable list
    }

    return false;
}

bool Item::SubclassesCompatible(ItemTemplate const* p_Transmogrifier, ItemTemplate const* p_Transmogrified)
{
    ///   Source     Destination
    if (!p_Transmogrifier || !p_Transmogrified)
        return false;

    /// Patch 5.2 - Throne of Thunder
    /// One-Handed
    /// One-handed axes, maces, and swords can be Transmogrified to each other.
    if ((p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_AXE ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_MACE ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_SWORD||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_WARGLAIVES) &&
        (p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_AXE ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_MACE ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_SWORD ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_WARGLAIVES))
        return true;

    /// Two-Handed
    /// Two-handed axes, maces, and swords can be Transmogrified to each other.
    /// WoD Patch 6.0.2 (2014-10-14): Polearms and Staves can now be used to transmogrify Two-Handed Axes, Maces and Swords, and vice versa.
    if ((p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_AXE2 ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_MACE2 ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_STAFF ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_SWORD2) &&
        (p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_AXE2 ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_MACE2 ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_STAFF ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_SWORD2))
        return true;

    /// Ranged
    if ((p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_BOW ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_GUN ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_CROSSBOW) &&
        (p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_BOW ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_GUN ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_CROSSBOW))
        return true;

    if ((p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_SWORD||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_SWORD2) &&
        (p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_SWORD ||
         p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_SWORD2))
        return true;

    return false;
}

bool Item::CanTransmogrifyItemWithItem(ItemTemplate const* p_Transmogrifier, ItemTemplate const* p_Transmogrified, bool p_CustomTransmo /*= false*/)
{
    if (!p_Transmogrified || !p_Transmogrifier)
        return false;

    if (!p_Transmogrifier->CanTransmogrify(p_CustomTransmo) || !p_Transmogrified->CanBeTransmogrified(p_CustomTransmo))
        return false;

    /// Allowed transmogrified type
    if (p_Transmogrified->InventoryType == INVTYPE_BAG ||
        p_Transmogrified->InventoryType == INVTYPE_RELIC ||
        p_Transmogrified->InventoryType == INVTYPE_FINGER ||
        p_Transmogrified->InventoryType == INVTYPE_TRINKET ||
        p_Transmogrified->InventoryType == INVTYPE_AMMO ||
        p_Transmogrified->InventoryType == INVTYPE_QUIVER ||
        p_Transmogrified->InventoryType == INVTYPE_NON_EQUIP )
        return false;

    /// Allowed transmogrifier type
    if (p_Transmogrifier->InventoryType == INVTYPE_BAG ||
        p_Transmogrifier->InventoryType == INVTYPE_RELIC ||
        p_Transmogrifier->InventoryType == INVTYPE_FINGER ||
        p_Transmogrifier->InventoryType == INVTYPE_TRINKET ||
        p_Transmogrifier->InventoryType == INVTYPE_AMMO ||
        p_Transmogrifier->InventoryType == INVTYPE_QUIVER ||
        p_Transmogrifier->InventoryType == INVTYPE_NON_EQUIP)
        return false;

    if (p_Transmogrified->Class != p_Transmogrifier->Class)
        return false;

    /// If subclass different
    if (p_Transmogrifier->SubClass != p_Transmogrified->SubClass)
    {
        switch (p_Transmogrified->Class)
        {
            case ITEM_CLASS_WEAPON:
                if (!Item::SubclassesCompatible(p_Transmogrified, p_Transmogrifier))
                    return false;
            break;

            case ITEM_CLASS_ARMOR:
                if (p_Transmogrifier->SubClass != ITEM_SUBCLASS_ARMOR_COSMETIC)
                    return false;
                break;

            default:
                return false;
        }
    }

    if (p_Transmogrified->InventoryType != p_Transmogrifier->InventoryType)
    {
        if (p_Transmogrified->Class == ITEM_CLASS_WEAPON && p_Transmogrifier->Class == ITEM_CLASS_WEAPON)
        {
            if (!((p_Transmogrified->InventoryType == INVTYPE_WEAPON || p_Transmogrified->InventoryType == INVTYPE_WEAPONMAINHAND ||
                p_Transmogrified->InventoryType == INVTYPE_WEAPONOFFHAND || p_Transmogrified->InventoryType == INVTYPE_RANGED || p_Transmogrified->InventoryType == INVTYPE_RANGEDRIGHT) &&
                (p_Transmogrifier->InventoryType == INVTYPE_WEAPON || p_Transmogrifier->InventoryType == INVTYPE_WEAPONMAINHAND ||
                p_Transmogrifier->InventoryType == INVTYPE_WEAPONOFFHAND || p_Transmogrifier->InventoryType == INVTYPE_RANGED || p_Transmogrifier->InventoryType == INVTYPE_RANGEDRIGHT)))
                return false;
        }
        else if (p_Transmogrified->Class == ITEM_CLASS_ARMOR && p_Transmogrifier->Class == ITEM_CLASS_ARMOR)
        {
            if (!((p_Transmogrified->InventoryType == INVTYPE_CHEST || p_Transmogrified->InventoryType == INVTYPE_ROBE) &&
                (p_Transmogrifier->InventoryType == INVTYPE_CHEST || p_Transmogrifier->InventoryType == INVTYPE_ROBE)))
                return false;
        }
    }

    /// Check armor types
    if (p_Transmogrifier->SubClass != ITEM_SUBCLASS_ARMOR_COSMETIC && (p_Transmogrified->Class == ITEM_CLASS_ARMOR || p_Transmogrifier->Class == ITEM_CLASS_ARMOR))
    {
        uint32 skill1 = p_Transmogrified->GetSkill();
        uint32 skill2 = p_Transmogrifier->GetSkill();

        if ((skill1 == SKILL_PLATE_MAIL || skill1 == SKILL_LEATHER ||
            skill1 == SKILL_MAIL || skill1 == SKILL_CLOTH) ||
            (skill2 == SKILL_PLATE_MAIL || skill2 == SKILL_LEATHER ||
            skill2 == SKILL_MAIL || skill2 == SKILL_CLOTH))
            if (skill1 != skill2)
                return false;
    }

    return true;
}

bool Item::HasStats() const
{
    if (GetItemRandomPropertyId() != 0)
        return true;

    ItemTemplate const* proto = GetTemplate();
    for (uint8 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        if (proto->ItemStat[i].ItemStatValue != 0)
            return true;

    return false;
}

bool Item::HasSpells() const
{
    ItemTemplate const* proto = GetTemplate();
    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        if (proto->Spells[i].SpellId != 0)
            return true;

    return false;
}

uint32 Item::GetBuyPrice(Player const* owner, bool& standardPrice) const
{
    return Item::GetBuyPrice(GetTemplate(), GetQuality(), GetItemLevel(owner), standardPrice);
}

uint32 Item::GetBuyPrice(ItemTemplate const* proto, uint32 quality, uint32 itemLevel, bool& standardPrice)
{
    standardPrice = false;

    if (proto->Flags2 & int32(ItemFlagsEX::OVERRIDE_GOLD_COST))
        return proto->BuyPrice;

    ImportPriceQualityEntry const* qualityPrice = sImportPriceQualityStore.LookupEntry(quality + 1);
    if (!qualityPrice)
        return 0;

    ItemPriceBaseEntry const* basePrice = sItemPriceBaseStore.LookupEntry(itemLevel);
    if (!basePrice)
        return 0;

    float qualityFactor = qualityPrice->Factor;
    float baseFactor = 0.0f;

    uint32 inventoryType = proto->GetInventoryType();

    if (inventoryType == INVTYPE_WEAPON ||
        inventoryType == INVTYPE_2HWEAPON ||
        inventoryType == INVTYPE_WEAPONMAINHAND ||
        inventoryType == INVTYPE_WEAPONOFFHAND ||
        inventoryType == INVTYPE_RANGED ||
        inventoryType == INVTYPE_THROWN ||
        inventoryType == INVTYPE_RANGEDRIGHT)
        baseFactor = basePrice->WeaponFactor;
    else
        baseFactor = basePrice->ArmorFactor;

    if (inventoryType == INVTYPE_ROBE)
        inventoryType = INVTYPE_CHEST;

    if (proto->Class == ITEM_CLASS_GEM && proto->GetSubClass() == ITEM_SUBCLASS_GEM_ARTIFACT_RELIC)
    {
        inventoryType = INVTYPE_WEAPON;
        baseFactor = basePrice->WeaponFactor / 3.0f;
    }

    float typeFactor = 0.0f;
    int8 weapType = -1;

    switch (inventoryType)
    {
        case INVTYPE_HEAD:
        case INVTYPE_NECK:
        case INVTYPE_SHOULDERS:
        case INVTYPE_CHEST:
        case INVTYPE_WAIST:
        case INVTYPE_LEGS:
        case INVTYPE_FEET:
        case INVTYPE_WRISTS:
        case INVTYPE_HANDS:
        case INVTYPE_FINGER:
        case INVTYPE_TRINKET:
        case INVTYPE_CLOAK:
        case INVTYPE_HOLDABLE:
        {
            ImportPriceArmorEntry const* armorPrice = sImportPriceArmorStore.LookupEntry(inventoryType);
            if (!armorPrice)
                return 0;

            switch (proto->GetSubClass())
            {
                case ITEM_SUBCLASS_ARMOR_MISCELLANEOUS:
                case ITEM_SUBCLASS_ARMOR_CLOTH:
                    typeFactor = armorPrice->ClothFactor;
                    break;
                case ITEM_SUBCLASS_ARMOR_LEATHER:
                    typeFactor = armorPrice->LeatherFactor;
                    break;
                case ITEM_SUBCLASS_ARMOR_MAIL:
                    typeFactor = armorPrice->MailFactor;
                    break;
                case ITEM_SUBCLASS_ARMOR_PLATE:
                    typeFactor = armorPrice->PlateFactor;
                    break;
                default:
                    typeFactor = 1.0f;
                    break;
            }

            break;
        }
        case INVTYPE_SHIELD:
        {
            ImportPriceShieldEntry const* shieldPrice = sImportPriceShieldStore.LookupEntry(2);
            if (!shieldPrice)
                return 0;

            typeFactor = shieldPrice->Factor;
            break;
        }
        case INVTYPE_WEAPONMAINHAND:
            weapType = 0;
            break;
        case INVTYPE_WEAPONOFFHAND:
            weapType = 1;
            break;
        case INVTYPE_WEAPON:
            weapType = 2;
            break;
        case INVTYPE_2HWEAPON:
            weapType = 3;
            break;
        case INVTYPE_RANGED:
        case INVTYPE_RANGEDRIGHT:
        case INVTYPE_RELIC:
            weapType = 4;
            break;
        default:
            return proto->BuyPrice;
    }

    if (weapType != -1)
    {
        ImportPriceWeaponEntry const* weaponPrice = sImportPriceWeaponStore.LookupEntry(weapType + 1);
        if (!weaponPrice)
            return 0;

        typeFactor = weaponPrice->Factor;
    }

    standardPrice = false;
    return uint32(proto->PriceVariance * typeFactor * baseFactor * qualityFactor * proto->PriceRandomValue);
}

uint32 Item::GetSellPrice(Player const* owner) const
{
    return Item::GetSellPrice(GetTemplate(), GetQuality(), GetItemLevel(owner));
}

uint32 Item::GetSellPrice(ItemTemplate const* proto, uint32 quality, uint32 itemLevel)
{
    if (proto->Flags2 & int32(ItemFlagsEX::OVERRIDE_GOLD_COST))
        return proto->SellPrice;
    else
    {
        bool standardPrice;
        uint32 cost = Item::GetBuyPrice(proto, quality, itemLevel, standardPrice);

        if (standardPrice)
        {
            if (ItemClassEntry const* classEntry = GetItemClassByOldEnum(proto->Class))
            {
                uint32 buyCount = std::max(proto->BuyCount, 1u);
                return cost * classEntry->PriceModifier / buyCount;
            }

            return 0;
        }
        else
            return proto->SellPrice;
    }

    return 0;
}

bool Item::IsPotion() const
{
    return GetTemplate()->IsPotion();
}

bool Item::IsPvPItem() const
{
    ItemTemplate const* proto = GetTemplate();
    if (!proto)
        return false;

    for (uint8 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
    {
        uint32 stat = proto->ItemStat[i].ItemStatType;
        if (stat == int32(ItemModType::PVP_POWER) || stat == int32(ItemModType::RESILIENCE_RATING))
            return true;
    }

    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        int32 spell = proto->Spells[i].SpellId;
        if (spell == 132586 || spell == 139891)
            return true;
    }

    return false;
}

bool Item::IsStuffItem() const
{
    ItemTemplate const* proto = GetTemplate();
    if (!proto)
        return false;

    if (proto->Class == ITEM_CLASS_GEM)
        return true;

    uint32 invType = proto->InventoryType;

    switch (invType)
    {
        case INVTYPE_NON_EQUIP:
        case INVTYPE_BODY:
        case INVTYPE_BAG:
        case INVTYPE_TABARD:
        case INVTYPE_AMMO:
        case INVTYPE_THROWN:
        case INVTYPE_QUIVER:
        case INVTYPE_RELIC:
            return false;
        default:
            return true;
    }
}

bool Item::CanUpgrade() const
{
    ItemTemplate const* proto = GetTemplate();
    if (!proto)
        return false;

    if (proto->ItemLevel < 458)
        return false;

    if (proto->Quality == ITEM_QUALITY_LEGENDARY && !IsLegendaryCloak())
        return false;

    if (!IsStuffItem())
        return false;

    if (proto->Class == ITEM_CLASS_WEAPON && proto->SubClass == ITEM_SUBCLASS_WEAPON_FISHING_POLE)
        return false;

    if (!HasStats() && proto->InventoryType != INVTYPE_TRINKET)
        return false;

    // PvP item can't be upgraded after Season 12
    if (IsPvPItem() && proto->ItemLevel > 483)
        return false;

    return true;
}

bool Item::IsLegendaryCloak() const
{
    ItemTemplate const* proto = GetTemplate();
    if (!proto)
        return false;

    switch (proto->ItemId)
    {
        case 102245: // Qian-Le, Courage of Niuzao
        case 102246: // Xing-Ho, Breath of Yu'lon
        case 102247: // Jina-Kang, Kindness of Chi-Ji
        case 102248: // Fen-Yu, Fury of Xuen
        case 102249: // Gong-Lu, Strength of Xuen
        case 102250: // Qian-Ying, Fortitude of Niuzao
            return true;
        default:
            break;
    }

    return false;
}

float ItemTemplate::GetScalingDamageValue(uint32 ilvl) const
{
    if (Quality > ITEM_QUALITY_HEIRLOOM)
        return 0.0f;

    ItemDamageEntry const* damageEntry = NULL;

    switch (InventoryType)
    {
        case INVTYPE_WEAPON:
        case INVTYPE_WEAPONMAINHAND:
        case INVTYPE_WEAPONOFFHAND:
            if (Flags2 & int32(ItemFlagsEX::CASTER_WEAPON))
            {
                damageEntry =  sItemDamageOneHandCasterStore.LookupEntry(ilvl);
                break;
            }
            damageEntry = sItemDamageOneHandStore.LookupEntry(ilvl);
            break;
    case INVTYPE_RANGED:
    case INVTYPE_THROWN:
    case INVTYPE_RANGEDRIGHT:
        if (SubClass < 4)
        {
            if (Flags2 & int32(ItemFlagsEX::CASTER_WEAPON))
            {
                damageEntry = sItemDamageTwoHandCasterStore.LookupEntry(ilvl);
                break;
            }
            damageEntry = sItemDamageTwoHandStore.LookupEntry(ilvl);
            break;
        }
        else if (SubClass == 19)
        {
            damageEntry = sItemDamageOneHandCasterStore.LookupEntry(ilvl);
            break;
        }
        else
        {
            if (Flags2 & int32(ItemFlagsEX::CASTER_WEAPON))
            {
                damageEntry = sItemDamageTwoHandCasterStore.LookupEntry(ilvl);
                break;
            }
            damageEntry = sItemDamageTwoHandStore.LookupEntry(ilvl);
            break;
        }
    case INVTYPE_AMMO:
        damageEntry = sItemDamageAmmoStore.LookupEntry(ilvl);
        break;
    case INVTYPE_2HWEAPON:
        if (Flags2 & int32(ItemFlagsEX::CASTER_WEAPON))
        {
            damageEntry = sItemDamageTwoHandCasterStore.LookupEntry(ilvl);
            break;
        }
        damageEntry = sItemDamageTwoHandStore.LookupEntry(ilvl);
        break;
    default:
        break;
    }
    return damageEntry ? damageEntry->DPS[Quality == ITEM_QUALITY_HEIRLOOM ? ITEM_QUALITY_RARE : Quality] : 0.0f;
}

int32 ItemTemplate::GetRandomPointsOffset() const
{
    switch (InventoryType)
    {
        case INVTYPE_NECK:
        case INVTYPE_WRISTS:
        case INVTYPE_FINGER:
        case INVTYPE_SHIELD:
        case INVTYPE_CLOAK:
        case INVTYPE_HOLDABLE:
            return 2;
        case INVTYPE_SHOULDERS:
        case INVTYPE_WAIST:
        case INVTYPE_FEET:
        case INVTYPE_HANDS:
        case INVTYPE_TRINKET:
            return 1;
        case INVTYPE_WEAPON:
        case INVTYPE_WEAPONMAINHAND:
        case INVTYPE_WEAPONOFFHAND:
            return 3;
        case INVTYPE_RANGEDRIGHT:
            return 3 * (SubClass == 19 ? 1 : 0);
        case INVTYPE_RELIC:
            return 4;
        case INVTYPE_HEAD:
        case INVTYPE_BODY:
        case INVTYPE_CHEST:
        case INVTYPE_LEGS:
        case INVTYPE_RANGED:
        case INVTYPE_2HWEAPON:
        case INVTYPE_ROBE:
        case INVTYPE_THROWN:
            return 0;
        default:
            return -1;
    }
}

uint32 ItemTemplate::GetArmor(uint32 ilvl) const
{
    uint32 quality = ItemQualities(Quality) != ITEM_QUALITY_HEIRLOOM ? ItemQualities(Quality) : ITEM_QUALITY_RARE;
    if (quality > ITEM_QUALITY_ARTIFACT)
        return 0;

    // all items but shields
    if (Class != ITEM_CLASS_ARMOR || GetSubClass() != ITEM_SUBCLASS_ARMOR_SHIELD)
    {
        ItemArmorQualityEntry const* armorQuality = sItemArmorQualityStore.LookupEntry(ilvl);
        ItemArmorTotalEntry const* armorTotal = sItemArmorTotalStore.LookupEntry(ilvl);
        if (!armorQuality || !armorTotal)
            return 0;

        uint32 inventoryType = GetInventoryType();
        if (inventoryType == INVTYPE_ROBE)
            inventoryType = INVTYPE_CHEST;

        ArmorLocationEntry const* location = sArmorLocationStore.LookupEntry(inventoryType);
        if (!location)
            return 0;

        if (GetSubClass() < ITEM_SUBCLASS_ARMOR_CLOTH || GetSubClass() > ITEM_SUBCLASS_ARMOR_PLATE)
            return 0;

        return uint32(armorQuality->Value[quality] * armorTotal->Value[GetSubClass() - 1] * location->Value[GetSubClass() - 1] + 0.5f);
    }

    // shields
    ItemArmorShieldEntry const* shield = sItemArmorShieldStore.LookupEntry(ilvl);
    if (!shield)
        return 0;

    return uint32(shield->Value[quality] + 0.5f);
}

void ItemTemplate::GetDamage(uint32 ilvl, float& minDamage, float& maxDamage) const
{
    minDamage = 0;
    maxDamage = 0;

    if (!IsWeapon())
        return;

    float weaponMinDamageCalc = (float)Delay * GetScalingDamageValue(ilvl) * 0.001f;
    float weaponMaxDamageCalc = (((StatScalingFactor * 0.5f) + 1.0f) * weaponMinDamageCalc) + 0.5f;

    if (Delay != 0)
    {
        float delayModifier = 1000.0f / (float)Delay;
        float midCalc = (delayModifier * ((1.0f - (StatScalingFactor * 0.5f)) * weaponMinDamageCalc)) + ArmorDamageModifier;
        midCalc = midCalc > 1.0f ? midCalc : 1.0f;
        float delayCoeff = 1.0f / delayModifier;
        minDamage = floor((delayCoeff * midCalc) + 0.5f);
        maxDamage = floor((delayCoeff * ((delayModifier * weaponMaxDamageCalc) + ArmorDamageModifier)) + 0.5f);
    }
    else
    {
        maxDamage = floor(weaponMaxDamageCalc + 0.5f);
        minDamage = floor(((1.0f - (StatScalingFactor * 0.5f)) * weaponMinDamageCalc) + 0.5f);
    }
}

void Item::UpgradeItemWithBonuses(std::vector<uint32> const& p_ItemBonuses, bool p_RemoveOldBonuses /*= false*/)
{
    WorldPacket l_Data(Opcodes::SMSG_CHANGE_ITEM, 10);

    l_Data.appendPackGUID(GetOwnerGUID());

    Item::BuildDynamicItemDatas(l_Data, this);

    if (p_RemoveOldBonuses)
        RemoveAllItemBonuses();

    AddItemBonuses(p_ItemBonuses);

    Item::BuildDynamicItemDatas(l_Data, this);

    GetOwner()->SendDirectMessage(&l_Data);

    SetState(ItemUpdateState::ITEM_CHANGED, GetOwner());
}

bool Item::AddItemBonus(uint32 p_ItemBonusId, bool p_EnableLogging)
{
    auto l_Bonuses = GetItemBonusesByID(p_ItemBonusId);
    if (!l_Bonuses)
        return false;

    if (HasItemBonus(p_ItemBonusId))
        return false;

    for (auto l_Bonus : *l_Bonuses)
    {
        if (l_Bonus == nullptr)
            continue;

        _bonusData.AddBonus(l_Bonus->Type, l_Bonus->Value);

        if (l_Bonus ->Type == ITEM_BONUS_SCALING_STAT_DISTRIBUTION || l_Bonus->Type == ITEM_BONUS_SCALING_STAT_DISTRIBUTION_2)
        {
            if (SandboxScalingEntry const* l_SandboxScaling = sSandboxScalingStore.LookupEntry(l_Bonus->Value[2]))
                SetModifier(ITEM_MODIFIER_SCALING_STAT_DISTRIBUTION_FIXED_LEVEL, std::min(l_SandboxScaling->MaxLevel, GetModifier(ITEM_MODIFIER_SCALING_STAT_DISTRIBUTION_FIXED_LEVEL)));
        }
    }

    SetUInt32Value(ITEM_FIELD_ITEM_APPEARANCE_MOD_ID, _bonusData.AppearanceModID);
    SetDynamicValue(ITEM_DYNAMIC_FIELD_BONUS_LIST_IDS, GetAllItemBonuses().size(), p_ItemBonusId);

    if (p_EnableLogging)
        UpdateLogs(GetGUIDLow(), GetAllItemBonuses());

    return true;
}

void Item::AddItemBonuses(std::vector<uint32> const& p_ItemBonuses)
{
    if (!p_ItemBonuses.size())
        return;

    for (uint32 i = 0; i < p_ItemBonuses.size(); i++)
        AddItemBonus(p_ItemBonuses[i], false);

    UpdateLogs(GetGUIDLow(), p_ItemBonuses);
}

bool Item::HasItemBonus(uint32 p_ItemBonusId) const
{
    std::vector<uint32> const& l_BonusList = GetAllItemBonuses();
    for (auto& l_Bonus : l_BonusList)
        if (l_Bonus == p_ItemBonusId)
            return true;
    return false;
}

bool Item::RemoveItemBonus(uint32 p_ItemBonusId)
{
    std::vector<uint32> const& l_BonusList = GetAllItemBonuses();

    bool l_Find = false;

    for (uint32 i = 0; i < l_BonusList.size(); i++)
    {
        if (l_BonusList[i] == p_ItemBonusId && p_ItemBonusId)
        {
            RemoveDynamicValue(ITEM_DYNAMIC_FIELD_BONUS_LIST_IDS, l_BonusList[i]);

            std::vector<uint32> const l_NewBonusList = GetAllItemBonuses();
            ClearDynamicValue(ITEM_DYNAMIC_FIELD_BONUS_LIST_IDS);

            uint32 l_FieldIdx = 0;
            for (uint32 l_NewI = 0; l_NewI < l_NewBonusList.size(); l_NewI++)
            {
                if (l_NewBonusList[l_NewI] != 0)
                    SetDynamicValue(ITEM_DYNAMIC_FIELD_BONUS_LIST_IDS, l_FieldIdx++, l_NewBonusList[l_NewI]);
            }

            l_Find = true;
            break;
        }
    }

    if (l_Find)
    {
        auto l_OldBonuses = GetItemBonusesByID(p_ItemBonusId);
        if (l_OldBonuses)
        {
            for (auto l_Bonus : *l_OldBonuses)
            {
                if (l_Bonus == nullptr)
                    continue;

                _bonusData.RemoveBonus(l_Bonus->Type, l_Bonus->Value);
            }
        }
        return true;
    }

    return false;
}

void Item::RemoveAllItemBonuses()
{
    std::vector<uint32> const& l_BonusList = GetAllItemBonuses();
    for (auto& l_Bonus : l_BonusList)
        if (l_Bonus)
            RemoveItemBonus(l_Bonus);

    ClearDynamicValue(ITEM_DYNAMIC_FIELD_BONUS_LIST_IDS);
}

void Item::ModifyItemBonus(uint32 p_OldBonus, uint32 p_NewBonus)
{
    std::vector<uint32> const& l_BonusList = GetAllItemBonuses();

    for (uint32 i = 0; i < l_BonusList.size(); i++)
    {
        if (l_BonusList[i] == p_OldBonus && p_OldBonus)
        {
            ModifyDynamicValue(ITEM_DYNAMIC_FIELD_BONUS_LIST_IDS, p_OldBonus, p_NewBonus);
            break;
        }
    }

    std::ostringstream ssBonuses;
    std::vector<uint32> bonuses = GetAllItemBonuses();
    for (uint8 i = 0; i < bonuses.size(); ++i)
    {
        if (!bonuses[i])
            continue;

        ssBonuses << bonuses[i] << ' ';
    }

    PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ITEM_INSTANCE_BONUSES);
    l_Stmt->setString(0, ssBonuses.str());
    l_Stmt->setUInt64(1, GetGUID());
    CharacterDatabase.Execute(l_Stmt);
    return;
}

std::vector<uint32> const& Item::GetAllItemBonuses() const
{
    return GetDynamicValues(ITEM_DYNAMIC_FIELD_BONUS_LIST_IDS);
}

uint32 Item::GetItemLevel(Player const* p_Owner, bool p_IgnorePvPModifiers /*= false*/, bool p_ForcePvPItemLevel /*= false*/) const
{
    if (!p_Owner)
        return MIN_ITEM_LEVEL;

    return Item::GetItemLevel(GetTemplate(), _bonusData, p_Owner->getLevel(), GetModifier(ITEM_MODIFIER_SCALING_STAT_DISTRIBUTION_FIXED_LEVEL), GetModifier(ITEM_MODIFIER_UPGRADE_ID), p_Owner, p_IgnorePvPModifiers, p_ForcePvPItemLevel);
}

uint32 Item::GetItemLevel(ItemTemplate const* itemTemplate, BonusData const& bonusData, uint32 level, uint32 fixedLevel, uint32 upgradeId, Player const* p_Owner, bool p_IgnorePvPModifiers, bool p_ForcePvPItemLevel)
{
    ItemTemplate const* stats = itemTemplate;
    if (!stats)
        return MIN_ITEM_LEVEL;

    uint32 itemLevel = stats->GetBaseItemLevel();
    if (bonusData.HasItemLevelBonus || !bonusData.ItemLevelOverride)
    {
        if (ScalingStatDistributionEntry const* ssd = sScalingStatDistributionStore.LookupEntry(bonusData.ScalingStatDistribution))
        {
            if (fixedLevel)
            {
                if (stats->Quality != ItemQualities::ITEM_QUALITY_HEIRLOOM)
                    level = fixedLevel;
            }

            if (SandboxScalingEntry const* sandbox = sSandboxScalingStore.LookupEntry(bonusData.SandboxScalingId))
                if ((sandbox->Flags & 2 || sandbox->MinLevel || sandbox->MaxLevel) && !(sandbox->Flags & 4))
                    level = std::min(std::max(level, sandbox->MinLevel), sandbox->MaxLevel);

            if (uint32 heirloomIlvl = uint32(GetCurveValueAt(ssd->CurveProperties, std::min(level, ssd->MaxLevel))))
                itemLevel = heirloomIlvl;
        }

        itemLevel += bonusData.ItemLevelBonus;
    }
    else
        itemLevel = bonusData.ItemLevelOverride;

    if (ItemUpgradeEntry const* upgrade = sItemUpgradeStore.LookupEntry(upgradeId))
        itemLevel += upgrade->itemLevelUpgrade;

    for (uint32 i = 0; i < MAX_ITEM_PROTO_SOCKETS; ++i)
        itemLevel += bonusData.GemItemLevelBonus[i];

    if (p_Owner)
    {
        if (stats->PvPScalingLevel && !p_IgnorePvPModifiers)
            if (p_ForcePvPItemLevel || (p_Owner->GetMap() && p_Owner->GetMap()->IsBattlegroundOrArena()) || p_Owner->GetMapId() == 1280 || (p_Owner->IsInPvPCombat() && p_Owner->getDeathState() != JUST_DIED))
                itemLevel += stats->PvPScalingLevel;

        if (uint32 minItemLevel = p_Owner->GetUInt32Value(UNIT_FIELD_MIN_ITEM_LEVEL))
            if (itemLevel < minItemLevel && itemLevel > p_Owner->GetUInt32Value(UNIT_FIELD_MIN_ITEM_LEVEL_CUTOFF))
                itemLevel = minItemLevel;

        if (uint32 maxItemLevel = p_Owner->GetUInt32Value(UNIT_FIELD_MAX_ITEM_LEVEL))
            itemLevel = std::min(itemLevel, maxItemLevel);

        if (p_Owner->GetMap() && !(p_Owner->GetMap()->IsBattlegroundOrArena() && p_Owner->GetBattleground() && p_Owner->GetBattleground()->IsWargame()))
        {
            if (stats->Flags3 & int32(ItemFlagsEX2::PVP_TOURNAMENT_GEAR))
                itemLevel = 1;
        }
    }

    return std::min(std::max(itemLevel, uint32(MIN_ITEM_LEVEL)), uint32(MAX_ITEM_LEVEL));
}

int32 Item::GetItemStatValue(uint32 p_Index, Player* p_Owner, uint32 p_Ilevel) const
{
    ASSERT(p_Index < MAX_ITEM_PROTO_STATS);

    if (uint32 randomPropPoints = GetRandomPropertyPoints(p_Ilevel, GetQuality(), GetTemplate()->GetInventoryType(), GetTemplate()->GetSubClass()))
    {
        float statValue = std::max(0, _bonusData.ItemStatAllocation[p_Index]) * randomPropPoints * 0.0001f;
        if (ItemSocketCostPerLevelEntry const* gtCost = g_ItemSocketCostPerLevel.LookupEntry(p_Ilevel))
            statValue -= float(int32(_bonusData.ItemStatSocketCostMultiplier[p_Index] * gtCost->cost));

        if (GetTemplate()->GetInventoryType() == InventoryType::INVTYPE_TRINKET && GetItemStatType(p_Index) == (int32)ItemModType::EXTRA_ARMOR)
        {
            if (CombatRatingsMultByILvl const* l_IlvlRating = g_CombatRatingMultByIlvl.LookupEntry(p_Ilevel))
                statValue *= l_IlvlRating->ArmorMultiplier;
        }

        return int32(std::floor(statValue + 0.5f));
    }

    return _bonusData.ItemStatValue[p_Index];
}

uint16 Item::GetVisibleAppearanceModID(Player const* p_Owner) const
{
    ItemModifiers l_TransmogMod = ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS;
    if (HasFlag(EItemFields::ITEM_FIELD_MODIFIERS_MASK, g_AppearanceModifierMaskSpecSpecific))
        l_TransmogMod = g_AppearanceModifierSlotBySpec[p_Owner->GetActiveTalentGroup()];

    if (ItemModifiedAppearanceEntry const* l_Transmog = sItemModifiedAppearanceStore.LookupEntry(GetModifier(l_TransmogMod)))
        return uint16(l_Transmog->AppearanceModID);

    return uint16(GetAppearanceModId());
}

uint32 Item::GetVisibleEntry(Player const* p_Owner) const
{
    ItemModifiers l_TransmogMod = ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS;
    if (HasFlag(EItemFields::ITEM_FIELD_MODIFIERS_MASK, g_AppearanceModifierMaskSpecSpecific))
        l_TransmogMod = g_AppearanceModifierSlotBySpec[p_Owner->GetActiveTalentGroup()];

    if (ItemModifiedAppearanceEntry const* l_Transmog = sItemModifiedAppearanceStore.LookupEntry(GetModifier(l_TransmogMod)))
        return l_Transmog->ItemID;

    return GetEntry();
}

uint32 Item::GetVisibleEnchantmentId(Player const* p_Owner) const
{
    ItemModifiers l_IllusionMod = ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS;
    if (HasFlag(EItemFields::ITEM_FIELD_MODIFIERS_MASK, g_IllusionModifierMaskSpecSpecific))
        l_IllusionMod = g_IllusionModifierSlotBySpec[p_Owner->GetActiveTalentGroup()];

    if (ItemModifiedAppearanceEntry const* l_Transmog = sItemModifiedAppearanceStore.LookupEntry(GetModifier(l_IllusionMod)))
    {
        /// Check all specs first
        if (uint32 l_EnchantIllusion = GetModifier(ItemModifiers::ITEM_MODIFIER_ENCHANT_ILLUSION_ALL_SPECS))
            return l_EnchantIllusion;

        /// Then check for our specific spec if there isn't any illusion that apply for all specs
        if (uint32 l_EnchantIllusion = GetModifier(l_IllusionMod))
            return l_EnchantIllusion;
    }

    return GetEnchantmentId(EnchantmentSlot::PERM_ENCHANTMENT_SLOT);
}

uint16 Item::GetVisibleItemVisual(Player const* p_Owner) const
{
    if (SpellItemEnchantmentEntry const* l_Enchant = sSpellItemEnchantmentStore.LookupEntry(GetVisibleEnchantmentId(p_Owner)))
        return l_Enchant->ItemVisual;

    return 0;
}

void Item::SetModifier(ItemModifiers p_Modifier, uint32 p_Value)
{
    if (p_Modifier >= ItemModifiers::MAX_ITEM_MODIFIERS)
        return;

    m_Modifiers[p_Modifier] = p_Value;
    ApplyModFlag(EItemFields::ITEM_FIELD_MODIFIERS_MASK, 1 << p_Modifier, p_Value != 0);
    SetDynamicValue(EItemDynamicFields::ITEM_DYNAMIC_FIELD_MODIFIERS, p_Modifier, p_Value);
}

uint64 Item::GetRealOwnerGUID() const
{
    if (Player* l_Owner = GetOwner())
        return l_Owner->GetRealGUID();
    return 0;
}

bool Item::SubclassesCompatibleForRandomWeapon(ItemTemplate const* p_Transmogrifier, ItemTemplate const* p_Transmogrified)
{
    if (!p_Transmogrifier || !p_Transmogrified)
        return false;

    /// One-Handed can be transmogrified to each other
    if ((p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_AXE ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_MACE ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_SWORD ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_FIST_WEAPON ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_ARMOR_SHIELD) &&
        (p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_AXE ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_MACE ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_SWORD ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_FIST_WEAPON ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_ARMOR_SHIELD))
        return true;

    /// Two-Handed
    /// Two-handed axes, maces, and swords can be Transmogrified to each other.
    if ((p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_AXE2 ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_MACE2 ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_SWORD2 ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM ||
        p_Transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_STAFF) &&
        (p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_AXE2 ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_MACE2 ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_SWORD2 ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM ||
        p_Transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_STAFF))
        return true;

    return false;
}

bool Item::CanTransmogrifyIntoRandomWeapon(ItemTemplate const* p_Transmogrifier, ItemTemplate const* p_Transmogrified)
{
    if (!p_Transmogrified || !p_Transmogrifier)
        return false;

    if ((p_Transmogrified->Class != ITEM_CLASS_WEAPON || p_Transmogrifier->Class != ITEM_CLASS_WEAPON) &&
        (p_Transmogrified->Class != ITEM_CLASS_ARMOR || p_Transmogrifier->Class != ITEM_CLASS_ARMOR))
        return false;

    if (p_Transmogrified->InventoryType != INVTYPE_WEAPON &&
        p_Transmogrified->InventoryType != INVTYPE_2HWEAPON &&
        p_Transmogrified->InventoryType != INVTYPE_WEAPONMAINHAND &&
        p_Transmogrified->InventoryType != INVTYPE_WEAPONOFFHAND &&
        p_Transmogrified->InventoryType != INVTYPE_SHIELD)
        return false;

    if (p_Transmogrifier->InventoryType != INVTYPE_WEAPON &&
        p_Transmogrifier->InventoryType != INVTYPE_2HWEAPON &&
        p_Transmogrifier->InventoryType != INVTYPE_WEAPONMAINHAND &&
        p_Transmogrifier->InventoryType != INVTYPE_WEAPONOFFHAND &&
        p_Transmogrifier->InventoryType != INVTYPE_SHIELD)
        return false;

    if (!p_Transmogrifier->CanTransmogrify() || !p_Transmogrified->CanBeTransmogrified())
        return false;

    if (!SubclassesCompatibleForRandomWeapon(p_Transmogrifier, p_Transmogrified))
        return false;

    return true;
}

void Item::RandomWeaponTransmogrificationFromPrimaryBag(Player* p_Player, Item* p_Transmogrified, bool p_Apply)
{
    if (!p_Player || !p_Transmogrified)
        return;

    uint8 l_TransmogrifiedItemSlot = p_Transmogrified->GetSlot();
    /// Wrong transmogrified item, we can change just weapons
    if (l_TransmogrifiedItemSlot != EQUIPMENT_SLOT_MAINHAND && l_TransmogrifiedItemSlot != EQUIPMENT_SLOT_OFFHAND)
        return;

    /// Apply transmogrification on weapon
    if (p_Apply)
    {
        /// Find item template of Transmogrified weapon
        ItemTemplate const* l_TransmogrifiedTemplate = p_Transmogrified->GetTemplate();
        if (!l_TransmogrifiedTemplate)
            return;

        /// Select random weapon from primary bag
        uint32 l_RandomWeaponId = p_Player->GetRandomWeaponFromPrimaryBag(l_TransmogrifiedTemplate);

        /// Find item template of Transmogrifier weapon
        ItemTemplate const* l_TransmogrifierTemplate = sObjectMgr->GetItemTemplate(l_RandomWeaponId);
        if (!l_TransmogrifierTemplate)
            return;

        /// Find item in player inventory
        Item* l_Transmogrifier = p_Player->GetItemByEntry(l_TransmogrifierTemplate->ItemId);
        if (!l_Transmogrifier)
            return;

        p_Player->SetVisibleItemSlot(l_TransmogrifiedItemSlot, l_Transmogrifier);

        p_Transmogrified->UpdatePlayedTime(p_Player);

        p_Transmogrified->SetOwnerGUID(p_Player->GetGUID());
        p_Transmogrified->SetNotRefundable(p_Player);
        p_Transmogrified->ClearSoulboundTradeable(p_Player);

        /// Some rules for Transmogrifier weapon
        if (l_Transmogrifier != nullptr)
        {
            if (l_TransmogrifierTemplate->Bonding == BIND_WHEN_EQUIPED || l_TransmogrifierTemplate->Bonding == BIND_WHEN_USE)
                l_Transmogrifier->SetBinding(true);

            l_Transmogrifier->SetOwnerGUID(p_Player->GetGUID());
            l_Transmogrifier->SetNotRefundable(p_Player);
            l_Transmogrifier->ClearSoulboundTradeable(p_Player);
        }
    }
    /// Remove transmogrification from weapon
    else
    {
        p_Player->SetVisibleItemSlot(l_TransmogrifiedItemSlot, p_Transmogrified);
    }
}

void BonusData::Initialize(ItemTemplate const* proto)
{
    Quality = proto->Quality;
    ItemLevelBonus = 0;
    RequiredLevel = proto->RequiredLevel;
    for (uint32 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        ItemStatType[i] = proto->ItemStat[i].ItemStatType;

    for (uint32 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        ItemStatValue[i] = proto->ItemStat[i].ItemStatValue;;

    for (uint32 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        ItemStatAllocation[i] = proto->ItemStat[i].ScalingValue;

    for (uint32 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        ItemStatSocketCostMultiplier[i] = proto->ItemStat[i].SocketCostRate;

    for (uint32 i = 0; i < MAX_ITEM_PROTO_SOCKETS; ++i)
    {
        SocketColor[i] = proto->Socket[i].Color;
        GemItemLevelBonus[i] = 0;
    }

    AppearanceModID = 0;

    RepairCostMultiplier = 1.0f;
    ScalingStatDistribution = proto->ScalingStatDistribution;
    memset(DisplayToastMethod, 0, sizeof(DisplayToastMethod));

    ItemLevelOverride = 0;
    SandboxScalingId = 0;
    RelicType = -1;
    HasItemLevelBonus = false;

    _state.AppearanceModPriority = std::numeric_limits<int32>::max();
    _state.ScalingStatDistributionPriority = std::numeric_limits<int32>::max();
    _state.ItemLevelOverridePriority = std::numeric_limits<int32>::max();
    _state.HasQualityBonus = false;
}

void BonusData::AddBonus(uint32 type, int32 const (&values)[3])
{
    switch (type)
    {
        case ITEM_BONUS_ITEM_LEVEL:
            ItemLevelBonus += values[0];
            HasItemLevelBonus = true;
            break;
        case ITEM_BONUS_STAT:
        {
            uint32 statIndex = 0;
            for (statIndex = 0; statIndex < MAX_ITEM_PROTO_STATS; ++statIndex)
                if (ItemStatType[statIndex] == values[0] || ItemStatType[statIndex] == -1)
                    break;

            if (statIndex < MAX_ITEM_PROTO_STATS)
            {
                ItemStatType[statIndex] = values[0];
                ItemStatAllocation[statIndex] += values[1];
            }
            break;
        }
        case ITEM_BONUS_QUALITY:
            if (!_state.HasQualityBonus)
            {
                Quality = static_cast<uint32>(values[0]);
                _state.HasQualityBonus = true;
            }
            else if (Quality < static_cast<uint32>(values[0]))
                Quality = static_cast<uint32>(values[0]);
            break;
        case ITEM_BONUS_SOCKET:
        {
            uint32 socketCount = values[0];
            for (uint32 i = 0; i < MAX_ITEM_PROTO_SOCKETS && socketCount; ++i)
                if (!SocketColor[i])
                {
                    SocketColor[i] = values[1];
                    --socketCount;
                }
            break;
        }
        case ITEM_BONUS_APPEARANCE:
            if (values[1] < _state.AppearanceModPriority)
            {
                AppearanceModID = static_cast<uint32>(values[0]);
                _state.AppearanceModPriority = values[1];
            }
            break;
        case ITEM_BONUS_REQUIRED_LEVEL:
            RequiredLevel += values[0];
            break;
        case ITEM_BONUS_REPAIR_COST_MULTIPLIER:
            RepairCostMultiplier *= static_cast<float>(values[0]) * 0.01f;
            break;
        case ITEM_BONUS_SCALING_STAT_DISTRIBUTION:
        case ITEM_BONUS_SCALING_STAT_DISTRIBUTION_2:
            if (values[1] < _state.ScalingStatDistributionPriority)
            {
                ScalingStatDistribution = static_cast<uint32>(values[0]);
                SandboxScalingId = static_cast<uint32>(values[2]);
                _state.ScalingStatDistributionPriority = values[1];
            }
            break;
        case ITEM_BONUS_ITEM_LEVEL_OVERRIDE:
            if (values[1] < _state.ItemLevelOverridePriority)
            {
                ItemLevelOverride = static_cast<uint32>(values[0]);
                _state.ItemLevelOverridePriority = values[1];
            }
            break;
        case ITEM_BONUS_DISPLAY_TOAST_METHOD:
            if (values[0] < DISPLAY_TOAST_MAX)
            {
                if (static_cast<uint32>(values[1]) < DisplayToastMethod[1])
                {
                    DisplayToastMethod[0] = static_cast<uint32>(values[0]);
                    DisplayToastMethod[1] = static_cast<uint32>(values[1]);
                }
            }
            break;
        case ITEM_BONUS_RELIC_TYPE:
            RelicType = values[0];
            break;
        case ITEM_BONUS_OVERRIDE_REQUIRED_LEVEL:
            RequiredLevel = values[0];
            break;
        default:
            break;
    }
}

void BonusData::RemoveBonus(uint32 type, int32 const (&values)[3])
{
    switch (type)
    {
        case ITEM_BONUS_ITEM_LEVEL:
            ItemLevelBonus -= values[0];
            if (ItemLevelBonus < 0)
                ItemLevelBonus = 0;

            HasItemLevelBonus = ItemLevelBonus > 0;
            break;
        // TODO: create code for other types
        default:
            break;
    }
}

void Item::CheckRelicTalentData(uint8 p_GemSlot)
{
    ItemDynamicFieldGems const* l_Relic = GetGem(p_GemSlot);
    if (l_Relic && l_Relic->ItemId)
    {
        uint32 l_RelicType = 0;
        if (GemPropertiesEntry const* l_GemProperty = sGemPropertiesStore.LookupEntry(sObjectMgr->GetItemTemplate(l_Relic->ItemId)->GetGemProperties()))
        {
            if (SpellItemEnchantmentEntry const* l_GemEnchant = sSpellItemEnchantmentStore.LookupEntry(l_GemProperty->EnchantID))
            {
                for (uint32 l_I = 0; l_I < MAX_ENCHANTMENT_SPELLS; ++l_I)
                {
                    switch (l_GemEnchant->type[l_I])
                    {
                        case ItemEnchantmentType::ITEM_ENCHANTMENT_TYPE_ARTIFACT_POWER_BONUS_RANK_BY_TYPE:
                            l_RelicType = l_GemEnchant->spellid[l_I];
                            break;
                        default:
                            break;
                    }

                    if (l_RelicType)
                        break;
                }
            }
        }

        if (!l_RelicType)
            return;

        bool l_NeedReset = false;
        for (uint8 l_TalentIdx = 0; l_TalentIdx < MAX_RELIC_TALENT; ++l_TalentIdx)
        {
            if (!m_RelicTalentData.RelicTalent[p_GemSlot][l_TalentIdx])
                continue;

            /// Check if relic trait isn't one of the generated relic talent for Void Crucible
            if (RelicTalentEntry const* l_TalentEntry = sRelicTalentStore.LookupEntry(m_RelicTalentData.RelicTalent[p_GemSlot][l_TalentIdx]))
            {
                /// Duplicated trait, need to reset talent
                if (l_TalentEntry->ArtifactPowerLabel == l_RelicType)
                {
                    if (Player* l_Owner = GetOwner())
                        sLog->outAshran("Item::CheckRelicTalentData -> Player (Name: %s, Guid: %u) got a relic with a double trait proc!", l_Owner->GetName(), GUID_LOPART(GetOwnerGUID()));
                    else
                        sLog->outAshran("Item::CheckRelicTalentData -> Player (Guid: %u) got a relic with a double trait proc!", GUID_LOPART(GetOwnerGUID()));

                    l_NeedReset = true;
                    break;
                }
            }
        }

        if (l_NeedReset)
        {
            for (uint8 l_TalentIdx = 0; l_TalentIdx < MAX_RELIC_TALENT; ++l_TalentIdx)
                m_RelicTalentData.RelicTalent[p_GemSlot][l_TalentIdx] = 0;

            m_RelicTalentData.PathMask[p_GemSlot] = 0;

            m_RelicTalentData.NeedSave = true;

            auto l_DynamicFields = m_RelicTalentData.GetDynamicFieldStruct(p_GemSlot);
            SetDynamicStructuredValue(EItemDynamicFields::ITEM_DYNAMIC_FIELD_RELIC_TALENT_DATA, sizeof (ItemDynamicFieldRelicTalentData) * p_GemSlot, &l_DynamicFields);

            SetState(ItemUpdateState::ITEM_CHANGED, GetOwner());
        }
    }
}


void Item::AddServerSideBonus(uint32 p_ItemBonusListID)
{
    if (ServerSideItemBonusListIDs.count(p_ItemBonusListID) == 1)
        return;

    ServerSideItemBonusListIDs.insert(p_ItemBonusListID);

    if (auto l_Bonuses = GetItemBonusesByID(p_ItemBonusListID))
    {
        for (ItemBonusEntry const* l_ItemBonus : *l_Bonuses)
        {
            if (l_ItemBonus)
                _bonusData.AddBonus(l_ItemBonus->Type, l_ItemBonus->Value);
        }
    }
}

void Item::RemoveServerSideBonus(uint32 p_ItemBonusListID)
{
    if (!ServerSideItemBonusListIDs.count(p_ItemBonusListID))
        return;

    ServerSideItemBonusListIDs.erase(p_ItemBonusListID);

    if (auto l_Bonuses = GetItemBonusesByID(p_ItemBonusListID))
    {
        for (ItemBonusEntry const* l_ItemBonus : *l_Bonuses)
        {
            if (l_ItemBonus)
                _bonusData.RemoveBonus(l_ItemBonus->Type, l_ItemBonus->Value);
        }
    }
}

void Item::CopyArtifactDataFromParent(Item* parent)
{
    memcpy(_bonusData.GemItemLevelBonus, parent->GetBonus()->GemItemLevelBonus, sizeof(_bonusData.GemItemLevelBonus));
    SetModifier(ITEM_MODIFIER_ARTIFACT_APPEARANCE_ID, parent->GetModifier(ITEM_MODIFIER_ARTIFACT_APPEARANCE_ID));
    SetAppearanceModId(parent->GetAppearanceModId());
}

void Item::AddToObjectUpdate()
{
    if (Player* owner = GetOwner())
        owner->GetMap()->AddUpdateObject(this);
}

void Item::RemoveFromObjectUpdate()
{
    if (Player* owner = GetOwner())
    {
        if (owner->GetMap())
            owner->GetMap()->RemoveUpdateObject(this);
    }
}

uint32 Item::GetArtifactKnowledgeOnAcquire() const
{
    return m_ScaleArtifactKnowledge;
}

void Item::SetArtifactKnowledgeOnAcquire(uint32 p_ArtifactKnowledge)
{
    m_ScaleArtifactKnowledge = p_ArtifactKnowledge;
}

ItemDisenchantLootEntry const* Item::GetDisenchantLoot(Player const* owner) const
{
    return Item::GetDisenchantLoot(GetTemplate(), GetQuality(), GetItemLevel(owner));
}

ItemDisenchantLootEntry const* Item::GetDisenchantLoot(ItemTemplate const* itemTemplate, uint32 quality, uint32 itemLevel)
{
    if (itemTemplate->Flags & (int32(ItemFlags::CONJURED) | int32(ItemFlags::NO_DISENCHANT)) || itemTemplate->Bonding == BIND_QUEST_ITEM)
        return nullptr;

    if (itemTemplate->Area || itemTemplate->Map || itemTemplate->Stackable > 1)
        return nullptr;

    if (!Item::GetSellPrice(itemTemplate, quality, itemLevel) && !sItemCurrencyCostStore.LookupEntry(itemTemplate->ItemId))
        return nullptr;

    uint32 itemClass  = itemTemplate->Class;
    int8 itemSubClass = itemTemplate->GetSubClass();
    uint8 expansion   = itemTemplate->RequiredExpansion;

    for (ItemDisenchantLootEntry const* disenchant : sItemDisenchantLootStore)
    {
        if (disenchant->ItemClass != itemClass)
            continue;

        if (disenchant->ItemSubClass >= 0 && itemSubClass != disenchant->ItemSubClass)
            continue;

        if (disenchant->ItemQuality != quality)
            continue;

        if (disenchant->MinItemLevel > itemLevel || disenchant->MaxItemLevel < itemLevel)
            continue;

        if (disenchant->Expansion != -2 && disenchant->Expansion != expansion)
            continue;

        return disenchant;
    }

    return nullptr;
}

enum RelicTalents
{
    NetherlightFortification = 1,
};

namespace RelicTalentSlot
{
    enum
    {
        First = 0,
        Void = 1,
        Light = 2,
        Neutral1 = 3,
        Neutral2 = 4,
        Neutral3 = 5,
        Max
    };
}

namespace RelicTalentType
{
    enum
    {
        None    = 0,
        Void    = 1,
        Light   = 2,
        Neutral = 3
    };
}

void RelicTalentData::GenerateTalents(uint8 p_Slot, uint32 p_Except)
{
    std::map<uint32, std::vector<RelicTalentEntry const*>> l_RelicTalentPerType;

    for (RelicTalentEntry const* l_RelicTalent : sRelicTalentStore)
        if (!p_Except || l_RelicTalent->ArtifactPowerLabel != p_Except)
            l_RelicTalentPerType[l_RelicTalent->Type].push_back(l_RelicTalent);

    RelicTalent[p_Slot][RelicTalentSlot::First] = RelicTalents::NetherlightFortification;
    RelicTalent[p_Slot][RelicTalentSlot::Void]  = JadeCore::Containers::SelectRandomContainerElement(l_RelicTalentPerType[RelicTalentType::Void])->ID;
    RelicTalent[p_Slot][RelicTalentSlot::Light] = JadeCore::Containers::SelectRandomContainerElement(l_RelicTalentPerType[RelicTalentType::Light])->ID;

    std::random_device l_Rng;
    std::mt19937 l_Urng(l_Rng());
    std::shuffle(l_RelicTalentPerType[RelicTalentType::Neutral].begin(), l_RelicTalentPerType[RelicTalentType::Neutral].end(), l_Urng);

    RelicTalent[p_Slot][RelicTalentSlot::Neutral1] = l_RelicTalentPerType[RelicTalentType::Neutral][0]->ID;
    RelicTalent[p_Slot][RelicTalentSlot::Neutral2] = l_RelicTalentPerType[RelicTalentType::Neutral][1]->ID;
    RelicTalent[p_Slot][RelicTalentSlot::Neutral3] = l_RelicTalentPerType[RelicTalentType::Neutral][2]->ID;

    NeedSave = true;
}

void RelicTalentData::EquipNewRelic(Item* p_Relic, uint8 p_Slot)
{
    RelicTalentData* l_TalentData = p_Relic->GetRelicTalentData();

    PathMask[p_Slot] = 0;

    for (int l_I = RelicTalentSlot::First; l_I < RelicTalentSlot::Max; l_I++)
        RelicTalent[p_Slot][l_I] = l_TalentData->RelicTalent[0][l_I];

    NeedSave = true;
}

ItemDynamicFieldRelicTalentData RelicTalentData::GetDynamicFieldStruct(uint8 p_Slot)
{
    ItemDynamicFieldRelicTalentData l_RelicTalentData;

    l_RelicTalentData.Enable                    = RelicTalent[p_Slot][0] != 0;
    l_RelicTalentData.PathMask                  = PathMask[p_Slot];
    l_RelicTalentData.RelicSlot                 = p_Slot + SOCK_ENCHANTMENT_SLOT;

    for (int l_I = 0; l_I < MAX_RELIC_TALENT; l_I++)
        l_RelicTalentData.Talents[l_I] = RelicTalent[p_Slot][l_I];

    return l_RelicTalentData;
}

void Item::ChangeItemTo(uint32 p_NewItemEntry)
{
    WorldPacket l_Data(Opcodes::SMSG_CHANGE_ITEM, 10);

    l_Data.appendPackGUID(GetOwnerGUID());

    Item::BuildDynamicItemDatas(l_Data, this);

    std::vector<uint32> l_Bonuses = GetAllItemBonuses();

    GetOwner()->_ApplyItemMods(this, GetSlot(), false, true);
    SetEntry(p_NewItemEntry);

    Item::BuildDynamicItemDatas(l_Data, p_NewItemEntry, l_Bonuses);
    GetOwner()->SendDirectMessage(&l_Data);

    ItemTemplate const* l_ItemProto = sObjectMgr->GetItemTemplate(p_NewItemEntry);
    if (l_ItemProto)
    {
        for (uint32 l_I = 0; l_I < MAX_ITEM_PROTO_STATS; ++l_I)
            _bonusData.ItemStatType[l_I] = l_ItemProto->ItemStat[l_I].ItemStatType;

        for (uint32 l_I = 0; l_I < MAX_ITEM_PROTO_STATS; ++l_I)
            _bonusData.ItemStatValue[l_I] = l_ItemProto->ItemStat[l_I].ItemStatValue;;

        for (uint32 l_I = 0; l_I < MAX_ITEM_PROTO_STATS; ++l_I)
            _bonusData.ItemStatAllocation[l_I] = l_ItemProto->ItemStat[l_I].ScalingValue;

        for (uint32 l_I = 0; l_I < MAX_ITEM_PROTO_STATS; ++l_I)
            _bonusData.ItemStatSocketCostMultiplier[l_I] = l_ItemProto->ItemStat[l_I].SocketCostRate;
    }
    GetOwner()->_ApplyItemMods(this, GetSlot(), true, true);

    SetState(ItemUpdateState::ITEM_CHANGED, GetOwner());
}
