////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "BattlegroundBRH.hpp"

BattlegroundBRH::BattlegroundBRH()
{
    BgObjects.resize(BG_BRH_OBJECT_MAX);

    StartDelayTimes[BG_STARTING_EVENT_FIRST]    = BG_START_DELAY_1M;
    StartDelayTimes[BG_STARTING_EVENT_SECOND]   = BG_START_DELAY_30S;
    StartDelayTimes[BG_STARTING_EVENT_THIRD]    = BG_START_DELAY_15S;
    StartDelayTimes[BG_STARTING_EVENT_FOURTH]   = BG_START_DELAY_NONE;

    /// We must set messageIds
    StartMessageIds[BG_STARTING_EVENT_FIRST]    = LANG_ARENA_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_SECOND]   = LANG_ARENA_THIRTY_SECONDS;
    StartMessageIds[BG_STARTING_EVENT_THIRD]    = LANG_ARENA_FIFTEEN_SECONDS;
    StartMessageIds[BG_STARTING_EVENT_FOURTH]   = LANG_ARENA_HAS_BEGUN;

    m_HandleDoorsTimer = 0;
}

BattlegroundBRH::~BattlegroundBRH()
{
}

void BattlegroundBRH::AddPlayer(Player* p_Player)
{
    Battleground::AddPlayer(p_Player);

    /// Create score and add it to map, default values are set in constructor
    BattlegroundBRHScore* l_Scores = new BattlegroundBRHScore;

    PlayerScores[p_Player->GetGUID()] = l_Scores;

    UpdateArenaWorldState();
}

void BattlegroundBRH::RemovePlayer(Player* /*p_Player*/, uint64 /*p_Guid*/, uint32 /*p_Team*/)
{
    if (GetStatus() == BattlegroundStatus::STATUS_WAIT_LEAVE)
        return;

    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

void BattlegroundBRH::StartingEventCloseDoors()
{
    for (uint32 i = BG_BRH_OBJECT_DOOR_1; i < BG_BRH_OBJECT_BUFF_1; ++i)
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);

    Battleground::StartingEventCloseDoors();
}

void BattlegroundBRH::StartingEventOpenDoors()
{
    for (uint32 i = BG_BRH_OBJECT_DOOR_1; i < BG_BRH_OBJECT_BUFF_1; ++i)
    {
        switch (i)
        {
            /// Middle doors aren't opened at arena start
            case BG_BRH_OBJECT_DOOR_1:
            case BG_BRH_OBJECT_DOOR_4:
            case BG_BRH_OBJECT_DOOR_7:
                break;
            default:
                DoorOpen(i);
                break;
        }
    }

    SpawnBGObject(BG_BRH_OBJECT_BUFF_1, 60);
    SpawnBGObject(BG_BRH_OBJECT_BUFF_2, 60);

    m_HandleDoorsTimer = BG_DRH_HANDLE_DOORS_TIMER;

    Battleground::StartingEventOpenDoors();
}

void BattlegroundBRH::FillInitialWorldStates(ByteBuffer& p_Packet)
{
    p_Packet << uint32(5344) << uint32(1);
    p_Packet << uint32(11330) << uint32(1);
    p_Packet << uint32(11419) << uint32(1);
    p_Packet << uint32(11573) << uint32(1);

    UpdateArenaWorldState();
}

bool BattlegroundBRH::SetupBattleground()
{
    if (!AddObject(BG_BRH_OBJECT_DOOR_1, BG_BRH_OBJECT_TYPE_DOOR, BgBrhDoorPositions[0][0], BgBrhDoorPositions[0][1], RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_BRH_OBJECT_DOOR_2, BG_BRH_OBJECT_TYPE_DOOR, BgBrhDoorPositions[1][0], BgBrhDoorPositions[1][1], RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_BRH_OBJECT_DOOR_3, BG_BRH_OBJECT_TYPE_DOOR, BgBrhDoorPositions[2][0], BgBrhDoorPositions[2][1], RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_BRH_OBJECT_DOOR_4, BG_BRH_OBJECT_TYPE_DOOR, BgBrhDoorPositions[3][0], BgBrhDoorPositions[3][1], RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_BRH_OBJECT_DOOR_5, BG_BRH_OBJECT_TYPE_DOOR, BgBrhDoorPositions[4][0], BgBrhDoorPositions[4][1], RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_BRH_OBJECT_DOOR_6, BG_BRH_OBJECT_TYPE_DOOR, BgBrhDoorPositions[5][0], BgBrhDoorPositions[5][1], RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_BRH_OBJECT_DOOR_7, BG_BRH_OBJECT_TYPE_DOOR, BgBrhDoorPositions[6][0], BgBrhDoorPositions[6][1], RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_BRH_OBJECT_DOOR_8, BG_BRH_OBJECT_TYPE_DOOR, BgBrhDoorPositions[7][0], BgBrhDoorPositions[7][1], RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_BRH_OBJECT_DOOR_9, BG_BRH_OBJECT_TYPE_DOOR, BgBrhDoorPositions[8][0], BgBrhDoorPositions[8][1], RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_BRH_OBJECT_BUFF_1, BG_BRH_OBJECT_TYPE_BUFF_1, BgBrhDoorPositions[9][0], BgBrhDoorPositions[9][1], 120)               ||
        !AddObject(BG_BRH_OBJECT_BUFF_2, BG_BRH_OBJECT_TYPE_BUFF_2, BgBrhDoorPositions[10][0], BgBrhDoorPositions[10][1], 120))
        return false;

    return true;
}

void BattlegroundBRH::PostUpdateImpl(uint32 p_Diff)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (m_HandleDoorsTimer <= p_Diff)
    {
        for (uint32 i = BG_BRH_OBJECT_DOOR_1; i < BG_BRH_OBJECT_BUFF_1; ++i)
        {
            switch (i)
            {
            case BG_BRH_OBJECT_DOOR_1:
            case BG_BRH_OBJECT_DOOR_2:
            case BG_BRH_OBJECT_DOOR_4:
            case BG_BRH_OBJECT_DOOR_5:
            case BG_BRH_OBJECT_DOOR_7:
            case BG_BRH_OBJECT_DOOR_8:
                if (urand(0, 1))
                {
                    if (IsDoorClosed(i))
                        DoorOpen(i);
                    else
                        DoorClose(i);
                }
                break;
            case BG_BRH_OBJECT_DOOR_3:
                if (IsDoorClosed(BG_BRH_OBJECT_DOOR_1) && IsDoorClosed(BG_BRH_OBJECT_DOOR_2) && IsDoorClosed(i))
                    DoorOpen(i);
                else if (!IsDoorClosed(BG_BRH_OBJECT_DOOR_1) || !IsDoorClosed(BG_BRH_OBJECT_DOOR_2))
                {
                    if (urand(0, 1))
                    {
                        if (IsDoorClosed(i))
                            DoorOpen(i);
                        else
                            DoorClose(i);
                    }
                }
                break;
            case BG_BRH_OBJECT_DOOR_6:
                if (IsDoorClosed(BG_BRH_OBJECT_DOOR_4) && IsDoorClosed(BG_BRH_OBJECT_DOOR_5) && IsDoorClosed(i))
                    DoorOpen(i);
                else if (!IsDoorClosed(BG_BRH_OBJECT_DOOR_4) || !IsDoorClosed(BG_BRH_OBJECT_DOOR_5))
                {
                    if (urand(0, 1))
                    {
                        if (IsDoorClosed(i))
                            DoorOpen(i);
                        else
                            DoorClose(i);
                    }
                }
                break;
            case BG_BRH_OBJECT_DOOR_9:
                if (IsDoorClosed(BG_BRH_OBJECT_DOOR_7) && IsDoorClosed(BG_BRH_OBJECT_DOOR_8) && IsDoorClosed(i))
                    DoorOpen(i);
                else if (!IsDoorClosed(BG_BRH_OBJECT_DOOR_7) || !IsDoorClosed(BG_BRH_OBJECT_DOOR_8))
                {
                    if (urand(0, 1))
                    {
                        if (IsDoorClosed(i))
                            DoorOpen(i);
                        else
                            DoorClose(i);
                    }
                }
                break;
            default:
                break;
            }
        }

        m_HandleDoorsTimer = BG_DRH_HANDLE_DOORS_TIMER;
    }
    else
        m_HandleDoorsTimer -= p_Diff;
}

void BattlegroundBRH::HandleKillPlayer(Player* p_Player, Player* p_Killer)
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
