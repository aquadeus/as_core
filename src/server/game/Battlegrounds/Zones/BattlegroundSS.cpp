////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "gamePCH.h"
#include "Battleground.h"
#include "BattlegroundSS.h"
#include "Creature.h"
#include "GameObject.h"
#include "Language.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "BattlegroundMgr.hpp"
#include "Player.h"
#include "SpellScript.h"
#include "World.h"
#include "WorldPacket.h"

BattlegroundSS::BattlegroundSS() : Battleground()
{
    BgObjects.resize(eShoreGameObjectTypes::GameObjectTypeMax);
    BgCreatures.resize(eShoreCreatureTypes::CreatureTypeMax);

    StartMessageIds[BG_STARTING_EVENT_FIRST] = LANG_BG_SS_START_TWO_MINUTES;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_SS_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD] = LANG_BG_SS_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_SS_HAS_BEGUN;

    m_GunshipHordeGUID      = 0;
    m_GunshipAllianceGUID   = 0;
}

BattlegroundSS::~BattlegroundSS() {}

void BattlegroundSS::Reset()
{
    Battleground::Reset();

    m_TeamScores[TEAM_ALLIANCE] = 0;
    m_TeamScores[TEAM_HORDE] = 0;
    m_LastAzeriteTimer = 3000;
    m_FissureCheckTimer = 30000;
    m_FallCheckTimer = 500;
    m_FallDownCheckTimer = 1000;
    m_FirstThirtySeconds = true;
    m_ObjectsAttached = false;
    m_Locked = false;
    m_FirstTime = false;
}

void BattlegroundSS::StartBattleground()
{
    Battleground::StartBattleground();
}

bool BattlegroundSS::SetupBattleground()
{
    for (BattlegroundPlayerMap::const_iterator l_itr = GetPlayers().begin(); l_itr != GetPlayers().end(); ++l_itr)
    {
        if (Player* l_Player = ObjectAccessor::FindPlayer(l_itr->first))
            GetBgMap()->SendInitTransports(l_Player);
    }

    Creature* l_Mathias     = AddCreature(eShoreCreatures::MasterMathias, eShoreCreatureTypes::MasterMathiasType, TeamId::TEAM_ALLIANCE, 0.0f, 0.0f, 0.0f, 0.0f);
    Creature* l_Nathanos    = AddCreature(eShoreCreatures::Nathanos, eShoreCreatureTypes::NathanosType, TeamId::TEAM_HORDE, 0.0f, 0.0f, 0.0f, 0.0f);
    if (!l_Mathias || !l_Nathanos)
        return false;

    return true;
}

// virtual void StartingEventOpenDoors() override;
void BattlegroundSS::StartingEventOpenDoors()
{
    GameObject* l_GunshipAlliance = sObjectAccessor->FindGameObject(m_GunshipAllianceGUID);
    GameObject* l_GunshipHorde = sObjectAccessor->FindGameObject(m_GunshipHordeGUID);
    if (!l_GunshipAlliance || !l_GunshipHorde)
        return;

    std::list<GameObject*> l_HordeGameObjectList;
    std::list<GameObject*> l_AllianceGameObjectList;

    l_GunshipAlliance->GetGameObjectListWithEntryInGrid(l_AllianceGameObjectList, eShoreGameObjects::AllianceCollisionWall, 100.0f);
    l_GunshipHorde->GetGameObjectListWithEntryInGrid(l_HordeGameObjectList, eShoreGameObjects::HordeCollisionWall, 100.0f);

    for (GameObject* l_GameObject : l_AllianceGameObjectList)
        l_GameObject->SetGoState(GO_STATE_ACTIVE);

    for (GameObject* l_GameObject : l_HordeGameObjectList)
        l_GameObject->SetGoState(GO_STATE_ACTIVE);

    SpawnAzeriteFissures(2);

    for (auto iter : GetPlayers())
        if (auto player = ObjectAccessor::FindPlayer(iter.first))
            player->CastSpell(player, eShoreSpells::RocketParachute2);
}

void BattlegroundSS::SetOnGunship(Player* p_Player)
{
    Transport* l_GunshipHorde       = NULL;
    Transport* l_GunshipAlliance    = NULL;

    if (m_GunshipAllianceGUID == 0 || m_GunshipHordeGUID == 0)
    {
        l_GunshipHorde      = sTransportMgr->CreateTransport(eShoreGameObjects::HordeGunship, 0, GetBgMap());
        l_GunshipAlliance   = sTransportMgr->CreateTransport(eShoreGameObjects::AllianceGunship, 0, GetBgMap());

        m_GunshipAllianceGUID   = l_GunshipAlliance->GetGUID();
        m_GunshipHordeGUID      = l_GunshipHorde->GetGUID();
    }
    else
    {
        l_GunshipAlliance   = sObjectAccessor->FindGameObject(m_GunshipAllianceGUID)->ToTransport();
        l_GunshipHorde      = sObjectAccessor->FindGameObject(m_GunshipHordeGUID)->ToTransport();
    }

    if (!l_GunshipHorde && !l_GunshipAlliance)
        return;

    Transport* l_Transport = nullptr;
    switch (p_Player->GetTeamId())
    {
        case TeamId::TEAM_ALLIANCE:
            l_Transport = l_GunshipAlliance;
            break;
        case TeamId::TEAM_HORDE:
            l_Transport = l_GunshipHorde;
            break;
        default:
            break;
    }

    Position l_Position;
    uint64 l_TransportGUID = l_Transport->GetGUID();

    l_Position.m_positionZ = l_Transport->GetEntry() == eShoreGameObjects::AllianceGunship ? 40.0f : 50.0f;
    l_Transport->CalculatePassengerPosition(l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ, l_Position.m_orientation);
    p_Player->TeleportTo(l_Transport->GetMapId(), l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ, l_Position.m_orientation);

    p_Player->AddDelayedEvent([p_Player, l_TransportGUID]() -> void
    {
        GameObject* l_Gob = sObjectAccessor->FindGameObject(l_TransportGUID);
        if (!l_Gob || !l_Gob->IsTransport())
            return;

        Transport* l_Transport = l_Gob->ToTransport();
        if (!l_Transport)
            return;

        p_Player->SetTransport(l_Transport);
        l_Transport->AddPassenger(p_Player);
    }, 1);
}

void BattlegroundSS::PostUpdateImpl(uint32 p_Diff)
{
    if (GetStatus() == STATUS_WAIT_JOIN)
    {
        m_FallDownCheckTimer -= p_Diff;
        if (m_FallDownCheckTimer <= 0)
        {
            for (BattlegroundPlayerMap::const_iterator l_Itr = GetPlayers().begin(); l_Itr != GetPlayers().end(); l_Itr++)
            {
                Player* l_Player = ObjectAccessor::FindPlayer(l_Itr->first);
                if (!l_Player)
                    continue;

                if (l_Player->GetPositionZ() <= 288.0f)
                    SetOnGunship(l_Player);
            }
            m_FallDownCheckTimer = 1000;
        }
    }
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        if (GetElapsedTime() >= 30 * IN_MILLISECONDS && m_FirstThirtySeconds)
            m_FirstThirtySeconds = false;

        if (GetElapsedTime() >= 25 * MINUTE * IN_MILLISECONDS)
        {
            if (GetTeamScore(ALLIANCE) == 0)
            {
                if (GetTeamScore(HORDE) == 0)
                    EndBattleground(0);
                else
                    EndBattleground(HORDE);
            }

            else if (GetTeamScore(HORDE) == 0)
                EndBattleground(ALLIANCE);

            else if (GetTeamScore(HORDE) > GetTeamScore(ALLIANCE))
                EndBattleground(HORDE);
            else
                EndBattleground(ALLIANCE);
        }

        m_FallCheckTimer -= p_Diff;
        if (m_FallCheckTimer <= 0)
        {
            for (BattlegroundPlayerMap::const_iterator l_Itr = GetPlayers().begin(); l_Itr != GetPlayers().end(); l_Itr++)
            {
                Player* l_Player = ObjectAccessor::FindPlayer(l_Itr->first);
                if (!l_Player)
                    continue;

                if (!l_Player->IsFalling())
                {
                    if (l_Player->HasAura(eShoreSpells::Parachute))
                    {
                        l_Player->RemoveAura(eShoreSpells::Parachute);

                        uint64 l_PlayerGUID = l_Player->GetGUID();
                        l_Player->m_SpellHelper.GetBool(eSpellHelpers::SeethingShoreFirstThirtySeconds) = true;

                        l_Player->AddDelayedEvent([l_Player]() -> void
                        {
                            l_Player->m_SpellHelper.GetBool(eSpellHelpers::SeethingShoreFirstThirtySeconds) = false;
                        }, 30000);
                    }
                    continue;
                }
                /*
                if (!l_Player->HasAura(eShoreSpells::RocketParachute) && !l_Player->HasAura(eShoreSpells::Parachute) && m_FallingPlayers.find(l_Itr->first) == m_FallingPlayers.end() && l_Player->GetPositionZ() <= 288.0f  && l_Player->GetPositionZ() > 75.0f)
                {
                    l_Player->AddAura(eShoreSpells::RocketParachute, l_Player);
                    l_Player->SendCanTurnWhileFalling(false);
                    m_FallingPlayers.insert(l_Itr->first);

                    continue;
                }

                if (m_FallingPlayers.find(l_Itr->first) != m_FallingPlayers.end())
                {
                    if (l_Player->GetPositionZ() <= 73.0f && l_Player->HasAura(eShoreSpells::RocketParachute))
                    {
                        l_Player->RemoveAura(eShoreSpells::RocketParachute);
                        l_Player->AddAura(eShoreSpells::Parachute, l_Player);
                        m_FallingPlayers.erase(l_Itr->first);
                    }
                }*/
            }
            m_FallCheckTimer = 500;
        }

        m_FissureCheckTimer -= p_Diff;
        if (m_FissureCheckTimer <= 0)
        {
            if (IsFissureSpawned())
                ConvertFissures();

            if (GetActiveNodesAndFissures() < 2)
                SpawnAzeriteFissures(2);

            m_FissureCheckTimer = 15000;
        }
    }
}

uint8 BattlegroundSS::GetActiveNodesAndFissures()
{
    uint8 l_Amount = 0;
    for (uint8 l_Itr = 0; l_Itr < g_AzeriteFissureSpawns.size(); l_Itr++)
    {
        GameObject* l_GameObject = GetBGObject(l_Itr);
        if (l_GameObject == nullptr)
            continue;

        if (l_GameObject->GetEntry() == eShoreGameObjects::AzeriteNode || l_GameObject->GetEntry() == eShoreGameObjects::RichAzeriteNode)
            l_Amount++;
    }

    for (uint8 l_Itr = 0; l_Itr < g_AzeriteFissureSpawns.size(); l_Itr++)
    {
        Creature* l_Fissure = GetBGCreature(l_Itr);
        if (l_Fissure == nullptr)
            continue;

        if (l_Fissure->GetEntry() == eShoreCreatures::AzeriteFissure)
            l_Amount++;
    }

    return l_Amount;
}

void BattlegroundSS::HandleAchievementProgress(Player* p_Player, uint32 p_Points)
{
    p_Player->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, 744, p_Points);
    if (m_FirstThirtySeconds)
    {
        AchievementEntry const* l_Entry = sAchievementStore.LookupEntry(eShoreAchievements::ClaimJumper);
        p_Player->CompletedAchievement(l_Entry);
    }
}

void BattlegroundSS::UpdateTeamScore(Player* p_Player, uint32 p_Points)
{
    Creature* l_Mathias     = GetBGCreature(eShoreCreatureTypes::MasterMathiasType);
    Creature* l_Nathanos    = GetBGCreature(eShoreCreatureTypes::NathanosType);
    if (!l_Mathias || !l_Nathanos)
        return;

    if (m_TeamScores[0] == 0 && m_TeamScores[1] == 0)
    {
        if (p_Player->GetTeam() == HORDE)
            sCreatureTextMgr->SendChat(l_Nathanos, 1, 0, ChatMsg::CHAT_MSG_MONSTER_SAY, Language::LANG_UNIVERSAL, TextRange::TEXT_RANGE_MAP, 0, (Team)Team::HORDE);
        else
            sCreatureTextMgr->SendChat(l_Mathias, 10, 0, ChatMsg::CHAT_MSG_MONSTER_SAY, Language::LANG_UNIVERSAL, TextRange::TEXT_RANGE_MAP, 0, (Team)Team::ALLIANCE);
    }

    BattlegroundTeamId l_TeamID = GetTeamIndexByTeamId(p_Player->GetTeam());
    m_TeamScores[l_TeamID] += p_Points;

    if (m_TeamScores[l_TeamID] >= eShoreBattlegroundScores::MaxTeamScore)
    {
        m_TeamScores[l_TeamID] = eShoreBattlegroundScores::MaxTeamScore;
        EndBattleground(p_Player->GetTeam());
    }

    HandleAchievementProgress(p_Player, p_Points);

    if (p_Player->GetTeam() == Team::ALLIANCE)
        UpdateWorldState(eShoreWorldStates::AzeriteDisplayAlliance, m_TeamScores[l_TeamID]);
    else
        UpdateWorldState(eShoreWorldStates::AzeriteDisplayHorde, m_TeamScores[l_TeamID]);
}

void BattlegroundSS::UpdatePlayerScore(Player* p_Source, Player* p_Victim, uint32 p_Type, uint32 p_Value, bool p_DoAddHonor, MS::Battlegrounds::RewardCurrencyType::Type p_RewardType)
{
    std::map<uint64, BattlegroundScore*>::iterator l_Itr = PlayerScores.find(p_Source->GetGUID());
    if (l_Itr == PlayerScores.end())
        return;

    if (p_Type == ScoreType::SCORE_AZERITE_MINED)
        ((BattlegroundSSScore*)l_Itr->second)->MinedAzerite += p_Value;
    else
        Battleground::UpdatePlayerScore(p_Source, p_Victim, p_Type, p_Value, p_DoAddHonor, p_RewardType);
}

void BattlegroundSS::EventPlayerClickedOnFlag(Player* p_Player, GameObject* p_Gameobject)
{
    if (!p_Player || !p_Gameobject)
    {
        sLog->outAshran("Error in BattlegroundSS::EventPlayerClickedOnFlag : p_Player || p_Gameobject == nullptr. Abort rewarding azerite!");
        return;
    }

    for (uint8 l_Itr = 0; l_Itr < g_AzeriteFissureSpawns.size(); l_Itr++)
    {
        GameObject* l_GameObject = GetBGObject(l_Itr);
        if (l_GameObject == nullptr)
            continue;

        if (l_GameObject != p_Gameobject)
            continue;

        uint8 l_Value = p_Gameobject->GetEntry() == eShoreGameObjects::AzeriteNode ? 100 : 200;

        UpdatePlayerScore(p_Player, nullptr, ScoreType::SCORE_AZERITE_MINED, l_Value);
        UpdateTeamScore(p_Player, l_Value);
        DelObject(l_Itr);
        break;
    }

    p_Player->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_BE_SPELL_TARGET, eShoreSpells::AchievementCredit);

    if (p_Player->m_SpellHelper.GetBool(eSpellHelpers::SeethingShoreFirstThirtySeconds))
    {
        AchievementEntry const* l_Entry = sAchievementStore.LookupEntry(eShoreAchievements::ClaimJumper);
        if (!l_Entry)
            return;

        if (!p_Player->HasAchieved(l_Entry->ID))
            p_Player->CompletedAchievement(l_Entry);
    }

    UpdateVignettes();
}

void BattlegroundSS::HandleKillPlayer(Player* p_Killed, Player* p_Killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!p_Killer)
        return;

    Battleground::HandleKillPlayer(p_Killed, p_Killer);

    if (p_Killer->m_SpellHelper.GetBool(eSpellHelpers::SeethingShoreFirstThirtySeconds))
    {
        AchievementEntry const* l_Entry = sAchievementStore.LookupEntry(eShoreAchievements::DeathFromAbove);
        if (!l_Entry)
            return;

        if (!p_Killer->HasAchieved(eShoreAchievements::DeathFromAbove))
            p_Killer->CompletedAchievement(l_Entry);
    }

    std::list<GameObject*> m_GameObjectList;
    p_Killer->GetGameObjectListWithEntryInGrid(m_GameObjectList, eShoreGameObjects::RichAzeriteNode, 13.0f);
    p_Killer->GetGameObjectListWithEntryInGrid(m_GameObjectList, eShoreGameObjects::AzeriteNode, 13.0f);

    if (m_GameObjectList.size() > 0)
        p_Killer->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_HONORABLE_KILL_AT_AREA, p_Killer->GetAreaId());
}

void BattlegroundSS::EndBattleground(uint32 p_Winner)
{
    Creature* l_Mathias = GetBGCreature(eShoreCreatureTypes::MasterMathiasType);
    Creature* l_Nathanos = GetBGCreature(eShoreCreatureTypes::NathanosType);
    if (!l_Mathias || !l_Nathanos)
        return;

    bool l_WonZero = false;
    if (p_Winner == ALLIANCE)
    {
        sCreatureTextMgr->SendChat(l_Mathias, 3, 0, CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, TEXT_RANGE_MAP, 0, (Team)ALLIANCE);
        if (m_TeamScores[1] == 0)
            l_WonZero = true;
    }
    else
    {
        sCreatureTextMgr->SendChat(l_Nathanos, 5, 0, CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, TEXT_RANGE_MAP, 0, (Team)HORDE);
        if (m_TeamScores[0] == 0)
            l_WonZero = true;
    }

    Battleground::EndBattleground(p_Winner);
    AchievementEntry const* l_Entry             = sAchievementStore.LookupEntry(eShoreAchievements::Victory);
    AchievementEntry const* l_PerfectionEntry   = sAchievementStore.LookupEntry(eShoreAchievements::Perfection);
    for (BattlegroundPlayerMap::const_iterator l_Itr = GetPlayers().begin(); l_Itr != GetPlayers().end(); l_Itr++)
    {
        Player* l_Player = ObjectAccessor::FindPlayer(l_Itr->first);
        if (!l_Player || (l_Player->GetTeam() != p_Winner))
            continue;

        if (!l_Player->HasAchieved(l_Entry->ID))
            l_Player->CompletedAchievement(l_Entry);

        if (l_WonZero && !l_Player->HasAchieved(l_PerfectionEntry->ID))
            l_Player->CompletedAchievement(l_PerfectionEntry);
    }
}

bool BattlegroundSS::IsFissureSpawned()
{
    for (uint32 l_Itr = 0; l_Itr < g_AzeriteFissureSpawns.size(); l_Itr++)
    {
        Creature* l_Fissure = GetBGCreature(l_Itr);
        if (l_Fissure == nullptr)
            continue;

        if (l_Fissure->GetEntry() != eShoreCreatures::AzeriteFissure)
            continue;

        return true;
    }

    return false;
}

bool BattlegroundSS::IsAzeriteSpawned()
{
    for (uint32 l_Itr = 0; l_Itr < g_AzeriteFissureSpawns.size(); l_Itr++)
    {
        GameObject* l_Azerite = GetBGObject(l_Itr);
        if (l_Azerite == nullptr)
            continue;

        if (l_Azerite->GetEntry() != eShoreGameObjects::AzeriteNode && l_Azerite->GetEntry() != eShoreGameObjects::RichAzeriteNode)
            continue;

        return true;
    }
    return false;
}

void BattlegroundSS::UpdateVignettes()
{
    VignetteEntry const* l_FissureVignette          = sVignetteStore.LookupEntry(eShoreVignettes::Fissure);
    VignetteEntry const* l_AzeriteVignette          = sVignetteStore.LookupEntry(eShoreVignettes::Azerite);
    VignetteEntry const* l_CrashSiteVignette        = sVignetteStore.LookupEntry(eShoreVignettes::CrashSite);
    VignetteEntry const* l_WaterfallVignette        = sVignetteStore.LookupEntry(eShoreVignettes::Waterfall);
    VignetteEntry const* l_RuinsVignette            = sVignetteStore.LookupEntry(eShoreVignettes::Ruins);
    VignetteEntry const* l_OverlookVignette         = sVignetteStore.LookupEntry(eShoreVignettes::Overlook);
    VignetteEntry const* l_PlungeAzeriteVignette    = sVignetteStore.LookupEntry(eShoreVignettes::Plunge);
    VignetteEntry const* l_TowerVignette            = sVignetteStore.LookupEntry(eShoreVignettes::Tower);
    VignetteEntry const* l_TidePoolsVignette        = sVignetteStore.LookupEntry(eShoreVignettes::TidePools);
    VignetteEntry const* l_TempleVignette           = sVignetteStore.LookupEntry(eShoreVignettes::Temple);
    VignetteEntry const* l_SoonVignette             = sVignetteStore.LookupEntry(eShoreVignettes::SoonAzerite);
    VignetteEntry const* l_RidgeVignette            = sVignetteStore.LookupEntry(eShoreVignettes::Ridge);
    VignetteEntry const* l_TarPitsVignette          = sVignetteStore.LookupEntry(eShoreVignettes::TarPits);

    for (BattlegroundPlayerMap::const_iterator l_PlayerItr = GetPlayers().begin(); l_PlayerItr != GetPlayers().end(); l_PlayerItr++)
    {
        Player* l_Player = ObjectAccessor::FindPlayer(l_PlayerItr->first);
        if (!l_Player)
            continue;

        l_Player->GetVignetteMgr().DestroyAndRemoveVignetteByEntry(l_FissureVignette);
        l_Player->GetVignetteMgr().DestroyAndRemoveVignetteByEntry(l_AzeriteVignette);
        l_Player->GetVignetteMgr().DestroyAndRemoveVignetteByEntry(l_CrashSiteVignette);
        l_Player->GetVignetteMgr().DestroyAndRemoveVignetteByEntry(l_WaterfallVignette);
        l_Player->GetVignetteMgr().DestroyAndRemoveVignetteByEntry(l_RuinsVignette);
        l_Player->GetVignetteMgr().DestroyAndRemoveVignetteByEntry(l_OverlookVignette);
        l_Player->GetVignetteMgr().DestroyAndRemoveVignetteByEntry(l_PlungeAzeriteVignette);
        l_Player->GetVignetteMgr().DestroyAndRemoveVignetteByEntry(l_TowerVignette);
        l_Player->GetVignetteMgr().DestroyAndRemoveVignetteByEntry(l_TidePoolsVignette);
        l_Player->GetVignetteMgr().DestroyAndRemoveVignetteByEntry(l_TempleVignette);
        l_Player->GetVignetteMgr().DestroyAndRemoveVignetteByEntry(l_SoonVignette);
        l_Player->GetVignetteMgr().DestroyAndRemoveVignetteByEntry(l_RidgeVignette);
        l_Player->GetVignetteMgr().DestroyAndRemoveVignetteByEntry(l_TarPitsVignette);

        Vignette::Entity* l_VignetteEntity = nullptr;

        for (uint32 l_Itr = 0; l_Itr < g_AzeriteFissureSpawns.size(); l_Itr++)
        {
            Creature* l_Fissure     = GetBGCreature(l_Itr);
            GameObject* l_Azerite   = GetBGObject(l_Itr);
            Position const l_Position = g_AzeriteFissureSpawns[l_Itr];
            if (!l_Fissure && !l_Azerite)
                continue;

            if (l_Fissure == nullptr)
            {
                switch (l_Itr)
                {
                    case 0:
                        l_VignetteEntity = l_Player->GetVignetteMgr().CreateAndAddVignette(l_RidgeVignette, l_Player->GetMapId(), Vignette::Type::SourceGameObject, G3D::Vector3(l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ));
                        break;
                    case 1:
                        l_VignetteEntity = l_Player->GetVignetteMgr().CreateAndAddVignette(l_PlungeAzeriteVignette, l_Player->GetMapId(), Vignette::Type::SourceGameObject, G3D::Vector3(l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ));
                        break;
                    case 2:
                        l_VignetteEntity = l_Player->GetVignetteMgr().CreateAndAddVignette(l_TempleVignette, l_Player->GetMapId(), Vignette::Type::SourceGameObject, G3D::Vector3(l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ));
                        break;
                    case 3:
                        l_VignetteEntity = l_Player->GetVignetteMgr().CreateAndAddVignette(l_TempleVignette, l_Player->GetMapId(), Vignette::Type::SourceGameObject, G3D::Vector3(l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ));
                        break;
                    case 4:
                        l_VignetteEntity = l_Player->GetVignetteMgr().CreateAndAddVignette(l_SoonVignette, l_Player->GetMapId(), Vignette::Type::SourceGameObject, G3D::Vector3(l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ));
                        break;
                    case 5:
                        l_VignetteEntity = l_Player->GetVignetteMgr().CreateAndAddVignette(l_TowerVignette, l_Player->GetMapId(), Vignette::Type::SourceGameObject, G3D::Vector3(l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ));
                        break;
                    case 6:
                        l_VignetteEntity = l_Player->GetVignetteMgr().CreateAndAddVignette(l_RuinsVignette, l_Player->GetMapId(), Vignette::Type::SourceGameObject, G3D::Vector3(l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ));
                        break;
                    case 7:
                        l_VignetteEntity = l_Player->GetVignetteMgr().CreateAndAddVignette(l_CrashSiteVignette, l_Player->GetMapId(), Vignette::Type::SourceGameObject, G3D::Vector3(l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ));
                        break;
                    case 8:
                        l_VignetteEntity = l_Player->GetVignetteMgr().CreateAndAddVignette(l_OverlookVignette, l_Player->GetMapId(), Vignette::Type::SourceGameObject, G3D::Vector3(l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ));
                        break;
                    default:
                        l_VignetteEntity = l_Player->GetVignetteMgr().CreateAndAddVignette(l_AzeriteVignette, l_Player->GetMapId(), Vignette::Type::SourceGameObject, G3D::Vector3(l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ));
                        break;
                }
            }
            else if (l_Azerite == nullptr)
            {
                if (l_Fissure->GetEntry() == eShoreCreatures::AzeriteFissure)
                    l_VignetteEntity = l_Player->GetVignetteMgr().CreateAndAddVignette(l_FissureVignette, l_Player->GetMapId(), Vignette::Type::SourceCreature, G3D::Vector3(l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ));
            }
        }
    }
}

void BattlegroundSS::ConvertFissures()
{
    for (uint32 l_Itr = 0; l_Itr < g_AzeriteFissureSpawns.size(); l_Itr++)
    {
        Creature* l_Fissure = GetBGCreature(l_Itr);
        if (l_Fissure == nullptr)
            continue;

        if (l_Fissure->GetEntry() !=eShoreCreatures::AzeriteFissure)
            continue;

        uint32 l_NodeEntry = eShoreGameObjects::AzeriteNode;

        /// Roll for rich deposit
        if (roll_chance_i(30))
            l_NodeEntry = eShoreGameObjects::RichAzeriteNode;

        /// Spawn deposit
        bool m_Found = false;

        GameObject* l_ExistingNode = GetBGObject(l_Itr);
        if (l_ExistingNode == nullptr)
            m_Found = false;
        else
            m_Found = true;

        if (m_Found)
            l_ExistingNode->Delete();
        else
        {
            l_Fissure->CastSpell(l_Fissure, eShoreSpells::AzeriteKnockBack, true);

            AddObject(l_Itr, l_NodeEntry, g_AzeriteFissureSpawns[l_Itr], {}, RESPAWN_IMMEDIATELY);
        }

        DelCreature(l_Itr);
    }
    UpdateVignettes();
}

void BattlegroundSS::SpawnAzeriteFissures(int8 p_Amount)
{
    /// How Azerite appears:
    /// 1. A fissure appears
    /// 2. The fissure disappears after a while
    /// 3. There is either a rich azerite deposit or a normal one
    /// Rich azerite Node gives 200 points, Normal ones 100

    Creature* l_Mathias = GetBGCreature(eShoreCreatureTypes::MasterMathiasType);
    Creature* l_Nathanos = GetBGCreature(eShoreCreatureTypes::NathanosType);
    if (!l_Mathias || !l_Nathanos)
        return;

    int8 l_Count = 0;
    while (l_Count < p_Amount)
    {
        uint8 l_NodeID = urand(0, 8);

        Creature* l_Fissure = GetBGCreature(l_NodeID);
        if (l_Fissure == nullptr)
        {
            if (GameObject* l_Node = GetBGObject(l_NodeID))
            {
                if (l_Node->GetEntry() == eShoreGameObjects::AzeriteNode || l_Node->GetEntry() == eShoreGameObjects::RichAzeriteNode)
                    continue;
            }

            Position l_Pos = g_AzeriteFissureSpawns[l_NodeID];
            AddCreature(eShoreCreatures::AzeriteFissure, l_NodeID, TEAM_NONE, l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ, l_Pos.m_orientation);

            if (!m_FirstTime)
            {
                m_FirstTime = true;
                switch (l_NodeID)
                {
                    case 4:
                    {
                        sCreatureTextMgr->SendChat(l_Mathias, 0, 0, CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, TEXT_RANGE_MAP, 0, (Team)ALLIANCE);
                        sCreatureTextMgr->SendChat(l_Nathanos, 2, 0, CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, TEXT_RANGE_MAP, 0, (Team)HORDE);
                        break;
                    }
                    default:
                    {
                        sCreatureTextMgr->SendChat(l_Mathias, 6, 0, CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, TEXT_RANGE_MAP, 0, (Team)ALLIANCE);
                        sCreatureTextMgr->SendChat(l_Nathanos, 8, 0, CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, TEXT_RANGE_MAP, 0, (Team)HORDE);
                        break;
                    }
                }
            }
            l_Count++;
        }
        else if (l_Fissure->GetEntry() != eShoreCreatures::AzeriteFissure)
        {
            if (GameObject* l_Node = GetBGObject(l_NodeID))
            {
                if (l_Node->GetEntry() == eShoreGameObjects::AzeriteNode || l_Node->GetEntry() == eShoreGameObjects::RichAzeriteNode)
                    continue;
            }

            Position l_Pos = g_AzeriteFissureSpawns[l_NodeID];
            AddCreature(eShoreCreatures::AzeriteFissure, l_NodeID, TEAM_NONE, l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ, l_Pos.m_orientation);

            if (!m_FirstTime)
            {
                m_FirstTime = true;
                switch (l_NodeID)
                {
                    case 4:
                    {
                        sCreatureTextMgr->SendChat(l_Mathias, 0, 0, CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, TEXT_RANGE_MAP, 0, (Team)ALLIANCE);
                        sCreatureTextMgr->SendChat(l_Nathanos, 2, 0, CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, TEXT_RANGE_MAP, 0, (Team)HORDE);
                        break;
                    }
                    default:
                    {
                        sCreatureTextMgr->SendChat(l_Mathias, 6, 0, CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, TEXT_RANGE_MAP, 0, (Team)ALLIANCE);
                        sCreatureTextMgr->SendChat(l_Nathanos, 8, 0, CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, TEXT_RANGE_MAP, 0, (Team)HORDE);
                        break;
                    }
                }
            }
            l_Count++;
        }
    }

    SendWarningToAll(eShoreStrings::FissureAppeared);
    m_FirstTime = false;
    UpdateVignettes();
}

void BattlegroundSS::FillInitialWorldStates(ByteBuffer& p_Data)
{
    p_Data << uint32(eShoreWorldStates::MaxAzeriteWorldState) << uint32(eShoreBattlegroundScores::MaxTeamScore);
    p_Data << uint32(eShoreWorldStates::AzeriteDisplayAlliance) << uint32(GetTeamScore(Team::ALLIANCE));
    p_Data << uint32(eShoreWorldStates::AzeriteDisplayHorde) << uint32(GetTeamScore(Team::HORDE));
}

WorldSafeLocsEntry const* BattlegroundSS::GetClosestGraveYard(Player* p_Player)
{
    return nullptr;
}

void BattlegroundSS::RepopPlayerAtGraveyard(uint64 p_GUID)
{
    Player* l_Player = ObjectAccessor::FindPlayer(p_GUID);
    if (!l_Player)
        return;

    GameObject* l_GunshipAllianceObject     = sObjectAccessor->FindGameObject(m_GunshipAllianceGUID);
    GameObject* l_GunshipHordeObject        = sObjectAccessor->FindGameObject(m_GunshipHordeGUID);
    if (!l_GunshipAllianceObject || !l_GunshipHordeObject)
        return;

    Transport* l_Transport = nullptr;
    if (l_Player->GetTeamId() == TeamId::TEAM_ALLIANCE)
        l_Transport = l_GunshipAllianceObject->ToTransport();
    else
        l_Transport = l_GunshipHordeObject->ToTransport();

    if (l_Transport == nullptr)
        return;

    float x = 0.0f;
    float y = 0.0f;
    float z = l_Transport->GetEntry() == eShoreGameObjects::AllianceGunship ? 40.0f : 50.0f;
    float o = 0.0f;

    l_Transport->CalculatePassengerPosition(x, y, z, o);
    l_Player->TeleportTo(l_Transport->GetMapId(), x, y, z, o);

    uint64 l_TransportGUID = l_Transport->GetGUID();
    l_Player->AddDelayedEvent([l_Player, l_TransportGUID]() -> void
    {
        GameObject* l_Gob = sObjectAccessor->FindGameObject(l_TransportGUID);
        if (!l_Gob || !l_Gob->IsTransport())
            return;

        Transport* l_Transport = l_Gob->ToTransport();
        if (!l_Transport)
            return;

        l_Player->SetTransport(l_Transport);
        l_Transport->AddPassenger(l_Player);
    }, 3 * TimeConstants::IN_MILLISECONDS);
}

void BattlegroundSS::AddPlayer(Player* p_Player)
{
    Battleground::AddPlayer(p_Player);
    BattlegroundSSScore* l_Score = new BattlegroundSSScore;
    PlayerScores[p_Player->GetGUID()] = l_Score;

    Transport* l_GunshipHorde       = NULL;
    Transport* l_GunshipAlliance    = NULL;

    if (m_GunshipAllianceGUID == 0 || m_GunshipHordeGUID == 0)
    {
        l_GunshipHorde      = sTransportMgr->CreateTransport(eShoreGameObjects::HordeGunship, 0, GetBgMap());
        l_GunshipAlliance   = sTransportMgr->CreateTransport(eShoreGameObjects::AllianceGunship, 0, GetBgMap());

        m_GunshipAllianceGUID   = l_GunshipAlliance ? l_GunshipAlliance->GetGUID() : 0;
        m_GunshipHordeGUID      = l_GunshipHorde ? l_GunshipHorde->GetGUID() : 0;
    }
    else
    {
        l_GunshipAlliance   = sObjectAccessor->FindGameObject(m_GunshipAllianceGUID)->ToTransport();
        l_GunshipHorde      = sObjectAccessor->FindGameObject(m_GunshipHordeGUID)->ToTransport();
    }

    if (!l_GunshipHorde && !l_GunshipAlliance)
        return;

    Transport* l_Transport = nullptr;
    switch (p_Player->GetTeamId())
    {
        case TeamId::TEAM_ALLIANCE:
            l_Transport = l_GunshipAlliance;
            break;
        case TeamId::TEAM_HORDE:
            l_Transport = l_GunshipHorde;
            break;
        default:
            return;
    }

    Position l_Position;
    l_Position.m_positionZ = l_Transport->GetEntry() == eShoreGameObjects::AllianceGunship ? 35.0f : 45.0f;

    l_Transport->CalculatePassengerPosition(l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ, l_Position.m_orientation);

    p_Player->TeleportTo(l_Transport->GetMapId(), l_Position.m_positionX, l_Position.m_positionY, l_Position.m_positionZ, l_Position.m_orientation);

    uint64 l_TransportGUID = l_Transport->GetGUID();

    p_Player->AddDelayedEvent([p_Player, l_TransportGUID]() -> void
    {
        GameObject* l_Gob = sObjectAccessor->FindGameObject(l_TransportGUID);
        if (!l_Gob || !l_Gob->IsTransport())
            return;

        Transport* l_Transport = l_Gob->ToTransport();
        if (!l_Transport)
            return;

        p_Player->SetTransport(l_Transport);
        l_Transport->AddPassenger(p_Player);
    }, 1);

    p_Player->CastSpell(p_Player, eShoreSpells::UnchartedTerritory);
    p_Player->CastSpell(p_Player, eShoreSpells::RocketParachute1);
    if (GetStatus() == STATUS_IN_PROGRESS)
        p_Player->CastSpell(p_Player, eShoreSpells::RocketParachute2, false);
}

void BattlegroundSS::HandlePlayerResurrect(Player* player)
{
    player->CastSpell(player, eShoreSpells::UnchartedTerritory);
    player->CastSpell(player, eShoreSpells::RocketParachute1);
    player->CastSpell(player, eShoreSpells::RocketParachute2);
}

void BattlegroundSS::RemovePlayer(Player* player, uint64 /*guid*/, uint32 /*team*/)
{
    if (!player)
        return;

    player->RemoveAura(eShoreSpells::UnchartedTerritory);
    player->RemoveAura(eShoreSpells::RocketParachute1);
    player->RemoveAura(eShoreSpells::RocketParachute2);
}

// 250917
class spell_bg_seething_shore_rocket_parachute2 : public AuraScript
{
    PrepareAuraScript(spell_bg_seething_shore_rocket_parachute2);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        auto caster = GetCaster();
        auto player = caster->ToPlayer();
        if (!caster || !player)
            return;

        if (player->HasAura(eShoreSpells::RocketParachute3) || player->GetPositionZ() <= 120.0f || player->GetPositionZ() >= 272.40f)
            return;

        player->CastSpell(player, eShoreSpells::RocketParachute3, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_bg_seething_shore_rocket_parachute2::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 250921
class spell_bg_seething_shore_rocket_parachute3 : public AuraScript
{
    PrepareAuraScript(spell_bg_seething_shore_rocket_parachute3);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        auto caster = GetCaster();
        auto player = caster->ToPlayer();
        if (!caster || !player)
            return;

        auto map = player->GetMap();
        if (!map)
            return;

        auto curZ = player->GetPositionZ();
        auto floorZ = map->GetHeight(player->GetPhaseMask(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
        if ((curZ - floorZ) > 30.f || player->IsInWater())
            return;

        player->RemoveAura(eShoreSpells::RocketParachute3);
        player->CastSpell(player, 252766, true);
        player->CastSpell(player, 261954, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_bg_seething_shore_rocket_parachute3::OnPeriodic, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

void AddSC_BattlegroundSSScripts()
{
    RegisterAuraScript(spell_bg_seething_shore_rocket_parachute2);
    RegisterAuraScript(spell_bg_seething_shore_rocket_parachute3);
}