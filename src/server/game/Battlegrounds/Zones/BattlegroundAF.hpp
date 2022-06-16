////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef BATTLEGROUND_AF_HPP
# define BATTLEGROUND_AF_HPP

# include "Arena.h"
# include "Language.h"
# include "Object.h"
# include "ObjectMgr.h"
# include "Player.h"
# include "WorldPacket.h"
# include "Battleground.h"

enum BattlegroundVshObjectTypes
{
    BG_AF_OBJECT_DOOR_1,
    BG_AF_OBJECT_DOOR_2,

    BG_AF_OBJECT_BUFF_1,
    BG_AF_OBJECT_BUFF_2,

    BG_AF_OBJECT_MAX
};

enum BattlegroundVshObjects
{
    BG_AF_OBJECT_TYPE_DOOR_1   = 250430,
    BG_AF_OBJECT_TYPE_DOOR_2   = 250431,
    BG_AF_OBJECT_TYPE_BUFF_1   = 184663,
    BG_AF_OBJECT_TYPE_BUFF_2   = 184664
};

Position const BgVshDoorPositions[BG_AF_OBJECT_MAX][2] =
{
    { { 3548.342f, 5584.779f, 323.6123f, 1.544616f }, { 0.0f, 0.0f, 0.6977901f, 0.7163023f } },
    { { 3539.870f, 5488.701f, 323.5819f, 1.553341f }, { 0.0f, 0.0f, 0.7009087f, 0.7132511f } },
    { { 3579.575f, 5578.006f, 326.767f,  0.0f },      { 0.0f, 0.0f, 0.0f, 0.0f } },
    { { 3505.941f, 5499.250f, 325.818f,  0.0f },      { 0.0f, 0.0f, 0.0f, 0.0f } },
};

class BattlegroundAFScore : public BattlegroundScore
{
    public:
        BattlegroundAFScore() { };
        virtual ~BattlegroundAFScore() { };
};

class BattlegroundAF : public Battleground
{
    public:
        BattlegroundAF();
        ~BattlegroundAF();

        void AddPlayer(Player* p_Player) override;
        void RemovePlayer(Player* p_Player, uint64 p_Guid, uint32 p_Team) override;

        void FillInitialWorldStates(ByteBuffer& p_Packet) override;
        void StartingEventCloseDoors() override;
        void StartingEventOpenDoors() override;
        void HandleKillPlayer(Player* p_Player, Player* p_Killer) override;

        bool SetupBattleground() override;

        uint32 GetZoneId() override { return BattlegroundAreaIds::BATTLEGROUND_AF_ZONEID; }

        virtual void PostUpdateImpl(uint32 p_Diff) override;

    private:

        int32 m_CheatersCheckTimer;
};

#endif ///< BATTLEGROUND_AF_HPP
