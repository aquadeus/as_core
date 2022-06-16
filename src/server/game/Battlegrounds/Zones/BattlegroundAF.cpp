////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "BattlegroundAF.hpp"

BattlegroundAF::BattlegroundAF()
{
    BgObjects.resize(BG_AF_OBJECT_MAX);

    StartDelayTimes[BG_STARTING_EVENT_FIRST]    = BG_START_DELAY_1M;
    StartDelayTimes[BG_STARTING_EVENT_SECOND]   = BG_START_DELAY_30S;
    StartDelayTimes[BG_STARTING_EVENT_THIRD]    = BG_START_DELAY_15S;
    StartDelayTimes[BG_STARTING_EVENT_FOURTH]   = BG_START_DELAY_NONE;

    /// We must set messageIds
    StartMessageIds[BG_STARTING_EVENT_FIRST]    = LANG_ARENA_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_SECOND]   = LANG_ARENA_THIRTY_SECONDS;
    StartMessageIds[BG_STARTING_EVENT_THIRD]    = LANG_ARENA_FIFTEEN_SECONDS;
    StartMessageIds[BG_STARTING_EVENT_FOURTH]   = LANG_ARENA_HAS_BEGUN;

    m_CheatersCheckTimer = 0;
}

BattlegroundAF::~BattlegroundAF()
{
}

void BattlegroundAF::AddPlayer(Player* p_Player)
{
    Battleground::AddPlayer(p_Player);

    /// Create score and add it to map, default values are set in constructor
    BattlegroundAFScore* l_Scores = new BattlegroundAFScore;

    PlayerScores[p_Player->GetGUID()] = l_Scores;

    UpdateArenaWorldState();
}

void BattlegroundAF::RemovePlayer(Player* /*p_Player*/, uint64 /*p_Guid*/, uint32 /*p_Team*/)
{
    if (GetStatus() == BattlegroundStatus::STATUS_WAIT_LEAVE)
        return;

    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

void BattlegroundAF::StartingEventCloseDoors()
{
    for (uint32 i = BG_AF_OBJECT_DOOR_1; i < BG_AF_OBJECT_BUFF_1; ++i)
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);

    Battleground::StartingEventCloseDoors();
}

void BattlegroundAF::StartingEventOpenDoors()
{
    for (uint32 i = BG_AF_OBJECT_DOOR_1; i < BG_AF_OBJECT_BUFF_1; ++i)
        DoorOpen(i);

    SpawnBGObject(BG_AF_OBJECT_BUFF_1, 60);
    SpawnBGObject(BG_AF_OBJECT_BUFF_2, 60);

    Battleground::StartingEventOpenDoors();
}

void BattlegroundAF::FillInitialWorldStates(ByteBuffer& p_Packet)
{
    p_Packet << uint32(11326) << uint32(1);
    p_Packet << uint32(11384) << uint32(1);
    p_Packet << uint32(12206) << uint32(1);

    UpdateArenaWorldState();
}

bool BattlegroundAF::SetupBattleground()
{
    if (!AddObject(BG_AF_OBJECT_DOOR_1, BG_AF_OBJECT_TYPE_DOOR_1, BgVshDoorPositions[0][0], BgVshDoorPositions[0][1], RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_AF_OBJECT_DOOR_2, BG_AF_OBJECT_TYPE_DOOR_2, BgVshDoorPositions[1][0], BgVshDoorPositions[1][1], RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_AF_OBJECT_BUFF_1, BG_AF_OBJECT_TYPE_BUFF_1, BgVshDoorPositions[2][0], BgVshDoorPositions[2][1], 120) ||
        !AddObject(BG_AF_OBJECT_BUFF_2, BG_AF_OBJECT_TYPE_BUFF_2, BgVshDoorPositions[3][0], BgVshDoorPositions[3][1], 120))
        return false;

    return true;
}

void BattlegroundAF::HandleKillPlayer(Player* p_Player, Player* p_Killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!p_Killer)
    {
        sLog->outError(LOG_FILTER_BATTLEGROUND, "Killer player not found");
        return;
    }

    Battleground::HandleKillPlayer(p_Player, p_Killer);

    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

void BattlegroundAF::PostUpdateImpl(uint32 p_Diff)
{
    if (GetStatus() == STATUS_WAIT_JOIN)
    {
        if (m_CheatersCheckTimer <= p_Diff)
        {
            auto l_Players = GetPlayers();
            Position l_AlliancePos = { 3540.27f, 5476.92f, 325.50f, 0.0f };
            Position l_HordePos = { 3548.22f, 5595.64f, 325.27f, 0.0f };
            for (auto l_Pair : l_Players)
            {
                Player* l_Player = ObjectAccessor::FindPlayer(l_Pair.first);
                if (!l_Player || !l_Player->IsInWorld())
                    continue;

                if (l_Player->GetBGTeam() == ALLIANCE && l_Player->GetExactDist(&l_AlliancePos) > 15.0f)
                {
                    l_Player->TeleportTo(GetMapId(), l_AlliancePos.GetPositionX(), l_AlliancePos.GetPositionY(), l_AlliancePos.GetPositionZ(), l_Player->GetOrientation(), 0);
                }
                else if (l_Player->GetBGTeam() == HORDE && l_Player->GetExactDist(&l_HordePos) > 15.0f)
                {
                    l_Player->TeleportTo(GetMapId(), l_HordePos.GetPositionX(), l_HordePos.GetPositionY(), l_HordePos.GetPositionZ(), l_Player->GetOrientation());
                }
            }
            m_CheatersCheckTimer = 4000;
        }
        else
        {
            m_CheatersCheckTimer -= p_Diff;
        }
    }
}
