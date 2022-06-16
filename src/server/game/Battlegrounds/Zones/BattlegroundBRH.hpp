////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef BATTLEGROUND_BRH_HPP
# define BATTLEGROUND_BRH_HPP

# include "Arena.h"
# include "Language.h"
# include "Object.h"
# include "ObjectMgr.h"
# include "Player.h"
# include "WorldPacket.h"
# include "Battleground.h"

enum BattlegroundBrhObjectTypes
{
    BG_BRH_OBJECT_DOOR_1,
    BG_BRH_OBJECT_DOOR_2,
    BG_BRH_OBJECT_DOOR_3,
    BG_BRH_OBJECT_DOOR_4,
    BG_BRH_OBJECT_DOOR_5,
    BG_BRH_OBJECT_DOOR_6,
    BG_BRH_OBJECT_DOOR_7,
    BG_BRH_OBJECT_DOOR_8,
    BG_BRH_OBJECT_DOOR_9,
    BG_BRH_OBJECT_BUFF_1,
    BG_BRH_OBJECT_BUFF_2,
    BG_BRH_OBJECT_MAX
};

enum BattlegroundBrhObjects
{
    BG_BRH_OBJECT_TYPE_DOOR   = 245762,
    BG_BRH_OBJECT_TYPE_BUFF_1 = 184663,
    BG_BRH_OBJECT_TYPE_BUFF_2 = 184664
};

enum BattlegroundBrhData
{
    BG_DRH_HANDLE_DOORS_TIMER = 1 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS
};

Position const BgBrhDoorPositions[BG_BRH_OBJECT_MAX][2] =
{
    { { 1455.901f, 1264.999f, 33.27016f, 0.5410506f }, { 0.0f, 0.0f, 0.26723770f, 0.9636307f } },
    { { 1464.322f, 1254.879f, 33.24255f, 0.8424869f }, { 0.0f, 0.0f, 0.40889550f, 0.9125812f } },
    { { 1450.722f, 1276.739f, 33.24255f, 0.2490755f }, { 0.0f, 0.0f, 0.12421610f, 0.9922552f } },
    { { 1385.847f, 1245.188f, 33.27016f, 3.1631660f }, { 0.0f, 0.0f, -0.9999418f, 0.0107864f } },
    { { 1384.068f, 1258.205f, 33.21188f, 3.4645110f }, { 0.0f, 0.0f, -0.9869938f, 0.1607584f } },
    { { 1384.612f, 1232.353f, 33.27019f, 2.8669370f }, { 0.0f, 0.0f, 0.99058530f, 0.1368967f } },
    { { 1424.420f, 1208.043f, 33.27061f, 4.7326070f }, { 0.0f, 0.0f, -0.6999226f, 0.7142187f } },
    { { 1411.418f, 1206.091f, 33.22338f, 5.0340440f }, { 0.0f, 0.0f, -0.5847492f, 0.8112141f } },
    { { 1437.221f, 1206.749f, 33.23870f, 4.4364770f }, { 0.0f, 0.0f, -0.7976294f, 0.6031479f } },
    { { 1461.577f, 1225.803f, 33.001f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } },
    { { 1399.563f, 1280.233f, 33.001f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } },

};

class BattlegroundBRHScore : public BattlegroundScore
{
    public:
        BattlegroundBRHScore() { };
        virtual ~BattlegroundBRHScore() { };
};

class BattlegroundBRH : public Battleground
{
    public:
        BattlegroundBRH();
        ~BattlegroundBRH();

        void AddPlayer(Player* p_Player) override;
        void RemovePlayer(Player* p_Player, uint64 p_Guid, uint32 p_Team) override;

        void FillInitialWorldStates(ByteBuffer& p_Packet) override;
        void StartingEventCloseDoors() override;
        void StartingEventOpenDoors() override;
        void HandleKillPlayer(Player* p_Player, Player* p_Killer) override;

        bool SetupBattleground() override;

        void PostUpdateImpl(uint32 p_Diff) override;

        uint32 GetZoneId() override { return BattlegroundAreaIds::BATTLEGROUND_BRH_ZONEID; }

    private:
        uint32 m_HandleDoorsTimer;
};

#endif ///< BATTLEGROUND_BRH_HPP
