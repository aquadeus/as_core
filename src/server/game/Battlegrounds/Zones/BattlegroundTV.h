////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __BATTLEGROUNDTV_H
#define __BATTLEGROUNDTV_H

class Battleground;

enum BattlegroundTVObjectTypes
{
    BG_TV_OBJECT_DOOR_1         = 0,
    BG_TV_OBJECT_DOOR_2         = 1,
    BG_TV_OBJECT_BUFF_1         = 2,
    BG_TV_OBJECT_BUFF_2         = 3,
    BG_TV_OBJECT_MAX            = 4
};

enum BattlegroundTVObjects
{
    BG_TV_OBJECT_TYPE_DOOR_1    = 213196,
    BG_TV_OBJECT_TYPE_DOOR_2    = 213197,
    BG_TV_OBJECT_TYPE_BUFF_1    = 184663,
    BG_TV_OBJECT_TYPE_BUFF_2    = 184664
};

class BattlegroundTVScore : public BattlegroundScore
{
    public:
        BattlegroundTVScore() {};
        virtual ~BattlegroundTVScore() {};
};

class BattlegroundTV : public Battleground
{
    public:
        BattlegroundTV();
        ~BattlegroundTV();

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player* player) override;
        virtual void StartingEventCloseDoors() override;
        virtual void StartingEventOpenDoors() override;

        void RemovePlayer(Player* player, uint64 guid, uint32 team) override;
        void HandleAreaTrigger(Player* Source, uint32 Trigger, bool p_Enter) override;
        bool SetupBattleground() override;
        virtual void Reset() override;
        virtual void FillInitialWorldStates(ByteBuffer &d) override;
        void HandleKillPlayer(Player* player, Player* killer) override;
        bool HandlePlayerUnderMap(Player* player) override;

        uint32 GetZoneId() override { return BATTLEGROUND_TV_ZONEID; }
};
#endif
