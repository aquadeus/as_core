////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "GarrisonMgr.hpp"
#include "Player.h"
#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "ObjectAccessor.h"
#include "CreatureAI.h"
#include "DisableMgr.h"

namespace MS { namespace Garrison
{
    /// Add follower
    bool Manager::AddFollower(uint32 p_FollowerID)
    {
        auto l_TempFollower = GetFollower(p_FollowerID);
        if (l_TempFollower && !(l_TempFollower->Flags & FollowerFlags::Type::isLimitedUse))
        {
            if (l_TempFollower->FollowerID != 0)
                return false;
        }
        else
        {
            if (GetMaxCount(p_FollowerID) && (GetFollowerCount(p_FollowerID) >= GetMaxCount(p_FollowerID)))
                return false;
        }

        GarrFollowerEntry const* l_Entry = sGarrFollowerStore.LookupEntry(p_FollowerID);

        if (!l_Entry)
            return false;

        GarrisonFollower l_Follower;
        l_Follower.DatabaseID           = sObjectMgr->GetNewGarrisonFollowerID();
        l_Follower.FollowerID           = p_FollowerID;
        l_Follower.Level                = l_Entry->Level;
        l_Follower.XP                   = 0;
        l_Follower.Durability           = 1;
        l_Follower.Quality              = l_Entry->Quality;
        l_Follower.ItemLevelArmor       = l_Entry->ItemLevelArmor;
        l_Follower.ItemLevelWeapon      = l_Entry->ItemLevelWeapon;
        l_Follower.CurrentBuildingID    = 0;
        l_Follower.CurrentMissionID     = 0;
        l_Follower.ZoneSupportSpellID   = 0;
        l_Follower.Flags                = 0;
        l_Follower.GarrClassSpecID      = m_Owner->getFaction() == FactionIndex::Alliance ? l_Entry->AllianceGarrClassSecID : l_Entry->HordeGarrClassSecID;

        /// Handle Quality auto-update case
        if (l_Entry->MaxDurability == 0 && l_Entry->GarrisonType != GarrisonType::GarrisonBrokenIsles)  ///< Troops doesn't upgrade
        {
            float l_Roll = frand(0.0f, 100.0f);

            /// If Follower is uncommon & chance match, set rare quality
            if (l_Follower.Quality == FollowerQualities::FOLLOWER_QUALITY_UNCOMMON && l_Roll > gQuestRewardBonusRareChanceRange[0] && l_Roll < gQuestRewardBonusRareChanceRange[1])
                l_Follower.Quality = FollowerQualities::FOLLOWER_QUALITY_RARE;

            if (l_Roll > gQuestRewardBonusEpicChanceRange[0] && l_Roll < gQuestRewardBonusEpicChanceRange[1])
                l_Follower.Quality = FollowerQualities::FOLLOWER_QUALITY_EPIC;
        }

        if (l_Entry->MaxDurability)
        {
            l_Follower.Durability    = l_Entry->MaxDurability;
            l_Follower.Flags        |= FollowerFlags::isLimitedUse;
        }

        if (l_Entry->GarrisonType == GarrisonType::GarrisonBrokenIsles)
        {
            for (uint32 l_I = 0; l_I < sGarrFollSupportSpellStore.GetNumRows(); ++l_I)
            {
                const GarrFollSupportSpellEntry* l_SpellEntry = sGarrFollSupportSpellStore.LookupEntry(l_I);

                if (!l_SpellEntry || l_SpellEntry->FollowerID != p_FollowerID)
                    continue;

                l_Follower.ZoneSupportSpellID = l_SpellEntry->Spells[GetGarrisonFactionIndex()];
                l_I = sGarrFollSupportSpellStore.GetNumRows();
            }
        }

        GenerateFollowerAbilities(l_Follower);

        std::ostringstream l_Abilities;

        for (uint32 l_Y = 0; l_Y < l_Follower.Abilities.size(); ++l_Y)
            l_Abilities << l_Follower.Abilities[l_Y] << ' ';

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GARRISON_FOLLOWER);

        uint32 l_Index = 0;
        l_Stmt->setUInt32(l_Index++, l_Follower.DatabaseID);
        l_Stmt->setUInt32(l_Index++, m_ID);
        l_Stmt->setUInt32(l_Index++, l_Follower.FollowerID);
        l_Stmt->setUInt32(l_Index++, l_Follower.Level);
        l_Stmt->setUInt32(l_Index++, l_Follower.XP);
        l_Stmt->setUInt32(l_Index++, l_Follower.Durability);
        l_Stmt->setUInt32(l_Index++, l_Follower.Quality);
        l_Stmt->setUInt32(l_Index++, l_Follower.ItemLevelArmor);
        l_Stmt->setUInt32(l_Index++, l_Follower.ItemLevelWeapon);
        l_Stmt->setUInt32(l_Index++, l_Follower.CurrentMissionID);
        l_Stmt->setUInt32(l_Index++, l_Follower.CurrentBuildingID);
        l_Stmt->setString(l_Index++, l_Abilities.str());
        l_Stmt->setUInt32(l_Index++, l_Follower.ZoneSupportSpellID);
        l_Stmt->setUInt32(l_Index++, l_Follower.Flags);
        l_Stmt->setString(l_Index++, l_Follower.ShipName);

        CharacterDatabase.AsyncQuery(l_Stmt);

        m_Followers.push_back(l_Follower);

        bool l_ShouldNotify = false;

        if (l_Follower.GetEntry()->GarrisonType == GarrisonType::GarrisonDraenor && GetDraenorGarrison())
            l_ShouldNotify = true;
        else if (l_Follower.GetEntry()->GarrisonType == GarrisonType::GarrisonBrokenIsles && GetBrokenIslesGarrison())
            l_ShouldNotify = true;

        if (l_ShouldNotify)
        {
            WorldPacket l_AddFollowerResult(SMSG_GARRISON_ADD_FOLLOWER_RESULT, 64);
            l_AddFollowerResult << uint32(l_Follower.GetEntry()->GarrisonType);
            l_AddFollowerResult << uint32(ResultEnum::SUCCESS);
            l_Follower.Write(l_AddFollowerResult);

            m_Owner->SendDirectMessage(&l_AddFollowerResult);
        }

        m_Owner->UpdateCriteria(CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER);

        m_Owner->UpdateCriteria(CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER_2, p_FollowerID);

        return true;
    }
    /// Add follower
    bool Manager::AddFollower(GarrisonFollower p_Follower)
    {
        GarrFollowerEntry const* l_Entry = sGarrFollowerStore.LookupEntry(p_Follower.FollowerID);

        if (!l_Entry)
            return false;

        std::ostringstream l_Abilities;

        for (uint32 l_Itr = 0; l_Itr < p_Follower.Abilities.size(); ++l_Itr)
            l_Abilities << p_Follower.Abilities[l_Itr] << ' ';

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GARRISON_FOLLOWER);

        uint32 l_Index = 0;
        l_Stmt->setUInt32(l_Index++, p_Follower.DatabaseID);
        l_Stmt->setUInt32(l_Index++, m_ID);
        l_Stmt->setUInt32(l_Index++, p_Follower.FollowerID);
        l_Stmt->setUInt32(l_Index++, p_Follower.Level);
        l_Stmt->setUInt32(l_Index++, p_Follower.XP);
        l_Stmt->setUInt32(l_Index++, p_Follower.Durability);
        l_Stmt->setUInt32(l_Index++, p_Follower.Quality);
        l_Stmt->setUInt32(l_Index++, p_Follower.ItemLevelArmor);
        l_Stmt->setUInt32(l_Index++, p_Follower.ItemLevelWeapon);
        l_Stmt->setUInt32(l_Index++, p_Follower.CurrentMissionID);
        l_Stmt->setUInt32(l_Index++, p_Follower.CurrentBuildingID);
        l_Stmt->setString(l_Index++, l_Abilities.str());
        l_Stmt->setUInt32(l_Index++, p_Follower.ZoneSupportSpellID);
        l_Stmt->setUInt32(l_Index++, p_Follower.Flags);
        l_Stmt->setString(l_Index++, p_Follower.ShipName);

        CharacterDatabase.AsyncQuery(l_Stmt);

        m_Followers.push_back(p_Follower);

        WorldPacket l_AddFollowerResult(SMSG_GARRISON_ADD_FOLLOWER_RESULT, 64);
        l_AddFollowerResult << uint32(l_Entry->GarrisonType);
        l_AddFollowerResult << uint32(ResultEnum::SUCCESS);
        p_Follower.Write(l_AddFollowerResult);

        m_Owner->SendDirectMessage(&l_AddFollowerResult);

        if (p_Follower.Quality == FollowerQualities::FOLLOWER_QUALITY_RARE)
        {
            m_Owner->UpdateCriteria(CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER);
        }

        m_Owner->UpdateCriteria(CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER_2, p_Follower.FollowerID);

        return true;
    }
    /// Removes the follower
    bool Manager::RemoveFollower(uint32 p_DatabaseID, bool p_Force /* = false */)
    {
        auto l_Itr = std::find_if(m_Followers.begin(), m_Followers.end(), [p_DatabaseID](GarrisonFollower l_Follower) -> bool
        {
            return l_Follower.DatabaseID == p_DatabaseID;
        });

        if (l_Itr == m_Followers.end())
            return false;

        /// Can never remove a follower on a mission
        if (l_Itr->CurrentMissionID)
            return false;

        if (!p_Force && (l_Itr->GetEntry()->Type == eGarrisonFollowerTypeID::FollowerTypeIDShipyard && GetTotalFollowerCount(eGarrisonFollowerTypeID::FollowerTypeIDShipyard) < GetFollowerSoftCap(eGarrisonFollowerTypeID::FollowerTypeIDShipyard)))
            return false;

        WorldPacket l_Data(SMSG_GARRISON_REMOVE_FOLLOWER_RESULT, 8 + 4 + 4);
        l_Data << uint64(p_DatabaseID);
        l_Data << uint32(l_Itr->GetEntry()->GarrisonType);
        l_Data << uint32(ResultEnum::SUCCESS);  ///< Guessed
        l_Data << uint32(ResultEnum::SUCCESS);  ///< Guessed only checked if NPC is a ship -- GARRISON_SHIP_DECOMMISSION_SUCCEEDED
        m_Owner->SendDirectMessage(&l_Data);

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_FOLLOWER);
        l_Stmt->setUInt32(0, p_DatabaseID);
        CharacterDatabase.AsyncQuery(l_Stmt);

        m_Followers.erase(l_Itr);

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    void Manager::DespawnBodyguardIfNeeded()
    {
        std::vector<GarrisonMission> l_ActiveMissions = GetMissions(GarrisonType::Type::GarrisonBrokenIsles);
        for (GarrisonMission l_Mission : l_ActiveMissions)
        {
            GarrMissionEntry const* l_MissionEntry = sGarrMissionStore.LookupEntry(l_Mission.MissionID);
            if (!l_MissionEntry || !(l_MissionEntry->Flags & Mission::Flags::IsZoneSupport))
                continue;

            std::vector<GarrisonFollower> l_Followers = GetFollowers(GarrisonType::Type::GarrisonBrokenIsles);
            for (GarrisonFollower l_Follower : l_Followers)
            {
                if (l_Follower.CurrentMissionID != l_Mission.MissionID)
                    continue;

                SpellInfo const* l_ZoneSupportSpellInfo = sSpellMgr->GetSpellInfo(l_Follower.ZoneSupportSpellID);
                if (!l_ZoneSupportSpellInfo)
                    return;

                uint32 l_SummonSpellId = l_ZoneSupportSpellInfo->Effects[EFFECT_0].TriggerSpell;

                SpellInfo const* l_SummonSpellInfo = sSpellMgr->GetSpellInfo(l_SummonSpellId);
                if (!l_SummonSpellInfo)
                    return;

                if (l_SummonSpellInfo->Effects[SpellEffIndex::EFFECT_0].Effect != SpellEffects::SPELL_EFFECT_SUMMON)
                    return;

                Creature* l_Bodyguard = nullptr;
                for (Unit* l_Summon : m_Owner->m_Controlled)
                {
                    if (l_Summon->GetEntry() == l_SummonSpellInfo->Effects[SpellEffIndex::EFFECT_0].MiscValue)
                    {
                        l_Bodyguard = l_Summon->ToCreature();
                        break;
                    }
                }

                if (!l_Bodyguard)
                    return;

                l_Bodyguard->DespawnOrUnsummon();
            }
        }
    }

    /// Get Current Combat Ally Follower if any
    GarrisonFollower* Manager::GetCombatAllyFollower()
    {
        for (GarrisonFollower& l_Follower : m_Followers)
        {
            GarrMissionEntry const* l_MissionEntry = sGarrMissionStore.LookupEntry(l_Follower.CurrentMissionID);
            if (!l_MissionEntry || !(l_MissionEntry->Flags & Mission::Flags::IsZoneSupport))
                continue;

            return &l_Follower;
        }

        return nullptr;
    }

    /// Get followers
    std::vector<GarrisonFollower> Manager::GetFollowers(GarrisonType::Type p_Type) const
    {
        std::vector<GarrisonFollower> l_Result;

        for (uint32 l_I = 0; l_I < m_Followers.size(); ++l_I)
        {
            if (m_Followers[l_I].GetEntry()->GarrisonType == p_Type)
                l_Result.push_back(m_Followers[l_I]);
        }

        return l_Result;
    }
    /// Get followers db2 list with abilities
    std::list<GarrisonFollower> Manager::GetFollowersWithAbility(uint32 p_AbilityID, bool p_IsTrait)
    {
        std::list<GarrisonFollower> l_FinalFollowers;
        std::vector<uint32> l_FollowerIDs;
        std::vector<uint32> l_PotentialFollowers;

        for (uint32 l_I = 0; l_I < sGarrFollowerXAbilityStore.GetNumRows(); ++l_I)
        {
            GarrFollowerEntry const* l_FollowerEntry = sGarrFollowerStore.LookupEntry(l_I);

            if (l_FollowerEntry != nullptr)
                l_FollowerIDs.push_back(l_FollowerEntry->ID);
        }

        l_FollowerIDs.erase(std::remove_if(l_FollowerIDs.begin(), l_FollowerIDs.end(), [this, p_AbilityID, &l_PotentialFollowers](uint32 p_FollowerID) -> bool
        {
            auto l_Found = std::find_if(m_Followers.begin(), m_Followers.end(), [this, p_FollowerID](GarrisonFollower p_Follower) -> bool
            {
                if (p_Follower.FollowerID == p_FollowerID)
                    return true;

                return false;
            });

            if (l_Found != m_Followers.end())
                return true;

            GarrFollowerEntry const* l_Entry = sGarrFollowerStore.LookupEntry(p_FollowerID);

            if (l_Entry == nullptr || l_Entry->Quality >= FollowerQualities::FOLLOWER_QUALITY_EPIC || l_Entry->Level > 90 || strcmp(l_Entry->HordeSourceText->Str[LOCALE_enUS], "") == 0)
                return true;

            l_PotentialFollowers.push_back(p_FollowerID);

            for (uint32 l_I = 0; l_I < sGarrFollowerXAbilityStore.GetNumRows(); ++l_I)
            {
                GarrFollowerXAbilityEntry const* l_AbilityEntry = sGarrFollowerXAbilityStore.LookupEntry(l_I);

                /// TEAMID server side is inverted with DBCs 0/1 values, so real condition for l_AbilityEntry->FactionIndex != m_Owner->GetTeamId() is ==
                if (l_AbilityEntry != nullptr && l_AbilityEntry->FollowerID == p_FollowerID && l_AbilityEntry->AbilityID == p_AbilityID && l_AbilityEntry->FactionIndex != m_Owner->GetTeamId())
                    return false;
            }

            return true;
        }), l_FollowerIDs.end());

        for (uint32 l_FollowerID : l_FollowerIDs)
        {
            l_FinalFollowers.push_back(GenerateNewFollowerEntity(l_FollowerID));
            l_PotentialFollowers.erase(std::remove(l_PotentialFollowers.begin(), l_PotentialFollowers.end(), l_FollowerID), l_PotentialFollowers.end());
        }

        if (l_PotentialFollowers.empty())
            return l_FinalFollowers;

        while (l_FinalFollowers.size() < 3)
        {
            GarrisonFollower l_Follower = GenerateNewFollowerEntity(l_PotentialFollowers[urand(0, l_PotentialFollowers.size() - 1)]);
            uint32 l_AbilityGenerated = 0;

            for (auto l_Itr = l_Follower.Abilities.begin(); l_Itr != l_Follower.Abilities.end(); ++l_Itr)
            {
                using namespace MS::Garrison::GDraenor::GarrisonAbilities;

                if (!p_IsTrait)
                {
                    std::vector<uint32> l_PotentialAbilities;
                    for (uint32 l_EffectID = 0; l_EffectID < sGarrAbilityEffectStore.GetNumRows(); ++l_EffectID)
                    {
                        GarrAbilityEffectEntry const* l_Effect = sGarrAbilityEffectStore.LookupEntry(l_EffectID);

                        if (!l_Effect)
                            continue;

                        if (l_Effect->CounterMechanicTypeID == p_AbilityID)
                            l_PotentialAbilities.push_back(l_Effect->AbilityID);
                    }

                    if (!l_PotentialAbilities.empty())
                        l_AbilityGenerated = l_PotentialAbilities[urand(0, l_PotentialAbilities.size() - 1)];

                    l_Itr = l_Follower.Abilities.erase(l_Itr);
                    break;
                }

                if (p_IsTrait && std::find(g_FollowerTraits.begin(), g_FollowerTraits.end(), *l_Itr) != g_FollowerTraits.end())
                {
                    l_Itr = l_Follower.Abilities.erase(l_Itr);
                    break;
                }
            }

            if (l_AbilityGenerated)
                l_Follower.Abilities.push_back(l_AbilityGenerated);
            else
                l_Follower.Abilities.push_back(p_AbilityID);

            l_FinalFollowers.push_back(l_Follower);
        }

        return l_FinalFollowers;
    }
    /// Get follower
    GarrisonFollower* Manager::GetFollower(uint32 p_FollowerID)
    {
        for (uint32 l_I = 0; l_I < m_Followers.size(); l_I++)
        {
            if (m_Followers[l_I].FollowerID == p_FollowerID)
                return &m_Followers[l_I];
        }

        return nullptr;
    }
    /// Get follower
    GarrisonFollower* Manager::GetFollowerWithDatabaseID(uint32 p_FollowerDatabaseID)
    {
        for (uint32 l_I = 0; l_I < m_Followers.size(); l_I++)
        {
            if (m_Followers[l_I].DatabaseID == p_FollowerDatabaseID)
                return &m_Followers[l_I];
        }

        return nullptr;
    }
    /// Gets the follower count of specified type
    uint32 Manager::GetTotalFollowerCount(eGarrisonFollowerTypeID p_Type)
    {
        return (uint32)std::count_if(m_Followers.begin(), m_Followers.end(), [p_Type](GarrisonFollower l_Follower) -> bool
        {
            GarrFollowerEntry const* l_Entry = l_Follower.GetEntry();
            return l_Entry && l_Entry->Type == p_Type;
        });
    }

    uint32 Manager::GetFollowerCount(uint32 p_FollowerID) const
    {
        uint32 l_Count = 0;
        for (auto l_Follower : m_Followers)
        {
            if (l_Follower.FollowerID == p_FollowerID)
                l_Count++;
        }

        return l_Count;
    }

    uint32 Manager::GetMaxCount(uint32 p_FollowerID)const
    {
        GarrFollowerEntry const* l_FollowerEntry = sGarrFollowerStore.LookupEntry(p_FollowerID);
        if (!l_FollowerEntry)
            return 0;

        GarrClassSpecEntry const* l_GarrClassSpecEntry = sGarrClassSpecStore.LookupEntry(m_Owner->getFaction() == FactionIndex::Alliance ? l_FollowerEntry->AllianceGarrClassSecID : l_FollowerEntry->HordeGarrClassSecID);
        if (!l_GarrClassSpecEntry)
            return 0;

        std::set<GarrAbilityEffectEntry const*> l_GarrAbilityEffectEntries = GetGarrAbilityEffectEntryByEffectType(AbilityEffectTypes::ModMaxTroopAmount);
        uint32 l_BonusTroops = 0;
        for (GarrAbilityEffectEntry const* l_GarrAbilityEffectEntry : l_GarrAbilityEffectEntries)
        {
            if (l_GarrAbilityEffectEntry->GarrClassSpecID == l_GarrClassSpecEntry->ID)
                l_BonusTroops += l_GarrAbilityEffectEntry->ActionValueFlat;
        }

        return l_GarrClassSpecEntry->MaxTroopAmount + l_BonusTroops;
    }

    uint32 Manager::GetFollowerIDForShipmentID(uint32 p_ShipmentID) const
    {
        CharShipmentEntry const* l_CharShipmentEntry = sCharShipmentStore.LookupEntry(p_ShipmentID);
        if (!l_CharShipmentEntry)
            return 0;

        return l_CharShipmentEntry->FollowerID;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get follower by ID or template
    GarrisonFollower Manager::GenerateNewFollowerEntity(uint32 p_FollowerID)
    {
        GarrisonFollower l_Follower;

        if (GetFollower(p_FollowerID) != nullptr)
            return *GetFollower(p_FollowerID);

        GarrFollowerEntry const* l_FollowerEntry = sGarrFollowerStore.LookupEntry(p_FollowerID);

        if (l_FollowerEntry == nullptr)
            return l_Follower;

        l_Follower.DatabaseID           = 0;
        l_Follower.FollowerID           = l_FollowerEntry->ID;
        l_Follower.Level                = l_FollowerEntry->Level;
        l_Follower.XP                   = 0;
        l_Follower.Durability           = 1;
        l_Follower.Quality              = l_FollowerEntry->Quality;
        l_Follower.ItemLevelArmor       = l_FollowerEntry->ItemLevelArmor;
        l_Follower.ItemLevelWeapon      = l_FollowerEntry->ItemLevelWeapon;
        l_Follower.CurrentBuildingID    = 0;
        l_Follower.CurrentMissionID     = 0;
        l_Follower.ZoneSupportSpellID   = 0;
        l_Follower.Flags                = 0;
        l_Follower.GarrClassSpecID      = m_Owner->getFaction() == FactionIndex::Alliance ? l_FollowerEntry->AllianceGarrClassSecID : l_FollowerEntry->HordeGarrClassSecID;

        GenerateFollowerAbilities(l_Follower, true, true, true, true);

        return l_Follower;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get activated followers count
    uint32 Manager::GetActiveFollowerCount(eGarrisonFollowerTypeID p_FollowerType) const
    {
        return (uint32)std::count_if(m_Followers.begin(), m_Followers.end(), [p_FollowerType](GarrisonFollower p_Follower) -> bool
        {
            auto l_Entry = p_Follower.GetEntry();

            return l_Entry ? (l_Entry->Type == p_FollowerType && (p_Follower.Flags & FollowerFlags::isInactive) != 0) : false;
        });
    }
    /// Change follower activation state
    void Manager::ChangeFollowerActivationState(uint64 p_FollowerDBID, bool p_Active)
    {
        GarrisonFollower* l_Follower = nullptr;

        if (p_Active)
        {
            if (!m_Owner->HasEnoughMoney((uint64)Globals::FollowerActivationCost))
                return;

            auto l_It = std::find_if(m_Followers.begin(), m_Followers.end(), [p_FollowerDBID](const GarrisonFollower& p_Follower) { return p_Follower.DatabaseID == p_FollowerDBID; });

            if (l_It != m_Followers.end())
            {
                GarrisonType::Type l_Type = static_cast<GarrisonType::Type>(l_It->GetEntry()->GarrisonType);

                uint32 l_RemainingActivationCount = GetNumFollowerActivationsRemaining(l_Type);
                uint32 l_SoftCapBonus = 0;

                switch (l_It->GetEntry()->GarrisonType)
                {
                    case GarrisonType::GarrisonDraenor:
                        l_SoftCapBonus = m_GarrisonDraenor ? m_GarrisonDraenor->GetFollowersCountBarracksBonus() : 0;
                        break;
                }

                if (l_RemainingActivationCount < 1)
                    return;

                GarrFollowerTypeEntry const* l_GarrFollowerTypeEntry = sGarrFollowerTypeStore.LookupEntry(l_It->GetEntry()->Type);

                if (GetActiveFollowerCount(static_cast<eGarrisonFollowerTypeID>(l_GarrFollowerTypeEntry->ID)) >= (l_GarrFollowerTypeEntry->SoftCap + l_SoftCapBonus))
                    return;

                m_Owner->ModifyMoney(-Globals::FollowerActivationCost, "Manager::ChangeFollowerActivationState");

                m_NumFollowerActivation[l_Type]--;
                m_NumFollowerActivationRegenTimestamp[l_Type] = time(0);

                l_RemainingActivationCount = GetNumFollowerActivationsRemaining(l_Type);

                l_It->Flags = l_It->Flags & ~FollowerFlags::isInactive;
                l_Follower = &(*l_It);

                WorldPacket l_Data(SMSG_GARRISON_NUM_FOLLOWER_ACTIVATIONS_REMAINING, 8);
                l_Data << uint32(l_It->GetEntry()->GarrisonType);
                l_Data << uint32(l_RemainingActivationCount);

                m_Owner->SendDirectMessage(&l_Data);
            }
        }
        else
        {
            if (!m_Owner->HasEnoughMoney((uint64)Globals::FollowerActivationCost))
                return;

            auto l_It = std::find_if(m_Followers.begin(), m_Followers.end(), [p_FollowerDBID](const GarrisonFollower& p_Follower) { return p_Follower.DatabaseID == p_FollowerDBID; });

            if (l_It != m_Followers.end())
            {
                m_Owner->ModifyMoney(-Globals::FollowerActivationCost, "Manager::ChangeFollowerActivationState");

                l_It->Flags |= FollowerFlags::isInactive;
                l_Follower = &(*l_It);
            }
        }

        if (!l_Follower)
            return;

        l_Follower->SendFollowerUpdate(m_Owner);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Returns error message of upgrade attempt
    SpellCastResult Manager::CanUpgradeFollowerItemLevelWith(uint32 p_FollowerID, SpellInfo const* p_SpellInfo, SpellEffIndex p_EffectIndex) const
    {
        auto l_It = std::find_if(m_Followers.begin(), m_Followers.end(), [p_FollowerID](GarrisonFollower const& p_Follower) { return p_Follower.FollowerID == p_FollowerID; });

        if (l_It == m_Followers.end())
            return SPELL_FAILED_BAD_TARGETS;

        GarrisonFollower const* l_Follower = &(*l_It);
        SpellEffectInfo const l_SpellEffect = p_SpellInfo->Effects[p_EffectIndex];

        GarrItemLevelUpgradeDataEntry const* l_GarrItemLevelUpgradeDataEntry = sGarrItemLevelUpgradeDataStore.LookupEntry(l_SpellEffect.MiscValue);
        if (!l_GarrItemLevelUpgradeDataEntry)
            return SPELL_FAILED_DONT_REPORT;

        if (l_Follower->GetEntry()->Type != l_GarrItemLevelUpgradeDataEntry->FollowerTypeID)
            return SPELL_FAILED_BAD_TARGETS;

        int32 l_Ilvl = l_GarrItemLevelUpgradeDataEntry->Operation % 2 ? l_Follower->ItemLevelArmor : l_Follower->ItemLevelWeapon;
        if (l_Ilvl >= l_GarrItemLevelUpgradeDataEntry->MaxItemLevel || l_Ilvl < l_GarrItemLevelUpgradeDataEntry->MinItemLevel)
            return SPELL_FAILED_GARRISON_FOLLOWER_MAX_ITEM_LEVEL;

        return SPELL_CAST_OK;
    }
    /// Upgrades follower with spell
    void Manager::UpgradeFollowerItemLevelWith(uint32 p_FollowerID, SpellInfo const* p_SpellInfo, SpellEffIndex p_EffectIndex)
    {
        if (CanUpgradeFollowerItemLevelWith(p_FollowerID, p_SpellInfo, p_EffectIndex) != SPELL_CAST_OK)
            return;

        auto l_It = std::find_if(m_Followers.begin(), m_Followers.end(), [p_FollowerID](GarrisonFollower const& p_Follower) { return p_Follower.FollowerID == p_FollowerID; });
        GarrisonFollower* l_Follower = const_cast<GarrisonFollower*>(&(*l_It));

        SpellEffectInfo const l_SpellEffect = p_SpellInfo->Effects[p_EffectIndex];
        GarrItemLevelUpgradeDataEntry const* l_GarrItemLevelUpgradeDataEntry = sGarrItemLevelUpgradeDataStore.LookupEntry(l_SpellEffect.MiscValue);
        if (!l_GarrItemLevelUpgradeDataEntry)
            return;

        int32 l_Cap = l_GarrItemLevelUpgradeDataEntry->MaxItemLevel;
        int32& l_Ilvl = l_GarrItemLevelUpgradeDataEntry->Operation % 2 ? l_Follower->ItemLevelArmor : l_Follower->ItemLevelWeapon;
        if (l_SpellEffect.MiscValue / 2)
            l_Ilvl = std::min(l_Cap, l_Ilvl + l_SpellEffect.BasePoints);
        else
            l_Ilvl = std::min(l_Cap, l_Ilvl);

        WorldPacket l_Data(SMSG_GARRISON_FOLLOWER_CHANGED_ITEM_LEVEL, 4 + 4 + 4 + 1);
        l_Follower->Write(l_Data);
        m_Owner->SendDirectMessage(&l_Data);
    }

    SpellCastResult Manager::CanIncreaseFollowerExperienceWith(uint32 p_FollowerID, SpellInfo const* p_SpellInfo) const
    {
        auto l_It = std::find_if(m_Followers.begin(), m_Followers.end(), [p_FollowerID](GarrisonFollower const& p_Follower) { return p_Follower.FollowerID == p_FollowerID; });

        if (l_It == m_Followers.end())
            return SPELL_FAILED_BAD_TARGETS;

        GarrisonFollower const* l_Follower = &(*l_It);
        if (!l_Follower->CanXP())
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void Manager::IncreaseFollowerExperienceWith(uint32 p_FollowerID, SpellInfo const* p_SpellInfo, SpellEffIndex p_EffectIndex)
    {
        if (CanIncreaseFollowerExperienceWith(p_FollowerID, p_SpellInfo) != SPELL_CAST_OK)
            return;

        auto l_It = std::find_if(m_Followers.begin(), m_Followers.end(), [p_FollowerID](GarrisonFollower const& p_Follower) { return p_Follower.FollowerID == p_FollowerID; });
        GarrisonFollower* l_Follower = const_cast<GarrisonFollower*>(&(*l_It));

        SpellEffectInfo const l_SpellEffect = p_SpellInfo->Effects[p_EffectIndex];

        l_Follower->EarnXP(l_SpellEffect.BasePoints, m_Owner);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Check if any followers has ability in parameter
    bool Manager::HasFollowerAbility(uint32 p_AbilityID) const
    {
        for (const GarrisonFollower& l_Follower : m_Followers)
        {
            for (auto l_Ability : l_Follower.Abilities)
            {
                if (l_Ability == p_AbilityID)
                    return true;
            }
        }

        return false;
    }
    /// Reroll Follower Abilities
    void Manager::GenerateFollowerAbilities(uint32 p_FollowerID, bool p_Reset /* = true */, bool p_Abilities /* = true */, bool p_Traits /* = true */, bool p_Update /* = false */)
    {
        auto l_Itr = std::find_if(m_Followers.begin(), m_Followers.end(), [p_FollowerID](GarrisonFollower p_FollowerEntry) -> bool
        {
            return p_FollowerEntry.DatabaseID == p_FollowerID;
        });

        if (l_Itr == m_Followers.end())
            return;

        return GenerateFollowerAbilities(*l_Itr, p_Reset, p_Abilities, p_Traits, p_Update);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Returns error message of the attempt
    SpellCastResult Manager::CanLearnTrait(uint32 p_FollowerID, uint32 p_Slot, SpellInfo const* p_SpellInfo, uint32 p_EffIndex) const
    {
        auto l_Iter = std::find_if(m_Followers.begin(), m_Followers.end(), [p_FollowerID](GarrisonFollower P_RefFollower) -> bool
        {
            return P_RefFollower.FollowerID == p_FollowerID;
        });

        if (l_Iter == m_Followers.end())
            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

        GarrisonFollower l_Follower = *l_Iter;

        for (int l_Index = 0; l_Index < (int)l_Iter->Abilities.size(); ++l_Index)
        {
            if (l_Iter->Abilities[l_Index] == p_Slot)
                break;

            if (l_Index == (l_Iter->Abilities.size() - 1))
                return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
        }

        if (p_SpellInfo->Effects[p_EffIndex].MiscValueB == eGarrisonFollowerTypeID::FollowerTypeIDShipyard && l_Iter->GetEntry()->Type != eGarrisonFollowerTypeID::FollowerTypeIDShipyard)
            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

        if ((p_SpellInfo->Effects[p_EffIndex].MiscValueB == eGarrisonFollowerTypeID::FollowerTypeIDDraenor || p_SpellInfo->Effects[p_EffIndex].MiscValueB == 0) && l_Iter->GetEntry()->Type != eGarrisonFollowerTypeID::FollowerTypeIDDraenor)
            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

        if ((p_SpellInfo->Effects[p_EffIndex].MiscValueB == eGarrisonFollowerTypeID::FollowerTypeIDClassHall || p_SpellInfo->Effects[p_EffIndex].MiscValueB == 0) && l_Iter->GetEntry()->Type != eGarrisonFollowerTypeID::FollowerTypeIDClassHall)
            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

        GarrAbilityEntry const* l_Ability = sGarrAbilityStore.LookupEntry(p_Slot);

        if (!l_Ability)
            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

        if (l_Ability->Flags == 3 && p_SpellInfo->Effects[p_EffIndex].MiscValueB == eGarrisonFollowerTypeID::FollowerTypeIDClassHall)
        {
            bool l_HasFreeEquipmentSlot = false;
            for (uint32 l_FollowerAbility : l_Follower.Abilities)
            {
                if (l_FollowerAbility == GBrokenIsles::FollowerConstants::SlotA_AbilityID
                    || l_FollowerAbility == GBrokenIsles::FollowerConstants::SlotB_AbilityID
                    || l_FollowerAbility == GBrokenIsles::FollowerConstants::SlotC_AbilityID)
                {
                    l_HasFreeEquipmentSlot = true;
                    break;
                }
            }

            if (!l_HasFreeEquipmentSlot)
            {
                for (size_t l_Index = 0; l_Index < l_Follower.Abilities.size(); ++l_Index)
                {
                    if (l_Follower.Abilities[l_Index] == p_Slot)
                        break;

                    if (l_Index == (l_Follower.Abilities.size() - 1))
                        return SpellCastResult::SPELL_FAILED_BAD_TARGETS;;
                }
            }
        }

        /// Cannot remove ships or ship types
        if (l_Iter->GetEntry()->Type == eGarrisonFollowerTypeID::FollowerTypeIDShipyard && (l_Ability->Flags == 51 || l_Ability->Flags == 9 || l_Ability->Flags == 5 || l_Ability->Flags == 1 || l_Ability->Flags == 3))
            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

        /// Cannot cast on abilities
        if (l_Iter->GetEntry()->Type != eGarrisonFollowerTypeID::FollowerTypeIDShipyard && (l_Ability->Flags == 0 || l_Ability->Flags == 2))
            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

        if (std::find(l_Iter->Abilities.begin(), l_Iter->Abilities.end(), p_SpellInfo->Effects[p_EffIndex].MiscValue) != l_Iter->Abilities.end())
            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

        return SpellCastResult::SPELL_CAST_OK;
    }
    /// Learns the specific trait
    void Manager::LearnFollowerTrait(uint32 p_FollowerID, uint32 p_Slot, SpellInfo const* p_SpellInfo, uint32 p_EffIndex)
    {
        auto l_Iter = std::find_if(m_Followers.begin(), m_Followers.end(), [p_FollowerID](GarrisonFollower P_RefFollower) -> bool
        {
            return P_RefFollower.FollowerID == p_FollowerID;
        });

        if (l_Iter == m_Followers.end())
            return;

        size_t l_Index = 0;
        for (; l_Index < l_Iter->Abilities.size(); ++l_Index)
        {
            if (l_Iter->Abilities[l_Index] == p_Slot)
                break;

            if (l_Index == (l_Iter->Abilities.size() - 1))
                return;
        }

        /// The client refreshes the UI only if value 1 or 4 of flag is modified in the update
        l_Iter->Flags ^= static_cast<uint32>(Garrison::FollowerFlags::Type::Favorite);

        l_Iter->Abilities[l_Index] = p_SpellInfo->Effects[p_EffIndex].MiscValue;
        l_Iter->SendFollowerUpdate(m_Owner);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get num follower activation remaining
    uint32 Manager::GetNumFollowerActivationsRemaining(GarrisonType::Type p_Type) const
    {
        return m_NumFollowerActivation[p_Type];
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Returns the cap of the specified follower type
    uint32 Manager::GetFollowerSoftCap(eGarrisonFollowerTypeID p_FollowerType) const
    {
        GarrFollowerTypeEntry const* l_TypeEntry = sGarrFollowerTypeStore.LookupEntry(p_FollowerType);

        if (!l_TypeEntry)
            return 0;

        uint32 l_Cap = l_TypeEntry->SoftCap;

        if (p_FollowerType == eGarrisonFollowerTypeID::FollowerTypeIDDraenor && m_GarrisonDraenor)
        {
            GDraenor::GarrisonBuilding l_Building = m_GarrisonDraenor->GetBuildingWithType(static_cast<GDraenor::Building::Type>(l_TypeEntry->SoftCapBuildingIncreaseID));

            if (l_Building.DatabaseID != 0)
                l_Cap += sGarrBuildingStore.LookupEntry(l_Building.BuildingID)->BonusAmount;
        }

        return l_Cap;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Returns if the mission with specified follower type can be started after soft cap
    bool Manager::CanMissionBeStartedAfterSoftCap(uint32 p_FollowerType) const
    {
        GarrFollowerTypeEntry const* l_Entry = sGarrFollowerTypeStore.LookupEntry(p_FollowerType);
        return l_Entry ? l_Entry->Flags & FollowerTypeFlags::CanStartMissionPastSoftCap : false;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Generate random NPC Ability
    /// TODO: Only class specific - not fully random
    uint32 Manager::GenerateRandomAbility(uint32 p_FollowerID)
    {
        std::vector<uint32> l_PossibleEntiers;

        for (uint32 l_ID = 0; l_ID < sGarrAbilityStore.GetNumRows(); ++l_ID)
        {
            GarrAbilityEntry const* l_Entry = sGarrAbilityStore.LookupEntry(l_ID);

            if (!l_Entry)
                continue;

            if (l_Entry->FollowerType != eGarrisonFollowerTypeID::FollowerTypeIDDraenor)
                continue;

            if (l_Entry->Flags != 0)
                continue;

            /// Not ability but trait
            if (std::find(GDraenor::GarrisonAbilities::g_FollowerAbilities.begin(), GDraenor::GarrisonAbilities::g_FollowerAbilities.end(), l_Entry->ID) == GDraenor::GarrisonAbilities::g_FollowerAbilities.end())
                continue;

            /// Ability class Verification
            ///{
            ///    std::map<uint32, uint32>::iterator l_Iterator = g_FollowerAbilitiesClass.find(l_Entry->ID);
            ///
            ///    if (l_Iterator != g_FollowerAbilitiesClass.end())
            ///    {
            ///        GarrFollowerEntry const* l_FollowerEntry = sGarrFollowerStore.LookupEntry(p_FollowerID);
            ///
            ///        if (l_FollowerEntry == nullptr)
            ///            continue;
            ///
            ///        using namespace Follower;
            ///
            ///        std::map<uint32, uint32>::const_iterator l_SecondIterator = g_ClassSpecIndex.find(l_FollowerEntry->AllianceGarrClassSecID);
            ///
            ///        if (l_SecondIterator == g_ClassSpecIndex.end() || l_SecondIterator->second != l_Iterator->second)
            ///            continue;
            ///    }
            ///}

            l_PossibleEntiers.push_back(l_Entry->ID);
        }

        if (!l_PossibleEntiers.size())
            return 0;

        return l_PossibleEntiers[urand(0, l_PossibleEntiers.size() - 1)];
    }
    /// Generate random trait
    uint32 Manager::GenerateRandomTrait(uint32 p_Type, std::vector<uint32> const& p_KnownAbilities)
    {
        std::vector<uint32> l_PossibleEntiers;
        std::vector<uint32> l_KnownTraits;

        uint32 l_MyFactionType      = GetGarrisonFactionIndex() == FactionIndex::Alliance ? 5 : 9;
        uint32 l_OtherFactionType   = GetGarrisonFactionIndex() == FactionIndex::Alliance ? 9 : 5;

        if (p_Type != eGarrisonFollowerTypeID::FollowerTypeIDShipyard)
        {
            for (auto l_AbilityID : p_KnownAbilities)
            {
                if (GarrAbilityEntry const* l_Entry = sGarrAbilityStore.LookupEntry(l_AbilityID))
                {
                    if (std::find(GDraenor::GarrisonAbilities::g_FollowerTraits.begin(), GDraenor::GarrisonAbilities::g_FollowerTraits.end(), l_AbilityID) != GDraenor::GarrisonAbilities::g_FollowerTraits.end())
                        l_KnownTraits.push_back(l_Entry->ID);
                }
            }
        }
        else
        {
            /// Ships do not gain crew
            l_KnownTraits.push_back(l_MyFactionType);
            l_KnownTraits.push_back(1);                  ///< Neutral Crews
            l_KnownTraits.push_back(3);                  ///< Ship Specific crews
            l_KnownTraits.push_back(51);                 ///< Ship Abilities
            l_KnownTraits.push_back(l_OtherFactionType);
        }

        for (uint32 l_ID = 0; l_ID < sGarrAbilityStore.GetNumRows(); ++l_ID)
        {
            GarrAbilityEntry const* l_Entry = sGarrAbilityStore.LookupEntry(l_ID);

            if (!l_Entry)
                continue;

            if (l_Entry->FollowerType != p_Type)
                continue;

            /// Wrong Faction (According to a serious tester, traits dont need that kind of restriction)
            if (l_Entry->Flags == l_OtherFactionType)
                continue;

            if (std::find(p_KnownAbilities.begin(), p_KnownAbilities.end(), l_ID) != p_KnownAbilities.end())
                continue;

            /// Not trait but ability
            if (std::find(GDraenor::GarrisonAbilities::g_FollowerTraits.begin(), GDraenor::GarrisonAbilities::g_FollowerTraits.end(), l_ID) == GDraenor::GarrisonAbilities::g_FollowerTraits.end())
                continue;

            /// Test Entries
            if (l_ID == 300 || l_ID == 299 || l_ID == 257)
                continue;

            l_PossibleEntiers.push_back(l_ID);
        }

        if (!l_PossibleEntiers.size())
            return 0;

        return l_PossibleEntiers[urand(0, l_PossibleEntiers.size() - 1)];
    }
    /// Reroll Follower Abilities
    void Manager::GenerateFollowerAbilities(GarrisonFollower& p_Follower, bool p_Reset /* = true */, bool /*p_Abilities*/ /* = true */, bool /*p_Traits*/ /* = true */, bool p_Update /* = false */)
    {
        if (p_Reset)
            p_Follower.Abilities.clear();

        std::vector<uint32> l_FollowerBaseAbilities;
        uint32 l_FollowerEmptyEquipmentSlots = 0;

        switch (p_Follower.Quality)
        {
            case FollowerQualities::FOLLOWER_QUALITY_RARE:
                l_FollowerEmptyEquipmentSlots = 1;
                break;
            case FollowerQualities::FOLLOWER_QUALITY_EPIC:
                l_FollowerEmptyEquipmentSlots = 2;
                break;
            case FollowerQualities::FOLLOWER_QUALITY_LEGENDARY:
                l_FollowerEmptyEquipmentSlots = 3;
                break;
        }

        for (uint32 l_Ability : p_Follower.Abilities)
        {
            GarrAbilityEntry const* l_GarrAbility = sGarrAbilityStore.LookupEntry(l_Ability);
            if (!l_GarrAbility)
                continue;

            if (l_GarrAbility->Flags == 3 && l_FollowerEmptyEquipmentSlots > 0) ///< TOENUM
                l_FollowerEmptyEquipmentSlots--;
        }

        switch (l_FollowerEmptyEquipmentSlots)
        {
            case 3:
            {
                bool l_Found = false;
                for (uint32 l_FollowerAbility : p_Follower.Abilities)
                {
                    if (l_FollowerAbility == GBrokenIsles::FollowerConstants::SlotC_AbilityID)
                        l_Found = true;
                }
                if (!l_Found)
                    p_Follower.Abilities.push_back(GBrokenIsles::FollowerConstants::SlotC_AbilityID);
                /// No Break Intended
            }
            case 2:
            {
                bool l_Found = false;
                for (uint32 l_FollowerAbility : p_Follower.Abilities)
                {
                    if (l_FollowerAbility == GBrokenIsles::FollowerConstants::SlotB_AbilityID)
                        l_Found = true;
                }
                if (!l_Found)
                    p_Follower.Abilities.push_back(GBrokenIsles::FollowerConstants::SlotB_AbilityID);
                /// No Break Intended
            }
            case 1:
            {
                bool l_Found = false;
                for (uint32 l_FollowerAbility : p_Follower.Abilities)
                {
                    if (l_FollowerAbility == GBrokenIsles::FollowerConstants::SlotA_AbilityID)
                        l_Found = true;
                }
                if (!l_Found)
                    p_Follower.Abilities.push_back(GBrokenIsles::FollowerConstants::SlotA_AbilityID);

                break;
            }
            default:
                break;
        }

        for (uint32 l_I = 0; l_I < sGarrFollowerXAbilityStore.GetNumRows(); ++l_I)
        {
            GarrFollowerXAbilityEntry const* l_Entry = sGarrFollowerXAbilityStore.LookupEntry(l_I);

            if (!l_Entry)
                continue;

            /// Class hall follower slots
            if (l_Entry->AbilityID == GBrokenIsles::FollowerConstants::SlotA_AbilityID && p_Follower.Quality < FollowerQualities::FOLLOWER_QUALITY_RARE)
                continue;
            else if (l_Entry->AbilityID == GBrokenIsles::FollowerConstants::SlotB_AbilityID && p_Follower.Quality < FollowerQualities::FOLLOWER_QUALITY_EPIC)
                continue;
            else if (l_Entry->AbilityID == GBrokenIsles::FollowerConstants::SlotC_AbilityID && p_Follower.Quality < FollowerQualities::FOLLOWER_QUALITY_LEGENDARY)
                continue;

            if (l_Entry->FollowerID == p_Follower.FollowerID && l_Entry->FactionIndex == GetGarrisonFactionIndex() && sGarrAbilityStore.LookupEntry(l_Entry->AbilityID))
            {
                if (l_Entry->AbilityID == GBrokenIsles::FollowerConstants::SlotA_AbilityID
                    || l_Entry->AbilityID == GBrokenIsles::FollowerConstants::SlotB_AbilityID
                    || l_Entry->AbilityID == GBrokenIsles::FollowerConstants::SlotC_AbilityID)
                    continue;

                l_FollowerBaseAbilities.push_back(l_Entry->AbilityID);

                if (std::find(p_Follower.Abilities.begin(), p_Follower.Abilities.end(), l_Entry->AbilityID) == p_Follower.Abilities.end())
                    p_Follower.Abilities.push_back(l_Entry->AbilityID);
            }
        }

        if (p_Follower.GetEntry()->Type == eGarrisonFollowerTypeID::FollowerTypeIDShipyard && m_GarrisonDraenor)
        {
            if (uint32 l_AbillityId = m_GarrisonDraenor->GenerateCrewAbilityIdForShip(p_Follower))
                p_Follower.Abilities.push_back(l_AbillityId);

            uint32 l_AbilityCount = p_Follower.Quality - FollowerQualities::FOLLOWER_QUALITY_UNCOMMON; ///< rare 1, epic 2

            while ((1 /* ship type */ + 1 /* crew */ + l_AbilityCount) > p_Follower.Abilities.size())
            {
                if (uint32 l_NewAbility = GenerateRandomTrait(eGarrisonFollowerTypeID::FollowerTypeIDShipyard, p_Follower.Abilities))
                    p_Follower.Abilities.push_back(l_NewAbility);
            }
        }
        else if (p_Follower.GetEntry()->Type == eGarrisonFollowerTypeID::FollowerTypeIDDraenor)
        {
            using namespace GDraenor::GarrisonAbilities;

            uint8 l_MaxAbilities = p_Follower.Quality >= FollowerQualities::FOLLOWER_QUALITY_EPIC ? 2 : 1;

            uint32 l_AbilitiesCount = std::count_if(g_FollowerAbilities.begin(), g_FollowerAbilities.end(), [p_Follower](uint32 p_AbilityID) -> bool
            {
                for (auto l_Ability : p_Follower.Abilities)
                {
                    if (l_Ability == p_AbilityID)
                        return true;
                }

                return false;
            });

            while (l_AbilitiesCount < l_MaxAbilities)
            {
                uint32 l_NewAbility = GenerateRandomAbility(p_Follower.FollowerID);

                if (!l_NewAbility)
                {
                    sLog->outAshran("Can't find random ability for follower ID %u", p_Follower.FollowerID);
                    break;
                }

                p_Follower.Abilities.push_back(l_NewAbility);
                ++l_AbilitiesCount;
            }

            uint32 l_TraitCount = std::count_if(g_FollowerTraits.begin(), g_FollowerTraits.end(), [p_Follower](uint32 p_TraitID) -> bool
            {
                for (auto l_Ability : p_Follower.Abilities)
                {
                    if (l_Ability == p_TraitID)
                        return true;
                }

                return false;
            });

            while (l_TraitCount < (p_Follower.Quality - FollowerQualities::FOLLOWER_QUALITY_NORMAL))
            {
                if (uint32 l_NewAbility = GenerateRandomTrait(eGarrisonFollowerTypeID::FollowerTypeIDDraenor, p_Follower.Abilities))
                    p_Follower.Abilities.push_back(l_NewAbility);

                ++l_TraitCount;
            }
        }
        else if (p_Follower.GetEntry()->Type == eGarrisonFollowerTypeID::FollowerTypeIDClassHall)
        {
            uint8 l_MaxAbilities = 1 + (p_Follower.Quality >= FollowerQualities::FOLLOWER_QUALITY_UNCOMMON ? 2 : 1);

            if (l_FollowerBaseAbilities.size() > l_MaxAbilities)
            {
                for (int l_J = l_MaxAbilities; l_J < l_FollowerBaseAbilities.size(); ++l_J)
                {
                    auto l_It = std::find(p_Follower.Abilities.begin(), p_Follower.Abilities.end(), l_FollowerBaseAbilities[l_J]);

                    if (l_It != p_Follower.Abilities.end())
                        p_Follower.Abilities.erase(l_It);
                }
            }
        }

        if (p_Update)
            p_Follower.SendFollowerUpdate(m_Owner);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Reward missing followers from quests
    void Manager::RewardMissingFollowers()
    {
        std::vector<uint32> l_FollowerQuests = sObjectMgr->FollowerQuests;

        /// Quest non rewarded followers
        for (uint32 l_QuestID : l_FollowerQuests)
        {
            if (m_Owner->GetQuestStatus(l_QuestID) == QUEST_STATUS_REWARDED)
            {
                const Quest* l_QuestTemplate = sObjectMgr->GetQuestTemplate(l_QuestID);
                const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(l_QuestTemplate->GetRewSpellCast());

                if (l_SpellInfo->Effects[EFFECT_0].Effect == SPELL_EFFECT_ADD_GARRISON_FOLLOWER && GetFollower(l_SpellInfo->Effects[EFFECT_0].MiscValue) == nullptr)
                    AddFollower(l_SpellInfo->Effects[EFFECT_0].MiscValue);
            }
        }
    }
    /// Unstuck followers in a non existent mission
    void Manager::UnstuckFollowers()
    {
        /// Unstuck follower
        std::for_each(m_Followers.begin(), m_Followers.end(), [this](GarrisonFollower& p_Follower)
        {
            if (p_Follower.CurrentMissionID != 0)
            {
                auto l_It = std::find_if(m_Missions.begin(), m_Missions.end(), [p_Follower](const GarrisonMission& p_Mission) -> bool
                {
                    if (p_Mission.MissionID == p_Follower.CurrentMissionID)
                        return true;

                    return false;
                });

                if (l_It == m_Missions.end() || (*l_It).State == Mission::State::Offered)
                    p_Follower.CurrentMissionID = 0;
            }

            /// Corrupted DB
            if (p_Follower.Level == 0)
            {
                GarrFollowerEntry const* l_Entry = sGarrFollowerStore.LookupEntry(p_Follower.FollowerID);

                if (l_Entry)
                {
                    p_Follower.Level            = l_Entry->Level;
                    p_Follower.XP               = 0;
                    p_Follower.Quality          = l_Entry->Quality;
                    p_Follower.ItemLevelArmor   = l_Entry->ItemLevelArmor;
                    p_Follower.ItemLevelWeapon  = l_Entry->ItemLevelWeapon;

                    p_Follower.Abilities.clear();

                    for (uint32 l_I = 0; l_I < sGarrFollowerXAbilityStore.GetNumRows(); ++l_I)
                    {
                        GarrFollowerXAbilityEntry const* l_Entry = sGarrFollowerXAbilityStore.LookupEntry(l_I);

                        if (l_Entry && l_Entry->FollowerID == p_Follower.FollowerID && sGarrAbilityStore.LookupEntry(l_Entry->AbilityID) && l_Entry->FactionIndex == GetGarrisonFactionIndex())
                            p_Follower.Abilities.push_back(l_Entry->AbilityID);
                    }
                }
            }
        });
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update followers
    void Manager::UpdateFollowers()
    {
        auto l_Update = [this](GarrisonType::Type p_Type)
        {
            /// Update follower activation count
            if (m_NumFollowerActivation[p_Type] < Globals::FollowerActivationMaxStack && (time(0) - m_NumFollowerActivationRegenTimestamp[p_Type]) > DAY)
            {
                m_NumFollowerActivation[p_Type]++;
                m_NumFollowerActivationRegenTimestamp[p_Type] = time(0);

                WorldPacket l_Data(SMSG_GARRISON_NUM_FOLLOWER_ACTIVATIONS_REMAINING, 8);
                l_Data << uint32(p_Type);
                l_Data << uint32(m_NumFollowerActivation[p_Type]);

                m_Owner->SendDirectMessage(&l_Data);
            }
        };

        l_Update(GarrisonType::GarrisonDraenor);
        l_Update(GarrisonType::GarrisonBrokenIsles);
    }

}   ///< namespace Garrison
}   ///< namespace MS

