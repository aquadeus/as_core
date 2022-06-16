///////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2014-2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ArtifactMgr.h"
#include "gamePCH.h"
#include "GameTables.h"

namespace MS { namespace Artifact {

    Manager::Manager(uint32 p_ArtifactID) : m_AppearanceID(0), m_ArtifactPower(0), m_Owner(nullptr), m_Tier(0)
    {
        m_Entry = sArtifactStore.LookupEntry(p_ArtifactID);
        m_MaxLevel = _CalculateMaxLevel();
    }

    void Manager::SaveToDB(SQLTransaction& p_Trans)
    {
        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ARTIFACT);
        l_Stmt->setUInt32(0, GetOwner()->GetGUIDLow());
        l_Stmt->setUInt32(1, m_Entry->m_ID);
        p_Trans->Append(l_Stmt);

        l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ARTIFACT);
        l_Stmt->setUInt32(0, GetOwner()->GetGUIDLow());
        l_Stmt->setUInt32(1, m_Entry->m_ID);
        l_Stmt->setUInt64(2, m_ArtifactPower);
        l_Stmt->setUInt32(3, m_AppearanceID);
        l_Stmt->setUInt8(4, m_Tier);

        p_Trans->Append(l_Stmt);

        l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ARTIFACT_POWERS);
        l_Stmt->setUInt32(0, GetOwner()->GetGUIDLow());
        l_Stmt->setUInt32(1, m_Entry->m_ID);
        p_Trans->Append(l_Stmt);

        if (!m_Powers.size())
            return;

        std::ostringstream ss;
        ss << "INSERT INTO character_artifact_powers (guid, id, power, rank) VALUES ";

        bool l_First = true;

        for (auto l_Iter : m_Powers)
        {
            if (l_First)
            {
                l_First = false;
            }
            else
            {
                ss << ", ";
            }

            ss << "(" <<  GetOwner()->GetGUIDLow() << ", " << m_Entry->m_ID << ", " << l_Iter.first << ", " << (int)l_Iter.second.PurchasedRank << ")";
        }

        ss << ";";
        p_Trans->Append(ss.str().c_str());
    }

    void Manager::SetOwner(Player* p_Owner)
    {
        m_Owner = p_Owner;
    }

    void Manager::SetItemOwner(Item* p_Owner)
    {
        m_Item = p_Owner;

        if (!m_Item)
            return;

        _CopyPowersIntoDynamicFields();
        m_Item->SetUInt64Value(ITEM_FIELD_ARTIFACT_XP, GetArtifactPower());

        /// Prevent lost of the modifier artifact tier in case the artifact weapon got delete for some reasons
        if (m_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER) == 0 && m_Tier)
        {
            m_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER, 1);
            m_Item->SetState(ItemUpdateState::ITEM_CHANGED);
        }

        /// Set tier to the item artifact tier (usefull for all the artifact that was tier 1 before the implementation of 'tier' column in character_artifact table
        if (m_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER) && !m_Tier)
            m_Tier = m_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER);
    }

    void Manager::LoadFromDB(uint64 p_ArtifactPower, uint32 p_AppearanceId, uint8 p_Tier, std::unordered_map<uint32, ArtifactPowerRanks> const* p_Powers)
    {
        m_ArtifactPower = p_ArtifactPower;
        m_AppearanceID  = p_AppearanceId;
        m_Tier          = p_Tier;

        if (p_Powers)
            m_Powers = std::move(*p_Powers);
    }

    enum eArtifactPower
    {
        NetherlightFortification = 1739
    };

    void Manager::Init()
    {
        for (uint32 l_I = 0; l_I < sArtifactPowerStore.GetNumRows(); ++l_I)
        {
            ArtifactPowerEntry const* l_Entry = sArtifactPowerStore.LookupEntry(l_I);
            if (!l_Entry)
                continue;

            if (l_Entry->ArtifactID == m_Entry->m_ID)
            {
                if ((l_Entry->Flags & ArtifactPowerFlags::IsStart               ///< powers learn in the quest line, we didn't have script them atm
                    || l_Entry->MaxPurchasableRank == 0) && l_Entry->ArtifactTier == 0)   ///< default powers
                    AddPower(l_I, 0, 1, true, true);
            }
        }
    }

    uint32 Manager::GetMaxLevel() const
    {
        return m_MaxLevel;
    }

    uint8 Manager::GetPowerCount() const
    {
        return m_Powers.size();
    }

    void Manager::UpdateAppearance()
    {
        ArtifactAppearanceEntry const* l_Appearance = sArtifactAppearanceStore.LookupEntry(m_AppearanceID);
        if (!l_Appearance || (l_Appearance->UnlockPlayerConditionId && !GetOwner()->EvalPlayerCondition(l_Appearance->UnlockPlayerConditionId, false).first))
            l_Appearance = GenerateDefaultAppearance();

        if (l_Appearance && m_Item)
        {
            m_Item->SetModifier(ITEM_MODIFIER_ARTIFACT_APPEARANCE_ID, l_Appearance->ID);
            m_Item->SetAppearanceModId(l_Appearance->ItemAppearanceModifierId);

            if (m_Item->IsEquipped())
                GetOwner()->SetVisibleItemSlot(m_Item->GetSlot(), m_Item);

            if (Item* l_ChildItem = GetOwner()->GetChildItemByGuid(m_Item->GetChildItem()))
            {
                l_ChildItem->SetModifier(ITEM_MODIFIER_ARTIFACT_APPEARANCE_ID, l_Appearance->ID);
                l_ChildItem->SetAppearanceModId(l_Appearance->ItemAppearanceModifierId);

                if (l_ChildItem->IsEquipped())
                    GetOwner()->SetVisibleItemSlot(l_ChildItem->GetSlot(), l_ChildItem);
            }
        }

        if (l_Appearance && l_Appearance->OverrideShapeshiftDisplayId && l_Appearance->OverrideShapeshiftFormId && GetOwner()->GetShapeshiftForm() == ShapeshiftForm(l_Appearance->OverrideShapeshiftFormId))
            GetOwner()->RestoreDisplayId();
    }

    void Manager::OnEquip()
    {
        Init(); ///< Give initial spell
        m_Owner->SetByteValue(PLAYER_FIELD_NUM_RESPECS, PLAYER_FIELD_BYTES_OFFSET_LIFETIME_MAX_PVP_RANK, GetMaxLevel());

        _LearnSpells();
        UpdateAppearance();

        if (m_Item)
            m_Item->SetUInt64Value(ITEM_FIELD_ARTIFACT_XP, GetArtifactPower());

        UpdateRelicTalents(true);
    }

    void Manager::UpdateRelicTalents(bool p_Apply)
    {
        if (!m_Item)
            return;

        RelicTalentData* l_RelicTalents = m_Item->GetRelicTalentData();
        for (uint8 l_RelicSlot = 0; l_RelicSlot < MAX_GEM_SOCKETS; l_RelicSlot++)
        {
            for (uint8 l_TalentSlot = 0; l_TalentSlot < MAX_RELIC_TALENT; l_TalentSlot++)
            {
                if ((l_RelicTalents->PathMask[l_RelicSlot] & 1 << l_TalentSlot) == 0)
                    continue;

                RelicTalentEntry const* l_RelicTalentEntry = sRelicTalentStore.LookupEntry(l_RelicTalents->RelicTalent[l_RelicSlot][l_TalentSlot]);
                if (!l_RelicTalentEntry)
                    continue;

                if (l_RelicTalentEntry->ArtifactPowerLabel)
                    UpdatePowerBonusRankByType(l_RelicTalentEntry->ArtifactPowerLabel, 1, p_Apply);

                if (l_RelicTalentEntry->ArtifactPowerID)
                    UpdatePowerBonusRankById(l_RelicTalentEntry->ArtifactPowerID, 1, p_Apply);
            }
        }
    }

    ArtifactAppearanceEntry const* Manager::GenerateDefaultAppearance()
    {
        for (uint32 l_AppearanceEntry = 0; l_AppearanceEntry < sArtifactAppearanceStore.GetNumRows(); l_AppearanceEntry++)
        {
            ArtifactAppearanceEntry const* l_ArtifactAppearance = sArtifactAppearanceStore.LookupEntry(l_AppearanceEntry);
            if (!l_ArtifactAppearance)
                continue;

            if (ArtifactAppearanceSetEntry const* l_ArtifactApperanceSet = sArtifactAppearanceSetStore.LookupEntry(l_ArtifactAppearance->ArtifactAppearanceSetId))
            {
                if (GetArtifactId() != l_ArtifactApperanceSet->ArtifactID)
                    continue;

                if (l_ArtifactAppearance->UnlockPlayerConditionId && !GetOwner()->EvalPlayerCondition(l_ArtifactAppearance->UnlockPlayerConditionId, false).first)
                    continue;

                m_AppearanceID = l_ArtifactAppearance->ID;

                return l_ArtifactAppearance;
            }
        }

        return nullptr;
    }


    void Manager::OnUnequip()
    {
        m_Owner->SetByteValue(PLAYER_FIELD_NUM_RESPECS, PLAYER_FIELD_BYTES_OFFSET_LIFETIME_MAX_PVP_RANK, 0);
        _UnlearnSpells();
        UpdateRelicTalents(false);
    }

    void Manager::_LearnSpells()
    {
        for (auto l_Power : m_Powers)
            LearnOrUpdateSpellSpell(l_Power.first);
    }

    void Manager::_UnlearnSpells()
    {
        for (auto l_Power : m_Powers)
        {
            uint8 l_DB2Rank = l_Power.second.CurrentRankWithBonus;
            if (l_DB2Rank >= 1)
                l_DB2Rank -= 1;

            ArtifactPowerRankEntry const* l_Entry = GetArtifactPowerRank(l_Power.first, l_DB2Rank);
            if (!l_Entry)
                continue;

            GetOwner()->removeSpell(l_Entry->SpellID);
        }
    }

    void Manager::_CopyPowersIntoDynamicFields()
    {
        if (!m_Item)
            return;

        m_Item->ClearDynamicValue(ITEM_DYNAMIC_FIELD_ARTIFACT_POWERS);

        for (auto l_Iter : m_Powers)
        {
            m_Item->AddDynamicValue(ITEM_DYNAMIC_FIELD_ARTIFACT_POWERS, l_Iter.first);
            m_Item->AddDynamicValue(ITEM_DYNAMIC_FIELD_ARTIFACT_POWERS, l_Iter.second.ToDynamicField());
        }
    }

    bool Manager::IsArtifactPrimarySpellPowerId(uint32 p_ArtifactPowerId)
    {
        enum PrimarySpellPowerId
        {
            Rogue1      = 346,
            Rogue2      = 1052,
            Rogue3      = 851,
            Mage1       = 290,
            Mage2       = 748,
            Mage3       = 783,
            Warlock1    = 999,
            Warlock2    = 1170,
            Warlock3    = 803,
            Priest1     = 883,
            Priest2     = 834,
            Priest3     = 764,
            Druid1      = 1049,
            Druid2      = 1153,
            Druid3      = 960,
            Druid4      = 125,
            DH1         = 1010,
            DH2         = 1096,
            Shaman1     = 291,
            Shaman2     = 899,
            Shaman3     = 1102,
            Warrior1    = 1136,
            Warrior2    = 984,
            Warrior3    = 91,
            DK1         = 289,
            DK2         = 122,
            DK3         = 149,
            Hunter1     = 881,
            Hunter2     = 307,
            Hunter3     = 1068,
            Monk1       = 1277,
            Monk2       = 931,
            Monk3       = 831,
            Paladin1    = 965,
            Paladin2    = 1120,
            Paladin3    = 40
        };

        switch (p_ArtifactPowerId)
        {
        case PrimarySpellPowerId::Rogue1:
        case PrimarySpellPowerId::Rogue2:
        case PrimarySpellPowerId::Rogue3:
        case PrimarySpellPowerId::Mage1:
        case PrimarySpellPowerId::Mage2:
        case PrimarySpellPowerId::Mage3:
        case PrimarySpellPowerId::Warlock1:
        case PrimarySpellPowerId::Warlock2:
        case PrimarySpellPowerId::Warlock3:
        case PrimarySpellPowerId::Priest1:
        case PrimarySpellPowerId::Priest2:
        case PrimarySpellPowerId::Priest3:
        case PrimarySpellPowerId::Druid1:
        case PrimarySpellPowerId::Druid2:
        case PrimarySpellPowerId::Druid3:
        case PrimarySpellPowerId::Druid4:
        case PrimarySpellPowerId::DH1:
        case PrimarySpellPowerId::DH2:
        case PrimarySpellPowerId::Shaman1:
        case PrimarySpellPowerId::Shaman2:
        case PrimarySpellPowerId::Shaman3:
        case PrimarySpellPowerId::Warrior1:
        case PrimarySpellPowerId::Warrior2:
        case PrimarySpellPowerId::Warrior3:
        case PrimarySpellPowerId::DK1:
        case PrimarySpellPowerId::DK2:
        case PrimarySpellPowerId::DK3:
        case PrimarySpellPowerId::Hunter1:
        case PrimarySpellPowerId::Hunter2:
        case PrimarySpellPowerId::Hunter3:
        case PrimarySpellPowerId::Monk1:
        case PrimarySpellPowerId::Monk2:
        case PrimarySpellPowerId::Monk3:
        case PrimarySpellPowerId::Paladin1:
        case PrimarySpellPowerId::Paladin2:
        case PrimarySpellPowerId::Paladin3:
            return true;

        default:
            return false;
        }

        return false;
    }

    bool Manager::IsArtifactPassiveDamageOrStaminaPerTraitPowerId(uint32 p_ArtifactPowerId)
    {
        enum HiddenStaminaPassive
        {
            Rogue1      = 1211,
            Rogue2      = 1212,
            Rogue3      = 1213,
            Mage1       = 1199,
            Mage2       = 1200,
            Mage3       = 1201,
            Warlock1    = 1217,
            Warlock2    = 1218,
            Warlock3    = 1219,
            Priest1     = 1208,
            Priest2     = 1209,
            Priest3     = 1210,
            Druid1      = 1192,
            Druid2      = 1193,
            Druid3      = 1194,
            Druid4      = 1195,
            DH1         = 1190,
            DH2         = 1191,
            Shaman1     = 1214,
            Shaman2     = 1215,
            Shaman3     = 1216,
            Warrior1    = 1220,
            Warrior2    = 1221,
            Warrior3    = 1222,
            DK1         = 1187,
            DK2         = 1188,
            DK3         = 1189,
            Hunter1     = 1196,
            Hunter2     = 1197,
            Hunter3     = 1198,
            Monk1       = 1202,
            Monk2       = 1203,
            Monk3       = 1204,
            Paladin1    = 1205,
            Paladin2    = 1206,
            Paladin3    = 1207

        };

        enum HiddenDamagePassive
        {
            Priest01     = 1436,
            Priest02     = 1257,
            Druid01      = 1453,
            Druid02      = 1254,
            DH01         = 1452,
            Shaman01     = 1258,
            Warrior01    = 1456,
            DK01         = 1451,
            Monk01       = 1454,
            Monk02       = 1255,
            Paladin01    = 1256,
            Paladin02    = 1455
        };

        switch (p_ArtifactPowerId)
        {
        case HiddenStaminaPassive::Rogue1:
        case HiddenStaminaPassive::Rogue2:
        case HiddenStaminaPassive::Rogue3:
        case HiddenStaminaPassive::Mage1:
        case HiddenStaminaPassive::Mage2:
        case HiddenStaminaPassive::Mage3:
        case HiddenStaminaPassive::Warlock1:
        case HiddenStaminaPassive::Warlock2:
        case HiddenStaminaPassive::Warlock3:
        case HiddenStaminaPassive::Priest1:
        case HiddenStaminaPassive::Priest2:
        case HiddenStaminaPassive::Priest3:
        case HiddenStaminaPassive::Druid1:
        case HiddenStaminaPassive::Druid2:
        case HiddenStaminaPassive::Druid3:
        case HiddenStaminaPassive::Druid4:
        case HiddenStaminaPassive::DH1:
        case HiddenStaminaPassive::DH2:
        case HiddenStaminaPassive::Shaman1:
        case HiddenStaminaPassive::Shaman2:
        case HiddenStaminaPassive::Shaman3:
        case HiddenStaminaPassive::Warrior1:
        case HiddenStaminaPassive::Warrior2:
        case HiddenStaminaPassive::Warrior3:
        case HiddenStaminaPassive::DK1:
        case HiddenStaminaPassive::DK2:
        case HiddenStaminaPassive::DK3:
        case HiddenStaminaPassive::Hunter1:
        case HiddenStaminaPassive::Hunter2:
        case HiddenStaminaPassive::Hunter3:
        case HiddenStaminaPassive::Monk1:
        case HiddenStaminaPassive::Monk2:
        case HiddenStaminaPassive::Monk3:
        case HiddenStaminaPassive::Paladin1:
        case HiddenStaminaPassive::Paladin2:
        case HiddenStaminaPassive::Paladin3:
        case HiddenDamagePassive::Priest01:
        case HiddenDamagePassive::Priest02:
        case HiddenDamagePassive::Druid01:
        case HiddenDamagePassive::Druid02:
        case HiddenDamagePassive::DH01:
        case HiddenDamagePassive::Shaman01:
        case HiddenDamagePassive::Warrior01:
        case HiddenDamagePassive::DK01:
        case HiddenDamagePassive::Monk01:
        case HiddenDamagePassive::Monk02:
        case HiddenDamagePassive::Paladin01:
        case HiddenDamagePassive::Paladin02:
            return true;

        default:
            return false;
        }
    }

    bool Manager::IsArtifactNonPurshaseableTraitPowerId(uint32 p_ArtifactPowerId)
    {
        return (Manager::IsArtifactPrimarySpellPowerId(p_ArtifactPowerId) || Manager::IsArtifactPassiveDamageOrStaminaPerTraitPowerId(p_ArtifactPowerId));
    }


    std::unordered_map<uint32, ArtifactPowerRanks>::iterator Manager::GetPowerReference(uint32 p_Power)
    {
        return m_Powers.find(p_Power);
    }

    std::unordered_map<uint32, ArtifactPowerRanks>::const_iterator Manager::GetPowerReference(uint32 p_Power) const
    {
        return m_Powers.find(p_Power);
    }

    bool Manager::HasPower(uint32 p_ID) const
    {
        return GetPowerReference(p_ID) != m_Powers.end();
    }

    uint32 Manager::_CalculateMaxLevel()
    {
        uint32 l_RankCount = 1; ///< Init spell
        for (uint32 l_I = 0; l_I < sArtifactPowerStore.GetNumRows(); ++l_I)
        {
            ArtifactPowerEntry const* l_Entry = sArtifactPowerStore.LookupEntry(l_I);
            if (!l_Entry)
                continue;

            if (l_Entry->ArtifactID == m_Entry->m_ID)
            {
                l_RankCount += l_Entry->MaxPurchasableRank;
            }
        }

        return l_RankCount;
    }

    bool Manager::AddPower(uint32 p_ID, uint8 p_PurchasedRank, uint8 p_CurrentRankWithBonus, bool p_IgnoreChecks, bool p_IgnoreCost)
    {
        if (!p_CurrentRankWithBonus)
            p_CurrentRankWithBonus = p_PurchasedRank;

        if (!p_IgnoreChecks && GetMaxLevel() <= GetLevel())
            return false;

        if (HasPower(p_ID))
            return false;

        ArtifactPowerEntry const* l_ArtifactPower = sArtifactPowerStore.LookupEntry(p_ID);
        if (!l_ArtifactPower)
            return false;

        if (l_ArtifactPower->ArtifactID  && l_ArtifactPower->ArtifactID != m_Entry->m_ID)
            return false;

        if (!p_IgnoreCost)
        {
            int64 l_Cost = GetCostForNextRank();

            if ((int64)GetArtifactPower() < l_Cost)
                return false;

            ModifyArtifactPower(-l_Cost);
        }

        ArtifactPowerRanks l_PowerRanksInfo;
        l_PowerRanksInfo.CurrentRankWithBonus = p_CurrentRankWithBonus;
        l_PowerRanksInfo.PurchasedRank        = p_PurchasedRank;

        if (m_Item)
        {
            m_Item->AddDynamicValue(ITEM_DYNAMIC_FIELD_ARTIFACT_POWERS, l_ArtifactPower->ID);
            m_Item->AddDynamicValue(ITEM_DYNAMIC_FIELD_ARTIFACT_POWERS, l_PowerRanksInfo.ToDynamicField());
        }

        m_Powers.insert({ l_ArtifactPower->ID, l_PowerRanksInfo });
        LearnOrUpdateSpellSpell(p_ID);
        return true;
    }

    bool Manager::UpgradePower(uint32 p_ID, bool p_IgnoreCost, bool p_ByBonus)
    {
        if (GetMaxLevel() <= GetLevel())
            return false;

        ArtifactPowerEntry const* l_ArtifactPower = sArtifactPowerStore.LookupEntry(p_ID);
        if (!l_ArtifactPower)
            return false;

        uint32 l_MaxRank = l_ArtifactPower->MaxPurchasableRank;

        /// Don't allow to upgrade the 7.0.3 paragon trait
        if (l_ArtifactPower->ArtifactTier == 0 && l_ArtifactPower->Flags == 5)
            return false;

        /// Tier 1 allow some powers from the tier 0 to be upgraded to rank 4 instead 3
        if (l_ArtifactPower->Flags & ArtifactPowerFlags::Tier1Upgrade && l_ArtifactPower->ArtifactTier == 0 && m_Tier > 0)
            l_MaxRank++;

        if (!p_ByBonus && GetPurchasedRank(p_ID) >= l_MaxRank)
            return false;

        if (l_ArtifactPower->ArtifactID && l_ArtifactPower->ArtifactID != m_Entry->m_ID)
            return false;

        if (!p_IgnoreCost)
        {
            int64 l_Cost = GetCostForNextRank();

            if ((int64)GetArtifactPower() < l_Cost)
                return false;

            ModifyArtifactPower(-l_Cost);
        }

        uint8 l_Count = GetPowerCount();
        for (uint32 l_I = 0; l_I < l_Count; ++l_I)
        {
            if (m_Item && m_Item->GetDynamicValue(ITEM_DYNAMIC_FIELD_ARTIFACT_POWERS, l_I * 2) == p_ID)
            {
                if (!p_ByBonus)
                    GetPowerReference(p_ID)->second.PurchasedRank++;

                GetPowerReference(p_ID)->second.CurrentRankWithBonus++;

                m_Item->SetDynamicValue(ITEM_DYNAMIC_FIELD_ARTIFACT_POWERS, (l_I * 2) + 1, GetPowerReference(p_ID)->second.ToDynamicField());

                LearnOrUpdateSpellSpell(p_ID);
                return true;
            }
        }
        return false;
    }

    void Manager::RemoveOrDowngradeTrait(uint32 p_ID, bool p_ForceRemove)
    {
        ArtifactPowerEntry const* l_ArtifactPower = sArtifactPowerStore.LookupEntry(p_ID);
        if (!l_ArtifactPower)
            return;

        if (l_ArtifactPower->ArtifactID && l_ArtifactPower->ArtifactID != m_Entry->m_ID)
            return;

        if (!HasPower(p_ID))
            return;

        uint16 l_CurrentRankWithBonus = GetPowerReference(p_ID)->second.CurrentRankWithBonus;
        l_CurrentRankWithBonus--;

        /// Remove case (relics on respec)
        if (!l_CurrentRankWithBonus || p_ForceRemove)
        {
            m_Powers.erase(l_ArtifactPower->ID);

            ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(p_ID, l_CurrentRankWithBonus);
            if (l_RankEntry)
            {
                GetOwner()->removeSpell(l_RankEntry->SpellID);
                if (m_Item && l_RankEntry->ItemBonusListId)
                    m_Item->RemoveServerSideBonus(l_RankEntry->ItemBonusListId);
            }
            LearnOrUpdateSpellSpell(p_ID);
        }
        /// Downgrade case (can only happen with relics)
        else
        {
            GetPowerReference(p_ID)->second.CurrentRankWithBonus--;
            LearnOrUpdateSpellSpell(p_ID);
        }

        /// Update powers client-side
        _CopyPowersIntoDynamicFields();
    }

    void Manager::UpdatePowerBonusRankByType(uint32 p_RelicType, uint16 p_Bonus, bool p_Apply)
    {
        for (uint32 l_I = 0; l_I < sArtifactPowerStore.GetNumRows(); ++l_I)
        {
            ArtifactPowerEntry const* l_Entry = sArtifactPowerStore.LookupEntry(l_I);
            if (!l_Entry)
                continue;

            if (l_Entry->RelicType == p_RelicType)
            {
                for (uint16 l_Upgrade = 0; l_Upgrade < p_Bonus; l_Upgrade++)
                {
                    if (p_Apply)
                        AddOrUpgradeTrait(l_Entry->ID, true, true);
                    else
                        RemoveOrDowngradeTrait(l_Entry->ID);
                }
            }
        }
    }

    void Manager::UpdatePowerBonusRankById(uint32 p_Id, uint16 p_Bonus, bool p_Apply)
    {
        for (uint32 l_I = 0; l_I < sArtifactPowerStore.GetNumRows(); ++l_I)
        {
            ArtifactPowerEntry const* l_Entry = sArtifactPowerStore.LookupEntry(l_I);
            if (!l_Entry)
                continue;

            if (l_Entry->ID == p_Id)
            {
                for (uint16 l_Upgrade = 0; l_Upgrade < p_Bonus; l_Upgrade++)
                {
                    if (p_Apply)
                        AddOrUpgradeTrait(l_Entry->ID, true, true);
                    else
                        RemoveOrDowngradeTrait(l_Entry->ID);
                }
            }
        }
    }

    void Manager::LearnOrUpdateSpellSpell(ArtifactPowerRankEntry const* p_RankEntry) const
    {
        Player* l_Owner = GetOwner();
        if (!l_Owner)
            return;

        uint8 l_Rank = p_RankEntry->Rank;
        while (ArtifactPowerRankEntry const* l_UpperPowerRank = GetArtifactPowerRank(p_RankEntry->ArtifactPowerID, ++l_Rank))
        {
            if ((l_UpperPowerRank->SpellID != p_RankEntry->SpellID) && l_Owner->HasSpell(l_UpperPowerRank->SpellID))
                l_Owner->removeSpell(l_UpperPowerRank->SpellID, false, false);

            if (m_Item && l_UpperPowerRank->ItemBonusListId)
                m_Item->RemoveServerSideBonus(l_UpperPowerRank->ItemBonusListId);

            if (l_Rank > 20)
                break;
        }
        /// 7.1.5 - Build 23420 Added removal of lower trait ranks.
        l_Rank = p_RankEntry->Rank;
        while (ArtifactPowerRankEntry const* l_LowerPowerRank = GetArtifactPowerRank(p_RankEntry->ArtifactPowerID, --l_Rank))
        {
            if ((l_LowerPowerRank->SpellID != p_RankEntry->SpellID) && l_Owner->HasSpell(l_LowerPowerRank->SpellID))
                l_Owner->removeSpell(l_LowerPowerRank->SpellID, false, false);

            if (m_Item && l_LowerPowerRank->ItemBonusListId)
                m_Item->RemoveServerSideBonus(l_LowerPowerRank->ItemBonusListId);
        }

        if (!l_Owner->HasSpell(p_RankEntry->SpellID))
            l_Owner->learnSpell(p_RankEntry->SpellID, false, false, true);

        if (m_Item && p_RankEntry->ItemBonusListId)
            m_Item->AddServerSideBonus(p_RankEntry->ItemBonusListId);

        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_RankEntry->SpellID);
        if (!l_SpellInfo)
            return;

        if (l_SpellInfo->IsPassive())
        {
            if (Aura* l_Aura = l_Owner->GetAura(l_SpellInfo->Id))
            {
                for (uint8 l_I = 0; l_I < l_Aura->GetEffectCount(); ++l_I)
                {
                    if (AuraEffect* l_AurEff = l_Aura->GetEffect(l_I))
                    {
                        if (!l_AurEff)
                            continue;

                        if (l_Aura->HasEffect(l_AurEff->GetEffIndex()) && G3D::fuzzyNe(p_RankEntry->AuraPointsOverride, 0.0f))
                        {
                            l_AurEff->ChangeAmount(p_RankEntry->AuraPointsOverride, true, true, true, p_RankEntry->AuraPointsOverride);
                            l_AurEff->SetCanBeRecalculated(true);
                        }
                        else
                            l_AurEff->RecalculateAmount();
                    }
                }
                l_Aura->SetNeedClientUpdateForTargets();
            }
        }
    }

    void Manager::LearnOrUpdateSpellSpell(uint32 p_ArtifactPowerId)
    {
        ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(p_ArtifactPowerId, m_Owner->GetRankOfArtifactPowerId(p_ArtifactPowerId) - 1);

        if (l_RankEntry)
            LearnOrUpdateSpellSpell(l_RankEntry);
    }

    uint8 Manager::GetPurchasedRank(uint32 p_PowerID)
    {
        std::unordered_map<uint32, ArtifactPowerRanks>::iterator l_Ref = GetPowerReference(p_PowerID);
        return l_Ref != m_Powers.end() ? l_Ref->second.PurchasedRank : 0;
    }

    uint8 Manager::GetCurrentRankWithBonus(uint32 p_PowerID)
    {
        std::unordered_map<uint32, ArtifactPowerRanks>::iterator l_Ref = GetPowerReference(p_PowerID);
        return l_Ref != m_Powers.end() ? l_Ref->second.CurrentRankWithBonus : 0;
    }

    void Manager::AddOrUpgradeTrait(uint32 p_ID, bool p_IgnoreCost, bool p_ByBonus)
    {
        ArtifactPowerEntry const* l_ArtifactPower = sArtifactPowerStore.LookupEntry(p_ID);
        if (!l_ArtifactPower)
            return;

        if (HasPower(p_ID))
            UpgradePower(p_ID, p_IgnoreCost, p_ByBonus);
        else
            AddPower(p_ID, p_ByBonus ? 0 : 1, p_ByBonus ? 1 : 0, p_ByBonus, p_IgnoreCost);

        GetOwner()->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_ARTIFACT_TRAITS_UNLOCKED, GetLevel());

        // Complete quests, requires artifact trait upgrade
        std::map<uint32, uint32> m_quest_criteria
        {  // quest  criteria
            { 42672, 48616 },
            { 42613, 48523 },
            { 42600, 48521 },
            { 42517, 48406 },
            { 42481, 48430 },
            { 42379, 48228 },
            { 41125, 46860 },
            { 41064, 46840 },
            { 41047, 46833 },
            { 41017, 46798 },
            { 40995, 46763 },
            { 40821, 53142 },
            { 40698, 46403 },
            { 40651, 46473 },
            { 40276, 47008 },
            { 39761, 46595 },
            { 39722, 45896 },
            { 39192, 46331 },
            { 43263, 49441 },
            { 43260, 49433 },
            { 46744, 58025 },
        };

        for (auto l_quest_criteria : m_quest_criteria)
            if (m_Owner->HasQuest(l_quest_criteria.first))
                m_Owner->QuestObjectiveSatisfy(l_quest_criteria.second, 1, QUEST_OBJECTIVE_TYPE_CRITERIA_TREE, m_Owner->GetGUID());


        /// Update to tier 1 when we learn the paragon trait of the tier 0
        if (l_ArtifactPower->ArtifactTier == 0 && l_ArtifactPower->Flags == 5 && m_Tier == 0)
        {
            m_Tier = 1;
            m_Item->SetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER, 1);
            m_Item->SetState(ItemUpdateState::ITEM_CHANGED, GetOwner());
        }

        GetOwner()->RecalculateAllAurasAmount(true);
    }

    bool Manager::ChangeAppearance(uint32 p_ArtifactAppearanceID, bool p_IgnoreConditionCheck /* = false */)
    {
        ArtifactAppearanceEntry const* l_Appearance = sArtifactAppearanceStore.LookupEntry(p_ArtifactAppearanceID);

        if (!l_Appearance)
            return false;

        if (l_Appearance->UnlockPlayerConditionId && !GetOwner()->EvalPlayerCondition(l_Appearance->UnlockPlayerConditionId, false).first)
            return false;

        m_AppearanceID = l_Appearance->ID;

        UpdateAppearance();

        return true;
    }

    void Manager::ModifyArtifactPower(int64 p_Value, uint32 p_ArtifactCategoryId /*= 0*/, uint32 p_ArtifactKnowledgeLevel /*=0*/)
    {
        if (GetOwner()->GetSession()->IsPremium() && p_Value > 0)
            p_Value *= sWorld->getRate(RATE_XP_QUEST_PREMIUM);

        /// Artifact knowledge gain rates - made for Fun server
        if (p_Value > 0 && sWorld->getRate(Rates::RATE_GAIN_ARTIFACT_KNOWLEDGE) > 1.0f)
            p_Value *= sWorld->getRate(Rates::RATE_GAIN_ARTIFACT_KNOWLEDGE);

        if (p_ArtifactCategoryId)
        {
            if (ArtifactCategoryEntry const* artifactCategory = sArtifactCategoryStore.LookupEntry(p_ArtifactCategoryId))
            {
                uint32 l_PlayerArtifactKnowledge = GetOwner()->getLevel() >= 110 ? sWorld->GetCurrentArtifactKnowledge() : 0;
                uint32 artifactKnowledgeLevel = p_ArtifactKnowledgeLevel ? p_ArtifactKnowledgeLevel : l_PlayerArtifactKnowledge;

                if (auto l_ArtifactKnowledge = g_ArtifactKnowledgeMultiplier.LookupEntry(artifactKnowledgeLevel))
                    p_Value = int64(p_Value * l_ArtifactKnowledge->Multiplier);

                if (p_Value >= 5000)
                    p_Value = 50 * (p_Value / 50);
                else if (p_Value >= 1000)
                    p_Value = 25 * (p_Value / 25);
                else if (p_Value >= 50)
                    p_Value = 5 * (p_Value / 5);
            }
        }

        m_ArtifactPower = std::max((int64)0, (int64)m_ArtifactPower + p_Value);

        if (m_Item)
        {
            m_Item->SetUInt64Value(ITEM_FIELD_ARTIFACT_XP, m_ArtifactPower);

            if (p_Value < 0)
                return;

            WorldPacket l_Data(SMSG_ARTIFACT_XP_GAIN, 18 + 4);
            l_Data.appendPackGUID(m_Item->GetGUID());
            l_Data << int64(p_Value);
            m_Owner->SendDirectMessage(&l_Data);
        }
    }

    void Manager::SetArtifactPower(uint64 p_Value)
    {
        m_ArtifactPower = p_Value;

        if (m_Item)
        {
            m_Item->SetUInt64Value(ITEM_FIELD_ARTIFACT_XP, m_ArtifactPower);

            WorldPacket l_Data(SMSG_ARTIFACT_XP_GAIN, 18 + 4);
            l_Data.appendPackGUID(m_Item->GetGUID());
            l_Data << int64(p_Value);
            m_Owner->SendDirectMessage(&l_Data);
        }
    }

    uint64 Manager::GetArtifactPower() const
    {
        return m_ArtifactPower;
    }

    uint32 Manager::GetArtifactId() const
    {
        return m_Entry ? m_Entry->m_ID : 0;
    }

    Manager* Manager::GenerateNew(uint32 p_ArtifactID)
    {
        return sArtifactStore.LookupEntry(p_ArtifactID) ? new Manager(p_ArtifactID) : nullptr;
    }

    ItemTemplate const* Manager::GetItemTemplate()
    {
        return m_Item ? m_Item->GetTemplate() : nullptr;
    }

    uint8 Manager::GetLevel() const
    {
        uint8 l_Level = 0;

        for (auto l_Iter : m_Powers)
            l_Level += l_Iter.second.PurchasedRank;

        return l_Level;
    }

    uint64 Manager::GetCostForNextRank() const
    {
        ArtifactLevelXPTableEntry const* l_Entry = g_ArtifactLevelXP.LookupEntry(GetLevel() + 1);

        if (!l_Entry || !m_Item)
            return 0xEFFFFFFF;

        return m_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER) == 1 ? l_Entry->XP2 : l_Entry->XP;
    }

    uint64 Manager::GetCostForRank(uint8 p_Rank, bool p_ForceTier2 /*= false*/) const
    {
        ArtifactLevelXPTableEntry const* l_Entry = g_ArtifactLevelXP.LookupEntry(p_Rank);

        if (!l_Entry || !m_Item)
            return 0;

        return ((m_Item->GetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER) == 1) || p_ForceTier2) ? l_Entry->XP2 : l_Entry->XP;
    }

    void Manager::ResetPowers()
    {
        std::vector<uint32> l_PowerIdToClear;
        for (auto l_Power : m_Powers)
        {
            ArtifactPowerEntry const* l_Entry = sArtifactPowerStore.LookupEntry(l_Power.first);
            if (!l_Entry)
                continue;

            if ((l_Entry->Flags & ArtifactPowerFlags::IsStart && l_Entry->MaxPurchasableRank == 0) || l_Entry->MaxPurchasableRank == 0)
                continue;

            l_PowerIdToClear.push_back(l_Power.first);
        }

        for (uint32 l_PowerId : l_PowerIdToClear)
            RemoveOrDowngradeTrait(l_PowerId, true);

        /// Refresh artifacts traits client-side (need to send 0)
        for (uint32 l_PowerId : l_PowerIdToClear)
        {
            if (m_Item)
            {
                m_Item->AddDynamicValue(ITEM_DYNAMIC_FIELD_ARTIFACT_POWERS, l_PowerId);
                m_Item->AddDynamicValue(ITEM_DYNAMIC_FIELD_ARTIFACT_POWERS, 0);
            }
        }
    }

    void Manager::DumpPowerFields()
    {
        if (!m_Item)
            return;

        auto l_Fields = m_Item->GetDynamicValues(ITEM_DYNAMIC_FIELD_ARTIFACT_POWERS);

        for (uint32 l_I = 0; l_I < l_Fields.size() / 2; ++l_I)
            printf("UF: %i %X\n", l_Fields[l_I * 2], l_Fields[(l_I * 2) + 1]);

        for (auto l_Power : m_Powers)
            printf("Map: %i %i\n", l_Power.first, l_Power.second.PurchasedRank);
    }
}   ///< namespace Artifact
}   ///< namespace MS
