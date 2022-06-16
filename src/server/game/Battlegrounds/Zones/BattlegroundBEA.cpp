////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Battleground.h"
#include "BattlegroundBEA.h"
#include "Language.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "WorldPacket.h"

BattlegroundBEA::BattlegroundBEA()
{
    BgObjects.resize(BG_BEA_OBJECT_MAX);

    StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_1M;
    StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_30S;
    StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_15S;
    StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;

    /// We must set messageIds
    StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_ARENA_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_ARENA_THIRTY_SECONDS;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_ARENA_FIFTEEN_SECONDS;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_ARENA_HAS_BEGUN;

    m_HandleDoorsTimer = 0;
}

BattlegroundBEA::~BattlegroundBEA()
{

}

void BattlegroundBEA::AddPlayer(Player* p_player)
{
    Battleground::AddPlayer(p_player);
    BattlegroundBEAScore* l_Scores = new BattlegroundBEAScore;

    PlayerScores[p_player->GetGUID()] = l_Scores;

    UpdateArenaWorldState();
}

void BattlegroundBEA::RemovePlayer(Player* p_Player, uint64 p_Guid, uint32 p_Team)
{
    if (GetStatus() == BattlegroundStatus::STATUS_WAIT_LEAVE)
        return;

    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

void BattlegroundBEA::StartingEventCloseDoors()
{
    SpawnBGObject(BG_BEA_OBJECT_DOOR_1, RESPAWN_IMMEDIATELY);
    SpawnBGObject(BG_BEA_OBJECT_DOOR_2, RESPAWN_IMMEDIATELY);

    Battleground::StartingEventCloseDoors();
}

void BattlegroundBEA::StartingEventOpenDoors()
{
    DoorOpen(BG_BEA_OBJECT_DOOR_1);
    DoorOpen(BG_BEA_OBJECT_DOOR_2);

    SpawnBGObject(BG_BEA_OBJECT_BUFF_1, 60);
    SpawnBGObject(BG_BEA_OBJECT_BUFF_2, 60);

    m_HandleDoorsTimer = BG_BEA_HANDLE_DOORS_TIMER;

    Battleground::StartingEventOpenDoors();
}

void BattlegroundBEA::FillInitialWorldStates(ByteBuffer& p_Packet)
{
    /// Don't know what to add here, need some help at this point
    UpdateArenaWorldState();
}

bool BattlegroundBEA::SetupBattleground()
{
    /// Spawn Gates
    if (!AddObject(BG_BEA_OBJECT_DOOR_1, BG_BEA_OBJECT_TYPE_DOOR, BgBEDoorPositions[0][0], BgBEDoorPositions[0][1], RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_BEA_OBJECT_DOOR_2, BG_BEA_OBJECT_TYPE_DOOR, BgBEDoorPositions[1][0], BgBEDoorPositions[1][1], RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_BEA_OBJECT_BUFF_1, BG_BEA_OBJECT_TYPE_BUFF_1, BgBEDoorPositions[2][0], BgBEDoorPositions[2][1], 120) ||
        !AddObject(BG_BEA_OBJECT_BUFF_2, BG_BEA_OBJECT_TYPE_BUFF_2, BgBEDoorPositions[3][0], BgBEDoorPositions[3][1], 120))
        return false;

    /// Orgre will come at a later time but is not a must have to make the arena functional/playable.

    return true;
}

void BattlegroundBEA::PostUpdateImpl(uint32 p_Diff)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (m_HandleDoorsTimer <= p_Diff)
    {
        if (IsDoorClosed(BG_BEA_OBJECT_DOOR_1))
            DoorOpen(BG_BEA_OBJECT_DOOR_1);

        if (IsDoorClosed(BG_BEA_OBJECT_DOOR_2))
            DoorOpen(BG_BEA_OBJECT_DOOR_2);

        m_HandleDoorsTimer = BG_BEA_HANDLE_DOORS_TIMER;
    }
    else
        m_HandleDoorsTimer -= p_Diff;
}

void BattlegroundBEA::HandleKillPlayer(Player* p_Player, Player* p_Killer)
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
