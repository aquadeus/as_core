////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __BattlegroundBEA_H
#define __BattlegroundBEA_H

class Battleground;

enum BattlegroundBEAObjectTypes
{
    BG_BEA_OBJECT_DOOR_1,
    BG_BEA_OBJECT_DOOR_2,
    BG_BEA_OBJECT_BUFF_1,
    BG_BEA_OBJECT_BUFF_2,
    BG_BEA_OBJECT_MAX
};

enum BattlegroundBEAObjects
{
    BG_BEA_OBJECT_TYPE_DOOR     = 265571,
    BG_BEA_OBJECT_TYPE_BUFF_1   = 184664,
    BG_BEA_OBJECT_TYPE_BUFF_2   = 184663
};

enum BattlegroundBEAACreatureTypes
{
    BG_BEA_CREATURE_1
};

enum BattlegroundBEAACreatures
{
    BG_BEA_CREATURE_TYPE_1 = 115494
};

enum BattlegroundBEAData
{
    BG_BEA_HANDLE_DOORS_TIMER = 1 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS
};

Position const BgBEDoorPositions[BG_BEA_OBJECT_MAX][4] =
{
    { { 2776.371f, 6055.702f, -3.733995f, 2.217981f }, { 0.0f, 0.0f, 0.89524940f, 0.4455655f } },
    { { 2797.014f, 5953.527f, -3.733995f, 2.217981f }, { 0.0f, 0.0f, -0.4226179f, 0.9063079f } },
    { { 2815.556f, 5971.962f, -4.459123f, 6.213374f }, { 0.0f, 0.0f, -0.0348988f, 0.9993908f } },
    { { 2760.417f, 6037.502f, -3.527965f, 4.834563f }, { 0.0f, 0.0f, -0.6626196f, 0.7489561f } }
};

class BattlegroundBEAScore : public BattlegroundScore
{
    public:
        BattlegroundBEAScore() {};
        virtual ~BattlegroundBEAScore() {};
};

class BattlegroundBEA : public Battleground
{
    public:
        BattlegroundBEA();
        ~BattlegroundBEA();

        virtual void AddPlayer(Player* player) override;
        void RemovePlayer(Player* player, uint64 guid, uint32 team) override;

        virtual void FillInitialWorldStates(ByteBuffer &d) override;
        virtual void StartingEventCloseDoors() override;
        virtual void StartingEventOpenDoors() override;
        void HandleKillPlayer(Player* player, Player* killer) override;

        bool SetupBattleground() override;

        void PostUpdateImpl(uint32 p_Diff) override;

        uint32 GetZoneId() override { return BATTLEGROUND_BEA_ZONEID; }

    private:
        uint32 m_HandleDoorsTimer;
};
#endif
