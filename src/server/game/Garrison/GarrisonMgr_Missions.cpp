////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////


#include "GarrisonPackets.h"
#include "GarrisonMgr.hpp"
#include "Player.h"
#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "ObjectAccessor.h"
#include "CreatureAI.h"
#include "DisableMgr.h"
#include "Chat.h"

namespace MS { namespace Garrison
{
    /// Add mission
    bool Manager::AddMission(uint32 p_MissionRecID, uint32 p_OfferDuration /*= 0*/)
    {
        GarrMissionEntry const* l_MissionEntry = sGarrMissionStore.LookupEntry(p_MissionRecID);

        if (!l_MissionEntry)
            return false;

        uint32 l_Count = (uint32)std::count_if(m_Missions.begin(), m_Missions.end(), [p_MissionRecID](const GarrisonMission& p_Mission)
        {
            return p_Mission.MissionID == p_MissionRecID;
        });

        if (l_Count && !(l_MissionEntry->Flags & MS::Garrison::Mission::Flags::IsZoneSupport))
            return false;

        GarrTypeEntry const* l_GarrType = sGarrTypeStore.LookupEntry(l_MissionEntry->GarrisonType);

        GarrisonMission l_Mission;
        l_Mission.DatabaseID        = sObjectMgr->GetNewGarrisonMissionID();
        l_Mission.MissionID         = p_MissionRecID;
        l_Mission.OfferTime         = time(nullptr);
        l_Mission.OfferMaxDuration  = p_OfferDuration ? p_OfferDuration : l_MissionEntry->OfferTime;
        l_Mission.StartTime         = 0;
        l_Mission.TravelDuration    = l_MissionEntry->TravelTime;
        l_Mission.Duration          = l_MissionEntry->Duration;
        l_Mission.State             = Mission::State::Offered;
        l_Mission.ChestChance       = 0;
        l_Mission.Unk               = 0;
        l_Mission.IsLethalToTroops  = 0;

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GARRISON_MISSION);

        uint32 l_Index = 0;
        l_Stmt->setUInt32(l_Index++, l_Mission.DatabaseID);
        l_Stmt->setUInt32(l_Index++, m_ID);
        l_Stmt->setUInt32(l_Index++, l_Mission.MissionID);
        l_Stmt->setUInt32(l_Index++, l_Mission.OfferTime);
        l_Stmt->setUInt32(l_Index++, l_Mission.OfferMaxDuration);
        l_Stmt->setUInt32(l_Index++, l_Mission.StartTime);
        l_Stmt->setUInt32(l_Index++, l_Mission.State);

        CharacterDatabase.AsyncQuery(l_Stmt);

        m_Missions.push_back(l_Mission);

        WorldPacket l_AddMissionResult(SMSG_GARRISON_ADD_MISSION_RESULT, 40);
        l_AddMissionResult << uint32(l_MissionEntry->GarrisonType);
        l_AddMissionResult << uint32(ResultEnum::SUCCESS);
        l_AddMissionResult << uint8(l_Mission.State);

        l_AddMissionResult << uint64(l_Mission.DatabaseID);
        l_AddMissionResult << uint32(l_Mission.MissionID);
        l_AddMissionResult << uint32(l_Mission.OfferTime);
        l_AddMissionResult << uint32(l_Mission.OfferMaxDuration);
        l_AddMissionResult << uint32(l_Mission.StartTime);
        l_AddMissionResult << uint32(0);   ///< Travel duration
        l_AddMissionResult << uint32(0);   ///< Mission duration
        l_AddMissionResult << uint32(l_Mission.State);
        l_AddMissionResult << uint32(0/*l_Mission->Unk1*/);
        l_AddMissionResult << uint32(0/*l_Mission->Unk2*/);

        l_AddMissionResult << uint32(0);    ///< Reward
        l_AddMissionResult << uint32(0);    ///< Bonus Reward
        l_AddMissionResult.WriteBit(true);
        l_AddMissionResult.FlushBits();

        m_Owner->SendDirectMessage(&l_AddMissionResult);

        return true;
    }
    /// Player have mission
    bool Manager::HasMission(uint32 p_MissionRecID)  const
    {
        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            if (m_Missions[l_I].MissionID == p_MissionRecID)
            {
                if (m_Missions[l_I].State < Mission::State::Completed)
                {
                    GarrMissionEntry const* l_MissionEntry = sGarrMissionStore.LookupEntry(p_MissionRecID);

                    if (l_MissionEntry && l_MissionEntry->Flags & Mission::Flags::IsZoneSupport)
                        return true;

                     if (!((m_Missions[l_I].OfferMaxDuration && m_Missions[l_I].OfferTime + m_Missions[l_I].OfferMaxDuration < time(0)) && m_Missions[l_I].State == Mission::State::Offered))
                        return true;
                }
            }
        }

        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Start mission
    void Manager::StartMission(uint32 p_MissionRecID, std::vector<uint64> p_Followers)
    {
        if (!HasMission(p_MissionRecID))
        {
            StartMissionFailed(p_MissionRecID, p_Followers);
            return;
        }

        GarrMissionEntry const* l_MissionTemplate   = sGarrMissionStore.LookupEntry(p_MissionRecID);
        GarrTypeEntry    const* l_GarrisonType      = sGarrTypeStore.LookupEntry(l_MissionTemplate->GarrisonType);

        if (CanMissionBeStartedAfterSoftCap(l_MissionTemplate->FollowerType) || (GetActiveFollowerCount(static_cast<eGarrisonFollowerTypeID>(l_MissionTemplate->FollowerType)) > GetFollowerSoftCap(static_cast<eGarrisonFollowerTypeID>(l_MissionTemplate->FollowerType))))
            return;

        if (l_MissionTemplate->GarrisonType == GarrisonType::GarrisonBrokenIsles)
        {
            uint32 l_ChampionCount = std::count_if(m_Followers.begin(), m_Followers.end(), [](const GarrisonFollower p_Follower) -> bool
            {
                return (p_Follower.Flags & FollowerFlags::isLimitedUse) == 0;
            });

            if (!l_ChampionCount)
            {
                StartMissionFailed(p_MissionRecID, p_Followers);
                return;
            }

            if (p_Followers.size() > l_MissionTemplate->RequiredFollowersCount)
            {
                StartMissionFailed(p_MissionRecID, p_Followers);
                return;
            }
        }
        else
        {
            if (p_Followers.size() < l_MissionTemplate->RequiredFollowersCount)
            {
                StartMissionFailed(p_MissionRecID, p_Followers);
                return;
            }
        }

        for (uint32 l_I = 0; l_I < p_Followers.size(); ++l_I)
        {
            std::vector<GarrisonFollower>::iterator l_It = std::find_if(m_Followers.begin(), m_Followers.end(), [this, p_Followers, l_I](const GarrisonFollower p_Follower) -> bool
            {
                if (p_Follower.DatabaseID == uint32(p_Followers[l_I]))
                    return true;

                return false;
            });

            if (l_It == m_Followers.end())
            {
                StartMissionFailed(p_MissionRecID, p_Followers);
                return;
            }

            if (l_It->CurrentBuildingID != 0 || l_It->CurrentMissionID != 0 || (m_GarrisonBrokenIsles && m_GarrisonBrokenIsles->GetSupportFollowerDatabaseID() == l_It->DatabaseID))
            {
                StartMissionFailed(p_MissionRecID, p_Followers);
                return;
            }

            if (l_It->GetEntry()->Type != l_MissionTemplate->FollowerType)
            {
                StartMissionFailed(p_MissionRecID, p_Followers);
                return;
            }

            /// Should not happen
            if (l_It->Flags & FollowerFlags::isInactive)
                return;
        }

        GarrisonMission * l_Mission = nullptr;

        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            if (m_Missions[l_I].MissionID == p_MissionRecID && m_Missions[l_I].State == Mission::State::Offered)
            {
                l_Mission = &m_Missions[l_I];
                break;
            }
        }

        if (l_Mission)
        {
            InitMission(l_Mission, p_Followers);

            if (l_MissionTemplate->CurrencyCost && !m_Owner->HasCurrency(l_GarrisonType->CurrencyType, l_Mission->Price))
            {
                StartMissionFailed(p_MissionRecID, p_Followers);
                return;
            }

            if (l_MissionTemplate->CurrencyCost)
                m_Owner->ModifyCurrency(l_GarrisonType->CurrencyType, -static_cast<int32>(l_Mission->Price));

            l_Mission->State        = Mission::State::InProgress;

            l_Mission->StartTime    = time(nullptr);
            l_Mission->OfferTime    = time(nullptr);

            std::vector<uint32> l_FollowersIDs;
            GarrisonFollower l_FirstFollower;
            for (uint32 l_I = 0; l_I < p_Followers.size(); ++l_I)
            {
                std::vector<GarrisonFollower>::iterator l_It = std::find_if(m_Followers.begin(), m_Followers.end(), [this, p_Followers, l_I](const GarrisonFollower p_Follower) -> bool
                {
                    if (p_Follower.DatabaseID == p_Followers[l_I])
                        return true;

                    return false;
                });

                l_It->CurrentMissionID = p_MissionRecID;
                l_FollowersIDs.push_back(l_It->FollowerID);

                if (!l_FirstFollower.FollowerID)
                    l_FirstFollower = *l_It;
            }

            WorldPacket l_Result(SMSG_GARRISON_START_MISSION_RESULT, 200);
            l_Result << uint32(0);                                                                                                      ///< Result (0 = OK, 1 = failed)
            l_Result << uint16(IncreaseMissionsStartedTodayCount(static_cast<GarrisonType::Type>(l_MissionTemplate->GarrisonType)));    ///< MissionsStartedTodayCount

            l_Result << uint64(l_Mission->DatabaseID);
            l_Result << uint32(l_Mission->MissionID);
            l_Result << uint32(l_Mission->OfferTime);
            l_Result << uint32(l_Mission->OfferMaxDuration);
            l_Result << uint32(l_Mission->StartTime);
            l_Result << uint32(l_Mission->TravelDuration);
            l_Result << uint32(l_Mission->Duration);
            l_Result << uint32(l_Mission->State);
            l_Result << uint32(l_Mission->ChestChance);
            l_Result << uint32(l_Mission->Unk);

            l_Result << uint32(p_Followers.size());

            for (uint32 l_I = 0; l_I < p_Followers.size(); ++l_I)
                l_Result << uint64(p_Followers[l_I]);

            m_Owner->SendDirectMessage(&l_Result);

            if (GetGarrisonScript())
                GetGarrisonScript()->OnMissionStart(m_Owner, p_MissionRecID, l_FollowersIDs);

            if (l_MissionTemplate && (l_MissionTemplate->Flags & MS::Garrison::Mission::Flags::IsZoneSupport) && l_FirstFollower.FollowerID)
                m_Owner->CastSpell(m_Owner, l_FirstFollower.ZoneSupportSpellID, true);
        }
    }

    /// Remove mission
    void Manager::RemoveMission(uint32 p_RecMissionID)
    {
        auto l_It = std::find_if(m_Missions.begin(), m_Missions.end(), [p_RecMissionID](GarrisonMission const& p_Mission)
        {
            return p_Mission.MissionID == p_RecMissionID;
        });

        if (l_It == m_Missions.end())
            return;

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_MISSION);
        l_Stmt->setUInt32(0, l_It->DatabaseID);
        CharacterDatabase.Execute(l_Stmt);

        m_Missions.erase(l_It);

        WorldPacket l_PlaceHolder;
        m_Owner->GetSession()->HandleGetGarrisonInfoOpcode(l_PlaceHolder);
    }

    uint32 Manager::GetMissionStartedTodayCount(GarrisonType::Type p_Type) const
    {
        auto l_Iter = m_MissionsStartedToday.find(p_Type);
        return l_Iter == m_MissionsStartedToday.end() ? 0 : l_Iter->second;
    }

    uint32 Manager::IncreaseMissionsStartedTodayCount(GarrisonType::Type p_Type)
    {
        auto l_Iter = m_MissionsStartedToday.find(p_Type);

        if (l_Iter == m_MissionsStartedToday.end())
            m_MissionsStartedToday[p_Type] = 1;
        else
            ++m_MissionsStartedToday[p_Type];

        return m_MissionsStartedToday[p_Type];
    }

    void Manager::ResetMissionStartedTodayCount()
    {
        m_MissionsStartedToday.clear();

        WorldPacket l_Data(SMSG_GARRISON_UNK_MISSION_OPCODE);
        l_Data << uint32(GarrisonType::GarrisonDraenor);
        l_Data << uint32(0);    ///< Unk
        l_Data << uint32(0);    ///< Mission Loop of some kind
        l_Data.WriteBit(1);     ///< Reset daily Missions
        l_Data.WriteBit(0);     ///< LUA window stuff
        l_Data.FlushBits();
        m_Owner->SendDirectMessage(&l_Data);

        l_Data.Initialize(SMSG_GARRISON_UNK_MISSION_OPCODE);
        l_Data << uint32(GarrisonType::GarrisonBrokenIsles);
        l_Data << uint32(0);    ///< Unk
        l_Data << uint32(0);    ///< Mission Loop of some kind
        l_Data.WriteBit(1);     ///< Reset daily Missions
        l_Data.WriteBit(1);     ///< LUA window stuff
        l_Data.FlushBits();
        m_Owner->SendDirectMessage(&l_Data);
    }

    /// Send mission start failed packet
    void Manager::StartMissionFailed(uint32 p_MissionRecID, std::vector<uint64> p_Followers)
    {
        auto l_MissionTemplate = sGarrMissionStore[p_MissionRecID];
        GarrisonMission * l_Mission = nullptr;

        WorldPacket l_Data(SMSG_GARRISON_START_MISSION_RESULT, 200);
        l_Data << uint32(1);    ///< Result (0 = OK, 1 = failed)
        l_Data << uint16(GetMissionStartedTodayCount(static_cast<GarrisonType::Type>(l_MissionTemplate->GarrisonType)));

        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            if (m_Missions[l_I].MissionID == p_MissionRecID)
            {
                l_Mission = &m_Missions[l_I];
                break;
            }
        }

        if (l_Mission)
        {
            l_Data << uint64(l_Mission->DatabaseID);
            l_Data << uint32(l_Mission->MissionID);
            l_Data << uint32(l_Mission->OfferTime);
            l_Data << uint32(l_Mission->OfferMaxDuration);
            l_Data << uint32(l_Mission->StartTime);
            l_Data << uint32(l_Mission->TravelDuration);                            ///< TravelDuration
            l_Data << uint32(l_MissionTemplate->Duration);
            l_Data << uint32(l_Mission->State);
            l_Data << uint32(l_Mission->ChestChance);
            l_Data << uint32(l_Mission->Unk);
        }
        else
        {
            /// Mission details
            l_Data << uint64(0);
            l_Data << uint32(0);
            l_Data << uint32(0);
            l_Data << uint32(0);
            l_Data << uint32(0);
            l_Data << uint32(0);
            l_Data << uint32(0);
            l_Data << uint32(0);
            l_Data << uint32(0/*l_Mission->Unk1*/);
            l_Data << uint32(0/*l_Mission->Unk2*/);
        }

        l_Data << uint32(p_Followers.size());

        for (uint32 l_I = 0; l_I < p_Followers.size(); ++l_I)
            l_Data << uint64(p_Followers[l_I]);

        m_Owner->SendDirectMessage(&l_Data);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Complete a mission
    void Manager::CompleteMission(uint32 p_MissionRecID, bool p_IsResponse)
    {
        if (!HasMission(p_MissionRecID))
            return;

        GarrMissionEntry const* l_MissionTemplate = sGarrMissionStore.LookupEntry(p_MissionRecID);

        GarrisonMission* l_Mission = nullptr;

        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            if (m_Missions[l_I].MissionID == p_MissionRecID && m_Missions[l_I].State < MS::Garrison::Mission::State::Completed)
            {
                l_Mission = &m_Missions[l_I];
                break;
            }
        }

        if (l_Mission == nullptr)
            return;

        std::vector<GarrisonFollower*> l_MissionFollowers = GetMissionFollowers(p_MissionRecID);

        /// Hack attempt
        if ((l_Mission->StartTime + (l_Mission->TravelDuration + l_Mission->Duration)) > time(0))
            return;

        bool l_Succeeded        = roll_chance_i(l_Mission->ChestChance);  ///< Seems to be MissionChance
        bool l_SucceededBonus   = false;
        bool l_CanComplete      = true;

        if (l_MissionTemplate->MissionType == Garrison::Mission::MissionType::QuestMission)
            l_Succeeded = true;

        if (l_MissionTemplate->GarrisonType == GarrisonType::GarrisonBrokenIsles && l_Mission->ChestChance > 100)
            m_PendingMissionReward.BonusChest = roll_chance_i(l_Mission->ChestChance - 100);
        else
            m_PendingMissionReward.BonusChest = false;

        bool l_IsZoneSupportMission = l_MissionTemplate && l_MissionTemplate->Flags & MS::Garrison::Mission::Flags::IsZoneSupport;
        if (l_MissionTemplate && l_MissionTemplate->Flags & MS::Garrison::Mission::Flags::AlwaysFail)
            l_Succeeded = false;

        if (l_MissionFollowers.size() == 1)
        {
            if (l_MissionFollowers[0]->FollowerID)
            {
                m_Owner->RemoveAurasDueToSpell(l_MissionFollowers[0]->ZoneSupportSpellID);
                DespawnBodyguardIfNeeded();
            }
        }

        if (!l_Succeeded)
            l_Mission->State = MS::Garrison::Mission::State::Reward3Claimed;
        else
        {
            if (m_PendingMissionReward.BonusChest)
                l_Mission->State = MS::Garrison::Mission::State::OvermaxRewardPending;
            else
                l_Mission->State = MS::Garrison::Mission::State::Completed;
        }

        WorldPacket l_Result(p_IsResponse ? SMSG_GARRISON_UPDATE_MISSION : SMSG_GARRISON_COMPLETE_MISSION_RESULT, 100);
        l_Result << uint32(!l_CanComplete);

        l_Result << uint64(l_Mission->DatabaseID);
        l_Result << uint32(l_Mission->MissionID);
        l_Result << uint32(l_Mission->OfferTime);
        l_Result << uint32(l_Mission->OfferMaxDuration);
        l_Result << uint32(l_Mission->StartTime);
        l_Result << uint32(l_Mission->TravelDuration);
        l_Result << uint32(l_Mission->Duration);
        l_Result << uint32(l_Mission->State);
        l_Result << uint32(l_Mission->ChestChance);
        l_Result << uint32(l_Mission->Unk);

        l_Result << uint32(l_Mission->MissionID);
        l_Result << uint32(l_MissionFollowers.size());  ///< Unk Loop
        for (uint32 l_I = 0; l_I < l_MissionFollowers.size(); ++l_I)
        {
            l_Result << uint64(l_MissionFollowers[l_I]->DatabaseID);
            l_Result << uint32(l_MissionFollowers[l_I]->Flags);
        }
        l_Result.WriteBit(l_Succeeded);
        l_Result.FlushBits();

        m_Owner->SendDirectMessage(&l_Result);

        //std::vector<uint32> l_PartyXPModifiersEffect = GetMissionFollowersAbilitiesEffects(p_MissionRecID, AbilityEffectTypes::ModXp, AbilityEffectTargetMasks::Unk | AbilityEffectTargetMasks::Party);
        std::vector<uint32> l_PassiveEffects;

        if (l_MissionTemplate->GarrisonType == GarrisonType::GarrisonDraenor && m_GarrisonDraenor)
            l_PassiveEffects = m_GarrisonDraenor->GetBuildingsPassiveAbilityEffects();

        /// Global XP Bonus modifier
        float l_XPModifier = l_Mission->ChestChance > 100 ? static_cast<float>(l_Mission->ChestChance) / 100.0f : 1.0f;
/*
        for (uint32 l_I = 0; l_I < l_PartyXPModifiersEffect.size(); ++l_I)
        {
            GarrAbilityEffectEntry const* l_AbilityEffectEntry = sGarrAbilityEffectStore.LookupEntry(l_PartyXPModifiersEffect[l_I]);

            if (!l_AbilityEffectEntry)
                continue;

            l_XPModifier = (l_AbilityEffectEntry->Amount - 1.0) + l_XPModifier;
        }*/

/*
        for (uint32 l_Y = 0; l_Y < l_PassiveEffects.size(); ++l_Y)
        {
            GarrAbilityEffectEntry const* l_AbilityEffectEntry = sGarrAbilityEffectStore.LookupEntry(l_PassiveEffects[l_Y]);

            if (!l_AbilityEffectEntry)
                continue;

            if (l_AbilityEffectEntry->EffectType == AbilityEffectTypes::ModXp && (l_AbilityEffectEntry->TargetMask == AbilityEffectTargetMasks::Party || l_AbilityEffectEntry->TargetMask == AbilityEffectTargetMasks::Unk))
                l_XPModifier = (l_AbilityEffectEntry->Amount - 1.0) + l_XPModifier;
        }*/
        /// ------------------------------------------

        float l_BonusXP = (l_XPModifier - 1.0f) * l_MissionTemplate->RewardFollowerExperience;

        uint32 l_TroopsToRegenerateHP = 0;
        for (GarrAbilityEffectEntry const* l_Ability : GetApplicableGarrAbilitiesEffectsByType(MS::Garrison::AbilityEffectTypes::OneTroopRegeneratesHp, p_MissionRecID))
            l_TroopsToRegenerateHP++;

        std::vector<uint64> l_ToRemoveList;
        for (uint32 l_FollowerIt = 0; l_FollowerIt < l_MissionFollowers.size(); ++l_FollowerIt)
        {
            if (!l_MissionFollowers[l_FollowerIt]->CanXP())
            {
                if ((l_MissionFollowers[l_FollowerIt]->Flags & FollowerFlags::isLimitedUse) != 0)
                {
                    if (l_Mission->IsLethalToTroops)
                        l_MissionFollowers[l_FollowerIt]->Durability = 0;
                    else
                    {
                        if (l_TroopsToRegenerateHP)
                            l_TroopsToRegenerateHP--;
                        else
                            l_MissionFollowers[l_FollowerIt]->Durability--;
                    }

                    if (l_MissionFollowers[l_FollowerIt]->Durability)
                        l_MissionFollowers[l_FollowerIt]->SendFollowerUpdate(m_Owner);
                    else
                    {
                        uint32 l_Chance = 0;
                        for (GarrAbilityEffectEntry const* l_Ability : GetApplicableGarrAbilitiesEffectsByType(MS::Garrison::AbilityEffectTypes::CanRecoverDeadTroops, p_MissionRecID))
                        {
                            if (l_Ability->ActionValueFlat > l_Chance)
                                l_Chance = l_Ability->ActionValueFlat;
                        }
                        if (roll_chance_i(l_Chance))
                            l_MissionFollowers[l_FollowerIt]->Durability = 1;
                        else
                            l_ToRemoveList.push_back(l_MissionFollowers[l_FollowerIt]->DatabaseID);
                    }
                }

                continue;
            }

            uint32 l_FollowerLevel = l_MissionFollowers[l_FollowerIt]->Level;
            float l_SecondXPModifier = 1.0f;

            /// Personal XP Bonus
            for (uint32 l_AbilityIt = 0; l_AbilityIt < l_MissionFollowers[l_FollowerIt]->Abilities.size(); l_AbilityIt++)
            {
                uint32 l_CurrentAbilityID = l_MissionFollowers[l_FollowerIt]->Abilities[l_AbilityIt];

                for (uint32 l_EffectIt = 0; l_EffectIt < sGarrAbilityEffectStore.GetNumRows(); l_EffectIt++)
                {
                    GarrAbilityEffectEntry const* l_AbilityEffectEntry = sGarrAbilityEffectStore.LookupEntry(l_EffectIt);

                    if (!l_AbilityEffectEntry || l_AbilityEffectEntry->AbilityID != l_CurrentAbilityID)
                        continue;

                    if (l_AbilityEffectEntry->EffectType == AbilityEffectTypes::ModXp && l_AbilityEffectEntry->TargetMask == AbilityEffectTargetMasks::Self)
                        l_SecondXPModifier = (l_AbilityEffectEntry->ActionValueFlat - 1.0) + l_SecondXPModifier;
                }
            }

            uint32 l_AddedXP = (l_BonusXP + l_MissionTemplate->RewardFollowerExperience) * l_SecondXPModifier;
            l_MissionFollowers[l_FollowerIt]->EarnXP(l_AddedXP, m_Owner);

            GenerateFollowerAbilities(l_MissionFollowers[l_FollowerIt]->DatabaseID, false, true, true, true);

            if (l_FollowerLevel != l_MissionFollowers[l_FollowerIt]->Level && l_MissionFollowers[l_FollowerIt]->Level == 100)
                m_Owner->UpdateCriteria(CRITERIA_TYPE_RAISE_GARRISON_FOLLOWER_LEVEL);
        }

        for (auto l_DB_ID : l_ToRemoveList)
        {
            auto l_It = std::find_if(l_MissionFollowers.begin(), l_MissionFollowers.end(), [l_DB_ID](const GarrisonFollower* p_Follower)
            {
                return p_Follower->DatabaseID == l_DB_ID;
            });

            if (l_It == l_MissionFollowers.end())
                continue;

            (*l_It)->CurrentMissionID = 0;
            l_MissionFollowers.erase(l_It);
        }

        for (auto l_DB_ID : l_ToRemoveList)
            RemoveFollower(l_DB_ID, true);

        m_PendingMissionReward.RewardFollowerXPBonus.clear();
        m_PendingMissionReward.MissionFollowers.clear();
        m_PendingMissionReward.RewardCurrencies.clear();
        m_PendingMissionReward.RewardItems.clear();
        m_PendingMissionReward.RewardGold       = 0;
        m_PendingMissionReward.RewardFollowerXP = 0;
        m_PendingMissionReward.Rewarded         = false;
        m_PendingMissionReward.MissionID        = p_MissionRecID;

        if (l_Succeeded)
        {
            for (GarrAbilityEffectEntry const* l_Ability : GetApplicableGarrAbilitiesEffectsByType(MS::Garrison::AbilityEffectTypes::ModTreasureOnMission, p_MissionRecID))
            {
                if (roll_chance_i(20)) ///< Wowhead comment
                    m_Owner->AutoStoreLoot(MS::Garrison::GBrokenIsles::s_ArmamentLootTemplates[m_Owner->getClass()], LootTemplates_Item);
            }

            m_Owner->UpdateCriteria(CRITERIA_TYPE_COMPLETE_GARRISON_MISSION, p_MissionRecID);

            for (uint32 l_I = 0; l_I < l_MissionFollowers.size(); ++l_I)
            {
                m_PendingMissionReward.MissionFollowers.push_back(l_MissionFollowers[l_I]->DatabaseID);
            }

            auto l_Rewards = sObjectMgr->GetMissionRewards(p_MissionRecID);

            if (l_Rewards)
            {
                for (auto l_Reward = l_Rewards->begin(); l_Reward != l_Rewards->end(); ++l_Reward)
                {
                    if ((*l_Reward).ItemID)
                        m_PendingMissionReward.RewardItems.push_back(std::make_pair((*l_Reward).ItemID, (*l_Reward).ItemQuantity));

                    if ((*l_Reward).CurrencyType == 0)
                        m_PendingMissionReward.RewardGold += (*l_Reward).CurrencyQuantity;

                    if ((*l_Reward).CurrencyType)
                    {
                        uint32 l_Amount = (*l_Reward).CurrencyQuantity;

                        if ((*l_Reward).CurrencyType == sGarrTypeStore.LookupEntry(l_MissionTemplate->GarrisonType)->CurrencyType
                            || (*l_Reward).CurrencyType == sGarrTypeStore.LookupEntry(l_MissionTemplate->GarrisonType)->SecondCurrencyType)
                        {
                            std::vector<uint32> l_PartyCurrencyModifiersEffect = GetMissionFollowersAbilitiesEffects(p_MissionRecID, AbilityEffectTypes::ModCurrency, AbilityEffectTargetMasks::Unk | AbilityEffectTargetMasks::Party);

                            /// Global currency Bonus modifier
                            float l_Modifier = 1.0f;
                            for (uint32 l_I = 0; l_I < l_PartyCurrencyModifiersEffect.size(); ++l_I)
                            {
                                GarrAbilityEffectEntry const* l_AbilityEffectEntry = sGarrAbilityEffectStore.LookupEntry(l_PartyCurrencyModifiersEffect[l_I]);

                                if (!l_AbilityEffectEntry)
                                    continue;

                                l_Modifier = (l_AbilityEffectEntry->ActionValueFlat - 1.0) + l_Modifier;
                            }

                            l_Amount += (l_Modifier - 1.0f) * l_Amount;
                        }

                        m_PendingMissionReward.RewardCurrencies.push_back(std::make_pair((*l_Reward).CurrencyType, l_Amount));
                    }

                    if ((*l_Reward).FollowerXP)
                        m_PendingMissionReward.RewardFollowerXP += (*l_Reward).FollowerXP;
                }
            }

            if (l_MissionTemplate->GarrisonType == GarrisonType::GarrisonDraenor && m_GarrisonDraenor)
            {
                switch (l_MissionTemplate->RequiredLevel)
                {
                    case 90:
                    case 91:
                    case 92:
                    case 93:
                    case 94:
                    {
                        if (m_GarrisonDraenor->HasActiveBuilding(GDraenor::Building::ID::SalvageYard_SalvageYard_Level1) && roll_chance_i(30))
                            m_PendingMissionReward.RewardItems.push_back(std::make_pair(GDraenor::Items::ItemBagOfSalvagedGoods, 1));
                        else if (m_GarrisonDraenor->HasActiveBuilding(GDraenor::Building::ID::SalvageYard_SalvageYard_Level2) && roll_chance_i(50))
                            m_PendingMissionReward.RewardItems.push_back(std::make_pair(GDraenor::Items::ItemBagOfSalvagedGoods, 1));
                        else if (m_GarrisonDraenor->HasActiveBuilding(GDraenor::Building::ID::SalvageYard_SalvageYard_Level3) && roll_chance_i(75))
                            m_PendingMissionReward.RewardItems.push_back(std::make_pair(GDraenor::Items::ItemBagOfSalvagedGoods, 1));
                        break;
                    }
                    case 95:
                    case 96:
                    case 97:
                    case 98:
                    case 99:
                    {
                        if (m_GarrisonDraenor->HasActiveBuilding(GDraenor::Building::ID::SalvageYard_SalvageYard_Level1)&& roll_chance_i(50))
                            m_PendingMissionReward.RewardItems.push_back(std::make_pair(GDraenor::Items::ItemBagOfSalvagedGoods, 1));
                        else if (m_GarrisonDraenor->HasActiveBuilding(GDraenor::Building::ID::SalvageYard_SalvageYard_Level2) && roll_chance_i(30))
                            m_PendingMissionReward.RewardItems.push_back(std::make_pair(GDraenor::Items::ItemCrateOfSalvage, 1));
                        else if (m_GarrisonDraenor->HasActiveBuilding(GDraenor::Building::ID::SalvageYard_SalvageYard_Level3) && roll_chance_i(50))
                            m_PendingMissionReward.RewardItems.push_back(std::make_pair(GDraenor::Items::ItemCrateOfSalvage, 1));
                        break;
                    }
                    case 100:
                    {
                        if (m_GarrisonDraenor->HasActiveBuilding(GDraenor::Building::ID::SalvageYard_SalvageYard_Level1) && roll_chance_i(75))
                            m_PendingMissionReward.RewardItems.push_back(std::make_pair(GDraenor::Items::ItemBagOfSalvagedGoods, 1));
                        else if (m_GarrisonDraenor->HasActiveBuilding(GDraenor::Building::ID::SalvageYard_SalvageYard_Level2) && roll_chance_i(50))
                            m_PendingMissionReward.RewardItems.push_back(std::make_pair(GDraenor::Items::ItemCrateOfSalvage, 1));
                        else if (m_GarrisonDraenor->HasActiveBuilding(GDraenor::Building::ID::SalvageYard_SalvageYard_Level3) && roll_chance_i(50))
                            m_PendingMissionReward.RewardItems.push_back(std::make_pair(GDraenor::Items::ItemBigCrateOfSalvage, 1));
                        break;
                    }
                    default:
                        break;
                }
            }

            l_BonusXP = (l_XPModifier - 1.0f) * m_PendingMissionReward.RewardFollowerXP;
            m_PendingMissionReward.RewardFollowerXP += l_BonusXP;

            for (uint32 l_FollowerIt = 0; l_FollowerIt < l_MissionFollowers.size(); ++l_FollowerIt)
            {
                float l_SecondXPModifier = 1.0f;

                /// Personnal XP Bonus
                for (uint32 l_AbilityIt = 0; l_AbilityIt < l_MissionFollowers[l_FollowerIt]->Abilities.size(); l_AbilityIt++)
                {
                    uint32 l_CurrentAbilityID = l_MissionFollowers[l_FollowerIt]->Abilities[l_AbilityIt];

                    for (uint32 l_EffectIt = 0; l_EffectIt < sGarrAbilityEffectStore.GetNumRows(); l_EffectIt++)
                    {
                        const GarrAbilityEffectEntry * l_AbilityEffectEntry = sGarrAbilityEffectStore.LookupEntry(l_EffectIt);

                        if (!l_AbilityEffectEntry || l_AbilityEffectEntry->AbilityID != l_CurrentAbilityID)
                            continue;

                        if (l_AbilityEffectEntry->EffectType == AbilityEffectTypes::ModXp && l_AbilityEffectEntry->TargetMask == AbilityEffectTargetMasks::Self)
                            l_SecondXPModifier = (l_AbilityEffectEntry->ActionValueFlat - 1.0) + l_SecondXPModifier;
                    }
                }

                m_PendingMissionReward.RewardFollowerXPBonus.push_back(std::make_pair(l_MissionFollowers[l_FollowerIt]->DatabaseID, (l_BonusXP + m_PendingMissionReward.RewardFollowerXP) * l_SecondXPModifier));
            }
        }

        /// Unassign follower to the mission
        for (uint32 l_I = 0; l_I < m_Followers.size(); ++l_I)
        {
            if (m_Followers[l_I].CurrentMissionID == p_MissionRecID)
            {
                m_Followers[l_I].CurrentMissionID = 0;
                m_Followers[l_I].SendFollowerUpdate(m_Owner);
            }
        }

        UpdateMissionDistribution(true);

        sScriptMgr->OnGarrisonMissionComplete(m_Owner, p_MissionRecID, l_Succeeded);
    }
    /// Do mission bonus roll
    void Manager::DoMissionBonusRoll(uint64 p_MissionTableGUID, uint32 p_MissionRecID)
    {
        auto l_It = std::find_if(m_Missions.begin(), m_Missions.end(), [p_MissionRecID](const GarrisonMission& p_Mission) -> bool
        {
            if (p_Mission.State == Mission::State::Reward3Claimed || p_Mission.State == Mission::State::Completed || p_Mission.State == Mission::State::OvermaxRewardPending)
            {
                if (p_Mission.MissionID == p_MissionRecID)
                    return true;
            }

            return false;
        });

        GarrisonMission* l_Mission = nullptr;

        if (l_It != m_Missions.end())
            l_Mission = reinterpret_cast<GarrisonMission*>(&(*l_It));

        GarrMissionEntry const* l_MissionTemplate = sGarrMissionStore.LookupEntry(p_MissionRecID);

        if (m_PendingMissionReward.MissionID != p_MissionRecID || m_PendingMissionReward.Rewarded == true || !l_Mission || (l_Mission && (l_Mission->State == Mission::State::Reward3Claimed)))
        {
            m_PendingMissionReward.Rewarded = true;

            WorldPacket l_Packet(SMSG_GARRISON_MISSION_BONUS_ROLL_RESULT, 100);

            if (l_Mission)
            {
                l_Packet << uint64(l_Mission->DatabaseID);
                l_Packet << uint32(l_Mission->MissionID);
                l_Packet << uint32(l_Mission->OfferTime);
                l_Packet << uint32(l_Mission->OfferMaxDuration);
                l_Packet << uint32(l_Mission->StartTime);
                l_Packet << uint32(l_Mission->TravelDuration);
                l_Packet << uint32(l_Mission->Duration);
                l_Packet << uint32(l_Mission->State);
                l_Packet << uint32(l_Mission->ChestChance);
                l_Packet << uint32(l_Mission->Unk);
            }
            else
            {
                l_Packet << uint64(0);
                l_Packet << uint32(0);
                l_Packet << uint32(0);
                l_Packet << uint32(0);
                l_Packet << uint32(0);
                l_Packet << uint32(0);
                l_Packet << uint32(0);
                l_Packet << uint32(0);
                l_Packet << uint32(0/*l_Mission->Unk1*/);
                l_Packet << uint32(0/*l_Mission->Unk2*/);
            }

            l_Packet << uint32(p_MissionRecID);
            l_Packet << uint32(Mission::BonusRollResults::Ok);

            m_Owner->SendDirectMessage(&l_Packet);

            return;
        }

        bool l_BonusSuccess = true;
        l_Mission->State = Mission::State::Reward3Claimed;

        if (l_MissionTemplate && l_MissionTemplate->GarrisonType == GarrisonType::GarrisonBrokenIsles)
            l_BonusSuccess = m_PendingMissionReward.BonusChest;

        WorldPacket l_Packet(SMSG_GARRISON_MISSION_BONUS_ROLL_RESULT, 100);

        l_Packet << uint64(l_Mission->DatabaseID);
        l_Packet << uint32(l_Mission->MissionID);
        l_Packet << uint32(l_Mission->OfferTime);
        l_Packet << uint32(l_Mission->OfferMaxDuration);
        l_Packet << uint32(l_Mission->StartTime);
        l_Packet << uint32(l_Mission->TravelDuration);
        l_Packet << uint32(l_Mission->Duration);
        l_Packet << uint32(l_Mission->State);
        l_Packet << uint32(l_Mission->ChestChance);
        l_Packet << uint32(l_Mission->Unk);

        l_Packet << uint32(p_MissionRecID);
        l_Packet << uint32(Mission::BonusRollResults::Ok);

        m_Owner->SendDirectMessage(&l_Packet);

        if (l_BonusSuccess)
        {
            auto l_Rewards = sObjectMgr->GetMissionRewards(p_MissionRecID);

            if (l_Rewards)
            {
                for (auto l_Reward = l_Rewards->begin(); l_Reward != l_Rewards->end(); ++l_Reward)
                {
                    if ((*l_Reward).BonusItemID)
                        m_PendingMissionReward.RewardItems.push_back(std::make_pair((*l_Reward).BonusItemID, 1));

                    if ((*l_Reward).BonusGold)
                        m_PendingMissionReward.RewardGold += (*l_Reward).BonusGold;
                }
            }
        }


        m_PendingMissionReward.Rewarded = true;

        m_Owner->ModifyMoney(m_PendingMissionReward.RewardGold, "Manager::DoMissionBonusRoll");

        for (auto l_Currency : m_PendingMissionReward.RewardCurrencies)
        {
            CurrencyTypesEntry const* l_CurrencyEntry = sCurrencyTypesStore.LookupEntry(l_Currency.first);

            if (l_CurrencyEntry)
                m_Owner->ModifyCurrency(l_Currency.first, l_Currency.second, false);
        }

        for (auto l_Item : m_PendingMissionReward.RewardItems)
        {
            const ItemTemplate* l_ItemTemplate = sObjectMgr->GetItemTemplate(l_Item.first);

            if (!l_ItemTemplate)
                continue;

            /// Check if the rewarded item is an follower contract, because make server crash on Player::SendNewItem,
            /// the spell is auto casted by the player, and the spell delete the caster item itself.
            bool l_IsContractItem = false;
            if (l_ItemTemplate->Spells[0].SpellId)
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_ItemTemplate->Spells[0].SpellId);

                if (l_SpellInfo && l_SpellInfo->Effects[0].Effect == SPELL_EFFECT_ADD_GARRISON_FOLLOWER)
                {
                    l_IsContractItem = true;
                    m_Owner->CastSpell(m_Owner, l_SpellInfo, TRIGGERED_FULL_MASK);
                }
            }

            if (!l_IsContractItem)
            {
                uint32 l_NoSpaceForCount = 0;
                uint32 l_Count = l_Item.second;
                ItemPosCountVec l_Dest;

                if (m_Owner->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Dest, l_Item.first, l_Count, &l_NoSpaceForCount) != EQUIP_ERR_OK)
                    l_Count -= l_NoSpaceForCount;

                if (l_Count == 0 || l_Dest.empty())
                {
                    Item* l_NewItem = l_Item.first ? Item::CreateItem(l_Item.first, l_Item.second, m_Owner) : nullptr;

                    int l_LocIDX = m_Owner->GetSession()->GetSessionDbLocaleIndex(); ///< l_LocIDX is never read 01/18/16

                    MailDraft l_Draft("Garrison mission reward", "");

                    SQLTransaction l_Transaction = CharacterDatabase.BeginTransaction();
                    if (l_NewItem)
                    {
                        // /Save new item before send
                        l_NewItem->SaveToDB(l_Transaction);                               /// Save for prevent lost at next mail load, if send fail then item will deleted

                        /// Item
                        l_Draft.AddItem(l_NewItem);
                    }

                    l_Draft.SendMailTo(l_Transaction, m_Owner, MailSender(MAIL_CREATURE, GUID_ENPART(p_MissionTableGUID)));
                    CharacterDatabase.CommitTransaction(l_Transaction);
                }
                else
                    m_Owner->AddItem(l_Item.first, l_Item.second);
            }
        }

        std::vector<GarrisonFollower*> l_MissionFollowers;

        std::for_each(m_Followers.begin(), m_Followers.end(), [this, &l_MissionFollowers](const GarrisonFollower& p_Follower) -> void
        {
            if (std::find(m_PendingMissionReward.MissionFollowers.begin(), m_PendingMissionReward.MissionFollowers.end(), p_Follower.DatabaseID) != m_PendingMissionReward.MissionFollowers.end())
                l_MissionFollowers.push_back(const_cast<GarrisonFollower*>(&p_Follower));
        });

        std::for_each(l_MissionFollowers.begin(), l_MissionFollowers.end(), [this](const GarrisonFollower* p_Follower) -> void
        {
            uint32 l_FollowerLevel = p_Follower->Level;
            uint32 l_AddedXP = m_PendingMissionReward.RewardFollowerXP;

            std::for_each(m_PendingMissionReward.RewardFollowerXPBonus.begin(), m_PendingMissionReward.RewardFollowerXPBonus.end(), [p_Follower, &l_AddedXP](const std::pair<uint64, uint32>& p_Values)
            {
                if (p_Values.first == p_Follower->DatabaseID)
                    l_AddedXP += p_Values.second;
            });

            l_AddedXP = const_cast<GarrisonFollower*>(p_Follower)->EarnXP(l_AddedXP, m_Owner);
            GenerateFollowerAbilities(p_Follower->DatabaseID, false, true, true, true);

            if (l_FollowerLevel != p_Follower->Level && p_Follower->Level == 100)
                m_Owner->UpdateCriteria(CRITERIA_TYPE_RAISE_GARRISON_FOLLOWER_LEVEL);
        });
    }
    /// Set mission has complete
    void Manager::SetAllInProgressMissionAsComplete()
    {
        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            if (m_Missions[l_I].State == Mission::State::InProgress)
                m_Missions[l_I].StartTime = time(0) - (GetMissionTravelDuration(m_Missions[l_I].MissionID) + m_Missions[l_I].Duration);
        }

        WorldPacket l_PlaceHolder;
        m_Owner->GetSession()->HandleGetGarrisonInfoOpcode(l_PlaceHolder);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get the mission travel time
    uint32 Manager::GetMissionTravelDuration(uint32 p_MissionRecID)
    {
        GarrMissionEntry const* l_MissionTemplate = sGarrMissionStore.LookupEntry(p_MissionRecID);

        std::vector<uint32> l_AbilitiesEffects = GetMissionFollowersAbilitiesEffects(p_MissionRecID);
        std::vector<uint32> l_PassiveEffects;
        float               l_MissionTravelTime = l_MissionTemplate->TravelTime;

        if (l_MissionTemplate->GarrisonType == GarrisonType::GarrisonDraenor && m_GarrisonDraenor)
            l_PassiveEffects = m_GarrisonDraenor->GetBuildingsPassiveAbilityEffects();

        return floorf(l_MissionTravelTime);
    }
    /// Get the mission duration
    uint32 Manager::GetMissionDuration(uint32 p_MissionRecID)
    {
        GarrMissionEntry const*     l_MissionTemplate   = sGarrMissionStore.LookupEntry(p_MissionRecID);
        std::vector<uint32>         l_AbilitiesEffects  = GetMissionFollowersAbilitiesEffects(p_MissionRecID);
        float                       l_MissionDuration   = l_MissionTemplate->Duration;

        for (uint32 l_EffectI = 0; l_EffectI < l_AbilitiesEffects.size(); l_EffectI++)
        {
            GarrAbilityEffectEntry const* l_AbilityEffectEntry = sGarrAbilityEffectStore.LookupEntry(l_AbilitiesEffects[l_EffectI]);

            if (!l_AbilityEffectEntry)
                continue;

            if (l_AbilityEffectEntry->EffectType == AbilityEffectTypes::ModMissionTime)
                l_MissionDuration = l_MissionDuration * l_AbilityEffectEntry->ActionValueFlat;
        }

        return floorf(l_MissionDuration);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get missions
    std::vector<GarrisonMission> Manager::GetMissions(GarrisonType::Type p_Type) const
    {
        std::vector<GarrisonMission> l_Result;

        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            GarrMissionEntry const* l_MissionEntry = sGarrMissionStore.LookupEntry(m_Missions[l_I].MissionID);
            if (l_MissionEntry && m_Missions[l_I].State < Mission::State::Completed
                && (!((m_Missions[l_I].OfferMaxDuration && (m_Missions[l_I].OfferTime + m_Missions[l_I].OfferMaxDuration) < time(nullptr)) && m_Missions[l_I].State == Mission::State::Offered) || l_MissionEntry->Flags & Mission::Flags::IsZoneSupport)
                && l_MissionEntry->GarrisonType == p_Type)
                l_Result.push_back(m_Missions[l_I]);
        }

        return l_Result;
    }
    /// Get all completed missions
    std::vector<GarrisonMission> Manager::GetCompletedMissions(GarrisonType::Type p_Type) const
    {
        std::vector<GarrisonMission> l_Result;

        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            if (m_Missions[l_I].State >= Mission::State::Completed
                && sGarrMissionStore.LookupEntry(m_Missions[l_I].MissionID)->GarrisonType == p_Type)
                l_Result.push_back(m_Missions[l_I]);
        }

        return l_Result;
    }
    /// Get all completed missions
    std::vector<GarrisonMission> Manager::GetPendingMissions(GarrisonType::Type p_Type) const
    {
        std::vector<GarrisonMission> l_Result;

        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            if (m_Missions[l_I].State >= Mission::State::InProgress
                && sGarrMissionStore.LookupEntry(m_Missions[l_I].MissionID)->GarrisonType == p_Type)
                l_Result.push_back(m_Missions[l_I]);
        }

        return l_Result;
    }
    /// Check if player has pending mission
    bool Manager::HasPendingMission(uint32 p_MissionID)
    {
        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            if (m_Missions[l_I].State == Mission::State::InProgress && m_Missions[l_I].MissionID == p_MissionID)
                return true;
        }

        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get mission with mission ID
    GarrisonMission* Manager::GetMissionWithID(uint32 p_MissionID)
    {
        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            if (m_Missions[l_I].MissionID == p_MissionID)
                return &m_Missions[l_I];
        }

        return nullptr;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get mission chest chance
    void Manager::InitMission(GarrisonMission* p_Mission, std::vector<uint64> const& p_Followers)
    {
        GarrMissionEntry const* l_MissionTemplate   = sGarrMissionStore[p_Mission->MissionID];
        GarrTypeEntry    const* l_GarrType          = sGarrTypeStore[l_MissionTemplate->GarrisonType];

        std::vector<GarrisonFollower*>          l_MissionFollowers;

        for (auto l_GUID : p_Followers)
            l_MissionFollowers.push_back(GetFollowerWithDatabaseID(l_GUID));

        std::vector<uint32>                     l_Encounters;
        std::vector<std::pair<uint32, GarrMechanicEntry const*>>  l_EncoutersMechanics;
        std::vector<uint32>                     l_PassiveEffects;
        std::map<uint64, double>                l_FollowersBiasMap;
        std::vector<uint32>                     l_CounterAbilityUsed;
        std::map<uint64, GarrAbilityEffectEntry const*>  l_AdditionalAbilityEffects;
        std::vector<GarrAbilityEffectEntry const*>  l_FollowerAbilities;
        std::vector<GarrAbilityEffectEntry const*>  l_ProccedEffects;
        std::vector<GarrAbilityEffectEntry const*>  l_Talents;

        if (l_MissionTemplate->GarrisonType == GarrisonType::GarrisonDraenor && m_GarrisonDraenor)
            l_PassiveEffects = m_GarrisonDraenor->GetBuildingsPassiveAbilityEffects();

        /// ------------------- WARNING --------------
        /// DO NOT MODIFY THE CODE BELOW, TRUST ME YOU DO NOT KNOW HOW IT WORKS
        /// ONLY MODIFY IT IF YOU ARE SUCH A NERD THAT IDA IS YOUR BEST FRIEND
        /// OR BETTER DO NOT DO IT EVEN IF SO
        /// IT MUST BE EXECUTED IN THE EXACT ORDER
        /// TRUST ME
        /// IVE SPENT MY CHILDHOOD ON THIS CODE

        uint64 l_RandomNumber = p_Mission->DatabaseID;

        for (auto l_EncounterStore : sGarrMissionXEncouterStore)
        {
            if (l_EncounterStore->MissionID == p_Mission->MissionID)
            {

                uint32 l_EncounterID;

                if ((l_EncounterID = l_EncounterStore->EncounterID))
                {
                    l_Encounters.push_back(l_EncounterID);
                }
                else if (uint32 l_EncounterSetID = l_EncounterStore->GarrEncounterSetID)
                {
                    if (auto l_EncounterList = GetGarrEncounterListBySetID(l_EncounterSetID))
                    {
                        l_RandomNumber = (0xBC8FLL * l_RandomNumber) % 0x7FFFFFFFLL;
                        int32 l_Index = l_RandomNumber % l_EncounterList->size();
                        l_EncounterID = (*l_EncounterList)[l_Index];
                        l_Encounters.push_back(l_EncounterID);

                    }
                }

                for (auto l_Entry : sGarrEncouterXMechanicStore)
                {
                    if (l_Entry->EncounterID == l_EncounterID)
                    {
                        if (uint32 l_MechanicSetID = l_Entry->GarrMechanicSetID)
                        {
                            if (auto l_MechanicList = GetGarrMechanicListBySetID(l_MechanicSetID))
                            {
                                l_RandomNumber = (0xBC8FLL * l_RandomNumber) % 0x7FFFFFFFLL;
                                int32 l_Index = l_RandomNumber % l_MechanicList->size();

                                auto l_Mechanic = (*l_MechanicList)[l_Index];
                                l_EncoutersMechanics.push_back(std::make_pair(l_EncounterID, l_Mechanic));

                            }
                        }

                        if (uint32 l_MechanicID = l_Entry->MechanicID)
                        {
                            l_EncoutersMechanics.push_back(std::make_pair(l_EncounterID, sGarrMechanicStore[l_MechanicID]));
                        }
                    }
                }
            }
        }
        /// ------------------END OF WARNING------------------

        for (auto& l_Follower : l_MissionFollowers)
        {
            for (uint32 l_CurrentAbilityID : l_Follower->Abilities)
            {
                for (auto l_AbilityEffectEntry : sGarrAbilityEffectStore)
                {
                    if (l_AbilityEffectEntry->AbilityID == l_CurrentAbilityID)
                    {
                        l_AdditionalAbilityEffects.insert({ l_Follower->DatabaseID, l_AbilityEffectEntry});
                        l_FollowerAbilities.push_back(l_AbilityEffectEntry);
                    }
                }
            }
        }

        auto l_GarrisonTalents = GetGarrisonTalents(static_cast<GarrisonType::Type>(l_MissionTemplate->GarrisonType));
        for (auto& l_Talent : l_GarrisonTalents)
        {
            if (uint32 l_CurrentAbilityID = l_Talent->GetEntry()->GarrAbilityID)
            {
                for (auto l_AbilityEffectEntry : sGarrAbilityEffectStore)
                {
                    if (l_AbilityEffectEntry->AbilityID == l_CurrentAbilityID)
                    {
                        /// only 1 per day for abbilies like bloodlust -- @TODO: Find a better how to do this
                        if (l_AbilityEffectEntry->EffectType == AbilityEffectTypes::ModsuccessChance && GetMissionStartedTodayCount(static_cast<GarrisonType::Type>(l_MissionTemplate->GarrisonType)))
                            continue;

                        l_Talents.push_back(l_AbilityEffectEntry);
                    }
                }
            }
        }

        /// 100% 7.1.5
        for (auto& l_Follower : l_MissionFollowers)
        {
            auto l_GarrFollowerType = l_Follower->GetFollowerType();

            double l_FollowerBias = 0.0;

            if (!(l_Follower->GetEntry()->Flags & FollowerTypeFlags::Unk2))
            {
                l_FollowerBias = (static_cast<float>(l_Follower->Level) - static_cast<float>(l_MissionTemplate->RequiredLevel)) / static_cast<float>(l_GarrFollowerType->LevelBiasDividor);

                if (l_MissionTemplate->RequiredLevel == GetMaxLevelForExpansion(l_GarrType->ExpansionRequired) || l_Follower->Level == GetMaxLevelForExpansion((l_GarrType->ExpansionRequired)))
                {
                    uint32 l_RequiredIlevel = l_MissionTemplate->RequiredItemLevel;
                    if (l_RequiredIlevel == 0)
                        l_RequiredIlevel = 600;

                    l_FollowerBias += (static_cast<float>((l_Follower->ItemLevelArmor + l_Follower->ItemLevelWeapon) >> 1) - static_cast<float>(l_RequiredIlevel)) / static_cast<float>(l_GarrFollowerType->ItemLevelBiasDividor);
                }
            }

            if (l_FollowerBias < -1.0)
                l_FollowerBias = -1.0;
            else if (l_FollowerBias > 1.0)
                l_FollowerBias = 1.0;

            l_FollowersBiasMap[l_Follower->DatabaseID] = l_FollowerBias;
        }


        double l_BaseChance = l_GarrType->Flags & 1 ? 100.f : l_MissionTemplate->RequiredFollowersCount * 100.f;
        double l_SeilDividor = l_BaseChance;

        for (auto l_EncounterEntry : l_EncoutersMechanics)
        {
            auto l_MechanicTypeEntry = sGarrMechanicTypeStore[l_EncounterEntry.second->MechanicTypeID];

            if (!l_MechanicTypeEntry || l_MechanicTypeEntry->Type == MechanicTypes::Ability)
                l_SeilDividor += l_EncounterEntry.second->Factor;
        }

        double l_CurrentAdditionalWinChance = 0.f;
        double l_SeilMultiplier = 100.0 / l_SeilDividor;

        /// OK 100% 7.1.5
        #pragma region Followers Bias
        for (uint32 l_Y = 0; l_Y < l_MissionFollowers.size(); ++l_Y)
        {
            double l_Seil;

            if (l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID] >= 0.0)
                l_Seil = (150.f - 100.f) * l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID] + 100.f;
            else
                l_Seil = (l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID] + 1.0) * 100.f;

            l_CurrentAdditionalWinChance += (l_Seil * (100.0 / l_SeilDividor));
        }
        #pragma endregion
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        double l_UnseiledChance = 0.f;
        /// OK 100%
        #pragma region Counter mechanic
        for (uint32 l_I = 0; l_I < l_EncoutersMechanics.size(); ++l_I)
        {
            GarrMechanicEntry const* l_MechanicEntry = l_EncoutersMechanics[l_I].second;
            uint32 l_MechanicId = l_MechanicEntry->ID;

            double l_Chance = l_MechanicEntry->Factor;
            bool l_Countered = false;
            bool l_SoftCountered = false;

            if (l_MissionFollowers.size() > 0)
            {
                for (uint32 l_Y = 0; l_Y < l_MissionFollowers.size(); ++l_Y)
                {
                    if (G3D::fuzzyEq(l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID], -1.0))
                        continue;

                    for (uint32 l_Z = 0; l_Z < l_MissionFollowers[l_Y]->Abilities.size(); l_Z++)
                    {
                        uint32 l_CurrentAbilityID = l_MissionFollowers[l_Y]->Abilities[l_Z];

                        for (auto l_AbilityEffectEntry : sGarrAbilityEffectStore)
                        {
                            if (l_AbilityEffectEntry->AbilityID == l_CurrentAbilityID && l_AbilityEffectEntry->CounterMechanicTypeID == l_MechanicEntry->MechanicTypeID && !(l_AbilityEffectEntry->Flags & 1) && !l_AbilityEffectEntry->EffectType)
                            {
                                if (l_Chance != 0.0)
                                {
                                    float l_Seil;

                                    if (l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID] >= 0.0)
                                        l_Seil = (l_AbilityEffectEntry->CombatWeightMax - l_AbilityEffectEntry->CombatWeightBase) * l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID] + l_AbilityEffectEntry->CombatWeightBase;
                                    else
                                        l_Seil = (l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID] + 1.0) * l_AbilityEffectEntry->CombatWeightBase;

                                    l_Chance -= l_Seil;

                                    if (l_Chance < 0.0)
                                        l_Chance = 0.0;

                                    l_Countered = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

           for (auto l_Iter = l_AdditionalAbilityEffects.begin(); l_Iter != l_AdditionalAbilityEffects.end();)
            {
                auto l_AbilityEffectEntry = l_Iter->second;

                if (!G3D::fuzzyEq(l_FollowersBiasMap[l_Iter->first], -1.0))
                {
                    auto l_GarrMechanicType = sGarrMechanicTypeStore[l_MechanicEntry->MechanicTypeID];

                    if (l_AbilityEffectEntry->CounterMechanicTypeID == l_MechanicEntry->MechanicTypeID)
                    {
                        l_SoftCountered = true;

                        if (!(l_AbilityEffectEntry->Flags & 1) && l_AbilityEffectEntry->EffectType)
                        {
                            l_UnseiledChance += l_AbilityEffectEntry->ActionValueFlat;
                            l_Iter = l_AdditionalAbilityEffects.erase(l_Iter);
                            continue;
                        }
                    }
                }
                ++l_Iter;
            }

            if (l_Countered)
            {
                if (l_Chance < 0.0)
                    l_Chance = 0.0;

                l_Chance = (l_MechanicEntry->Factor - l_Chance) * l_SeilMultiplier;
                l_CurrentAdditionalWinChance += l_Chance;
            }

            if (!l_Countered && !l_SoftCountered && l_MechanicEntry->AbilityID)
            {
                for (auto l_GarrAbilityEffectEntry : sGarrAbilityEffectStore)
                {
                    if (l_GarrAbilityEffectEntry->AbilityID == l_MechanicEntry->AbilityID)
                    {
                        l_ProccedEffects.push_back(l_GarrAbilityEffectEntry);
                    }
                }

            }
        }
        #pragma endregion

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// UNTESTED
        #pragma region Race Ability Counter
        for (uint32 l_I = 0; l_I < l_EncoutersMechanics.size(); ++l_I)
        {
            GarrMechanicEntry const* l_MechanicEntry = l_EncoutersMechanics[l_I].second;
            uint32 l_MechanicId = l_MechanicEntry->ID;

            GarrMechanicTypeEntry const* l_MechanicTypeEntry = sGarrMechanicTypeStore.LookupEntry(l_MechanicEntry->MechanicTypeID);

            if (!l_MechanicTypeEntry)
                continue;

            if (l_MechanicTypeEntry->Type == MechanicTypes::Racial)
            {
                for (uint32 l_Y = 0; l_Y < l_MissionFollowers.size(); ++l_Y)
                {
                    for (uint32 l_Z = 0; l_Z < l_MissionFollowers[l_Y]->Abilities.size(); l_Z++)
                    {
                        uint32 l_CurrentAbilityID = l_MissionFollowers[l_Y]->Abilities[l_Z];

                        for (uint32 l_EffectI = 0; l_EffectI < sGarrAbilityEffectStore.GetNumRows(); l_EffectI++)
                        {
                            GarrAbilityEffectEntry const* l_AbilityEffectEntry = sGarrAbilityEffectStore.LookupEntry(l_EffectI);

                            if (!l_AbilityEffectEntry || l_AbilityEffectEntry->AbilityID != l_CurrentAbilityID)
                                continue;

                            if (l_AbilityEffectEntry->CounterMechanicTypeID == l_MissionTemplate->GarrMechanicTypeRecID)
                            {
                                double l_Seil = 0;

                                if (l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID] >= 0.0)
                                    l_Seil = (l_AbilityEffectEntry->CombatWeightMax - l_AbilityEffectEntry->CombatWeightBase) * l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID] + l_AbilityEffectEntry->CombatWeightBase;
                                else
                                    l_Seil = (l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID] + 1.0) * l_AbilityEffectEntry->CombatWeightBase;

                                l_CurrentAdditionalWinChance += l_Seil * l_SeilMultiplier;
                            }
                        }
                    }
                }
            }
        }
        #pragma endregion

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// OK 100%
        #pragma region Environment Ability
        for (uint32 l_Y = 0; l_Y < l_MissionFollowers.size(); ++l_Y)
        {

            if (G3D::fuzzyEq(l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID], -1.0))
                continue;

            for (uint32 l_Z = 0; l_Z < l_MissionFollowers[l_Y]->Abilities.size(); l_Z++)
            {
                uint32 l_CurrentAbilityID = l_MissionFollowers[l_Y]->Abilities[l_Z];

                for (uint32 l_EffectI = 0; l_EffectI < sGarrAbilityEffectStore.GetNumRows(); l_EffectI++)
                {
                    GarrAbilityEffectEntry const* l_AbilityEffectEntry = sGarrAbilityEffectStore.LookupEntry(l_EffectI);

                    if (!l_AbilityEffectEntry || l_AbilityEffectEntry->AbilityID != l_CurrentAbilityID)
                        continue;

                    if (l_AbilityEffectEntry->CounterMechanicTypeID == l_MissionTemplate->GarrMechanicTypeRecID)
                    {
                        double l_Seil;

                        if (l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID] >= 0.0)
                            l_Seil = (l_AbilityEffectEntry->CombatWeightMax - l_AbilityEffectEntry->CombatWeightBase) * l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID] + l_AbilityEffectEntry->CombatWeightBase;
                        else
                            l_Seil = (l_FollowersBiasMap[l_MissionFollowers[l_Y]->DatabaseID] + 1.0) * l_AbilityEffectEntry->CombatWeightBase;

                        l_CurrentAdditionalWinChance += l_Seil * l_SeilMultiplier;
                    }
                }
            }
        }
        #pragma endregion

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// OK 100% - Draenor
        #pragma region Follower Trait

        if (l_MissionTemplate->GarrisonType == GarrisonType::GarrisonDraenor)
        {
            double l_MissionDuration = p_Mission->Duration;
            double l_MissionTravelTime = p_Mission->TravelDuration;

            for (auto l_Follower : l_MissionFollowers)
            {
                for (auto l_CurrentAbilityID : l_Follower->Abilities)
                {
                    for (auto l_AbilityEffectEntry : sGarrAbilityEffectStore)
                    {
                        if (l_AbilityEffectEntry->AbilityID != l_CurrentAbilityID)
                            continue;

                        bool l_Proc = false;

                        switch (l_AbilityEffectEntry->EffectType)
                        {
                            /// Proc if MissionFollowerCount == 1
                            case AbilityEffectTypes::ModSoloMission:
                                l_Proc = l_MissionFollowers.size() == 1;
                                break;

                            /// Proc every time, no condition
                            case AbilityEffectTypes::ModsuccessChance:
                                l_Proc = true;
                                break;

                            /// Proc if Find(MissionFollowers[Class], MiscValueA) != NULL
                            case AbilityEffectTypes::ModFriendlyRace:
                                for (auto l_OtherFollower : l_MissionFollowers)
                                {
                                    if (l_OtherFollower->DatabaseID != l_Follower->DatabaseID)
                                    {
                                        auto l_FollowerTemplate = sGarrFollowerStore[l_OtherFollower->FollowerID];

                                        if (l_FollowerTemplate && l_FollowerTemplate->CreatureID[GetGarrisonFactionIndex()] == l_AbilityEffectEntry->MiscValueA)
                                        {
                                            l_Proc = true;
                                            break;
                                        }
                                    }
                                }
                                break;

                            /// Proc if Duration > (3600 * MiscValueB)
                            case AbilityEffectTypes::ModLongMission:
                                l_Proc = l_MissionDuration > (3600 * l_AbilityEffectEntry->MiscValueB);
                                break;

                            /// Proc if Duration < (3600 * MiscValueB)
                            case AbilityEffectTypes::ModShortMission:
                                l_Proc = l_MissionDuration < (3600 * l_AbilityEffectEntry->MiscValueB);
                                break;

                            /// Proc if TravelDuration > (3600 * MiscValueB)
                            case AbilityEffectTypes::ModLongTravel:
                                l_Proc = l_MissionTravelTime > (3600 * l_AbilityEffectEntry->MiscValueB);
                                break;

                            /// Proc if TravelDuration < (3600 * MiscValueB)
                            case AbilityEffectTypes::ModShortTravel:
                                l_Proc = l_MissionTravelTime < (3600 * l_AbilityEffectEntry->MiscValueB);
                                break;

                            case AbilityEffectTypes::ModCounterMechanic:
                            case AbilityEffectTypes::ModXp:
                            case AbilityEffectTypes::ModCurrency:
                            case AbilityEffectTypes::ModBias:
                            case AbilityEffectTypes::ModProfession:
                            case AbilityEffectTypes::ModBronzeTreasureDrop:
                            case AbilityEffectTypes::ModSilverTreasureDrop:
                            case AbilityEffectTypes::ModGoldTreasureDrop:
                            case AbilityEffectTypes::ModChestDropRate:
                            case AbilityEffectTypes::ModMissionTime:
                                break;

                            default:
                                break;

                        }

                        if (!l_Proc)
                            break;

                        double l_Seil = 0.0;

                        if (l_FollowersBiasMap[l_Follower->DatabaseID] >= 0.0)
                            l_Seil = (l_AbilityEffectEntry->CombatWeightMax - l_AbilityEffectEntry->CombatWeightBase) * l_FollowersBiasMap[l_Follower->DatabaseID] + l_AbilityEffectEntry->CombatWeightBase;
                        else
                            l_Seil = (l_FollowersBiasMap[l_Follower->DatabaseID] + 1.0) * l_AbilityEffectEntry->CombatWeightBase;

                        if ((l_Follower->Flags & MS::Garrison::FollowerFlags::isLimitedUse) && l_AbilityEffectEntry->EffectType == MS::Garrison::AbilityEffectTypes::ModsuccessChance)
                        {
                            l_CurrentAdditionalWinChance += l_AbilityEffectEntry->ActionValueFlat;
                        }
                        else
                        {
                            l_CurrentAdditionalWinChance += l_Seil * l_SeilMultiplier;
                        }

                        l_ProccedEffects.push_back(l_AbilityEffectEntry);
                    }
                }
            }
        }
        #pragma endregion

        if (l_MissionTemplate->GarrisonType == GarrisonType::GarrisonBrokenIsles)
        {
            std::vector<GarrAbilityEffectEntry const*> l_ProccableEffects;
            l_ProccableEffects.insert(l_ProccableEffects.end(), l_Talents.begin(), l_Talents.end());

            for (auto const& l_Pair : l_AdditionalAbilityEffects)
                l_ProccableEffects.push_back(l_Pair.second);

            for (auto l_GarrAbilityEffect : l_ProccableEffects)
            {
                bool l_Proc = false;
                double l_ProcAmount = 0.0;

                switch (l_GarrAbilityEffect->EffectType)
                {
                    case AbilityEffectTypes::ModsuccessChance:          ///< Tested
                        l_ProcAmount = l_GarrAbilityEffect->ActionValueFlat;
                        l_Proc = true;
                        break;
                    case AbilityEffectTypes::ModFriendlyClass:          ///< Tested
                        if (std::count_if(l_MissionFollowers.begin(), l_MissionFollowers.end(), [l_GarrAbilityEffect](GarrisonFollower* p_Follower) -> bool { return l_GarrAbilityEffect->GarrClassSpecID == p_Follower->GarrClassSpecID; }))
                            l_Proc = true;
                        l_ProcAmount = l_GarrAbilityEffect->ActionValueFlat;
                        break;
                    case AbilityEffectTypes::ModSuccessPerDurability:   ///<Tested
                    {
                        for (auto l_Follower : l_MissionFollowers)
                        {
                            if (l_Follower->Flags & FollowerFlags::isLimitedUse)
                                l_ProcAmount += l_Follower->Durability;
                        }

                        l_ProcAmount *= l_GarrAbilityEffect->ActionValueFlat;

                        if (l_ProcAmount)
                            l_Proc = true;
                        break;
                    }
                    case AbilityEffectTypes::ModFriendlyFollower:       ///< Tested
                    {
                        if (std::count_if(l_MissionFollowers.begin(), l_MissionFollowers.end(), [l_GarrAbilityEffect](GarrisonFollower* p_Follower) -> bool { return l_GarrAbilityEffect->GarrClassSpecID == p_Follower->FollowerID; }))
                            l_Proc = true;

                        l_ProcAmount = l_GarrAbilityEffect->ActionValueFlat;
                        break;
                    }
                    default:
                        break;
                }

                if (l_ProcAmount && l_Proc)
                {
                    l_UnseiledChance += l_ProcAmount;
                }
            }
        }
        //for  (auto )
        ///
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// UNTESTED
        #pragma region Passive Effect
        for (uint32 l_Y = 0; l_Y < l_PassiveEffects.size(); ++l_Y)
        {
            GarrAbilityEffectEntry const* l_AbilityEffectEntry = sGarrAbilityEffectStore.LookupEntry(l_PassiveEffects[l_Y]);

            if (!l_AbilityEffectEntry)
                continue;

            if (l_AbilityEffectEntry->EffectType == AbilityEffectTypes::ModsuccessChance)
            {
                l_CurrentAdditionalWinChance = (l_AbilityEffectEntry->CombatWeightBase * l_SeilMultiplier) + l_CurrentAdditionalWinChance;
            }
        }
        #pragma endregion

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        l_CurrentAdditionalWinChance = (((100.0 - l_MissionTemplate->BaseBonusChance) * l_CurrentAdditionalWinChance) * 0.0099999998) + l_MissionTemplate->BaseBonusChance + l_UnseiledChance;
        l_CurrentAdditionalWinChance = round(l_CurrentAdditionalWinChance);

        /// Process uncountered ability effect

        p_Mission->IsLethalToTroops = false;

        for (auto l_GarrAbilityEffectEntry : l_ProccedEffects)
        {
            switch (l_GarrAbilityEffectEntry->EffectType)
            {
                case AbilityEffectTypes::ModKillTroops:
                {
                    p_Mission->IsLethalToTroops = true;
                    break;
                }
                case AbilityEffectTypes::ModBonusChestChanceFlat:
                {
                    double l_BonusChance = l_CurrentAdditionalWinChance - 100.0;
                    if (l_BonusChance > 0.0)
                    {
                        l_BonusChance += l_GarrAbilityEffectEntry->ActionValueFlat / 100.0;

                        l_BonusChance = std::max(l_BonusChance, 0.0);
                        l_BonusChance = std::min(l_BonusChance, 100.0);

                        l_CurrentAdditionalWinChance = 100.0 + l_BonusChance;
                        break;
                    }
                }
                default:
                    break;
            }
        }

        if (l_MissionTemplate->GarrisonType == GarrisonType::GarrisonDraenor && l_CurrentAdditionalWinChance > 100.0)
            l_CurrentAdditionalWinChance = 100.0;

        /// Traved duration

        std::vector<uint32> l_TravelPassiveEffects;
        float               l_MissionTravelTime = l_MissionTemplate->TravelTime;

        if (l_MissionTemplate->GarrisonType == GarrisonType::GarrisonDraenor && m_GarrisonDraenor)
            l_TravelPassiveEffects = m_GarrisonDraenor->GetBuildingsPassiveAbilityEffects();

        p_Mission->TravelDuration = l_MissionTravelTime;
        ///

        /// Duration & Price
        float l_MissionDuration   = l_MissionTemplate->Duration;
        float l_Price   = l_MissionTemplate->CurrencyCost;

        std::vector<GarrAbilityEffectEntry const*> l_DurationProccableEffects;
        l_DurationProccableEffects.insert(l_DurationProccableEffects.end(), l_Talents.begin(), l_Talents.end());
        l_DurationProccableEffects.insert(l_DurationProccableEffects.end(), l_FollowerAbilities.begin(), l_FollowerAbilities.end());
        l_DurationProccableEffects.insert(l_DurationProccableEffects.end(), l_ProccedEffects.begin(), l_ProccedEffects.end());

        for (auto l_AbilityEffectEntry : l_DurationProccableEffects)
        {
            switch (l_AbilityEffectEntry->EffectType)
            {
                case AbilityEffectTypes::ModMissionTime:
                    l_MissionDuration = l_MissionDuration * l_AbilityEffectEntry->ActionValueFlat;
                    break;
                case AbilityEffectTypes::ModMissionCost:
                    l_Price *= l_AbilityEffectEntry->ActionValueFlat;
                    break;
                default:
                    break;
            }
        }

        p_Mission->Duration = l_MissionDuration;
        p_Mission->Price = l_Price;

        p_Mission->ChestChance = l_CurrentAdditionalWinChance;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get followers on a mission
    std::vector<GarrisonFollower*> Manager::GetMissionFollowers(uint32 p_MissionRecID)
    {
        std::vector<GarrisonFollower*> l_MissionFollowers;

        for (uint32 l_I = 0; l_I < m_Followers.size(); ++l_I)
        {
            if (m_Followers[l_I].CurrentMissionID == p_MissionRecID)
                l_MissionFollowers.push_back(&m_Followers[l_I]);
        }

        return l_MissionFollowers;
    }
    /// Get mission followers abilities effect
    std::vector<uint32> Manager::GetMissionFollowersAbilitiesEffects(uint32 p_MissionRecID)
    {
        std::vector<GarrisonFollower*> l_MissionFollowers = GetMissionFollowers(p_MissionRecID);
        std::vector<uint32>            l_AbilitiesEffects;

        for (uint32 l_FollowerIt = 0; l_FollowerIt < l_MissionFollowers.size(); ++l_FollowerIt)
        {
            for (uint32 l_AbilityIt = 0; l_AbilityIt < l_MissionFollowers[l_FollowerIt]->Abilities.size(); l_AbilityIt++)
            {
                uint32 l_CurrentAbilityID = l_MissionFollowers[l_FollowerIt]->Abilities[l_AbilityIt];

                for (uint32 l_EffectIt = 0; l_EffectIt < sGarrAbilityEffectStore.GetNumRows(); l_EffectIt++)
                {
                    GarrAbilityEffectEntry const* l_AbilityEffectEntry = sGarrAbilityEffectStore.LookupEntry(l_EffectIt);

                    if (!l_AbilityEffectEntry || l_AbilityEffectEntry->AbilityID != l_CurrentAbilityID)
                        continue;

                    l_AbilitiesEffects.push_back(l_AbilityEffectEntry->ID);
                }
            }
        }

        return l_AbilitiesEffects;
    }
    /// Get mission followers abilities effect
    std::vector<uint32> Manager::GetMissionFollowersAbilitiesEffects(uint32 p_MissionRecID, AbilityEffectTypes::Type p_Type, uint32 p_TargetMask)
    {
        std::vector<GarrisonFollower*> l_MissionFollowers = GetMissionFollowers(p_MissionRecID);
        std::vector<uint32>            l_AbilitiesEffects;

        for (uint32 l_FollowerIt = 0; l_FollowerIt < l_MissionFollowers.size(); ++l_FollowerIt)
        {
            for (uint32 l_AbilityIt = 0; l_AbilityIt < l_MissionFollowers[l_FollowerIt]->Abilities.size(); l_AbilityIt++)
            {
                uint32 l_CurrentAbilityID = l_MissionFollowers[l_FollowerIt]->Abilities[l_AbilityIt];

                for (uint32 l_EffectIt = 0; l_EffectIt < sGarrAbilityEffectStore.GetNumRows(); l_EffectIt++)
                {
                    GarrAbilityEffectEntry const* l_AbilityEffectEntry = sGarrAbilityEffectStore.LookupEntry(l_EffectIt);

                    if (!l_AbilityEffectEntry || l_AbilityEffectEntry->AbilityID != l_CurrentAbilityID)
                        continue;

                    if (l_AbilityEffectEntry->EffectType != p_Type || (l_AbilityEffectEntry->TargetMask != 0 && (l_AbilityEffectEntry->TargetMask & p_TargetMask) == 0))
                        continue;

                    l_AbilitiesEffects.push_back(l_AbilityEffectEntry->ID);
                }
            }
        }

        return l_AbilitiesEffects;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Cleanup missions
    void Manager::CleanupMissions()
    {
        /// Remove doubloon mission
        std::map<uint32, uint32> l_MissionToRemoveCount;

        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            GarrisonMission & l_Mission = m_Missions[l_I];

            uint32 l_Count = (uint32)std::count_if(m_Missions.begin(), m_Missions.end(), [l_Mission](const GarrisonMission& p_Mission)
            {
                return p_Mission.MissionID == l_Mission.MissionID;
            });

            l_MissionToRemoveCount[l_Mission.MissionID] = l_Count - 1;
        }

        std::vector<uint64> l_MissionToRemove;
        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            GarrisonMission& l_Mission = m_Missions[l_I];

            if (l_MissionToRemoveCount[l_Mission.MissionID] > 0)
            {
                l_MissionToRemove.push_back(l_Mission.DatabaseID);
                l_MissionToRemoveCount[l_Mission.MissionID]--;
            }
        }

        for (uint64 l_MissionBD_ID : l_MissionToRemove)
        {
            auto l_It = std::find_if(m_Missions.begin(), m_Missions.end(), [l_MissionBD_ID](const GarrisonMission& p_Mission)
            {
                return p_Mission.DatabaseID == l_MissionBD_ID;
            });

            if (l_It != m_Missions.end())
            {
                PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_MISSION);
                l_Stmt->setUInt32(0, l_MissionBD_ID);

                CharacterDatabase.AsyncQuery(l_Stmt);

                m_Missions.erase(l_It);
            }
        }

        for (uint32 l_I = 0; l_I < m_Missions.size(); ++l_I)
        {
            GarrisonMission& l_Mission = m_Missions[l_I];

            if (l_Mission.State != Mission::State::InProgress)
                continue;

            uint32 l_FollowerCount = (uint32)std::count_if(m_Followers.begin(), m_Followers.end(), [l_Mission](const GarrisonFollower & p_Follower) -> bool
            {
                if (p_Follower.CurrentMissionID == l_Mission.MissionID)
                    return true;

                return false;
            });

            if (l_FollowerCount == 0)
            {
                l_MissionToRemove.push_back(l_Mission.DatabaseID);
                continue;
            }

            GarrMissionEntry const* l_MissionTemplate = sGarrMissionStore.LookupEntry(l_Mission.MissionID);

/*
            if (!l_MissionTemplate || l_MissionTemplate->RequiredFollowersCount != l_FollowerCount)
                l_MissionToRemove.push_back(l_Mission.DatabaseID);*/
        }

        for (uint64 l_MissionBD_ID : l_MissionToRemove)
        {
            auto l_It = std::find_if(m_Missions.begin(), m_Missions.end(), [l_MissionBD_ID](const GarrisonMission & p_Mission)
            {
                return p_Mission.DatabaseID == l_MissionBD_ID;
            });

            if (l_It != m_Missions.end())
            {
                PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_MISSION);
                l_Stmt->setUInt32(0, l_MissionBD_ID);

                CharacterDatabase.AsyncQuery(l_Stmt);

                m_Missions.erase(l_It);
            }
        }

        l_MissionToRemove.clear();
    }
    /// Cleanup missions
    void Manager::CleanupOfferMissions()
    {
        auto l_Function = [this](GarrisonType::Type p_Type)
        {
            std::size_t l_FollowerCount = std::count_if(m_Followers.begin(), m_Followers.end(), [p_Type](const GarrisonFollower & p_Follower)
            {
                return p_Follower.GetEntry()->GarrisonType == p_Type;
            });

            uint32 l_MaxMissionCount            = ceil(l_FollowerCount * GARRISON_MISSION_DISTRIB_FOLLOWER_COEFF);
            uint32 l_CurrentAvailableMission    = 0;

            std::for_each(m_Missions.begin(), m_Missions.end(), [p_Type, &l_CurrentAvailableMission](const GarrisonMission& p_Mission) -> void
            {
                if (sGarrMissionStore.LookupEntry(p_Mission.MissionID)->GarrisonType == p_Type && p_Mission.State == Mission::State::Offered && (!p_Mission.OfferMaxDuration || (p_Mission.OfferTime + p_Mission.OfferMaxDuration) > time(nullptr)))
                    l_CurrentAvailableMission++;
            });

            if (l_CurrentAvailableMission > l_MaxMissionCount)
            {
                m_Missions.erase(std::remove_if(m_Missions.begin(), m_Missions.end(), [p_Type, l_CurrentAvailableMission, l_MaxMissionCount](const GarrisonMission& p_Mission) -> bool
                {
                    if (sGarrMissionStore.LookupEntry(p_Mission.MissionID)->GarrisonType == p_Type && p_Mission.State == Mission::State::Offered && (!p_Mission.OfferMaxDuration || (p_Mission.OfferTime + p_Mission.OfferMaxDuration) > time(nullptr)) && l_CurrentAvailableMission > l_MaxMissionCount)
                        return true;

                    return false;
                }), m_Missions.end());
            }
        };

        if (m_GarrisonDraenor)
            l_Function(GarrisonType::GarrisonDraenor);

        if (m_GarrisonBrokenIsles)
            l_Function(GarrisonType::GarrisonBrokenIsles);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    bool Manager::EvaluateMissionConditions(GarrMissionEntry const* p_Entry)
    {
        if (DisableMgr::IsDisabledFor(DISABLE_TYPE_GARRISON_MISSION, p_Entry->MissionRecID, m_Owner))
            return false;

        if (p_Entry->RequiredFollowersCount > m_Followers.size())
            return false;

        if (m_Owner->GetTeamId() == TEAM_HORDE && p_Entry->LocPrefixID == 106) ///< Presumed Alliance only
            return false;
        else if (m_Owner->GetTeamId() == TEAM_ALLIANCE && p_Entry->LocPrefixID == 101) ///< Presumed Horde only
            return false;

        if (p_Entry->Duration <= 10)
            return false;

        if (p_Entry->RequiredFollowersCount > Globals::MaxFollowerPerMission)
            return false;

        if (m_GarrisonDraenor && p_Entry->GarrisonType == GarrisonType::GarrisonDraenor)
        {
            std::map<GDraenor::Mission::Type, GDraenor::Building::Type>::const_iterator l_Iterator = GDraenor::Mission::g_MissionBuildingTypesMap.find(GDraenor::Mission::Type(p_Entry->MissionType));

            if (l_Iterator != GDraenor::Mission::g_MissionBuildingTypesMap.end())
            {
                if (!m_GarrisonDraenor->HasBuildingType(l_Iterator->second))
                    return false;
            }
        }

        if (p_Entry->SubCategory1 == 45 || p_Entry->SubCategory1 == 89) ///< WoD Legendaries, unavailable in Legion
            return false;

        if (m_GarrisonDraenor && p_Entry->GarrisonType == GarrisonType::GarrisonDraenor)
        {
            /// If player has Tavern lvl 3, he'll have 50% chance to get a treasure hunter type mission
            if (p_Entry->MissionType == GDraenor::Mission::Type::Treasure)
            {
                if (m_GarrisonDraenor->GetBuildingLevel(m_GarrisonDraenor->GetBuildingWithType(GDraenor::Building::Type::Inn)) != 3)
                    return false;
                else if (urand(0, 1))
                    return false;
            }
        }

        /// We are getting too many rare missions compared to retail
        if (p_Entry->Flags & MS::Garrison::Mission::Flags::IsRare)
        {
            if (urand(0, 100) <= 15)
                return false;
        }

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    std::vector<GarrAbilityEffectEntry const*> Manager::GetApplicableGarrAbilitiesEffectsByType(MS::Garrison::AbilityEffectTypes::Type p_Type, uint32 p_MissionRecID)
    {
        std::vector<GarrisonFollower*> l_MissionFollowers = GetMissionFollowers(p_MissionRecID);
        std::vector<GarrAbilityEffectEntry const*> l_Effects;

        for (uint32 l_EffectIt = 0; l_EffectIt < sGarrAbilityEffectStore.GetNumRows(); l_EffectIt++)
        {
            GarrAbilityEffectEntry const* l_AbilityEffectEntry = sGarrAbilityEffectStore.LookupEntry(l_EffectIt);

            if (!l_AbilityEffectEntry || l_AbilityEffectEntry->EffectType != p_Type)
                continue;

            if (m_PlayerGarrAbilities.find(l_AbilityEffectEntry->AbilityID) != m_PlayerGarrAbilities.end())
                l_Effects.push_back(l_AbilityEffectEntry);

            for (uint32 l_FollowerIt = 0; l_FollowerIt < l_MissionFollowers.size(); ++l_FollowerIt)
            {
                for (uint32 l_AbilityIt = 0; l_AbilityIt < l_MissionFollowers[l_FollowerIt]->Abilities.size(); l_AbilityIt++)
                {
                    uint32 l_CurrentAbilityID = l_MissionFollowers[l_FollowerIt]->Abilities[l_AbilityIt];
                    if (l_AbilityEffectEntry->AbilityID == l_CurrentAbilityID)
                        l_Effects.push_back(l_AbilityEffectEntry);
                }
            }
        }

        return l_Effects;
    }

    /// Update mission distribution
    void Manager::UpdateMissionDistribution(bool p_Force /* = false */)
    {
        /// Do ramdom mission distribution
        if (p_Force || ((time(nullptr) - m_MissionDistributionLastUpdate) > Globals::MissionDistributionInterval))
        {
            uint32_t l_DraenorNPCCap        = GetFollowerSoftCap(eGarrisonFollowerTypeID::FollowerTypeIDDraenor);
            uint32_t l_BrokenIslesNPCCap    = GetFollowerSoftCap(eGarrisonFollowerTypeID::FollowerTypeIDClassHall);

            auto l_Update = [this, l_DraenorNPCCap, l_BrokenIslesNPCCap](GarrisonType::Type p_Type)
            {
                auto l_AllFollowers = GetFollowers(p_Type);
                auto l_FollowerCount = l_AllFollowers.size();

                uint32 l_MinCap = std::min<uint32_t>(p_Type == GarrisonType::GarrisonDraenor ? l_DraenorNPCCap : l_BrokenIslesNPCCap, l_FollowerCount);

                /// Random, no detail about how blizzard do
                uint32 l_MaxMissionCount         = ceil(l_MinCap * GARRISON_MISSION_DISTRIB_FOLLOWER_COEFF);
                uint32 l_CurrentAvailableMission = 0;

                std::for_each(m_Missions.begin(), m_Missions.end(), [&l_CurrentAvailableMission, p_Type](const GarrisonMission& p_Mission) -> void
                {
                    if (sGarrMissionStore.LookupEntry(p_Mission.MissionID)->GarrisonType == p_Type && p_Mission.State == Mission::State::Offered && (!p_Mission.OfferMaxDuration || (p_Mission.OfferTime + p_Mission.OfferMaxDuration) > time(nullptr)))
                        l_CurrentAvailableMission++;
                });

                if (l_CurrentAvailableMission <= l_MaxMissionCount)
                {
                    uint32 l_MaxFollowerLevel     = 90;
                    uint32 l_MinFollowerLevel     = 100;
                    uint32 l_MaxFollowerItemLevel = 600;
                    bool l_HasCombatAllyFollower = false;

                    if (p_Type == GarrisonType::GarrisonBrokenIsles)
                    {
                        l_MaxFollowerLevel      = 100;
                        l_MinFollowerLevel      = 110;
                        l_MaxFollowerItemLevel  = 750;
                    }

                    std::for_each(m_Followers.begin(), m_Followers.end(), [&l_MaxFollowerLevel, &l_MaxFollowerItemLevel, &l_MinFollowerLevel, p_Type, &l_HasCombatAllyFollower](const GarrisonFollower& p_Follower) -> void
                    {
                        if (static_cast<GarrisonType::Type>(p_Follower.GetEntry()->GarrisonType) != p_Type)
                            return;

                        if (p_Follower.ZoneSupportSpellID)
                            l_HasCombatAllyFollower = true;

                        l_MinFollowerLevel      = std::min(l_MinFollowerLevel, (uint32)p_Follower.Level);
                        l_MaxFollowerLevel      = std::max(l_MaxFollowerLevel, (uint32)p_Follower.Level);
                        l_MaxFollowerItemLevel  = std::max(l_MaxFollowerItemLevel, (uint32)((p_Follower.ItemLevelArmor + p_Follower.ItemLevelWeapon) / 2));
                    });

                    std::vector<const GarrMissionEntry*> l_Candidates;

                    for (uint32 l_I = 0; l_I < sGarrMissionStore.GetNumRows(); ++l_I)
                    {
                        GarrMissionEntry const* l_Entry = sGarrMissionStore.LookupEntry(l_I);

                        if (!l_Entry)
                            continue;

                        if (strlen(l_Entry->Name->Get(g_DefaultLocale)) == 0)
                            continue;

                        if (l_Entry->GarrisonType != p_Type)
                            continue;

                        uint32 l_Count = (uint32)std::count_if(m_Missions.begin(), m_Missions.end(), [l_Entry](const GarrisonMission & p_Mission)
                        {
                            return p_Mission.MissionID == l_Entry->MissionRecID;
                        });

                        if (l_Count)
                            continue;

                        if (!l_Entry->OfferTime || l_Entry->RequiredLevel == 0 || l_Entry->RequiredLevel == 255)
                            continue;

                        /// Max Level cap : 2
                        if (l_Entry->RequiredLevel > (int32)(l_MaxFollowerLevel + 2))
                            continue;

                        if ((uint32)l_Entry->RequiredLevel == 110 && (uint32)l_Entry->RequiredItemLevel > l_MaxFollowerItemLevel)
                            continue;

                        /// We have less chances to get a low lvl mission if your followers' lowest lvl is higher
                        if (l_Entry->RequiredLevel < l_MinFollowerLevel)
                        {
                            if (urand(0, 100) <= 25)
                                continue;
                        }

                        auto l_Reward = sObjectMgr->GetMissionRewards(l_Entry->MissionRecID);

                        if (!l_Reward || l_Reward->empty())
                            continue;

                        if (m_Owner->getLevel() >= GetMaxLevelForExpansion(sGarrTypeStore.LookupEntry(l_Entry->GarrisonType)->ExpansionRequired))
                        {
                            enum
                            {
                                XPItemIDA = 120205,
                                XPItemIDB = 140584
                            };

                            size_t l_HasXPReward = std::count_if(l_Reward->begin(), l_Reward->end(), [](const ::GarrisonMissionReward& p_Rew)
                            {
                                return p_Rew.ItemID == XPItemIDA || p_Rew.ItemID == XPItemIDB;
                            });

                            if (l_HasXPReward)
                                continue;
                        }

                        if (EvaluateMissionConditions(l_Entry))
                            l_Candidates.push_back(l_Entry);
                    }

                    uint32 l_ShuffleCount = std::rand() % 4;

                    for (uint32 l_I = 0; l_I < l_ShuffleCount; ++l_I)
                        std::random_shuffle(l_Candidates.begin(), l_Candidates.end());

                    int32 l_MissionToAddCount = (int32)l_MaxMissionCount - (int32)l_CurrentAvailableMission;

                    if (l_MissionToAddCount > 0)
                    {
                        l_MissionToAddCount = std::min(l_MissionToAddCount, (int32)l_Candidates.size());

                        for (int32 l_I = 0; l_I < l_MissionToAddCount; ++l_I)
                            AddMission(l_Candidates[l_I]->MissionRecID);
                    }

                    if (l_HasCombatAllyFollower)
                    {
                        std::vector<GarrisonMission>::iterator l_Itr = std::find_if(m_Missions.begin(), m_Missions.end(), [](GarrisonMission const& p_Mission)
                        {
                            return (p_Mission.MissionID == MS::Garrison::Mission::SupportMission::CombatAllyMission && p_Mission.State < MS::Garrison::Mission::State::Completed);
                        });

                        if (l_Itr == m_Missions.end())
                            AddMission(MS::Garrison::Mission::SupportMission::CombatAllyMission);
                    }
                }
            };

            if (m_GarrisonDraenor)
                l_Update(GarrisonType::GarrisonDraenor);
            if (m_GarrisonBrokenIsles)
                l_Update(GarrisonType::GarrisonBrokenIsles);

            m_MissionDistributionLastUpdate = time(0);
        }
    }

}   ///< namespace Garrison
}   ///< namespace MS

