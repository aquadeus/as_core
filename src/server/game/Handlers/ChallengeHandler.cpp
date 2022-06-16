////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "Player.h"
#include "World.h"
#include "WorldSession.h"
#include "ChallengeMgr.h"
#include "ChallengeModePackets.h"
#include "InstanceScript.h"
#include "ScenarioMgr.h"
#include "Chat.h"
#include "Group.h"
#include "LFGMgr.h"

void WorldSession::HandleRequestLeaders(WorldPacket& p_Packet)
{
    WorldPackets::ChallengeMode::RequestLeaders l_Packet(p_Packet);
    l_Packet.Read();

    WorldPackets::ChallengeMode::RequestLeadersResult l_Result;

    l_Result.MapID = l_Packet.MapId;
    l_Result.ChallengeID = l_Packet.ChallengeID;
    l_Result.LastGuildUpdate = time(nullptr);
    l_Result.LastRealmUpdate = time(nullptr);

    if (auto l_BestGuild = sChallengeMgr->BestGuildChallenge(m_Player->GetGuildId(), l_Packet.ChallengeID))
    {
        for (auto l_Itr = l_BestGuild->member.begin(); l_Itr != l_BestGuild->member.end(); ++l_Itr)
        {
            WorldPackets::ChallengeMode::ModeAttempt l_GuildLeaders;
            l_GuildLeaders.InstanceRealmAddress = g_RealmID;
            l_GuildLeaders.AttemptID            = l_BestGuild->ID;
            l_GuildLeaders.CompletionTime       = l_BestGuild->RecordTime;
            l_GuildLeaders.CompletionDate       = l_BestGuild->Date;
            l_GuildLeaders.MedalEarned          = l_BestGuild->ChallengeLevel;

            for (ChallengeMemberList::iterator::value_type const& l_Member : l_BestGuild->member)
            {
                WorldPackets::ChallengeMode::ModeAttempt::Member memberData;
                memberData.VirtualRealmAddress = g_RealmID;
                memberData.NativeRealmAddress  = g_RealmID;
                memberData.Guid                = l_Member.Guid;
                memberData.SpecializationID    = l_Member.SpecId;
                l_GuildLeaders.Members.emplace_back(std::move(memberData));
            }

            l_Result.GuildLeaders.emplace_back(std::move(l_GuildLeaders));
        }
    }

    if (ChallengeData* l_BestServer = sChallengeMgr->BestServerChallenge(l_Packet.ChallengeID))
    {
        WorldPackets::ChallengeMode::ModeAttempt l_RealmLeaders;
        l_RealmLeaders.InstanceRealmAddress = g_RealmID;
        l_RealmLeaders.AttemptID            = l_BestServer->ID;
        l_RealmLeaders.CompletionTime       = l_BestServer->RecordTime;
        l_RealmLeaders.CompletionDate       = l_BestServer->Date;
        l_RealmLeaders.MedalEarned          = l_BestServer->ChallengeLevel;


        for (ChallengeMemberList::iterator::value_type const& l_Itr : l_BestServer->member)
        {
            WorldPackets::ChallengeMode::ModeAttempt::Member l_MemberData;
            l_MemberData.VirtualRealmAddress = g_RealmID;
            l_MemberData.NativeRealmAddress  = g_RealmID;
            l_MemberData.Guid                = l_Itr.Guid;
            l_MemberData.SpecializationID    = l_Itr.SpecId;
            l_RealmLeaders.Members.emplace_back(l_MemberData);
        }

        l_Result.RealmLeaders.push_back(l_RealmLeaders);
    }

    SendPacket(l_Result.Write());
}

void WorldSession::HandleGetChallengeModeRewards(WorldPacket& p_Packet)
{
}

void WorldSession::HandleChallengeModeRequestMapStats(WorldPacket& p_Packet)
{
    WorldPackets::ChallengeMode::AllMapStats l_Stats;

    if (ChallengeByMap* l_Best = sChallengeMgr->BestForMember(m_Player->GetGUID()))
    {
        for (auto const& l_Challenge : *l_Best)
        {
            WorldPackets::ChallengeMode::ChallengeModeMap l_ModeMap;
            l_ModeMap.BestMedalDate              = l_Challenge.second->Date;
            l_ModeMap.MapId                      = l_Challenge.second->MapID;
            l_ModeMap.CompletedChallengeLevel    = l_Challenge.second->ChallengeLevel;
            l_ModeMap.BestCompletionMilliseconds = l_Challenge.second->RecordTime * IN_MILLISECONDS;
            l_ModeMap.ChallengeID                = l_Challenge.second->ChallengeID;

            if (ChallengeData* l_LastData = sChallengeMgr->LastForMemberMap(m_Player->GetGUID(), l_Challenge.second->ChallengeID))
                l_ModeMap.LastCompletionMilliseconds = l_LastData->RecordTime * IN_MILLISECONDS;
            else
                l_ModeMap.LastCompletionMilliseconds = l_Challenge.second->RecordTime * IN_MILLISECONDS;

            l_ModeMap.Affixes = l_Challenge.second->Affixes;

            for (auto const& l_Member : l_Challenge.second->member)
                l_ModeMap.BestSpecID.push_back(l_Member.SpecId);

            l_Stats.ChallengeModeMaps.push_back(l_ModeMap);
        }
    }

    SendPacket(l_Stats.Write());
}

void WorldSession::HandleStartChallengeMode(WorldPacket& p_Packet)
{
    WorldPackets::ChallengeMode::StartChallengeMode l_Packet(p_Packet);
    l_Packet.Read();

    if (GUID_ENPART(l_Packet.GameObjectGUID) != ChallengeModeOrb || !sWorld->getBoolConfig(CONFIG_CHALLENGE_ENABLED))
        return;

    InstanceMap* l_Instance = m_Player->GetMap()->ToInstanceMap();

    if (auto l_Item = m_Player->GetItemByEntry(138019))
    {
        auto l_ChallengeOrb = sObjectAccessor->FindGameObject(l_Packet.GameObjectGUID);
        if (!l_ChallengeOrb)
            return;

        auto l_Instance = m_Player->GetMap()->ToInstanceMap();
        float l_PositionX, l_PositionY, l_PositionZ, l_Orientation = 0.f;
        auto l_MapChallengeID = l_Item->GetModifier(ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID);

        if (l_ChallengeOrb->GetGoState() == GOState::GO_STATE_ACTIVE)
        {
            ChatHandler(m_Player).PSendSysMessage("Error: A key has already been activated.");
            return;
        }

        if (!l_Instance)
        {
            ChatHandler(m_Player).PSendSysMessage("Error: Is not a Instance Map.");
            return;
        }

        if (l_Instance->HasAnyCreatureRespawnTime())
        {
            ChatHandler(m_Player).PSendSysMessage("Error: Some creatures have been already killed.");
            return;
        }

        if (!sChallengeMgr->GetStartPosition(l_MapChallengeID, l_PositionX, l_PositionY, l_PositionZ, l_Orientation))
        {
            ChatHandler(m_Player).PSendSysMessage("Error: Start position not found.");
            return;
        }

        auto l_Progress = sScenarioMgr->GetScenario(l_Instance->GetScenarioGuid());
        if (!l_Progress)
        {
            ChatHandler(m_Player).PSendSysMessage("Error: Scenario not found on this map.");
            return;
        }

        auto IsPlayerReady = [&](Player* player)
        {
            if (!player || !player->isAlive())
            {
                ChatHandler(m_Player).PSendSysMessage("Error: Player not found or die.");
                return false;
            }

            if (player->isInCombat())
            {
                ChatHandler(m_Player).PSendSysMessage("Error: Player in combat.");
                return false;
            }

            if (!player->GetMap() || player->GetMap()->ToInstanceMap() != l_Instance)
            {
                ChatHandler(m_Player).PSendSysMessage("Error: Player in group not this map.");
                return false;
            }

            return true;
        };

        auto TeleportToChallenge = [=](Player* player, float x, float y, float z, float o)
        {
            player->SetDungeonDifficultyID(Difficulty::DifficultyMythicKeystone);
            uint32 mapId = player->GetMapId();
            player->TeleportTo(player->GetMapId(), x, y, z, o, 0, false);
            player->CastSpell(player, ChallengeSpells::ChallengersBurden, true);
            player->AddDelayedEvent([_player = player, mapId]()-> void
                {
                    WorldPackets::ChallengeMode::ChangePlayerDifficultyResult l_Result2;
                    l_Result2.Result = 11;
                    l_Result2.InstanceMapID = mapId;
                    l_Result2.DifficultyRecID = Difficulty::DifficultyMythicKeystone;
                    _player->SendDirectMessage(l_Result2.Write());
                }, 500);
            player->UpdateObjectVisibility(true);
        };

        auto group = m_Player->GetGroup();
        if (group)
        {
            if (group->GetMembersCount() > 5)
            {
                ChatHandler(m_Player).PSendSysMessage("Error: Member count in group > 5.");
                return;
            }

            group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Itr)
                {
                    auto player = l_Itr->player;
                    if (!IsPlayerReady(player))
                        return;
                });

            WorldPackets::ChallengeMode::ChangePlayerDifficultyResult l_Result;
            l_Result.CooldownReason = 2813862382;
            l_Result.Result = 5;
            group->BroadcastPacket(const_cast<WorldPacket*>(l_Result.Write()), true);

            group->m_ChallengeOwner = m_Player->GetGUID();
            group->m_ChallengeItem = l_Item->GetGUID();
        }
        else
        {
            if (!IsPlayerReady(m_Player))
                return;
            WorldPackets::ChallengeMode::ChangePlayerDifficultyResult l_Result;
            l_Result.CooldownReason = 2813862382;
            l_Result.Result = 5;
            SendPacket(const_cast<WorldPacket*>(l_Result.Write()), true);
        }


        l_Instance->SetSpawnMode(Difficulty::DifficultyMythicKeystone);

        ///< Hackfixes for Return to Karazhan
        if (!l_Progress->IsViable())
        {
            
            LFGDungeonEntry const* l_LFGDungeonEntry = nullptr;
            if (l_MapChallengeID == 234)
                l_LFGDungeonEntry = sLFGDungeonStore.LookupEntry(1475);

            if (!l_LFGDungeonEntry)
                l_LFGDungeonEntry = sLFGMgr->GetLFGDungeon(l_Instance->GetId(), l_Instance->GetDifficultyID(), m_Player->GetTeam());

            if (l_LFGDungeonEntry)
               {
                sScenarioMgr->RemoveScenario(l_Instance->GetScenarioGuid());
                l_Progress = sScenarioMgr->AddScenario(l_Instance, l_LFGDungeonEntry, m_Player);

                if (l_Progress)
                    l_Instance->SetScenarioGuid(l_Progress->GetScenarioGuid());
            }
        }

        auto challengeEntry = sMapChallengeModeStore.LookupEntry(l_MapChallengeID);
        if (group)
            group->m_challengeEntry = challengeEntry;
        else
            m_Player->m_challengeEntry = challengeEntry;

        l_Progress->CreateChallenge(m_Player, l_MapChallengeID);

        if (auto l_ChallengeOrb = sObjectAccessor->FindGameObject(l_Packet.GameObjectGUID))
        {
            l_ChallengeOrb->SetGoState(GOState::GO_STATE_ACTIVE);
            l_ChallengeOrb->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_NODESPAWN);
        }

        if (group)
        {
            group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Itr)
                {
                    if (auto plr = l_Itr->player)
                        TeleportToChallenge(plr, l_PositionX, l_PositionY, l_PositionZ, l_Orientation);
                });
        }
        else
            TeleportToChallenge(m_Player, l_PositionX, l_PositionY, l_PositionZ, l_Orientation);
    }
}

void WorldSession::HandleResetChallengeMode(WorldPacket& p_Packet)
{
    WorldPackets::ChallengeMode::ResetChallengeMode l_Packet(p_Packet);
    l_Packet.Read();

    if (auto const& l_InstanceScript = m_Player->GetInstanceScript())
    {
        if (l_InstanceScript->instance->IsChallengeMode())
            l_InstanceScript->ResetChallengeMode();
    }
}
